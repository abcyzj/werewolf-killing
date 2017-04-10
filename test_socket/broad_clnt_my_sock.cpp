#include<iostream>
#include<string>
#include"message.h"

int main(){
  Werewolf::Socket sock(AF_INET, SOCK_DGRAM, IPPROTO_UDP, "0.0.0.0", 2732);

  while(1){
    std::cout << sock.recv_broadcast() << std::endl;
  }
  return 0;
}
