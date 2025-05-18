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
from gurux_dlms.internal._GXCommon import _GXCommon
from gurux_dlms import GXDLMSException, GXDLMSExceptionResponse, GXDLMSConfirmedServiceError, GXDLMSTranslator
from gurux_dlms import GXByteBuffer, GXDLMSTranslatorMessage, GXReplyData
from gurux_dlms.enums import RequestTypes, Security, InterfaceType, ObjectType, DataType, DateTimeSkips
from gurux_dlms.secure.GXDLMSSecureClient import GXDLMSSecureClient
from gurux_dlms.objects import GXDLMSObject, GXDLMSObjectCollection, GXDLMSData, GXDLMSRegister, \
    GXDLMSDemandRegister, GXDLMSProfileGeneric, GXDLMSExtendedRegister, GXDLMSDisconnectControl, GXDLMSClock
from GXDLMSReader import GXDLMSReader

from datetime import datetime, timedelta
import json
import time


from paho.mqtt import client as mqtt_client
broker = '127.0.0.1'
port = 1883
client_id = f'amrDLMS_ECU'
username = 'mgi'
password = 'admin@mgi'

# topicHeader = 'huawei/029STH6RNB507115/'
topicHeader = ''


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
        bytes_data = dicts['systemTitle'].encode('ascii')
        hexString_systemTitle = bytes_data.hex()
        client.ciphering.systemTitle = GXByteBuffer.hexToBytes(
            hexString_systemTitle)
        # client.ciphering.systemTitle = GXByteBuffer.hexToBytes(
        #     dicts['systemTitle'])
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

print(dlmsConf)
# test = []
# counter = 0

# for v in kafka_meter_response_3Ph:
#     print(kafka_meter_response_3Ph[v])

# while True:


