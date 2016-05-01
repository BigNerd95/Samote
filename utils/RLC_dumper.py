#!/usr/bin/env python3

# Remote LAN Control (SamyGO) protocol dumper
# You can use this tool to dump the codes sent by apps who can control 2009 Samsung TVs with Remote LAN Control

from multiprocessing import Process
import socket
import struct
import sys


def ssdp():

    ssdp_response = "HTTP/1.1 200 OK\r\n" + \
                    "CACHE-CONTROL: max-age = 1800\r\n" + \
                    "EXT:\r\n" + \
                    "LOCATION: http://192.168.2.35:52235/dmr/SamsungMRDesc.xml\r\n" + \
                    "SERVER: Linux/9.0 UPnP/1.0 PROTOTYPE/1.0\r\n" + \
                    "ST: urn:schemas-upnp-org:device:MediaRenderer:1\r\n" + \
                    "USN: uuid:9fdfa33f-76f8-4df3-7d8a-48e6918e3055::urn:schemas-upnp-org:device:MediaRenderer:1\r\n" + \
                    "Content-Length: 0\r\n"

    multicast_group = '239.255.255.250'
    ssdp_address = ('', 1900)
    
    ssdp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # Create the socket
    
    ssdp_sock.bind(ssdp_address)# Bind to the server address
    
    group = socket.inet_aton(multicast_group) # Tell the operating system to add the socket to the multicast group on all interfaces.
    mreq = struct.pack('4sL', group, socket.INADDR_ANY)
    ssdp_sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    while True:
        data, address = ssdp_sock.recvfrom(1024)
        #print(data)
        if B'M-SEARCH *' in data and B'urn:schemas-upnp-org:device:MediaRenderer:1' in data:   
            print('[SSDP] Samsung MediaRenderer')
            ssdp_sock.sendto(bytes(ssdp_response, 'ascii'), address)
        #else:
        #    print('[SSDP] Unknown')


def manifest():
    resp01_body = """HTTP/1.1 200 OK\r\n
<?xml version="1.0"?>\r\n\
<root xmlns='urn:schemas-upnp-org:device-1-0' xmlns:sec='http://www.sec.co.kr/dlna' xmlns:dlna='urn:schemas-dlna-org:device-1-0'>\r\n\
 <specVersion>\r\n\
  <major>1</major>\r\n\
  <minor>0</minor>\r\n\
 </specVersion>\r\n\
 <device>\r\n\
  <deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>\r\n\
  <friendlyName>[TV]SamyGO 2009</friendlyName>\r\n\
  <manufacturer>Samsung Electronics</manufacturer>\r\n\
  <manufacturerURL>http://www.samsung.com/sec</manufacturerURL>\r\n\
  <modelDescription>Samsung TV RCR</modelDescription>\r\n\
  <modelName>UN55F6300</modelName>\r\n\
  <modelNumber>1.0</modelNumber>\r\n\
  <modelURL>http://www.samsung.com/sec</modelURL>\r\n\
  <serialNumber>20090804RCS</serialNumber>\r\n\
  <UDN>uuid:01234567-89ab-cdef-0123-456789abcdef</UDN>\r\n\
  <sec:deviceID>ABCDEFGHILMNO</sec:deviceID>\r\n\
  <sec:ProductCap>Resolution:1920X1080,ImageZoom,ImageRotate,Y2013</sec:ProductCap>\r\n\
  <serviceList>\r\n\
   <service>\r\n\
    <serviceType>urn:samsung.com:service:MultiScreenService:1</serviceType>\r\n\
    <serviceId>urn:samsung.com:serviceId:MultiScreenService</serviceId>\r\n\
    <controlURL>/smp_8_</controlURL>\r\n\
    <eventSubURL>/smp_9_</eventSubURL>\r\n\
    <SCPDURL>/smp_7_</SCPDURL>\r\n\
   </service>\r\n\
  </serviceList>\r\n\
 </device>\r\n\
</root>"""

    web_address = ('', 52235)
    web_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    web_sock.bind(web_address)
    web_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    web_sock.listen(1)

    while True:
        client_connection, client_address = web_sock.accept()
        request = client_connection.recv(1024)
        #print('[MANIFEST] ', request)
        if B'/dmr/SamsungMRDesc.xml' in request: 
            print('[MANIFEST] Samsung RemoteControlReceiver')
            client_connection.sendall(bytes(resp01_body,'ascii'))
        #else:
        #    print('[MANIFEST] Unknown')
        
        client_connection.close()



def rlc():
    web_address = ('', 2345)
    web_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    web_sock.bind(web_address)
    web_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    web_sock.listen(1)

    while True:
        client_connection, client_address = web_sock.accept()
        request = client_connection.recv(1024)
        print(request)
        client_connection.close()

if __name__=='__main__':
     ssdp_server = Process(target = ssdp)
     ssdp_server.start()
     manifest_server = Process(target = manifest)
     manifest_server.start()
     rlc_server = Process(target = rlc)
     rlc_server.start()


