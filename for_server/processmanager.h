#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <vector>
#include <iostream>
#include "process.h"
#include "client.h"
#include "character.h"
#include <string>

using namespace Werewolf;

class ProcessManager{
	std::vector<Process*> _process;
	std::vector<Client>* _client;
	int ptr;
public:
	ProcessManager(std::vector<Client>* client) : _client(client){}
	~ProcessManager(){
		_process.clear();
	}
	int calibration();
	void add(Character* cha, Client* cli);//��VIP��ɫ��ӵ���Ӧ��process
	void constructlist();//ȷ����Ϸ����
	void Init(Client *cli);//��ʼ���û�����������
	void Init();//��ʼ���û���
	void shutdown();//��Ϸ��ֹ����
	void run();//���к���
	void gameover();//������Ϸ������������Ϸ���
	
};


#endif