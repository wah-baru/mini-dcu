from gurux_dlms.enums import InterfaceType, Authentication, Security, Standard
from gurux_dlms import GXDLMSClient
from gurux_dlms.secure import GXDLMSSecureClient
from gurux_dlms.GXByteBuffer import GXByteBuffer
from gurux_dlms.objects import GXDLMSObject
from gurux_dlms.objects.enums import ControlMode, ControlState
from gurux_common.enums import TraceLevel
from gurux_common.io import Parity, StopBits, BaudRate
from gurux_net.enums import NetworkType
from gurux_net import GXNet
from gurux_serial.GXSerial import GXSerial

from gurux_dlms.GXDateTime import GXDateTime
from gurux_dlms.GXBitString import GXBitString
from gurux_dlms.internal._GXCommon import _GXCommon
from gurux_dlms import GXDLMSException, GXDLMSExceptionResponse, GXDLMSConfirmedServiceError, GXDLMSTranslator
from gurux_dlms import GXByteBuffer, GXDLMSTranslatorMessage, GXReplyData
from gurux_dlms import GXDLMSConverter, GXStandardObisCodeCollection
from gurux_dlms.enums import RequestTypes, Security, InterfaceType, ObjectType, DataType, DateTimeSkips
from gurux_dlms.secure.GXDLMSSecureClient import GXDLMSSecureClient
from gurux_dlms.objects import GXDLMSObject, GXDLMSObjectCollection, GXDLMSData, GXDLMSRegister, \
    GXDLMSDemandRegister, GXDLMSProfileGeneric, GXDLMSExtendedRegister, GXDLMSDisconnectControl, GXDLMSClock
from GXDLMSReader import GXDLMSReader

from datetime import datetime, timedelta
import json
import time
import mysql.connector as sql
from mysql.connector import Error
import os
from collections import defaultdict

from paho.mqtt import client as mqtt_client
import schedule


broker = '127.0.0.1'
port = 1883
client_id = 'amrDLMS_ECU_meter1'
username = 'mgi'
password = 'admin@mgi'

# topicHeader = 'huawei/029STH6RNB507115/'
topicHeader = ''

# db connection
db = None

def connect_db():
    try:    
        db = sql.connect(
            host = os.getenv("DB_HOST"),
            user = os.getenv("DB_USER"),
            password = os.getenv("DB_PASS"),
            database = "dcu"
        )
        return db
    except Error as e:
        print(f"Error while connecting to MySQL: {e}")
        return None

# if db.is_connected():
#     print("Connected to DB")

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)
    # Set Connecting Client ID
    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def dateTimestamp():
    dateNow = datetime.now()
    dateNowTimestamp = dateNow.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3]
    dateNowTimestamp = dateNowTimestamp+'+0800'
    return dateNowTimestamp


def dateToken():
    dateNow = datetime.now()
    dateNowToken = int(datetime.timestamp(dateNow.replace(microsecond=0)))
    dateNowTokenStr = str(dateNowToken)+'_'+str(dateNow.microsecond)
    return dateNowTokenStr


jsonFileName = '/home/mgi/project/MINIDCU/python/dlms_json/meter1.json'
jsonFile = open(jsonFileName, 'r')
jsonFileLoad = json.loads(jsonFile.read())
dlmsConf = []
readyData = []


def mediaSettings(dicts=dict):
    media = None
    if 'mode' in dicts:
        if dicts['mode'].lower() == 'tcp':
            media = GXNet(NetworkType.TCP, dicts['host'], dicts['ports'])
            print(media)
    else:
        media = GXSerial(None)
        if dicts['com'] == "rs485".lower():
            media.port = dicts['rs485']
        else:
            media.port = dicts['rs232']
        media.baudRate = int(dicts['baudRate'])
        media.dataBits = 8
        media.parity = Parity.NONE
        media.stopbits = StopBits.ONE
    print("media is : ", media)
    return media


