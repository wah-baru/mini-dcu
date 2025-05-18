from ast import Name

import asyncio
import json
#import grequests
from time import gmtime, strftime
import requests
from datetime import datetime
from pymysqlpool.pool import Pool
import pymysql.cursors
pool = Pool(host='localhost', port=3306, user='global', password='12345678', db='dcu')
db=[0]*5
for i in range(3):
    db[i] = pool.get_conn()

url=[]
def queryData(x,query):
    global db
    global pool
    try:
        db[x].ping(reconnect=True)
        cursor = db[x].cursor()
        cursor.execute(query)
        result = cursor.fetchall()     
        db[x].commit()
        cursor.close()
        pool.release(db[x])
        db[i] = pool.get_conn()
        #print(result)
        return result 
    
    except NameError:
        print(NameError)


# class updateDb:
#     def updData(x,tabel,row,val,where,val2):
#         query =f"update {tabel} set {row} = {val} where {where} = {val2}"
#        # print(query)
#         try:
#             results =queryData(x,query)
#             return results
#         except NameError:
#             print("query Error"+NameError)
    

class readDb:
    def readNetwork(x):
        query = f"select * from network "
        try:
            results = queryData(x, query)
            return results
        except:
            print("eror query")

    def readInstantMeterClient1(x):
        query= f"select * from dt_meter_profile_instant where `0.0.96.1.0.255` = '39400019112'"
        try:
           results =queryData(x,query)
           return results
        except:
            print("eror query")
    
    def readInstantMeterAll(x):
        query= f"select * from dt_meter_profile_instant"
        try:
           results =queryData(x,query)
           return results
        except:
            print("eror query")
                
    def readMtMeterAll(x):
        query= f"select * from dt_meter_profile_instant"
        try:
           results =queryData(x,query)
           return results
        except:
            print("eror query")

    def readMtMeter(x, idMtMeter):
        query = f'SELECT * FROM mt_meter WHERE id = {int(idMtMeter)}'
        try:
           results =queryData(x,query)
           return results
        except:
            print("eror query")
        

class updateDb:
    # db.updDb.updData(1,"network","flag",0,"flag",1)
    def updData(x,tabel,row,val,where,val2):
        query =f"update {tabel} set {row} = {val} where {where} = {val2}"
       # print(query)
        try:
            results =queryData(x,query)
            return results
        except NameError:
            print("query Error"+NameError)

class insertDb:
    def m_file_iec_insert(x,domainId,itemId,ip,relayId):
        #query= f"insert into m_file_iec (domain_id,item_id,relay_id,ip_address) values('{domainId}','{itemId}','{relayId}','{ip}')"
        query= f"insert into it_file_iec (domain_id,item_id,id_device,ip_address) values('{domainId}','{itemId}','{relayId}','{ip}')"
        try:
           results =queryData(x,query)
           return results
        except:
            print("eror query")

    def m_fileDR_temp(x,port,id_device,status,flag,nameFile):
        status=str(status).replace("'", "\"")
        nameFile = str(nameFile)
        #query= f"insert into it_file_iec (domain_id,item_id,id_device,ip_address) values('{domainId}','{itemId}','{relayId}','{ip}')"
        query = f"INSERT INTO fileDR_temp (port_device,id_device,status,flag,nama) values('{port}','{id_device}','{status}','{flag}','{nameFile}')"
        print(query)
        
        try:
           results =queryData(x,query)
           return results
        except:
            print("eror query")