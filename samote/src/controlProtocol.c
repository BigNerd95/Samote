/*
Add controls on effective length of frame fields
*/

#include <stdio.h>

#include "samote.h"
#include "base64.h"
#include "list.h"
#include "controlProtocol.h"


void processPacket(int client_sock, char* buffer, int size, List* codeList) {

    char* appName = buffer + 1;
    int appNameLen = appName[0] | (appName[1] << 8);
    appName += 2;
    //printf("appNameLength: %d\n", appNameLen);
    //printf("AppName: %.*s\n", appNameLen, appName);

    if (appNameLen < size - 3 && isRCSamsung(appName, appNameLen)) {
        printf("Received Samsung RCR control frame\n");

        char* payload = appName + appNameLen;
        int payloadLen = payload[0] | (payload[1] << 8);
        payload += 2;

        int frameType = payload[0] | (payload[1] << 8);
        payload += 2;

        switch(frameType){
            case 0: {
                puts("CMD frame");
                processCMD(payload, codeList);

                char resp[] = "\x00\x0c\x00iapp.samsung\x04\x00\x00\x00\x00\x00";
                send(client_sock, resp, sizeof(resp)-1, 0);

                break;
            }

            case 1: {
                puts("Keyboard frame");
                processKB(payload);

                char resp[] = "\x00\x0c\x00iapp.samsung\x04\x00\x01\x00\x00\x00";
                send(client_sock, resp, sizeof(resp)-1, 0);

                break;
            }

            case 100: {
                puts("Auth frame");
                processAuth(payload);

                char resp[] = "\x00\x0c\x00iapp.samsung\x04\x00\x64\x00\x01\x00";
                send(client_sock, resp, sizeof(resp)-1, 0);

                break;
            }

            default: {
                puts("Unknown frame");
                break;
            }
        }

    } 

}

int isRCSamsung(char* appName, int appNameLen) {
    char SamsungAppName[] = "iapp.samsung";
    return (strncmp(appName + appNameLen - sizeof(SamsungAppName), SamsungAppName, sizeof(SamsungAppName))); // sizeof -1 ?
}


void processCMD(char* payload, List* codeList){
    char* CMDencoded = payload + 1;
    int cmdLen = CMDencoded[0] | (CMDencoded[1] << 8);
    CMDencoded += 2;

    char* CMD = base64decode(CMDencoded, cmdLen);
    int code = getCode(codeList, CMD);
    printf("CMD: %s --> %d\n", CMD, code);

    if (code > -1){
        // send key code to tv firmware
        SendKey(pInstance, code);
    }

    free(CMD);
}

void processKB(char* payload){
    char* KBencoded = payload;
    int kbLen = KBencoded[0] | (KBencoded[1] << 8);
    KBencoded += 2;

    char* KB = base64decode(KBencoded, kbLen);
    printf("From IP: %s\n", KB);
    free(KB);
}

void processAuth(char* payload){
    char* IPencoded = payload;
    int ipLen = IPencoded[0] | (IPencoded[1] << 8);
    IPencoded += 2;

    char* IP = base64decode(IPencoded, ipLen);
    printf("From IP: %s\n", IP);
    free(IP);

    char* IDencoded = IPencoded + ipLen;
    int idLen = IDencoded[0] | (IDencoded[1] << 8);
    IDencoded += 2;

    char* ID = base64decode(IDencoded, idLen);
    printf("From ID: %s\n", ID);
    free(ID);

    char* NAMEencoded = IDencoded + idLen;
    int nameLen = NAMEencoded[0] | (NAMEencoded[1] << 8);
    NAMEencoded += 2;

    char* NAME = base64decode(NAMEencoded, nameLen);
    printf("Controller Name: %s\n", NAME);
    free(NAME);

}
