#include"for_client.h"
#include<iostream>
#include<sstream>

int main(){
  std::cout << "Input the port you want to use:\n";
  int port;
  std::cin >> port;
  Werewolf::Connect con(port);
  con.recv();
  std::cout << "Input q to quit, c to connect, or i to connect through IP address.\n";
  Werewolf::Socket order_sock;
  while(!order_sock.valid()){
    con.show(std::chrono::milliseconds(1000));
    char op;
    std::cin >> op;
    switch(op){
    case 'q':
      return 0;
      break;
    case 'c':
      int k;
      con.end_show();
      std::cout << "Which server do you want to connect?\n";
      std::cin >> k;
      order_sock = con.connect(k);
      if(!order_sock.valid())
        std::cout << "Fail to connect.\n";
      break;
    case 'i':
      std::string IP;
      con.end_show();
      std::cout << "Input IP address.\n";
      std::cin >> IP;
      order_sock = con.connect_via_IP(IP);
      if(!order_sock.valid())
        std::cout << "Fail to connect.\n";
      break;
    }
  }
  con.end_show();
  con.end_recv();

  Werewolf::Messenger messenger(std::move(order_sock));
  messenger.start_thread();

  return 0;
}
