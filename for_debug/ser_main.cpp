#include "client_test.h"
#include <iostream>
#include <string>
#include <vector>
#include "processmanager.h"
#include "character.h"

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
#endif
    std::cout << "Input the roomname you want to use:\n";
    std::string roomname;
    std::cin >> roomname;
    
    std::vector<Werewolf::Client> cl_vec;
    for (int i = 1; i <= 5; i++)
    {
        Werewolf :: Client tep = Werewolf :: Client(i);
        cl_vec.push_back(tep);
    }
    Werewolf::ProcessManager test(&cl_vec);
    Werewolf::Characterfac fac(&cl_vec, &test);
    fac.set_client();
    test.run();
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
