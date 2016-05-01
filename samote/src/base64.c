#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base64.h"

int get64Pos(char c){
    char dict[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char* posp = strchr(dict, c);
    return (posp)? (posp - dict) : 0;
}


char* base64decode(char* encoded, int size) {
    
    char* decoded = malloc(sizeof(char) * (size/4*3 + 1));
    
    int i, j;
    for (i=0, j=0; i<size; i+=4, j+=3){

        unsigned long temp = 
            (get64Pos(encoded[i+0]) << 18) |
            (get64Pos(encoded[i+1]) << 12) |
            (get64Pos(encoded[i+2]) <<  6) |
            (get64Pos(encoded[i+3]) <<  0);

        decoded[j+0] = (char)((temp >> 16) & 0xFF);
        decoded[j+1] = (char)((temp >>  8) & 0xFF);
        decoded[j+2] = (char)((temp >>  0) & 0xFF);

    }

    decoded[j] = 0x00;

    return decoded;
}
