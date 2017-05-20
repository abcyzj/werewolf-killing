#include"client_test.h"
#include <iostream>
using namespace Werewolf;

//new

void Client::set_charac(Character *ch){
    _charac = ch;
}


Character* Client::selfCharacter(){
    return _charac;
}

void Client::changename(std::string nickname){
    _nickname = nickname;
}

std::string Client::getnickname(){
    return _nickname;
}


Client :: Client(int rank) : _rank(rank), _available(true) {}

bool Client::available(){
    return _available;
}

void Client :: turn_on_input()
{
    std::cin >> ans;
}

std::string Client :: recv()
{
    std::string tep = ans;
    ans.clear();
    return tep;
}

void Client :: print(std::string s)
{
    std::cout << "Client " << _rank << " ";
    std::cout << s << std::endl;
}

void Client :: hold_on_input()
{
    std::cin >> ans;
}

void Client :: turn_off_input()
{
    
}
