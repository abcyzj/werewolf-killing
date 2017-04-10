#include<iostream>
#include"message.h"
#include<vector>
#include<sstream>
#include<thread>
#include<chrono>

std::string get_string(int n){
  std::stringstream stm;
  stm << n;
  return stm.str();
}
void vector_send(std::vector<Werewolf::Socket> &V, std::string msg){
  for(std::vector<Werewolf::Socket>::iterator i = V.begin(); i != V.end(); i++){
    i->send(msg);
  }
}

int main(){
  int por;
  std::vector<Werewolf::Socket> sock_vector;
  std::cout << "Input the port you want to use:\n";
  std::cin >> por;
  Werewolf::Socket br_sock(AF_INET, SOCK_DGRAM, IPPROTO_UDP, "255.255.255.255", por);
  std::cout << "Start broadcasting\n";
  std::string br_msg("CHAT");
  br_msg += br_sock.get_host_addr();
  Werewolf::Socket ls_sock(AF_INET, SOCK_STREAM, IPPROTO_TCP, "0.0.0.0", por);
  ls_sock.bind();
  ls_sock.listen();
  while(1){
    for(int i = 0; i < 10; i++){
      br_sock.broadcast(br_msg);
    }
    while(1){
      Werewolf::Socket new_cl = ls_sock.accept(1);
      if(new_cl.valid()){
        new_cl.send("Let's chat\n");
        sock_vector.push_back(std::move(new_cl));
      }
      else
        break;
    }

    std::cout << "Connected:" << sock_vector.size() << std::endl
              << "Continue?(y or n)\n";
    char op;
    std::cin >> op;
    if(op == 'n' && sock_vector.size() > 0)
      break;
    else if(op == 'n' && sock_vector.size() == 0)
      return 0;
  }

  while(1){
    std::string msg;
    for(auto &c : sock_vector){
      c.send("Your turn to speak\n");
      msg = c.recv();
       if(msg != ""){
        vector_send(sock_vector, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
  }
  return 0;
}
