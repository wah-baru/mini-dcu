# from gurux_dlms import *
from gurux_dlms.enums import InterfaceType, Authentication, Security, Standard
from gurux_dlms import GXDLMSClient
from gurux_dlms.secure import GXDLMSSecureClient
from gurux_dlms.GXByteBuffer import GXByteBuffer
from gurux_dlms.objects import GXDLMSObject
from gurux_dlms.objects.enums import ControlMode, ControlState
from gurux_common.enums import TraceLevel
from gurux_common.io import Parity, StopBits, BaudRate
# from gurux_net.enums import NetworkType
# from gurux_net import GXNet
from gurux_serial.GXSerial import GXSerial

from gurux_dlms.GXDateTime import GXDateTime
from gurux_dlms.internal._GXCommon import _GXCommon
# from gurux_dlms import GXDLMSException, GXDLMSExceptionResponse, GXDLMSConfirmedServiceError, GXDLMSTranslator
from gurux_dlms import GXByteBuffer, GXReplyData
from gurux_dlms.enums import RequestTypes, Security, InterfaceType, ObjectType, DataType, DateTimeSkips
from gurux_dlms.secure.GXDLMSSecureClient import GXDLMSSecureClient
from gurux_dlms.objects import GXDLMSObject, GXDLMSObjectCollection, GXDLMSData, GXDLMSRegister, \
    GXDLMSDemandRegister, GXDLMSProfileGeneric, GXDLMSExtendedRegister, GXDLMSDisconnectControl, GXDLMSClock
from GXDLMSReader import GXDLMSReader

from datetime import datetime, timedelta
import json
import time
import binascii

from paho.mqtt import client as mqtt_client
# import schedule


broker = '127.0.0.1'
port = 1883
client_id = 'amrDLMS_ECU'.format()
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
        bytes_data = dicts['systemTitle'].encode('ascii')
        hexString_systemTitle = binascii.hexlify(bytes_data).decode('ascii')
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

instant_profile_ln = [
    "1.0.32.7.0.255",
    "1.0.52.7.0.255",
    "1.0.72.7.0.255",
    "1.0.31.7.0.255",
    "1.0.51.7.0.255",
    "1.0.71.7.0.255",
    "1.0.91.7.0.255",
    "1.0.33.7.0.255",
    "1.0.53.7.0.255",
    "1.0.73.7.0.255",
    "1.0.13.7.0.255",
    "1.0.81.7.0.255",
    "1.0.81.7.1.255",
    "1.0.81.7.2.255",
    "1.0.81.7.4.255",
    "1.0.81.7.5.255",
    "1.0.81.7.6.255",
    "1.0.21.7.0.255",
    "1.0.41.7.0.255",
    "1.0.61.7.0.255",
    "1.0.1.7.0.255",
    "1.0.22.7.0.255",
    "1.0.42.7.0.255",
    "1.0.62.7.0.255",
    "1.0.2.7.0.255",
    "1.0.23.7.0.255",
    "1.0.43.7.0.255",
    "1.0.63.7.0.255",
    "1.0.3.7.0.255",
    "1.0.24.7.0.255",
    "1.0.44.7.0.255",
    "1.0.64.7.0.255",
    "1.0.4.7.0.255",
    "1.0.29.7.0.255",
    "1.0.49.7.0.255",
    "1.0.69.7.0.255",
    "1.0.9.7.0.255",
    "1.0.30.7.0.255",
    "1.0.50.7.0.255",
    "1.0.70.7.0.255",
    "1.0.10.7.0.255",
    "1.0.21.8.0.255",
    "1.0.41.8.0.255",
    "1.0.61.8.0.255",
    "1.0.1.8.0.255",
    "1.0.22.8.0.255",
    "1.0.42.8.0.255",
    "1.0.62.8.0.255",
    "1.0.2.8.0.255",
    "1.0.23.8.0.255",
    "1.0.43.8.0.255",
    "1.0.63.8.0.255",
    "1.0.3.8.0.255",
    "1.0.24.8.0.255",
    "1.0.44.8.0.255",
    "1.0.64.8.0.255",
    "1.0.4.8.0.255",
    # "1.0.128.8.0.255",
    "1.0.14.7.0.255",
    "1.0.11.7.124.255",
    "1.0.11.7.125.255",
    "0.0.96.6.3.255"
]

