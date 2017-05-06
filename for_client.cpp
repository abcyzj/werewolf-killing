#include"for_client.h"
#include<algorithm>
#include<iostream>

using namespace Werewolf;

Connect::Connect(): _recv_over(false), _show_over(false){}

Connect::Connect(int port): _por(port), _recv_over(false), _show_over(false), _recv_sock(AF_INET, SOCK_DGRAM, 0, "0.0.0.0", port){}

Connect::~Connect(){
  if(_recv_th.joinable())
    end_recv();
  if(_show_th.joinable())
    end_show();
}

void Connect::set_port(int port){
  _por = port;
  _recv_sock = Socket(AF_INET, SOCK_DGRAM, 0, "255.255.255.255", port);
}

void Connect::do_recv(){
  while(!_recv_over){
    for(int i = 0; i < 10; i++){
      std::string msg = _recv_sock.recv_broadcast(0.001);
      if(msg != ""){
        std::unique_lock<std::mutex> lck(_info_mtx);
        if(std::find(_info.begin(), _info.end(), msg) == _info.end())
          _info.push_back(msg);
        lck.unlock();
      }
    }
  }
}

void Connect::do_show(std::chrono::milliseconds ms){
  while(!_show_over){
    std::unique_lock<std::mutex> lck(_info_mtx);
    std::cout << "Found " << _info.size() << " server(s).\n";
    for(auto &i: _info){
      std::cout << i << std::endl;
    }
    lck.unlock();
    std::this_thread::sleep_for(ms);
  }
}

void Connect::recv(){
  if(_recv_th.joinable())
    return;

  _recv_over = false;
  _recv_th = std::thread(&Connect::do_recv, this);
}

void Connect::end_recv(){
  if(!_recv_th.joinable())
    return;

  _recv_over = true;
  _recv_th.join();
  _recv_over = false;
}

void Connect::show(std::chrono::milliseconds ms){
  if(_show_th.joinable())
    return;

  _show_over = false;
  _show_th = std::thread(&Connect::do_show, this, ms);
}

void Connect::end_show(){
  if(!_show_th.joinable())
    return;

  _show_over = true;
  _show_th.join();
  _show_over = false;
}

Socket Connect::connect(int k){
  std::unique_lock<std::mutex> lck(_info_mtx);
  if(k >= _info.size()){
    std::cout << "There's no " << k << "th server.\n";
    return Socket();
  }

  Socket newsock(AF_INET, SOCK_STREAM, 0, get_IP(_info[k]), _por);
  int res = newsock.connect(5);
  if(res >= 0)
    return std::move(newsock);
  else
    return Socket();
}


Socket Connect::connect_via_IP(const std::string &IP){
  Socket newsock(AF_INET, SOCK_STREAM, 0, IP, _por);
  int res = newsock.connect(5);
  if(res > 0)
    return std::move(newsock);
  else
    return Socket();
}

std::string Connect::get_IP(std::string msg){
  int a = msg.find("IP:");
  int b = msg.find(';', a);
  a += 3;
  b -= 1;
  return msg.substr(a, b - a + 1);
}

InputManager::InputManager(Messenger *parent): _parent_messenger(parent){}

InputManager::~InputManager(){
  //end_thread();
}

void InputManager::do_input(){
  // std::string msg;
  // std::cin.sync();
  // std::getline(std::cin, msg);
  // if(msg == "")
  //   continue;
  // else if(_is_on){
  //   _parent_messenger->send(msg);
  // }
  // else{
  //   std::cout << "You cannot input now!\n";
  // }
  std::string msg;
  std::cin.ignore(1024, '\n');
  while(msg == "")
    std::getline(std::cin, msg);
  _parent_messenger->send(msg);
}

void InputManager::turn_on(){
  if(_th.joinable())
    return;
  _th = std::thread(&InputManager::do_input, this);
}

// void InputManager::turn_off(){
//   _is_on = false;
// }

// void InputManager::start_thread(){
//   _input_over = false;
//   _th = std::thread(&InputManager::do_input, this);
// }

void InputManager::end_thread(){
  std::cout << "Game over.\nIf the program doesn't exit, please input q to quit.\n";
  if(_th.joinable())
    _th.join();
}

ClientExecutor::ClientExecutor(Messenger *parent): _parent_messenger(parent), inputmanager(parent){
  //inputmanager.start_thread();
}

ClientExecutor::~ClientExecutor(){
  //inputmanager.end_thread();
}

void ClientExecutor::add_order(std::string info){
  _order += info;
  unsigned long a = info.find(';');
  if(a != std::string::npos){
    execute(_order.substr(0, a));
    _order.erase(0, a + 1);
  }
}

void ClientExecutor::execute(std::string order){
  if(order.compare(0, 6, "PRINT:") == 0){
    std::cout << order.substr(6) << std::endl;
  }

  else if(order.compare(0, 7, "TURN_ON") == 0){
    inputmanager.turn_on();
  }

  // else if(order.compare(0, 8, "TURN_OFF") == 0){
  //   inputmanager.turn_off();
  //}

  else if(order.compare(0, 9, "SHUT_DOWN") == 0){
    inputmanager.end_thread();
    _parent_messenger->end_thread();
    std::cout << "Goodbye!" << std::endl;
  }
}

Messenger::Messenger(Socket &&sock): _sock(std::move(sock)), executor(this){}

Messenger::~Messenger(){
  end_thread();
}

void Messenger::do_commu(){
  while(!_thread_over){
    if(_info_mtx.try_lock() && _info != ""){//若有需要发送的信息
      _sock.send(_info);
      _info.clear();
      _info_mtx.unlock();
    }
    else
      _info_mtx.unlock();

    std::string order = _sock.recv(0.02);//尝试接受
    if(order != ""){
      executor.add_order(order);
    }
  }
}

void Messenger::start_thread(){
  _thread_over = false;
  //_th = std::thread(&Messenger::do_commu, this);
  do_commu();
}

void Messenger::end_thread(){
  //if(!_th.joinable())
  //return;

  _thread_over = true;
  //_th.join();
}

void Messenger::send(std::string msg){
  _info_mtx.lock();
  _info += msg;
  _info_mtx.unlock();
}
