import db
import os
import time

def netmask_to_cidr(m_netmask):
    return(sum([ bin(int(bits)).count("1") for bits in m_netmask.split(".") ]))
def saveFile(loc,data):
    f = open(f'/home/pi/dms/DMSv1.2/network/{loc}','w')
    f.write(data)
    f.close()
def changeNet():
    net = db.readDb.readDataTabel(1,"network")
    db.updDb.updData(1,"network","flag",0,"flag",1)
    ip = net[0]["iplocal"]
    print(net)
    gateway = net[0]["gateway"]
    netmask = net[0]["netmask"]
    dns = net[0]["dns"]
    
    prefik = netmask_to_cidr(netmask)
    dhcp = net[0]["dhcp"]   

    nmtuiCommand = f"sudo nmcli con mod 'Sambungan kabel 1' ipv4.addresses {ip}/{prefik} ipv4.gateway {gateway} ipv4.dns {dns} ipv4.method 'manual'"
    print(nmtuiCommand)
    os.popen(nmtuiCommand)

    time.sleep(2)
    #program ='sudo ifconfig eth0 down'
    #os.popen(program)
    #time.sleep(15)
    #program ='sudo ifconfig eth0 up'
    #os.popen(program)
    restart_LAN = 'sudo nmcli c down "Sambungan kabel 1" && sudo nmcli c up "Sambungan kabel 1"'
    os.popen(restart_LAN)
    print("LAN RESTARTED")


#changeNet()