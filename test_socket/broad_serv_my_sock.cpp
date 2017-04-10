#include<iostream>
#include"message.h"

int main(){
  Werewolf::Socket sock(AF_INET, SOCK_DGRAM, IPPROTO_UDP, "0.0.0.0", 2732);
  while(1){
    std::cout << "Input the message you want to send:\n";
    std::string str;
    std::getline(std::cin, str);
    sock.broadcast(str);
  }
  return 0;
}
