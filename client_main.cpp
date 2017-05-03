#include"for_client.h"
#include<iostream>
#include<sstream>

int main(){
  std::cout << "Input the port you want to use:\n";
  int port;
  std::cin >> port;
  Werewolf::Connect conec(port);
  conec.recv();
  std::cout << "Input q to quit, c to connect, or i to connect through IP address.\n";
  Werewolf::Socket order_sock;
  while(!order_sock.valid()){
    conec.show(std::chrono::milliseconds(1000));
    char op;
    std::cin >> op;
    bool wanna_quit = false;
    switch(op){
    case 'q':
      wanna_quit = true;
      break;
    case 'c':
      int k;
      conec.end_show();
      std::cout << "Which server do you want to connect?\n";
      std::cin >> k;
      order_sock = conec.connect(k);
      if(!order_sock.valid())
        std::cout << "Fail to connect.\n";
      break;
    case 'i':
      std::string IP;
      conec.end_show();
      std::cout << "Input IP address.\n";
      std::cin >> IP;
      order_sock = conec.connect_via_IP(IP);
      if(!order_sock.valid())
        std::cout << "Fail to connect.\n";
      break;
    }

    if(wanna_quit)
      break;
  }
  conec.end_show();
  conec.end_recv();
    return 0;
}
