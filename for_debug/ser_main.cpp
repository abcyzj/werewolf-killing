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
    for (int i = 1; i <= 13; i++)
    {
        Werewolf :: Client tep = Werewolf :: Client(i);
        cl_vec.push_back(tep);
    }
    Werewolf::ProcessManager test(&cl_vec);
    //test.constructlist();
    /*Werewolf :: Character* cha1 = new Werewolf :: Seer;
     test.add(cha1, &cl_vec[0]);
     cl_vec[0].set_charac(cha1);
     Werewolf :: Character* cha2 = new Werewolf :: Witch;
     test.add(cha2, &cl_vec[1]);
     cl_vec[1].set_charac(cha2);
     Werewolf :: Character* cha3 = new Werewolf :: Wolf;
     test.add(cha3, &cl_vec[2]);
     cl_vec[2].set_charac(cha3);
     Werewolf :: Character* cha4 = new Werewolf :: Wolf;
     test.add(cha4, &cl_vec[3]);
     cl_vec[3].set_charac(cha4);
     Werewolf :: Character* cha5 = new Werewolf :: Villager;
     test.add(cha5, &cl_vec[4]);
     cl_vec[4].set_charac(cha5);*/
    
    Werewolf::Characterfac fac(&cl_vec, &test);
    fac.set_client();
    test.run();
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