def clientSettings(dicts=dict):
    client = GXDLMSSecureClient(True)
    client.clientAddress = int(dicts['clientAddress'])

    # if client.serverAddress != 1:
    #     client.serverAddress = GXDLMSClient.getServerAddress(
    #         client.serverAddress, int(dicts['serverAddress']))
    # else:
    #     client.serverAddress = int(dicts['serverAddress'])
    # print(GXDLMSClient.getServerAddress(
    #     6, 3233))
    # client.serverAddress = 1
    # client.serverAddress = GXDLMSClient.getServerAddress(
        # 1, int(dicts['serverAddress']), 2)
    # print(client.serverAddress)
    client.serverAddress = int(dicts['serverAddress'])

    if dicts['interface'].lower() == "HDLC".lower():
        print("INTERFACE : HDLC")
        client.interfaceType = InterfaceType.HDLC
    elif dicts['interface'].lower() == "WRAPPER".lower():
        print("INTERFACE : WRAPPER")
        client.interfaceType = InterfaceType.WRAPPER
    elif dicts['interface'].lower() == "HdlcWithModeE".lower():
        print("INTERFACE : HDLC_WITH_MODE_E")
        client.interfaceType = InterfaceType.HDLC_WITH_MODE_E
    elif dicts['interface'].lower() == "Plc".lower():
        print("INTERFACE : PLC")
        client.interfaceType = InterfaceType.PLC
    elif dicts['interface'].lower() == "PlcHdlc".lower():
        print("INTERFACE : PLC_HDLC")
        client.interfaceType = InterfaceType.PLC_HDLC

    if dicts['auth'].lower() == "None".lower():
        client.authentication = Authentication.NONE
        print("AUTH NONE")
    elif dicts['auth'].lower() == "Low".lower():
        client.authentication = Authentication.LOW
        print("AUTH LOW")
    elif dicts['auth'].lower() == "High".lower():
        client.authentication = Authentication.HIGH
    elif dicts['auth'].lower() == "HighMd5".lower():
        client.authentication = Authentication.HIGH_MD5
    elif dicts['auth'].lower() == "HighSha1".lower():
        client.authentication = Authentication.HIGH_SHA1
    elif dicts['auth'].lower() == "HighGMac".lower():
        client.authentication = Authentication.HIGH_GMAC
        print("AUTH HIGH GMAC")
    elif dicts['auth'].lower() == "HighSha256".lower():
        client.authentication = Authentication.HIGH_SHA256

    if 'password' in dicts:
        if dicts['password'] != '':
            client.password = dicts['password']

    if 'securityLevel' in dicts:
        if dicts['securityLevel'].lower() == "None".lower():
            client.ciphering.security = Security.NONE
        elif dicts['securityLevel'].lower() == "Authentication".lower():
            client.ciphering.security = Security.AUTHENTICATION
        elif dicts['securityLevel'].lower() == "Encryption".lower():
            client.ciphering.security = Security.ENCRYPTION
        elif dicts['securityLevel'].lower() == "AuthenticationEncryption".lower():
            client.ciphering.security = Security.AUTHENTICATION_ENCRYPTION

    if 'systemTitle' in dicts:
        # bytes_data = dicts['systemTitle'].encode('ascii')
        # hexString_systemTitle = bytes_data.hex()
        # client.ciphering.systemTitle = GXByteBuffer.hexToBytes(
        #     hexString_systemTitle)
        client.ciphering.systemTitle = GXByteBuffer.hexToBytes(
            dicts['systemTitle'])
        print(client.ciphering.systemTitle)
    if 'authKey' in dicts:
        # bytes_data = dicts['authKey'].encode('ascii')
        # hexString_authKey = bytes_data.hex()
        client.ciphering.authenticationKey = GXByteBuffer.hexToBytes(
            dicts['authKey'])
        # client.ciphering.authenticationKey = 0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33 
    if 'blockKey' in dicts:
        # bytes_data = dicts['blockKey'].encode('ascii')
        # hexString_blockKey = bytes_data.hex()
        client.ciphering.blockCipherKey = GXByteBuffer.hexToBytes(
            dicts['blockKey'])
        # client.ciphering.blockCipherKey = 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11

    return client


for v in jsonFileLoad:
    dlmsConf.append(v)

def convert_to_readable(data):
    print("converting ",type(data))
    if isinstance(data, GXDateTime):
        print("GXDateTime : ", data)
        if str(data) == "":
            return 0  # Convert GXDateTime to string, printed as string of date
        else:
            return int(datetime.strptime(str(data), '%m/%d/%y %H:%M:%S').timestamp()) # convert to epoch string
    # elif isinstance(data, GXUInt16.GXUInt16):
    #     return int(data)  # Convert GXUInt16 to integer (or use str(data) if you prefer)
    elif isinstance(data, str):
        return str(data)
    elif isinstance(data, GXBitString):
        return str(data)
    else:
        return data  # If it's a simple type, return as is
    
