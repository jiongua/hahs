/******************************************************************************
 * Filename : transferServer.cpp
 * Copyright: 
 * Created  : 2018-2-11by gongzenghao
 * Description: 
 ******************************************************************************/
#include "transferServer.h"
#include <functional>
#include <cstdio>

//run in main_loop thread
void transferServer::onConnect(connPtr)
{
	//start send file
	fileTransTask(connPtr, filename_);
	connPtr->activeClose();
}

void transferServer::fileTransTask(connPtr conn, const std::string& fname)
{
	FILE *pf = fopen(fname.c_str(), "r");
	assert(pf);
    fseek(pf,0L,SEEK_END);  
    int size = ftell(pf);  
    cout << "file size = " << size << endl;
    
	std::string txt(size);
	int readLen = fread(&txt[0], 1, size, pf);
	cout << "readLen == " << readLen;
	conn->send(txt);
}

//run in sub_loop thread