for i in dlmsConf:
    for v in i['body']:
        reader = None
        try:
            trace = TraceLevel.VERBOSE
            invocationCounter = "0.0.43.1.0.255"  # 11017
            obis = v['obis']

            media = mediaSettings(v)
            client = clientSettings(v)
            client.setUseUtc2NormalTime(True)
            # print(client)

            reader = GXDLMSReader(client, media, trace, invocationCounter)
            media.open()

            # reader.updateFrameCounter()
            print("AUTENTICATION")
            reader.initializeConnection()

            # bodyValue = []
            # reply = GXReplyData()

            obj = GXDLMSData("0.0.96.1.0.255")
            readed = reader.read(obj, 2)
            if type(readed) == str:
                readed = readed
            else:
                readed = readed.decode("utf-8")
            print(readed)

            # print("READ DAILY PROFILE")
            # pg = GXDLMSProfileGeneric("1.0.99.1.0.255")
            # instant_profile = reader.read(pg, 3)
            # four = reader.read(pg, 4)
            # five = reader.read(pg, 5)
            # six = reader.read(pg, 6)
            # seven = reader.read(pg, 7)
            # eight = reader.read(pg, 8)
            # print(instant_profile)
            # print(four)
            # print(five)
            # print(six)
            # print(seven)
            # print(eight)

            # rowsByEntry = reader.readRowsByEntry(pg, 8927, 0)
            # print(rowsByEntry)
            # print(rowsByEntry[0][0])
            # dates = GXDateTime(datetime.now())
            # dates.skip(DateTimeSkips.DEVITATION)

            # end_time = datetime.now()
            # start_time = end_time - timedelta(days=1)
            # Data = reader.readRowsByRange(pg, start_time, end_time)
            # print(Data)
            # Data = reader.readRowsByEntry(pg,0,0)
            # print(Data)

            # print(instant_profile)
            # for obj in instant_profile:
            #     obis = obj[0]
            #     # print(type(obis))
            #     objects = obj[1]
            #     # print(obis, objects)
            #     if isinstance(obis, (GXDLMSRegister)):
            #         print(obis,reader.read(obis, 2))

            ################# PG #######################
            # array_profile = []
            
            # print("PROFILE GENERIC MONTH")
            # pg = GXDLMSProfileGeneric("1.0.98.1.0.255")
            # load_profile = reader.read(pg, 3)
            
            # i = 0
            # for lp in load_profile:
            #     obis = lp[0]
            #     # print(obis)
            #     objects = lp[1] 
                
            #     dict_format = {
            #         'obis':0,
            #         'type':0,
            #         'value':0,
            #         'scaler':0,
            #         'unit':0
            #     }
            #     dict_format['obis'] = str(lp[0])
            #     if isinstance(obis, (GXDLMSClock)):
            #         dict_format['type'] = 'clock'
            #     if isinstance(obis, (GXDLMSData)):
            #         dict_format['type'] = 'data'
            #     if isinstance(obis, (GXDLMSRegister)):
            #         # print("obis: %s scaler: %d" % (obis, reader.read(obis, 3)))
            #         scaler = reader.read(obis, 3)
            #         dict_format['type'] = 'register'
            #         dict_format['scaler'] = scaler[0]
            #         dict_format['unit'] = str(scaler[1])
            #     array_profile.append(dict_format)
            #     # print("counter: ",i)
            #     # i = i+1

            # Data = reader.readRowsByEntry(pg,0,0)
            # for month in Data:
            #     for items in month:
                   
            #         if isinstance(items, GXDateTime):
            #             str_time = str(items)
            #             DataTime = 0
            #             try:
            #                 DataTime = int(datetime.strptime( str_time, '%m/%d/%y %H:%M:%S:%f').timestamp())
            #             except:
            #                 DataTime = int(datetime.strptime( str_time, '%m/%d/%y %H:%M:%S').timestamp())
                        
            #             array_profile[i]['value'] = DataTime
            #         elif isinstance(items, bytearray):
            #             array_profile[i]['value'] = items.decode("utf-8")
            #         else:
            #             array_profile[i]['value'] = items
            #         i = i+1
            #     i = 0
            #     dictSendStr = str(array_profile).replace("'", '"')
            #     client_mqtt = connect_mqtt()
            #     client_mqtt.publish("monthlyProfile", dictSendStr)
                # result = client_mqtt.publish(dictSend['topic'], dictSendStr)
            ################# PG #######################


            
            #     obis = obj[0]
            #     # print(type(obis))
            #     objects = obj[1]
            #     # print(obis, objects)
            #     if isinstance(obis, (GXDLMSClock)):
            #         print(str(reader.read(obis, 2)))
            #         # print("obis: %s %s" % (obis, reader.read(obis, 2)))
            #     if isinstance(obis, (GXDLMSRegister)):
            #         print("obis: %s %d" % (obis, reader.read(obis, 2)))
            # PG

            # kafka_meter_response_3Ph[i] = v[i]
            # print(kafka_meter_response_3Ph)
            

            # pg = GXDLMSProfileGeneric("1.0.99.1.0.255")
            # pg_profile = reader.read(pg, 3)
            # Data = reader.readRowsByEntry(pg, 0, 1)
            # print(Data)
            # for value in Data:
            #     print(value)
            # end_time = datetime.now()
            # start_time = end_time - timedelta(minutes=15)
            # Data = reader.readRowsByRange(pg, start_time, end_time)
            # print(Data)
            # for value in Data:
            #     print(value)

            #     print(
            #         f"Timestamp: {value[0]} | SN:{value[1]} | VL1: {value[3]} V | VL2: {value[4]} V | VL3: {value[5]} V | CurrentL1: {value[6]} A | CurrentL2: {value[7]} A | CurrentL3: {value[8]} A | pF: {value[9]}")
            # 1phase
            # for value in Data:
            #     print(f"Timestamp: {value[0]}")
            #     print(f"Serial Number: {value[1]}")
            #     print(f"Status Register: {value[2]}")
            #     print(f"Voltage: {value[3]}")
            #     print(f"Current: {value[4]}")
            #     print(f"Power Factor: {value[5]}")
            #     print(f"Active Power Positive: {value[6]}")
            #     print(f"Active Power Negative: {value[7]}")
            #     print(f"Energy Import: {value[8]}")
            #     print(f"Energy Export: {value[9]}")
            #     print(f"Reactive Energy Export: {value[10]}")
            #     print(f"Reactive Energy Import: {value[11]}")
            #     print(f"Reactive Energy Billing: {value[12]}")
            #     print(f"Alarm Register: {value[13]}")
            # for val in obis:
            #     if val['type'] == 'data':
            #         obis = val['obis']
            #         obj = GXDLMSData(obis)
            #         readed = reader.read(obj, 2)
            #         if type(readed) == str:
            #             readed = readed
            #         else:
            #             readed = readed.decode("utf-8")
            #         temDict = {
            #             "name": val['name'],
            #             "val": readed,
            #             "timestamp": dateTimestamp()
            #         }
            #         bodyValue.append(temDict)
            #         print(temDict)

            #     elif val['type'] == 'register':
            #         obis = val['obis']
            #         obj = GXDLMSRegister(obis)
            #         read_unit = reader.read(obj, 3)
            #         # time.sleep(1)
            #         read_value = reader.read(obj, 2)
            #         # read_3 = reader.read(obj, 1)
            #         # print(read_3)
            #         temDict = {
            #             "name": val['name'],
            #             "val": read_value,
            #             "timestamp": dateTimestamp()
            #         }
            #         print(temDict)
            #         bodyValue.append(temDict)
            # print(read_value)
            # time.sleep(1)
            # topic = "asdads"

            # dictMessage = {}
            # dictMessage["token"] = dateToken()
            # dictMessage["timestamp"] = dateTimestamp()
            # dictMessage["body"] = bodyValue

            # dictSend = {}
            # dictSend['topic'] = topic
            # dictSend['message'] = dictMessage

            # dictSendStr = str(dictSend['message']).replace("'", '"')
            # client_mqtt = connect_mqtt()
            # client_mqtt.publish(Topic, dictSendStr)
            # result = client_mqtt.publish(dictSend['topic'], dictSendStr)
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

    # time.sleep(15)
    # return dictSend['topic'], dictSendStr

    # reader.close()
# for i in readyData:
#    print(i['topic'])
#    print(i['message'])
#    result = clients.publish(i['topic'], str(i['message']).replace("'", '"'))


# def run():
#    client_mqtt = connect_mqtt()
#     # subscribe(client_mqtt)
#    client_mqtt.loop_forever()


# if __name__ == '__main__':
#    run()
