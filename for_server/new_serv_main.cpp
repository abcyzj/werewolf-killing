#include "accept.h"
#include "client.h"
#include "broadcast.h"
#include <iostream>
#include <string>
#include <vector>
#include "processmanager.h"
#include "characterfac.h"

int main()
{
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2, 2), &wsaData);
#endif
	int port;
	std::cout << "Input the port you want to use:\n";
	std::cin >> port;
	Werewolf::Broadcast broadcast(port);
	std::cout << "Input the roomname you want to use.\n";
	std::string roomname;
	std::cin >> roomname;
	std::cout << "Start broadcasting\n";
	broadcast.start("ROOMNAME:" + roomname + ";IP:" + Werewolf::Socket::get_host_addr() + ";");

	std::vector<Werewolf::Client> cl_vec;
	Werewolf::Accept ac(port, &cl_vec);
	ac.start();
	std::cout << "Now waiting for clients to connect.\nYou can enter s to stop accepting.\n";
	ac.show(std::chrono::milliseconds(1000));
	char op;
	while(std::cin >> op){
		if(op == 's'){
			ac.end_show();
			ac.end();
			break;
		}
	}
	Werewolf::ProcessManager test(&cl_vec);
	Werewolf::Characterfac fac(&cl_vec, &test);
	fac.set();
	test.run();
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}

		 
