
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "list.h"

List* newList() {
    List* newlist = (List*) malloc(sizeof(List));
    newlist->first = NULL;
    newlist->last = NULL;
    return newlist;
}

Node* newNode(char* str, int code){
    Node* newnode = (Node*) malloc(sizeof(Node));
    int len = strlen(str);
    newnode->str = (char*) malloc( sizeof(char) * (len+1) );
    strcpy(newnode->str, str);
    newnode->str[len] = '\0';
    newnode->code = code;
    newnode->next = NULL;
    return newnode;
}

void listAppend(List* list, char* str, int code){
    if (list != NULL){
        Node* newnode = newNode(str, code);

        if (list->first == NULL)
            list->first = newnode;

        if (list->last != NULL)
            list->last->next = newnode;
        
        list->last = newnode;
    }
}

int getCode(List* list, char* str){
    int code = -1;

    if (list != NULL){

        Node* node;
        for (node=list->first; node!=NULL && code==-1; node=node->next) {
            if (!strcmp(node->str, str))
                code = node->code;
        }

    }

    return code;
}


void printList(Node* node, FILE* fp){
    if (node != NULL){
        fprintf(fp, "%s --> %d\n", node->str, node->code);
        printList(node->next, fp);
    }
}


