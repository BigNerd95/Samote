#ifndef _SSDP_H_
#define _SSDP_H_
 
void* ssdp_main();
void ProcessUDPPacket(unsigned char* buffer, int size);
int isSSDPsamsung(unsigned char* payload);
void replySSDP(unsigned int client_ip, unsigned short client_port);
void printIP(unsigned int ip);
char* createResponse();
void getInterface(char* def_interface, int size);
void getIP(char* def_interface, char* host);

#endif
