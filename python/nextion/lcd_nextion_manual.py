import serial 
import time 
import struct
from paho.mqtt import client as mqtt_client
import random
import subprocess
import os
import datetime
import json


k=struct.pack('B', 0xff)

ser = serial.Serial(
    port='/dev/serial0',
    baudrate =9600,           
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1)

#### MQTT ####
broker = '127.0.0.1'
port = 1883
mqtt_uname = "mgi"
mqtt_pass = "admin@mgi"
client_id = f'subscribe-{random.randint(0, 100)}'
topic_instant = "lcd/instant/#"



meterid_update = 0

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.username_pw_set(mqtt_uname, mqtt_pass)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        # print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        parsingDataInstant(msg.topic,msg.payload.decode())
        # print(value_data_instant)
    client.subscribe(topic_instant)
    client.on_message = on_message
#### MQTT ####




#### SERIAL NEXTION ####
time_now = time.time()
time_actionV = 0
time_actionI = 0
time_interval = 5
max_page = 3
page_counter = 0
time_blink = 1
def setValInt(obj, val):
	command = obj+'.val='+str(val)
	ser.write(command.encode())
	ser.write(k)
	ser.write(k)
	ser.write(k)
	
def setValFloat(obj, val, ):
	command = obj+'.val='+str(val)
	ser.write(command.encode())
	ser.write(k)
	ser.write(k)
	ser.write(k)

def setText(objText, text):
	command = objText+'.txt="' + str(text) + '"'
	ser.write(command.encode())
	ser.write(k)
	ser.write(k)
	ser.write(k)

def setRaw(text):
    print(text)
    ser.write(text.encode())
    ser.write(k)
    ser.write(k)
    ser.write(k)
    ser.flush()
     
# setText('t0','VR')
# setVal('n0',81414134)
# setText('t1','VS')
# setVal('n1',81414134)
# setText('t2','VT')
# setVal('n2',81414134)

def visibleObj(obj, state):
    command = 'vis '+obj+','+str(state)
    # print(command)
    ser.write(command.encode())
    ser.write(k)
    ser.write(k)
    ser.write(k)

def updateVoltage():
    visibleObj('textData3',1)
    visibleObj('valueData3',1)
    visibleObj('textData3',1)
    visibleObj('valueData3',1)
    setText('textData1','VR')
    setText('valueData1',str(value_data_instant_summary['VR']/10))
    setText('textData2','VS')
    setText('valueData2',value_data_instant_summary['VS']/10)
    setText('textData3','VT')
    setText('valueData3',value_data_instant_summary['VT']/10)

def updateCurrent():
    setText('textData1','IR')
    setText('valueData1',value_data_instant_summary['IR']/100)
    setText('textData2','IS')
    setText('valueData2',value_data_instant_summary['IS']/100)
    setText('textData3','IT')
    setText('valueData3',value_data_instant_summary['IT']/100)

def updateEnergy():
    visibleObj('textData3',0)
    visibleObj('valueData3',0)
    visibleObj('textData3',0)
    visibleObj('valueData3',0)
    setText('textData1','KWH')
    setText('valueData1',value_data_instant_summary['EactAct'])
    setText('textData2','Kvarh')
    setText('valueData2',value_data_instant_summary['EimpReact'])
    


def runLCD():
    global time_actionV
    global time_actionI
    global page_counter
    global max_page
    global time_blink

    if page_counter == 1:
        updateVoltage()
        visibleObj('picComm',1)
    elif page_counter ==2:
        updateCurrent()
        visibleObj('picComm',0)
    elif page_counter ==3:
        updateEnergy()
        visibleObj('picComm',1)

    if (time.time() - time_actionV > time_interval):
        time_actionV = time.time(); 
        if (page_counter < max_page):
            page_counter = page_counter + 1; 
        else:
            page_counter = 1;     

    # print(value_data_instant[0])    
    # print(value_data_instant[1])

    # for idx in range(len(value_data_instant)): 
    #     print("DATA: ",idx, value_data_instant);

 

    
#### SERIAL NEXTION ####


#### PARSING DATA INSTANT ####
max_meter = 4
topic_data_instant = [
    "clock" ,"meterSN","VR", "VS", "VT", "VAngR", "VAngS", "VAngT",
    "IR", "IS", "IT", "IN", "IAngR", "IAngS", "IAngT",
    "pfR", "pfS", "pfT", "pf",
    "PimpActR", "PimpActS", "PimpActT", "PimpAct",
    "PexpActR", "PexpActS", "PexpActT", "PexpAct",
    "PimpReactR", "PimpReactS", "PimpReactT", "PimpReact",
    "PexpReactR", "PexpReactS", "PexpReactT", "PexpReact",
    "PimpAppR", "PimpAppS", "PimpAppT", "PimpApp",
    "PexpAppR", "PexpAppS", "PexpAppT", "PexpApp",
    "EactActR", "EactActS", "EactActT", "EactAct",
    "EexpActR", "EexpActS", "EexpActT", "EexpAct",
    "EimpReactR", "EimpReactS", "EimpReactT", "EimpReact",
    "EexpReactR", "EexpReactS", "EexpReactT", "EexpReact",
    "EbillReact", "f", "Thd", "Tdd", "Vbatt"
]