def zip_with_duplicate_keys(keys, values):
    seen = defaultdict(int)
    result = {}

    for key, value in zip(keys, values):
        seen[key] += 1
        if seen[key] == 1:
            result[key] = value
        else:
            suffix = f"_t" if seen[key] == 2 else f"_t{seen[key] - 1}"
            result[f"{key}{suffix}"] = value

    return result

def meter_collect():
    

    for i in dlmsConf:
        for v in i['body']:
            reader = None
            try:
                trace = TraceLevel.INFO
                invocationCounter = "0.0.43.1.0.255"  # 11017
                
                media = mediaSettings(v)
                client = clientSettings(v)
                instant_profile_list = v["instant"]
                client.setUseUtc2NormalTime(True)
                # print(client)

                print("Begin read")
                reader = GXDLMSReader(client, media, trace, invocationCounter)
                try:
                    if reader is not None:
                        print("Reader is initialized.")
                    else:
                        print("Reader is None.")
                except NameError:
                    print("Reader is not defined.")
                print(media)
                try:
                    if media is not None:
                        print("media is initialized.")
                    else:
                        print("media is None.")
                except NameError:
                    print("media is not defined.")
                
                try:
                    media.open()
                    print("media.open() called successfully.")
                except Exception as e:
                    print(f"Failed to open media: {e}")

                

                reader.updateFrameCounter()
                print("AUTENTICATION")
                
                try:
                    reader.initializeConnection()
                    print("reader.initializeConnection() called successfully.")
                except Exception as e:
                    print(f"Failed to initialize connection: {e}")
                
                instant_profile_value = []

                # print("read clock!")
                converter = GXDLMSConverter(client.standard)
                # print(converter)
                clock_ln = GXDLMSClock("0.0.1.0.0.255")
                clock_value = reader.read(clock_ln,2)
                print(clock_value)
                clock_data = {
                        "name": "clock",
                        "obis": "0.0.1.0.0.255",
                        "value": int(datetime.strptime(str(clock_value), '%m/%d/%y %H:%M:%S').timestamp()),
                        "scaler": 1
                    }
                print(clock_data)
                instant_profile_value.append(clock_data)

                cosemid_ln = GXDLMSData("0.0.96.1.0.255")
                cosemid_value = reader.read(cosemid_ln,2)
                print(cosemid_value)
                cosemid_data = {
                        "name": "meterid",
                        "obis": "0.0.96.1.0.255",
                        "value": int(cosemid_value),
                        "scaler": 1
                    }
                print(cosemid_data)
                instant_profile_value.append(cosemid_data)
                
                cosemV1_ln = GXDLMSRegister("1.0.32.7.0.255")
                cosemV1_value = reader.read(cosemV1_ln,2)
                print(cosemV1_value)
                cosemV1_data = {
                        "name": "meterid",
                        "obis": "1.0.32.7.0.255",
                        "value": int(cosemV1_value),
                        "scaler": 1
                    }
                print(cosemV1_data)
                instant_profile_value.append(cosemV1_data)
                
                # print the name of obis code
                # cosemid_obis = reader.read(cosemid_ln,1)
                # cosemId_desc = GXDLMSConverter.getDescription(converter, cosemid_obis)
                # print("description : ", cosemId_desc)
                
                meterId = []
                list_meter_key = []
                
                ret_meterData = {}
                ret_unitData = {}
                
                # read profile bulanan
                # try:
                #     # Reconnect to DB at the start of each loop cycle
                #     db = connect_db()
                #     if db is None:
                #         print("Failed to reconnect to DB. Exiting loop.")
                #         # break  # or continue, depending on your needs
                # except Error as e:
                #     print(f"Error while connecting to MySQL: {e}")
                #     return
                # buff_bulanan = [] 
                # cosemProfile_ln = GXDLMSProfileGeneric("0.0.98.1.0.255")
                # cosemProfile_value = reader.read(cosemProfile_ln,1)
                # cosemProfile_value = reader.read(cosemProfile_ln,3)
                
                # buff_scalerData = []
                # buff_obisCode = []
                # buff_obisCode_name = []
                
                # # get the obis code and their corresponding name 
                # for obj in cosemProfile_value:
                #     obis = obj[0]
                #     buff_obisCode.append(reader.read(obis,1))
                    
                #     obisCode_name = GXDLMSConverter.getDescription(converter, reader.read(obis,1))[0]
                #     buff_obisCode_name.append(obisCode_name)
                    
                # # check if the data are correct
                # print("pg list of name : ", buff_obisCode_name)
                # print("pg obis code : ", buff_obisCode)
                
                # # save the data in dict (json) form
                # # note : change to list to accomodate duplicates
                # ret_meterData = zip_with_duplicate_keys(buff_obisCode_name, buff_obisCode)
                # print("meter profile generic data : ", ret_meterData)
                # # start reading the content of specified profile generic
                # cosemProfile_value = reader.read(cosemProfile_ln,2)
                # start = datetime(year=2025, month=4, day=1, hour=9, minute=0, second=0)
                # print("start : ", start)
                # end = datetime.now()
                # # rows = reader.readRowsByRange(cosemProfile_ln, start, end)
                # rows = reader.readRowsByEntry(cosemProfile_ln, 1, 5)
                # print("profile generic 1 : bulanan")
                # print(type(rows), "no. of data : ", len(rows))
                # buff_bulanan = [
                #     [convert_to_readable(data) for data in row]
                #     for row in rows
                # ]
                        
                # print("no. of data bulanan: ", len(buff_bulanan))
                # print("data bulanan : ", buff_bulanan)
                
                # for i in range(len(buff_bulanan)):
                #     ret_unitData[i] = zip_with_duplicate_keys(buff_obisCode, buff_bulanan[i])
              
                # # insert to db        
                # cursor = db.cursor()

                # for row in ret_unitData.values():
                #     db_columns = list(row.keys())
                #     db_values = list(row.values())

                #     col_str = ", ".join(f"`{col}`" for col in db_columns)
                #     placeholders = ", ".join(["%s"] * len(db_values))
                #     update_clause = ", ".join(f"`{col}` = VALUES(`{col}`)" for col in db_columns)

                #     sql = f"""
                #     INSERT INTO dt_meter1_profile_month ({col_str})
                #     VALUES ({placeholders})
                #     ON DUPLICATE KEY UPDATE {update_clause}
                #     """

                #     try:
                #         cursor.execute(sql, db_values)
                #         db.commit()
                #         print("✅ Added/Updated entry to table 'dt_meter1_profile_month'")
                #     except Error as e:
                #         print(f"❌ Failed to insert to table: {e}")

                # cursor.close()
                # db.close()
                # time.sleep(10)
                
                
                # # read profile instan
                # try:
                #     # Reconnect to DB at the start of each loop cycle
                #     db = connect_db()
                #     if db is None:
                #         print("Failed to reconnect to DB. Exiting loop.")
                #         # break  # or continue, depending on your needs
                # except Error as e:
                #     print(f"Error while connecting to MySQL: {e}")
                #     return
                
                # buff_instan = [] 
                # cosemProfileInstan_ln = GXDLMSProfileGeneric("1.96.98.128.0.255")
                # cosemProfileInstan_value = reader.read(cosemProfileInstan_ln,1)
                # # print("columns:", cosemProfile_value)
                # cosemProfileInstan_value = reader.read(cosemProfileInstan_ln,3)
                # # print("columns:", cosemProfile_value)
                
                # buff_scalerData = []
                # buff_obisCode_instan = []
                # buff_obisCode_instan_name = []
                # # scaler
                # # for obj in cosemProfile_value:
                #     # obis = obj[0]
                #     # object = obj[1]
                #     # if isinstance(obis, (GXDLMSRegister)):
                #     #     # print("register")
                #     #     print(reader.read(obis,3)) 
                #     #     buff_scalerData.append(reader.read(obis,3)[0])
                #     # elif isinstance(obis, (GXDLMSExtendedRegister)):
                #     #     # print("extended register")
                #     #     print(reader.read(obis,3)) 
                #     #     buff_scalerData.append(reader.read(obis,3)[0])
                #     # else:
                #     #     buff_scalerData.append(1)
                # # ret_scalerData[meterId] = dict(zip(list_meter_key, buff_scalerData))
                # # print("unit data : ", buff_scalerData)
                
                # # get the obis code and their corresponding name 
                # for obj in cosemProfileInstan_value:
                #     obis = obj[0]
                #     buff_obisCode_instan.append(reader.read(obis,1))
                    
                #     obisCode_instan_name = GXDLMSConverter.getDescription(converter, reader.read(obis,1))[0]
                #     buff_obisCode_instan_name.append(obisCode_instan_name)
                    
                # # check if the data are correct
                # print("pg list of name : ", buff_obisCode_instan_name)
                # print("pg obis code : ", buff_obisCode_instan)
                
                # # save the data in dict (json) form
                # ret_meterData = dict(zip(buff_obisCode_instan_name, buff_obisCode_instan))
                # print("meter profile generic data : ", ret_meterData)
                
                # # start reading the content of specified profile generic
                # cosemProfileInstan_value = reader.read(cosemProfileInstan_ln,2)
                # start = datetime(year=2025, month=1, day=1, hour=0, minute=0, second=0)
                # end = datetime.now()
                # rows = reader.readRowsByRange(cosemProfileInstan_ln, start, end)
                # print("profile generic 2 : instan")
                # print(type(rows), "no. of data : ", len(rows))
                
                # for i in range(len(rows)):
                #     print("rows : ", rows[i])
                #     for j in range(len(rows[i])):
                #         data = rows[i][j]
                #         # print("type : ", type(data))
                #         # print("data : ", convert_to_readable(data))
                #         # print("rows[i][j] : ", rows[i][j])
                #         buff_instan.append(convert_to_readable(data))
                # print("data instan : ", buff_instan)
                
                # ret_unitData_instan = zip_with_duplicate_keys(buff_obisCode_instan, buff_instan)
                # print("unit data (instan): ", ret_unitData_instan)
                # print("unit data items (instan): ", ret_unitData_instan.items())
                
                # # insert to db        
                # cursor = db.cursor()
                
                # db_columns_instan = list(ret_unitData_instan.keys())
                # db_values_instan = list(ret_unitData_instan.values())

                # # Wrap column names in backticks for safety
                # col_str_instan = ", ".join(f"`{col}`" for col in db_columns_instan)
                # placeholders_instan = ", ".join(["%s"] * len(db_values_instan))

                # # Add ON DUPLICATE KEY UPDATE clause
                # update_clause = ", ".join(f"`{col}` = VALUES(`{col}`)" for col in db_columns_instan)

                # sql_instan = f"""
                # INSERT INTO dt_meter_profile_instant ({col_str_instan})
                # VALUES ({placeholders_instan})
                # ON DUPLICATE KEY UPDATE {update_clause}
                # """
                # try:
                #     cursor.execute(sql_instan, db_values_instan)
                #     db.commit()
                #     print("✅ Table 'dt_meter_profile_instant' created or already exists.")
                # except Error as e:
                #     print(f"❌ Failed to insert to table: {e}")
                # finally:
                #     # Close the connection
                #     cursor.close()
                #     db.close()
                
                # topic_instant = str(v['devId'])+"/data/instant_profile"
                # dictSendStr = str(instant_profile_value).replace("'", '"')
                # client_mqtt = connect_mqtt()
                # client_mqtt.publish(topic_instant, dictSendStr)
                
                
                # read profile generic 15 menit
                try:
                    # Reconnect to DB at the start of each loop cycle
                    db = connect_db()
                    if db is None:
                        print("Failed to reconnect to DB. Exiting loop.")
                        # break  # or continue, depending on your needs
                except Error as e:
                    print(f"Error while connecting to MySQL: {e}")
                    return
                
                buff_menit = []
                buff_obisCode_menit = []
                buff_obisCode_menit_name = []
                cosemProfileMenit_ln = GXDLMSProfileGeneric("1.0.99.1.0.255")
                cosemProfileMenit_value = reader.read(cosemProfileMenit_ln,1)
                cosemProfileMenit_value = reader.read(cosemProfileMenit_ln,3)
                cosemProfileMenit_value = reader.read(cosemProfileMenit_ln,2)
                # start = datetime(year=2025, month=5, day=8, hour=21, minute=0, second=0)
                # end = datetime.now()
                # rows_menit = reader.readRowsByRange(cosemProfileMenit_ln, start, end)
                rows_menit = reader.readRowsByEntry(cosemProfileMenit_ln, 1, 1)
                print("profile generic 3")
                print(type(rows_menit), "no. of data : ", len(rows_menit))
                print(rows_menit[0])
                print(rows_menit)
                
                # get the obis code and their corresponding name 
                for obj in cosemProfileMenit_value:
                    obis = obj[0]
                    buff_obisCode_menit.append(reader.read(obis,1))
                    
                    obisCode2_name = GXDLMSConverter.getDescription(converter, reader.read(obis,1))[0]
                    buff_obisCode_menit_name.append(obisCode2_name)
                    
                # check if the data are correct
                print("pg list of name : ", buff_obisCode_menit_name)
                print("pg obis code : ", buff_obisCode_menit)
                
                # save the data in dict (json) form
                ret_meterData = dict(zip(buff_obisCode_menit_name, buff_obisCode_menit))
                print("meter profile generic data : ", ret_meterData)
                
                # start reading the content of specified profile generic
                cosemProfileMenit_value = reader.read(cosemProfileInstan_ln,2)
                start = datetime(year=2025, month=1, day=1, hour=0, minute=0, second=0)
                end = datetime.now()
                rows = reader.readRowsByRange(cosemProfileMenit_ln, 1, 4)
                print("profile generic 3 : menit")
                print(type(rows), "no. of data : ", len(rows))
                print(rows[0][23])
                
                for i in range(len(rows_menit)):
                    print("rows : ", rows_menit[i])
                    for j in range(len(rows_menit[i])):
                        data = rows_menit[i][j]
                        # print("type : ", type(data))
                        # print("data : ", convert_to_readable(data))
                        # print("rows[i][j] : ", rows_menit[i][j])
                        buff_menit.append(convert_to_readable(data))
                
                # buff_menit = [
                #     [convert_to_readable(data) for data in row]
                #     for row in rows
                # ]
                print("data 15 menit : ", buff_menit)
                
                ret_unitData_menit = zip_with_duplicate_keys(buff_obisCode_menit_name, buff_menit)
                print("unit data (15 menit): ", ret_unitData_menit)
                print("unit data items (15 menit): ", ret_unitData_menit.items)
                time.sleep(10)
                
                # insert to db        
                cursor = db.cursor()
                
                db_columns_menit = list(ret_unitData_menit.keys())
                db_values_menit = list(ret_unitData_menit.values())

                # Wrap column names in backticks for safety
                col_str_menit = ", ".join(f"`{col}`" for col in db_columns_menit)
                placeholders_menit = ", ".join(["%s"] * len(db_values_menit))

                # Add ON DUPLICATE KEY UPDATE clause
                update_clause = ", ".join(f"`{col}` = VALUES(`{col}`)" for col in db_columns_menit)

                sql_menit = f"""
                INSERT INTO dt_meter_profile_load ({col_str_menit})
                VALUES ({placeholders_menit})
                ON DUPLICATE KEY UPDATE {update_clause}
                """
                try:
                    cursor.execute(sql_menit, db_values_menit)
                    db.commit()
                    print("✅ Table 'dt_meter1_profile_load' created or already exists.")
                except Error as e:
                    print(f"❌ Failed to insert to table: {e}")
                finally:
                    # Close the connection
                    cursor.close()
                    db.close()
                
                # topic_menit = str(v['devId'])+"/data/menit_profile"
                # dictSendStr = str(menit_profile_value).replace("'", '"')
                # client_mqtt = connect_mqtt()
                # client_mqtt.publish(topic_menit, dictSendStr)

                reader.disconnect()
                reader.close()
            except (ValueError, GXDLMSException, GXDLMSExceptionResponse, GXDLMSConfirmedServiceError) as ex:
                print(ex)
            except (KeyboardInterrupt, SystemExit, Exception) as ex:
                print(ex)
                reader = None
            # finally:
            #     if reader:
            #         try:
            #             reader.close()
            #         except Exception:
            #             print("finally Error")
            #     print("Ended. Press any key to continue.")

# schedule.every(3).seconds.do(meter_collect)

while True:
    # schedule.run_pending()
    meter_collect()
    time.sleep(1)
    