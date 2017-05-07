#ifndef EXECUTOR_H
#define EXECUTOR_H

#include<string>

namespace Werewolf{
  class Executor{
  private:
    std::string _order;
    virtual void execute(std::string) = 0;
  public:
    Executor();
    virtual ~Executor();
    void add_order(std::string);//向命令队列中添加信息
  };
}

#endif
