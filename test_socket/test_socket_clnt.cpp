#include "message.h"
#include <iostream>
#include <vector>

int main()
{
  int por;
  std::cout << "which port do you want to use" << std::endl;
  std::cin >> por;
  Werewolf::Socket br_sock(AF_INET, SOCK_DGRAM, 0, "0.0.0.0", por);
  std::string str;
  while(1)
    {
      bool have_rec = false;
      for (int i = 0; i < 10; i++)
        {
          str =  br_sock.recv_broadcast(0.1);
          std::cout << str << std::endl;
          if (str != "")
            {
              have_rec = true;
              break;
            }
        }
      if (have_rec && str.substr(0,4) == "CHAT") break;
      else {
        std::cout << "continue searching?(Y or n)\n";
        char option;
        std::cin >> option;
        if(option == 'n') break;
      }
    }
  str = str.substr(4);
  Werewolf::Socket cli_sock(AF_INET, SOCK_STREAM, 0, str, por);
  cli_sock.connect();
  std::cout << "Connected!!\n";
  if(cli_sock.recv() == "Let's chat\n")
    {
      std::cout << "Input your nickname:\n";
      std::string nickname;
      std::cin >> nickname;
      nickname += ": ";
      while(1)
        {
          std :: string rev;
          rev = cli_sock.recv();
             if (rev == "Your turn to speak\n")
            {
              std::string s;
              std::cin.clear();
              std::cout << "Please speak\n";
              std::getline(std::cin, s);
              cli_sock.send(nickname + s);
            }
          else std::cout << rev << std::endl;
        }
    }
  return 0;
}
