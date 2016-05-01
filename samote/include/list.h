#ifndef _LIST_H_
#define _LIST_H_

typedef struct _Node {
    char* str;
    int code;
    struct _Node* next;
} Node;

typedef struct {
    Node* first;
    Node* last;
} List;

List* newList();
Node* newNode(char* str, int code);
void listAppend(List* list, char* str, int code);
int getCode(List* list, char* str);
void printList(Node* node, FILE* fp);

#endif
