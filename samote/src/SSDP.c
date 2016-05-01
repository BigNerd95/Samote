#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <ifaddrs.h>
#include <netdb.h>

#include "SSDP.h"

#define BUFFER_SIZE 4096

 
void* ssdp_main() {
    
    unsigned char* buffer = (unsigned char*) malloc(BUFFER_SIZE);

    puts("Starting...\n");

    //Create a raw socket that sniff (incoming) UDP packets
    int sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock_raw < 0) {
        puts("Socket Error");
        return NULL;
    }

    puts("Socket created\n");

    while (1) {

        //Receive a packet
        int data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, NULL, NULL);

        if (data_size < 0) {
            printf("Recvfrom error, failed to get packets\n");
            return NULL;
        }

        //Now process the packet
        ProcessUDPPacket(buffer, data_size);

    }

    close(sock_raw);
    printf("Finished");
    return NULL;
}



void ProcessUDPPacket(unsigned char* buffer, int size) {

    //Get the IP Header part of this packet
    struct iphdr* iph = (struct iphdr*) buffer;
    unsigned int ipHdrLen = iph->ihl * 4;

    if (iph->protocol == IPPROTO_UDP){

        //Get the UDP Header part of this packet
        struct udphdr* udph = (struct udphdr*) (buffer + ipHdrLen);
        unsigned int udpHdrLen = sizeof(struct udphdr);

        //Controls if this packet is destinated to port 1900 (SSDP) and contains Samsung Remote Control Receiver request
        if (ntohs(udph->dest) == 1900 && isSSDPsamsung(buffer + ipHdrLen + udpHdrLen)) {
            printf("Received Samsung RCR multicast request\n");

            /*
            printIP(iph->saddr);
            printf(":%d", ntohs(udph->source));
            printf(" --> ");
            printIP(iph->daddr);
            printf(":%d\n", ntohs(udph->dest));
            */

            replySSDP(iph->saddr, udph->source);

            printf("\n");
        }
    }

}



int isSSDPsamsung(unsigned char* payload) {
    return (strstr(payload, "M-SEARCH *") != NULL && strstr(payload, "urn:samsung.com:device:RemoteControlReceiver:1") != NULL);
}



void replySSDP(unsigned int client_ip, unsigned short client_port) {

    char* ssdp_response = createResponse();
    int dgram_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = client_ip;
    client.sin_port = client_port;

    sendto(dgram_socket, ssdp_response, strlen(ssdp_response), 0, (struct sockaddr*) &client, sizeof(client));

    /*
    printf(" --> ");
    printIP(client_ip);
    printf(":%d\n", ntohs(client_port));
    */

    close(dgram_socket);
    free(ssdp_response);
}



void printIP(unsigned int ip) {
    ip = ntohl(ip);
    printf("%d.%d.%d.%d", ip >> 24, (ip & 0x00FF0000) >> 16, (ip & 0x0000FF00) >>  8, ip & 0x000000FF);
}



char* createResponse(){
    
    char res1[] =   "HTTP/1.1 200 OK\r\n"
                    "CACHE-CONTROL: max-age=1800\r\n"
                    "Date: Thu, 01 Jan 1970 00:24:51 GMT\r\n"
                    "EXT:\r\n"
                    "LOCATION: http://";
    
    char res2[] =   ":80/RCR.txt\r\n"
                    "SERVER: SHP, UPnP/1.0, Samsung UPnP SDK/1.0\r\n"
                    "ST: urn:samsung.com:device:RemoteControlReceiver:1\r\n"
                    "USN: uuid:01234567-89ab-cdef-0123-456789abcdef::urn:samsung.com:device:RemoteControlReceiver:1\r\n"
                    "Content-Length: 0\r\n\r\n";

    char interface[32];
    getInterface(interface, sizeof(interface));

    char host[NI_MAXHOST];
    getIP(interface, host);

    char* response = (char*) malloc(sizeof(char) * (sizeof(res1) + strlen(host) + sizeof(res2) + 1));
    sprintf(response, "%s%s%s", res1, host, res2);

    return response;
}



void getInterface(char* def_interface, int size){
    FILE *fp;
    char line[256], *interface, *address;
     
    fp = fopen("/proc/net/route" , "r");
     
    while(fgets(line, 256, fp)) {
        interface = strtok(line , " \t");
        address = strtok(NULL , " \t");
         
        if(interface && address && !strcmp(address, "00000000")) {
            strncpy(def_interface, interface, size-1);
            break;
        }
    }
    
    fclose(fp);
}



void getIP(char* def_interface, char* host) {

    struct ifaddrs *ifaddr, *ifa;
    int family, info;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

        if (ifa->ifa_addr && !strcmp(ifa->ifa_name, def_interface) && ifa->ifa_addr->sa_family == AF_INET){
            info = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (info != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(info));
                exit(EXIT_FAILURE);
            }
        }

    }

    freeifaddrs(ifaddr);
}
 

