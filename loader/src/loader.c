#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>

struct ri_args {
    void* pInstance;
    int (*SendKey)(unsigned*, int);
    char* path;
};

int Game_Main(const char* path, const char* udn __attribute__ ((unused))) {

	FILE* fp = fopen("/mtd_ram/samote_loader.log", "a+");

	fputs("Loader launched\n", fp);
	fflush(fp);


	
	/********************************************
	Get SendKey function and Instance from exeDSP
	********************************************/

	unsigned* handle = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL);
	if (handle == NULL) {
		fputs("Error dlopen\n", fp);
		fflush(fp);
		fclose(fp);
		return 1;
	}

	// Get function SendKey
	unsigned* KeyInputBase_SendKey = dlsym(handle, "_ZN14SsKeyInputBase7SendKeyEi");
	if(KeyInputBase_SendKey == NULL){
		fputs("Error dlsym SendKey\n", fp);
		fflush(fp);
		fclose(fp);
		return 1;
	}

	// Get function ObjectList
	unsigned* pKeyInputBase = dlsym(handle, "_ZN8SsObject14m_poObjectListE");
	if(pKeyInputBase == NULL){
		fputs("Error dlsym ObjectList\n", fp);
		fflush(fp);
		fclose(fp);
		return 1;
	}

	dlclose(handle);

	// Get instance
	unsigned* pInstance = (unsigned*) (pKeyInputBase[2]);
	pKeyInputBase = (unsigned*) (*pInstance);
	if (pKeyInputBase[6] != (unsigned) KeyInputBase_SendKey) {
		fputs("Error pInstance\n", fp);
		fflush(fp);
		fclose(fp);
		return 1;
	}

	fprintf(fp, "pInstance: %p\n", pInstance);
	fprintf(fp, "SendKey: %p\n", KeyInputBase_SendKey);
	fflush(fp);



	/**********************************
	Get samote functions from samote.so
	**********************************/

	// Absolute path of samote.so
	char filename[128];
	strcpy(filename, path);
	strcat(filename, "samote.so");

	fprintf(fp, "Path: %s\n", filename);
	fflush(fp);

	// Load samote.so
	handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
	if (handle == NULL){
		fputs("Error cannot open samote.so!\n", fp);
		fputs (dlerror(), fp);
		fputs("\n", fp);
		fflush(fp);
		fclose(fp);
		return 1;
	}

	// Get function samote_main
	unsigned* samote_main = dlsym(handle, "samote_main");
	if (samote_main == NULL) {
		fputs("Error samote_main not found!\n", fp);
		fflush(fp);
		fclose(fp);
		return 1;
	}
	
	dlclose(handle);



	/**********************
	Launch samote as thread
	**********************/

	// Set arguments to pass to samote_main
	struct ri_args* argsp = (struct ri_args*) malloc(sizeof(struct ri_args));
	argsp->pInstance = pInstance;
	argsp->SendKey = (void*) KeyInputBase_SendKey;
	argsp->path = (char*) path;

	
	// Runs samote_main routine as separate thread
	pthread_t new_thread;
	pthread_create(&new_thread, NULL, (void*) samote_main, (void*) argsp);
		
	fputs("Thread samote_main launched!\n", fp);;
	fflush(fp);

	fclose(fp);

	return 0;
}



