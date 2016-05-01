#ifndef _INIT_H_
#define _INIT_H_

extern void* pInstance;
extern int (*SendKey)(unsigned*, int);
extern char* path;

struct ri_args {
    void* pInstance;
    int (*SendKey)(unsigned*, int);
    char* path;
};

void* samote_main(void *argsp);

#endif
