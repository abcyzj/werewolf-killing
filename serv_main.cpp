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

  std::cout << "Input o to turn on input, f to turn off input, p to print, and q to quit.\n";

  std::string msg;
  while(std::cin >> op){
    switch(op){
    case 'o':
      for(auto &i: cl_vec)
        i.turn_on_input();
      break;
    case 'f':
      for(auto &i: cl_vec)
        i.turn_off_input();
      break;
    case 'p':
      std::cout << "Input the message you want to print.\n";
      std::getline(std::cin, msg);
      for(auto &i: cl_vec)
        i.print(msg);
      break;
    case 'q':
      for(auto &i: cl_vec)
        i.shut_down();
      return 0;
      break;
    }
  }

  return 0;
}
