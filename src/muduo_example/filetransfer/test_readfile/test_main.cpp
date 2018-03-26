/******************************************************************************
 * Filename : test_main.cpp
 * Copyright: 
 * Created  : 2018-2-12 by gongzenghao
 * Description: 文件读写测试
 ******************************************************************************/
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
//#include "Thread.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

void test0()
{
	//use FILE
	FILE *ifile = fopen("infile", "r");
	FILE *ofile = fopen("outfile", "w");
	
    fseek(ifile,0L,SEEK_END);  
    cout << "ifile size = " << ftell(ifile) << endl;
    fseek(ifile,0L,SEEK_SET);  
	while(1) 
	{
		std::vector<char> buff(1024);
		
		int counts = fread(&buff[0], 1, 1024, ifile);
		if(counts == 1024)
		{
			//cout << "fread ok" << endl;
			int writeLen = fwrite(&buff[0], 1, counts ,ofile);
			if(writeLen < 1024)
			{
				cout << "111writeLen == " << writeLen << endl;
			}
		}
		else
		{
			if(feof(ifile))
			{
				cerr << "eof, counts = " << counts << endl;
				int writeLen = fwrite(&buff[0], 1, counts,ofile);
				if(writeLen < 1024)
				{
					cout << "222writeLen == " << writeLen << endl;
				}
			}
			else
			{
				cerr << "error, counts = " << counts << endl;
			}
			break;
		}
	}
	
    fseek(ofile,0L,SEEK_END);  
    cout << "ofile size = " << ftell(ofile) << endl;
	fclose(ifile);
	fclose(ofile);
}

//多线程同时读文件


/*void test1()
{
	//use FILE
	Thread th1();
	
	FILE *ifile = fopen("infile", "r");
	FILE *ofile = fopen("outfile", "w");
	assert(ifile);
	
    fseek(ifile,0L,SEEK_END);  
    int size = ftell(ifile);  
    cout << "file size = " << size << endl;
    
	std::string txt(size,'\0');
	
	int readLen = fread(&txt[0], 1, size, ifile);
	cout << "readLen == " << readLen << endl;
	int writeLen = fwrite(&txt[0], 1, size, ofile);
	cout << "writeLen == " << writeLen << endl;

	fclose(ifile);
	fclose(ofile);
}
*/
int main()
{
	test0();
	//test1();
}
