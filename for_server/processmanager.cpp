#include "processmanager.h"

using namespace Werewolf;

int ProcessManager :: calibration(){//ÈÄâÊã©Â±†ËæπËøòÊòØÂ±†Âüé
    (*_client)[0].print("please choose judge method : 1 for kill one side 2 for kill all");
    (*_client)[0].turn_on_input();
    if((*_client)[0].recv() == "1"){
        return (int) 1;
    }
    else if((*_client)[0].recv() == "2"){
        return (int) 2;
    }
    (*_client)[0].turn_off_input();
    return 1;
}
<<<<<<< HEAD
void ProcessManager::constructlist(){//Âü∫Á±ªÁöÑÊûÑÈÄ†ÂáΩÊï∞!!!
    int msg = calibration();
    Process* pro = new Hunting(_client);
    Process* Pro = new Po_passing(_client);
    _process.clear();
    Process* pro0 = new Guarding(_client);
    _process.push_back(pro0);
    Process* pro1 = new Killing(_client);
    _process.push_back(pro1);
    Process* pro2 = new Witching(_client);
    _process.push_back(pro2);
    Process* pro3 = new Predicting(_client);
    _process.push_back(pro3);
    Process* pro4 = new Calculating(_client, pro, msg, Pro);
    _process.push_back(pro4);
    Process* pro5 = new Po_electing(_client);
    _process.push_back(pro5);
    Process* pro6 = new Voting(_client, pro, Pro);
    _process.push_back(pro6);
    //‰ª•‰∏äÊòØÂèÇÂä†ÊµÅÁ®ãÁöÑÁ±ª
    _process.push_back(pro);
    Process* _pro = new Chat(_client);
    _process.push_back(_pro);
    _process.push_back(Pro);
    
    //‰ª•‰∏ãÊòØ‰∏≤ÊàêÈìæË°®ÁöÑËøáÁ®ã
    for(int i = 0; i < 7; i++){
        if(i != 6)
            _process[i] -> set_next(_process[i + 1]);
        else
            _process[i] -> set_next(_process[0]);
    }
=======
void ProcessManager::constructlist(){//基类的构造函�?!!
	int msg = calibration();
	Process* pro = new Hunting(_client);
	Process* Pro = new Po_passing(_client);
	_process.clear();
	Process* pro0 = new Guarding(_client);
	_process.push_back(pro0);
	Process* pro1 = new Killing(_client);
	_process.push_back(pro1);
	Process* pro2 = new Witching(_client);
	_process.push_back(pro2);
	Process* pro3 = new Predicting(_client);
	_process.push_back(pro3);
	Process* pro4 = new Calculating(_client, pro, msg, Pro);
	_process.push_back(pro4);
	Process* pro5 = new Po_electing(_client);
	_process.push_back(pro5);
	Process* pro6 = new Voting(_client, pro, Pro);
	_process.push_back(pro6);
	//以上是参加流程的�?
	_process.push_back(pro);
	Process* _pro = new Chat(_client);
	_process.push_back(_pro);
	_process.push_back(Pro);

	//以下是串成链表的过程
	for(int i = 0; i < 7; i++){
		if(i != 6)
			_process[i] -> set_next(_process[i + 1]);
		else 
			_process[i] -> set_next(_process[0]);
	}
>>>>>>> d1dc548a06dcf15324b28c11ddcb9db62bcb6862
}



void ProcessManager :: add(Character* cha, Client* cli){//ËÆæÁΩÆÂÖ≥ËÅîÊÄ
    switch(cha -> type()){
        case 1:
            _process[1] -> add_client(cli);
            break;
        case 2:
            break;
        case 3:
            _process[7] -> add_client(cli);
            break;
        case 4:
            _process[2] -> add_client(cli);
            break;
        case 5:
            _process[3] -> add_client(cli);
            break;
        case 6:
            _process[0] -> add_client(cli);
            break;
    }
}

void ProcessManager::Init(Client* cli){
    cli -> print("input your nickname");
    cli -> turn_on_input();
    std::string name = cli -> recv(5);
    cli -> turn_off_input();
    cli -> changename(name);
}

void ProcessManager::Init(){
    std::vector<Client>& client = *_client;
    int size = client.size();
    for(int i = 0;  i < size; i++){
        Init(&client[i]);
    }
}


<<<<<<< HEAD
void ProcessManager :: shutdown(){//ÈÄÄÂá∫Ê∏∏Êàè
    (*_client)[0].print("Press y to shut down the game");
    (*_client)[0].turn_on_input();
    if((*_client)[0].recv() == "y"){
        for(auto i = 0; i < (*_client).size(); i++){
            (*_client)[i].print("exited");
        }
        (*_client)[0].turn_off_input();
        exit(0);
    }
}

void ProcessManager::gameover(){//ÁªìÊùü‰πãÂêéÁöÑÊìç‰ΩúÔºåÂÖ¨Â∏ÉÊ∏∏ÊàèÁªìÊûúÔºåÂëäËØâÊâÄÊúâ‰∫∫Áõ∏Â∫îÁé©ÂÆ∂ÁöÑË∫´‰ªΩ
    for(auto i = 0; i < (*_client).size(); i++){
        (*_client)[i].print("game over!!!");
    }
    if(_process[ptr] -> con == 1){
        for(auto i = 0; i < (*_client).size(); i++){
            (*_client)[i].print("werewolf win");
        }
    }
    else{
        for(auto i = 0; i < (*_client).size(); i++){
            (*_client)[i].print("good man win");
        }
    }
    for(auto i = 0; i < (*_client).size(); i++){
        switch((*_client)[i].selfCharacter() -> type()){
            case 1:
                for(auto i = 0; i < (*_client).size(); i++){
                    (*_client)[i].print( (*_client)[i].getnickname());
                    (*_client)[i].print("is werewolf");
                }
                break;
            case 2:
                for(auto i = 0; i < (*_client).size(); i++){
                    (*_client)[i].print( (*_client)[i].getnickname());
                    (*_client)[i].print("is villager");
                }
                break;
            case 3:
                for(auto i = 0; i < (*_client).size(); i++){
                    (*_client)[i].print( (*_client)[i].getnickname());
                    (*_client)[i].print("is hunter");
                }
                break;
            case 4:
                for(auto i = 0; i < (*_client).size(); i++){
                    (*_client)[i].print( (*_client)[i].getnickname());
                    (*_client)[i].print("is witch");
                }
                break;
            case 5:
                for(auto i = 0; i < (*_client).size(); i++){
                    (*_client)[i].print( (*_client)[i].getnickname());
                    (*_client)[i].print("is prophet");
                }
                break;
            case 6:
                for(auto i = 0; i < (*_client).size(); i++){
                    (*_client)[i].print( (*_client)[i].getnickname());
                    (*_client)[i].print("is guard");
                }
                break;
        }
        shutdown();
    }
}
void ProcessManager :: run(){
    Init();//ÂàùÂßãÂåñÁî®Êà∑Âêç
    for(auto i = 0; i < (*_client).size(); i++){
        (*_client)[i].print("game started!!!");
    }
    _process[4] -> activate();
    _process[6] -> activate(); 
    Process* usepro;
    usepro = _process[0];
    for(int i = 0; i < 7; i++){
        int cnt = usepro -> get_size();
        if(usepro -> get_size() != 0){
            usepro -> activate();
        }
        usepro = usepro -> next();
    }
    while(1)
        for(int i = 0; i < 7; i++){
            if(i != 4 && i != 6){
                usepro -> begin();//‰∏çÁü•ÈÅìÊòØ‰∏çÊòØVCÁöÑÂéüÂõ†ÔºåËøô‰∏™ÊòØ‰∏çÊòØÊúâÈîôÂïäÔºÅ
                usepro = usepro -> next();
            }
            else{
                if(!usepro -> begin()){
                    ptr = i;
                    gameover();
                }
                else{
                    usepro = usepro -> next(); 
                }
=======
void ProcessManager :: shutdown(){//退出游�?
	(*_client)[0].print("Press y to shut down the game");
	(*_client)[0].turn_on_input();
	if((*_client)[0].recv() == "y"){
		for(auto i = 0; i < (*_client).size(); i++){
			(*_client)[i].print("exited");
		}
		(*_client)[0].turn_off_input();
		exit(0);
	}
}

void ProcessManager::gameover(){//结束之后的操作，公布游戏结果，告诉所有人相应玩家的身�?
		for(auto i = 0; i < (*_client).size(); i++){
			(*_client)[i].print("game over!!!");
		}
		if(_process[ptr] -> con == 1){
			for(auto i = 0; i < (*_client).size(); i++){
				(*_client)[i].print("werewolf win");
			}
		}
		else{
			for(auto i = 0; i < (*_client).size(); i++){
				(*_client)[i].print("good man win"); 
			}
		}
		for(auto i = 0; i < (*_client).size(); i++){
			switch((*_client)[i].selfCharacter() -> type()){
			case 1:
				for(auto i = 0; i < (*_client).size(); i++){
					(*_client)[i].print( (*_client)[i].getnickname());
					(*_client)[i].print("is werewolf");
					}
				break;
			case 2:
				for(auto i = 0; i < (*_client).size(); i++){
					(*_client)[i].print( (*_client)[i].getnickname());
					(*_client)[i].print("is villager");
					}
				break;
			case 3:
				for(auto i = 0; i < (*_client).size(); i++){
					(*_client)[i].print( (*_client)[i].getnickname());
					(*_client)[i].print("is hunter");
					}
				break;
			case 4:
				for(auto i = 0; i < (*_client).size(); i++){
					(*_client)[i].print( (*_client)[i].getnickname());
					(*_client)[i].print("is witch");
					}
				break;
			case 5:
				for(auto i = 0; i < (*_client).size(); i++){
					(*_client)[i].print( (*_client)[i].getnickname());
					(*_client)[i].print("is prophet");
					}
				break;
			case 6:
				for(auto i = 0; i < (*_client).size(); i++){
					(*_client)[i].print( (*_client)[i].getnickname());
					(*_client)[i].print("is guard");
					}
				break;
		}
			shutdown();
}
}
void ProcessManager :: run(){
	Init();//初始化用户名
	for(auto i = 0; i < (*_client).size(); i++){
		(*_client)[i].print("game started!!!");
	}
	_process[4] -> activate();
	_process[6] -> activate(); 
	Process* usepro;
	usepro = _process[0];
	for(int i = 0; i < 7; i++){
		int cnt = usepro -> get_size();
		if(usepro -> get_size() != 0){
			usepro -> activate();
		}
		usepro = usepro -> next();
	}
	while(1)
		for(int i = 0; i < 7; i++){
			if(i != 4 && i != 6){
				usepro -> begin();//不知道是不是VC的原因，这个是不是有错啊�?
				usepro = usepro -> next();
			}
			else{
				if(!usepro -> begin()){
					ptr = i;
					gameover();
				}
				else{
					usepro = usepro -> next(); 
				}
>>>>>>> d1dc548a06dcf15324b28c11ddcb9db62bcb6862
			}		

		}
	}

