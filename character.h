#ifndef CHARACTER_H
#define CHARACTER_H

#include"client.h"

namespace Werewolf{
  class Character{
  private:
    Client *client;//对应的client类
    bool _is_guard = false;//是否被守护
    
  public:
    Character(Client*);
    virtual ~Character();

  };
}

#endif
