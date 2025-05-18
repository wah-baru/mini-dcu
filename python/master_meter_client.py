import databases
import json

meter_client = databases.readDb.readMeterClient(0)
meter_master = databases.readDb.readMtMeterAll(0)
print(meter_client)
print(meter_master)



def id_to_string_dev_serial(id):
  if id == 1:
    return "/dev/ttyUSB3","/dev/ttyUSB4"
  if id == 2:
    return "/dev/ttyUSB2","/dev/ttyUSB5"
  if id == 3:
    return "/dev/ttyUSB1","/dev/ttyUSB6"
  if id == 4:
    return "/dev/ttyUSB0","/dev/ttyUSB7"
# Convert master meter list to a dictionary for quick lookup
meter_dict = {meter['id']: meter for meter in meter_master}


# Create JSON for each client
for client in meter_client:
    meter = meter_dict.get(client['meter_type'])
    if not meter:
        continue
    serials = id_to_string_dev_serial(client['dev_id'])
    body = {
        "devId": client['dev_id'],
        "type": client['meter_type'],
        "com": client['port_type'],
        "rs485": serials[0],
        "rs232": serials[1],
        "manufacture": meter.get('manufacture', ''),
        "model": meter.get('model', ''),
        "meter_number": client['meter_number'],
        "baudRate": client['port_baudrate'],
        "clientAddress": meter.get('client_address', 0),
        "logicalAddress": meter.get('logical_server', 0),
        "physicalAddress": meter.get('physical_server', 0),
        "serverAddressSize": meter.get('server_address_size', 0),
        "auth": meter.get('auth_mode', ''),
        "interface": meter.get('interface', ''),
        "securityLevel": meter.get('security_type', ''),
        "systemTitle": meter.get('system_title', ''),
        "authKey": meter.get('auth_key', ''),
        "blockKey": meter.get('block_chiper_key', 0),
        "password": meter.get('password', ''),
        "obis_invocation": meter.get('obis_invocation', '')
    }

    # Optionally add any other master meter keys dynamically if needed
    # (Exclude 'id' to avoid confusion)
    # for key, value in meter.items():
    #     if key not in body and key != 'id':
    #         body[key] = value

    final_json = [
        {
            "app": f"meter{client['dev_id']}",
            "body": [body]
        }
    ]

    # Save to meter<devId>.json
    filename = f"meter{client['dev_id']}.json"
    with open(filename, 'w+') as f:
        json.dump(final_json, f, indent=2)

    print(f"Created JSON: {filename}")