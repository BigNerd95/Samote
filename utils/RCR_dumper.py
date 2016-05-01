#!/usr/bin/env python3

# Samsung Remote Control Receiver protocol dumper
# You can use this tool to dump the strings sent by apps who can control newer (>2010) Samsung TVs

from multiprocessing import Process
import base64
import socket
import struct
import sys


def ssdp():

    ssdp_response = "HTTP/1.1 200 OK\r\n" + \
                    "CACHE-CONTROL: max-age=1800\r\n" + \
                    "Date: Thu, 01 Jan 1970 00:24:51 GMT\r\n" + \
                    "EXT:\r\n" + \
                    "LOCATION: http://192.168.2.35:52235/rcr/RemoteControlReceiver.xml\r\n" + \
                    "SERVER: SHP, UPnP/1.0, Samsung UPnP SDK/1.0\r\n" + \
                    "ST: urn:samsung.com:device:RemoteControlReceiver:1\r\n" + \
                    "USN: uuid:0a21fe81-00aa-1000-8787-f47b5e7620f1::urn:samsung.com:device:RemoteControlReceiver:1\r\n" + \
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
        if B'M-SEARCH *' in data and B'urn:samsung.com:device:RemoteControlReceiver:1' in data:   
            print('[SSDP] Samsung RemoteControlReceiver')
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
  <deviceType>urn:samsung.com:device:RemoteControlReceiver:1</deviceType>\r\n\
  <friendlyName>[TV]SamyGO 2009</friendlyName>\r\n\
  <manufacturer>Samsung Electronics</manufacturer>\r\n\
  <manufacturerURL>http://www.samsung.com/sec</manufacturerURL>\r\n\
  <modelDescription>Samsung TV RCR</modelDescription>\r\n\
  <modelName>UN55F6300</modelName>\r\n\
  <modelNumber>1.0</modelNumber>\r\n\
  <modelURL>http://www.samsung.com/sec</modelURL>\r\n\
  <serialNumber>20090804RCS</serialNumber>\r\n\
  <UDN>uuid:0a21fe81-00aa-1000-8787-f47b5e7620f1</UDN>\r\n\
  <sec:deviceID>BDCHCBZODCVXU</sec:deviceID>\r\n\
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
        if B'/rcr/RemoteControlReceiver.xml' in request: # and (B'myTifi' in request or B'Remotie' in request):
            print('[MANIFEST] Samsung RemoteControlReceiver')
            client_connection.sendall(bytes(resp01_body,'ascii'))
        #else:
        #    print('[MANIFEST] Unknown')
        
        client_connection.close()



def rlc():
    web_address = ('', 55000)
    web_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    web_sock.bind(web_address)
    web_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    web_sock.listen(1)

    while True:
        client_connection, client_address = web_sock.accept()
        manage_server = Process(target = manage, args=(client_connection, client_address))
        manage_server.start()


def manage(client_connection, client_address):

    while True:
        request = client_connection.recv(1024)
        if len(request) == 0:
            break
        #print(request)

        # unpack header

        appname_len = struct.unpack('<BH', request[:3])[1]
        #print('[RLC] App name length: ', appname_len)

        appname = request[3:3+appname_len]
        print('[RLC] App name: ', appname)

        payload_len = struct.unpack('<H', request[3+appname_len:3+appname_len+2])[0]
        #print('[RLC] Payload length: ', payload_len)

        payload = request[3+appname_len+2:]
        #print('[RLC] Real payload length: ',len(payload))

        # unpack payload

        frame_type = struct.unpack('<H', payload[:2])[0]
        print('[RLC] Frame type:', hex(frame_type))

        if frame_type == 100:
            ip_len = struct.unpack('<H', payload[2:2+2])[0]
            ip_strb64 = payload[ 2+2 : 2+2+ip_len ]
            ip_str = base64.b64decode(str(ip_strb64, 'ascii'))
            print('[RLC] IP: ', ip_str)

            id_len = struct.unpack('<H', payload[2+2+ip_len:2+2+ip_len+2])[0]
            id_strb64 = payload[ 2+2+ip_len+2 : 2+2+ip_len+2+id_len ]
            id_str = base64.b64decode(str(id_strb64, 'ascii'))
            print('[RLC] ID: ', id_str)

            cname_len = struct.unpack('<H', payload[ 2+2+ip_len+2+id_len : 2+2+ip_len+2+id_len+2 ])[0]
            cname_strb64 = payload[ 2+2+ip_len+2+id_len+2 : 2+2+ip_len+2+id_len+2+cname_len ]
            cname_str = base64.b64decode(str(cname_strb64, 'ascii'))
            print('[RLC] Controller name:', cname_str)

            # response
            resp_packet = struct.pack('<BH12sHBBBB', 0x00, 0x000c, bytes('iapp.samsung','ascii'), 0x0004, 0x64, 0x00, 0x01, 0x00)

        elif frame_type == 0:
            cmd_len = struct.unpack('<BH', payload[2:2+3])[1]
            cmd_strb64 = payload[ 2+3 : 2+3+cmd_len ]
            cmd_str = base64.b64decode(str(cmd_strb64, 'ascii'))
            print('[RLC] CMD: ', cmd_str)

            # response
            resp_packet = struct.pack('<BH12sHBBBB', 0x00, 0x000c, bytes('iapp.samsung','ascii'), 0x0004, 0x00, 0x00, 0x00, 0x00)
        elif frame_type == 1:
            kb_len = struct.unpack('<H', payload[2:2+2])[0]
            kb_strb64 = payload[ 2+2 : 2+2+kb_len ]
            print("Len: ", kb_len, "B64: ", kb_strb64)
            kb_str = base64.b64decode(str(kb_strb64, 'ascii'))
            print('[RLC] KB: ', kb_str)

            # response
            resp_packet = struct.pack('<BH12sHBBBB', 0x00, 0x000c, bytes('iapp.samsung','ascii'), 0x0004, 0x00, 0x00, 0x00, 0x00)
        else:
            print('[RLC] Unknown frame type')
            resp_packet = 0x00


        client_connection.sendall(resp_packet)
        #client_connection.close()

    print("Client disconnected!!")

if __name__=='__main__':
     ssdp_server = Process(target = ssdp)
     ssdp_server.start()
     manifest_server = Process(target = manifest)
     manifest_server.start()
     rlc_server = Process(target = rlc)
     rlc_server.start()




