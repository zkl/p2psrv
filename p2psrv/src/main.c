#include <stdio.h>
#include "server.h"
#include "log.h"

int main(int argc, char * argv[])
{
	pserver_t server;
	pserver_init(&server, 10, 1245);
	pserver_exec(&server);
	return 0;
}
