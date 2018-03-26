#include "transferServer.h"

int main(int  argc, char *argv[])
{
	//"./trans -s port filename"
	//"./trans -c ip:port"
	if(argc < 3)
	{
		return;
	}
	EventLoop mainLoop;
	InetAddr addr(atoi(argv[1]));
	transferServer server(&addr, &mainLoop);
	server.start();
	mainLoop.loop();
}