# for i in range(len(topic_data_instant)):
#     print(i, topic_data_instant[i])
value_data_instant_summary = {el:0 for el in topic_data_instant}
value_data_instant = [{el:0 for el in topic_data_instant}] * max_meter

def parsingDataInstant(topic, value):
    global value_data_instant
    val_dic = json.loads(value) #decode json data
    # print(val_dic)
    topic = str(topic)
    # print(topic)
    if "summary" in topic:
        for index in range(len(topic_data_instant)):
            value_data_instant_summary[topic_data_instant[index]] = val_dic[topic_data_instant[index]]
    else:
        topic = topic.replace("lcd/instant/",'')
        meterid = int(topic[0])-1
        # print(meterid)
        idx = 0

        # value_data_instant[meterid] = val_dic
        # print(value_data_instant)
        # print(val_dic)
        # print(value_data_instant)
        for idx in range(len(topic_data_instant)):
            # if topic_data_instant[idx] == topic:
            # print(topic_data_instant[idx])
            # print(value_data_instant[meterid])
            # print(val_dic[topic_data_instant[idx]])
            
            value_data_instant[meterid][topic_data_instant[idx]] = val_dic[topic_data_instant[idx]]
            idx = idx+1
        
        UpdatePageHome(meterid)
    setRaw("start.val=1")
    
#### PARSING DATA INSTANT ####
def writeTime():
    now = datetime.datetime.now()
    setRaw("rtc0="+str(now.year))
    setRaw("rtc1="+str(now.month))
    setRaw("rtc2="+str(now.day))
    setRaw("rtc3="+str(now.hour))
    setRaw("rtc4="+str(now.minute))
    setRaw("rtc5="+str(now.second))

#### HOME STATUS ####
def checkClientCount():
    json_file_count = 0 
    for roots,dirs, files in os.walk('/home/mgi/project/MINIDCU/python/dlms_json'):
        for file in files:
            if os.path.splitext(file)[1] == '.json':
                json_file_count += 1   
                
    # print(json_file_count)
    return json_file_count

def UpdatePageHome(idMeter):
    countMeterClient = checkClientCount()
    setRaw("meterClient.val="+str(int(countMeterClient)))
    
    # print("idmeter: ",idMeter)
    if idMeter == 0:
        # print(value_data_instant[0])
        setRaw('meterSN1.txt="'+str(int(value_data_instant[0]['meterSN']))+'"')
        setRaw('kwhMeter1.txt="'+str(int(value_data_instant[0]['EactAct']))+'"')
    elif idMeter == 1:
        setRaw('meterSN2.txt="'+str(int(value_data_instant[1]['meterSN']))+'"')
        setRaw('kwhMeter2.txt="'+str(int(value_data_instant[1]['EactAct']))+'"')
    elif idMeter == 2:
        setRaw('meterSN4.txt="'+str(int(value_data_instant[2]['meterSN']))+'"')
        setRaw('kwhMeter3.txt="'+str(int(value_data_instant[2]['EactAct']))+'"')
    elif idMeter == 3:
        setRaw('meterSN4.txt="'+str(int(value_data_instant[3]['meterSN']))+'"')
        setRaw('kwhMeter4.txt="'+str(int(value_data_instant[3]['EactAct']))+'"')

#### HOME STATUS ####

def handlerLCD():
    global ser
    # data = ser.read_until(b'\xFF\xFF\xFF')
    # if len(data) >= 7 and data[0] == 0x65:
    #     page_id = data[1]
    #     component_id = data[2]
    #     event_type = data[3]

    #     event_name = "Pressed" if event_type == 0x01 else "Released"
    #     print(f"Touch event - Page: {page_id}, Component: {component_id}, Event: {event_name}")

    if data.startswith(b'\x68') and data.endswith(b'\xFF\xFF\xFF') and len(data) == 7:
        page_id = data[1]
        comp_id = data[2]
        touch_type = data[3]
        ret =  {
            "page": page_id,
            "component": comp_id,
            "event": "Pressed" if touch_type == 0x01 else "Released"
        }
        print(ret)


### RUNNNNN ####
def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_start()
    time_now = time.time()
    for i in range(5):
        time.sleep(1)
    
    while True:
        runLCD()
        handlerLCD()
        # setRaw("start.val=1")
        time.sleep(1)
        

run()
# writePageHome()

