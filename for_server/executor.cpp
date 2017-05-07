#include"executor.h"
#include<string>

using namespace Werewolf;

Executor::Executor(){}

Executor::~Executor(){}

void Executor::add_order(std::string info){
  _order += info;
  unsigned long a = info.find(';');
  if(a != std::string::npos){
    execute(_order.substr(0, a));
    _order.erase(0, a + 1);
  }
}