instant_profile_data = [
    {"name": "voltage phase R", "obis": "1.0.32.7.0.255", "value": 0, "scaler": 0},
    {"name": "voltage phase S", "obis": "1.0.52.7.0.255", "value": 0, "scaler": 0},
    {"name": "voltage phase T", "obis": "1.0.72.7.0.255", "value": 0, "scaler": 0},
    {"name": "current phase R", "obis": "1.0.31.7.0.255", "value": 0, "scaler": 0},
    {"name": "current phase S", "obis": "1.0.51.7.0.255", "value": 0, "scaler": 0},
    {"name": "current phase T", "obis": "1.0.71.7.0.255", "value": 0, "scaler": 0},
    {"name": "current Neutral", "obis": "1.0.91.7.0.255", "value": 0, "scaler": 0},
    {"name": "power factor phase R", "obis": "1.0.33.7.0.255", "value": 0, "scaler": 0},
    {"name": "power factor phase S", "obis": "1.0.53.7.0.255", "value": 0, "scaler": 0},
    {"name": "power factor phase T", "obis": "1.0.73.7.0.255", "value": 0, "scaler": 0},
    {"name": "power factor", "obis": "1.0.13.7.0.255", "value": 0, "scaler": 0},
    {"name": "Phase voltage angle R", "obis": "1.0.81.7.0.255", "value": 0, "scaler": 0},
    {"name": "Phase voltage angle S", "obis": "1.0.81.7.1.255", "value": 0, "scaler": 0},
    {"name": "Phase voltage angle T", "obis": "1.0.81.7.2.255", "value": 0, "scaler": 0},
    {"name": "Phase current angle R", "obis": "1.0.81.7.4.255", "value": 0, "scaler": 0},
    {"name": "Phase current angle S", "obis": "1.0.81.7.65.255", "value": 0, "scaler": 0},
    {"name": "Phase current angle T", "obis": "1.0.81.7.76.255", "value": 0, "scaler": 0},
    {"name": "import active power phase R (send)", "obis": "1.0.21.7.0.255", "value": 0, "scaler": 0},
    {"name": "import active power phase S (send)", "obis": "1.0.41.7.0.255", "value": 0, "scaler": 0},
    {"name": "import active power phase T (send)", "obis": "1.0.61.7.0.255", "value": 0, "scaler": 0},
    {"name": "import active power", "obis": "1.0.1.7.0.255", "value": 0, "scaler": 0},
    {"name": "export active power phase R (receive)", "obis": "1.0.22.7.0.255", "value": 0, "scaler": 0},
    {"name": "export active power phase S (receive)", "obis": "1.0.42.7.0.255", "value": 0, "scaler": 0},
    {"name": "export active power phase T (receive)", "obis": "1.0.62.7.0.255", "value": 0, "scaler": 0},
    {"name": "export active power", "obis": "1.0.2.7.0.255", "value": 0, "scaler": 0},
    {"name": "import reactive power phase R (send)", "obis": "1.0.23.7.0.255", "value": 0, "scaler": 0},
    {"name": "import reactive power phase S (send)", "obis": "1.0.43.7.0.255", "value": 0, "scaler": 0},
    {"name": "import reactive power phase T (send)", "obis": "1.0.63.7.0.255", "value": 0, "scaler": 0},
    {"name": "import reactive power (send)", "obis": "1.0.3.7.0.255", "value": 0, "scaler": 0},
    {"name": "export reactive power phase R (receive)", "obis": "1.0.24.7.0.255", "value": 0, "scaler": 0},
    {"name": "export reactive power phase S (receive)", "obis": "1.0.44.7.0.255", "value": 0, "scaler": 0},
    {"name": "export reactive power phase T (receive)", "obis": "1.0.64.7.0.255", "value": 0, "scaler": 0},
    {"name": "export reactive power (receive)", "obis": "1.0.4.7.0.255", "value": 0, "scaler": 0},
    {"name": "frequency", "obis": "1.0.14.7.0.255", "value": 0, "scaler": 0},
    {"name": "THD Current", "obis": "1.0.11.7.124.255", "value": 0, "scaler": 0},
    {"name": "TDD Current", "obis": "1.0.11.7.125.255", "value": 0, "scaler": 0},
    {"name": "battery capacity", "obis": "0.0.96.6.3.255", "value": 0, "scaler": 0}
]



def meter_collect():
    for i in dlmsConf:
        for v in i['body']:
            reader = None
            try:
                trace = TraceLevel.VERBOSE
                invocationCounter = "0.0.43.1.0.255"  # 11017
                

                media = mediaSettings(v)
                client = clientSettings(v)
                instant_profile_list = v["instant"]
                # client.setUseUtc2NormalTime(True)
                # print(client)

                reader = GXDLMSReader(client, media, trace, invocationCounter)
                media.open()

                # reader.updateFrameCounter()
                print("AUTENTICATION")
                reader.initializeConnection()

                # bodyValue = []
                # reply = GXReplyData()
                instant_profile_value = []

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
                
                index_profile = 0
                for obj in instant_profile_list:
                    # print(obj)
                    reg = GXDLMSRegister(obj['obis'])
                    read_value = reader.read(reg, 2)
                    if read_value == None:
                        read_value = "null"
                    read_unit = reader.read(reg, 3)
                    instant = {
                        "name": obj['name'],
                        "obis": instant_profile_ln[index_profile],
                        "value": read_value,
                        "scaler": read_unit[0]
                    }
                    instant_profile_value.append(instant)
                    index_profile = index_profile + 1
                    # print(obj['name'],obj['obis'],read_value, read_unit)

                index_profile = 0
                # print(instant_profile_value)
                topic_instant = str(v['devId'])+"/data/instant_profile"
                dictSendStr = str(instant_profile_value).replace("'", '"')
                client_mqtt = connect_mqtt()
                client_mqtt.publish(topic_instant, dictSendStr)

                
                reader.disconnect()
                reader.close()
            except (ValueError) as ex:
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
    time.sleep(10)
# schedule.every(30).seconds.do(meter_collect)

while True:
    # schedule.run_pending()
    meter_collect()
    # time.sleep(10)
    