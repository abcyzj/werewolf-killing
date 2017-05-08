#ifndef PROCESS_H
#define PROCESS_H

#include "character.h"
#include "process.h"
#include "client.h"
#include <vector>
#include <stdlib.h>

namespace Werewolf
{
    enum Act{BITE=0,POISON=1,SAVE=2,GUARDING=3,SHOOT=4,PREDICT=5,VOTE=6};
	enum Cha{ALL,WOLF,WITCH,GUARD,HUNTER,SEER};//请设置为和Character类编号一致
    struct logging
    {
        Cha _doer;
        Act _act;
        int _geter;
    };
    class Process
    {
    protected:
        bool _valid;
        Process* _next;
        std::vector<Client*> _rel_cli;
        std::vector<Client>* allclient;
        virtual bool func() = 0;    //���̵���Ҫִ�к���
        static std::vector<logging> _log;
    public:
		int con;
		Process(std::vector<Client>*) : _valid(false){};//allclient
        virtual bool begin() ;//true������false�˳�
        virtual Process* next();
        virtual void add_client(Client*);
        virtual void set_next(Process*);
        virtual void activate();
        virtual bool valid(); //�жϸ�������������Ƿ���Ҫ���У���Ҫ�����func����
        static void deletelog();
        static std::vector<logging>* readlog();
        static void writelog(Cha,Act,int);
		static int have_police;
    };
    
    class Guarding : public Process    //����
    {
    protected:
        bool func();
    public:
        Guarding(std::vector<Client> *cli) : Process(cli){};
		
		
    };
    
    class Killing : public Process //����ɱ��
    {
    protected:
        bool func();
    public:
		Killing(std::vector<Client> *cli) : Process(cli){};
    };
    
    class Witching : public Process//Ů��
    {
    protected:
        bool func();
    public:
        Witching(std::vector<Client> *cli) : Process(cli){}
		
    };
    
    class Predicting : public Process   //Ԥ�Լ�
    {
    protected:
        bool func();
    public:
        Predicting(std::vector<Client> *cli) : Process(cli){}
    };

	class Calculating : public Process{
protected:
	Process* _hun;
	Process* _po;
	std::vector<logging>* _log;
	int _calibra;//判断屠边还是屠城的标准，1为屠边，2为屠城
	bool calculatewolf();
	bool calculatepeo();
	bool calculategod();
	bool is_guarded(Character* _cha);
	bool is_saved(Character* _cha);
	bool is_poisoned(Character* _cha);
	bool func();
public:
	Calculating(std::vector<Client> *cli, Process* hun, int calibra, Process* po) : Process(cli), _hun(hun), _calibra(calibra), _po(po){
		_log = readlog();
	}
	~Calculating(){
		delete _hun;
	}
};

	class Chat:public Process{
		protected:
			bool func();
		public:
			Chat(std::vector<Client> *cli) : Process(cli){}
	};

    class Po_electing : public Process //ѡ�پ���
    {
    protected:
        bool func();
    public:
        Po_electing(std::vector<Client> *cli) : Process(cli){}
		
    };
    class Hunting;
    class Voting : public Process //ͶƱ
    {
    protected:
        bool func();
		Process* ht; 
		bool is_end();
    public:
        Voting(std::vector<Client> *cli) : Process(cli){}
		
    };
    
    class Hunting : public Process
    {
    protected:
		friend class Voting;
		friend class Calculating;
		bool func();
	public:
        Hunting(std::vector<Client> *cli) : Process(cli){}
		};
		
    };
	class Po_passing:public Process{//移交警徽
		protected:
			bool func();
		public:
			Po_passing();
	};





#endif
