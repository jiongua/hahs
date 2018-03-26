#include <iostream>
#include "Buffer.h"

using namespace std;

void printBytesCnt(Buffer *pbuf)
{
	cout << "readable = " << pbuf->readableBytes() << ", writeableBytes = " << pbuf->writeableBytes() << endl; 
}

int main()
{
	{
		Buffer buf(32);

		buf.putInt8('a');
		
		buf.putInt16(1989);
		
		buf.putInt32(111);
		
		buf.putInt64(19890111);
		
		buf.putString("hello world");
		buf.putString("gongzenghao");
		string str(100,'z');
		buf.putString(str);
		
		printBytesCnt(&buf);
		buf.putString("hello world");
		printBytesCnt(&buf);
		int8_t ch = buf.getAsInt8();
		cout << "out: " << ch+0<<endl;
		
		printBytesCnt(&buf);
		int16_t s16 = buf.getAsInt16();
		cout << "out: " << s16<<endl;
		
		printBytesCnt(&buf);
		int32_t s32 = buf.getAsInt32();
		cout << "out: " << s32 <<endl;
		
		printBytesCnt(&buf);
		int64_t s64 =  buf.getAsInt64();
		cout << "out: " << s64 <<endl;
		
		printBytesCnt(&buf);
		cout << "all string = " << buf.getAllAsString() << endl;
		printBytesCnt(&buf);
	}
}