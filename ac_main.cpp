#include"accept.h"
#include"client.h"
#include"broadcast.h"
#include<iostream>
#include<string>
#include<vector>

int main(){
  int port;
  std::cout << "Input the port you want to use:\n";
  std::cin >> port;
  Werewolf::Broadcast broadcast(port);
  std::cout << "Start broadcasting\n";
  broadcast.start(std::string("CHAT") + Werewolf::Socket::get_host_addr());

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
  return 0;
}
