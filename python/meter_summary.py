import databases
import json

# meter_client1 = databases.readDb.readMeterClient1(0)
meter_master = databases.readDb.readMtMeterAll(0)

meter_data = []

# print("Meter Client 1")
# print(list(meter_client1))

print("Meter Master")
print(meter_master)

# for meter in meter_client1:
    
    
    
# DATA INSTAN
meter_client1_instan = databases.readDb.readMtMeterAll(0)
client1_instan_data = []
# print("data instant dari meter client 1")
# print(meter_client1_instan)
for meter in meter_client1_instan:
    client1_instan_data.append(list(meter.keys()))
    # print("Meter Data")
    # print(client1_instan_data)
print("data instan client 1 : ", client1_instan_data)

# for client in data:
#     value = client.get('1.0.32.7.0.255')
#     print(f"Client meter {client.get('0.0.96.1.0.255')}, value: {value}")



# for meter in meter_client1:
#     meter_data.append(meter)
#     print("Meter Data")
#     print(meter_data)

# print ("Member 0 : ", member_0)
# print ("Member 0 type : ", type(member_0))
# print ("Member 0 key(s) : ", list(member_0.keys()))