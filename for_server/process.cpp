#include "process.h"
#include <iostream>
using namespace Werewolf;
Process::Process(std::vector<Client>* _all):allclient(_all){
    std::cout<<"All clients have been loaded.\n";

}
bool Process :: begin()
{
    if(!valid())
        return true;
    else
        return func();
}

bool Process :: valid()
{
    return _valid;
}

void Process :: activate()
{
    _valid = 1;
}

Process* Process :: next()
{
    return _next;
}

void Process :: add_client(Client* cli)
{
    _rel_cli.push_back(cli);
}

void Process :: set_next(Process* pro)
{
    _next = pro;
}
void Process :: deletelog()
{
    _log.clear();
}
std::vector<logging>* Process::readlog(){
    return &_log;
}
void Process::writelog(Cha doer,Act act,int geter){
    _log.push_back(log{doer,act,geter});
}
bool Guarding::func(){
		(*allclient)[dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> last()].selfCharacter()->canc_guarded();
		if(!_rel_cli[0]->selfCharacter()->is_dead()){
		_rel_cli[0]->print("Please input the player number you want to guard:\n");
		_rel_cli[0]->turn_on_input();
		std::string res1 = _rel_cli[0]->recv();
		if ((*allclient)[res1[0]-'1'].selfCharacter() -> type() ==
			dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> last())
		{
			_rel_cli[0] -> print("You have guarded the same one as last night, please change\n");
			_rel_cli[0] -> print("Please input the player number you want to guard:\n");
			_rel_cli[0] -> turn_on_input();
			std :: string res2 = _rel_cli[0]->recv();
			(*allclient)[res2[0]-'1'].selfCharacter() -> guardfunc();
			dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> who_i_guard(res2[0]-'1');
			writelog(GUARD,GUARDING,res2[0]-'1');
		}
		else
		{
			(*allclient)[res1[0]-'1'].selfCharacter()->guardfunc();
			dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> who_i_guard(res1[0]-'1');
			writelog(GUARD,GUARDING,res1[0]-'1');
		}
	}
    return true;
}

bool Killing::func(){
    int cnt=0;
    int num=-1;
    bool flag=true;
	bool isalive[_rel_cli.size()];
	for(int i=0;i<_rel_cli.size();i++)
		isalive[i]=!_rel_cli[i]->selfCharacter()->is_dead();
    while(true){
        cnt++;
        for(int i=0;i<_rel_cli.size();i++)
			if(isalive[i]){
				_rel_cli[i]->turn_on_input();
				std::string words=_rel_cli[i]->recv();
				for(int j=0;j<_rel_cli.size();j++)
					if(isalive[j])
						_rel_cli[j]->print(words);
			}
        for(int i=0;i<_rel_cli.size();i++)
			if(isalive[i]){
				_rel_cli[i]->print("Please input the player number you want to kill.\nPlease reach a consensus!!!\n");
				_rel_cli[i]->turn_on_input();
				std::string tgt=_rel_cli[i]->recv();
				if(i==0){
					num=tgt[0]-'1';
				}
				else if(num!=tgt[0]-'1'){
					flag=false;
				}
			}
        if(flag){
            (*allclient)[num].selfCharacter()->set_dead();
			writelog(WOLF,BITE,num);
            break;
        }
        else{
            flag=true;
            num=-1;
            for(int i=0;i<_rel_cli.size();i++)
				if(isalive[i])
					_rel_cli[i]->print("You don't have the same target.\nYou have only "+std::to_string(5-cnt)+" chances\n");
        }
        if(cnt>=5){
            for(int i=0;i<_rel_cli.size();i++)
				if(isalive[i])
					_rel_cli[i]->print("No chance left.You didn't kill anyone.\n");
            break;
        }
    }
    return true;
}

bool Witching :: func() //女巫使用毒药或者解药// not done
{
    _rel_cli[0] -> print("Please choose to use poison or antidote, p or a\n");
    _rel_cli[0] -> turn_on_input();
    std::string drug = _rel_cli[0] -> recv();
    _rel_cli[0] -> print("Please Input the player you want to act on\n");
    _rel_cli[0] -> turn_on_input();
    std::string num = _rel_cli[0] -> recv();
    if (drug == "p")
        (*allclient)[num[0] - '1'].selfCharacter() -> set_dead();
    else
        (*allclient)[num[0] - '1'].selfCharacter() -> set_alive();
    return true;
}

bool Predicting :: func()    //预言家进行身份检测 not done
{
    _rel_cli[0] -> print("Please input the player number you want to see\n");
    _rel_cli[0] -> turn_on_input();
    std::string player = _rel_cli[0] -> recv();
    int t = (*allclient)[player[0] - '1'].selfCharacter() -> type();
    if (t == 1)
        _rel_cli[0] -> print("The identity of the player is werewolf\n");
    else
        _rel_cli[0] -> print("The identity of the player is goodman\n");
    return true;
}

bool Voting::func(){
	int n=allclient->size();
	bool isalive[n];//有谁在场
	double num[n];//记录每个人被投票数
	int voteinfo[n];//记录每个人投票情况
	std::vector<int> player;
	for(int i=0;i<n;i++){
		isalive[i]=!(*allclient)[i].selfCharacter()->is_dead();
		if(isalive[i])
			player.push_back(i);
		num[i]=0;
		voteinfo[i]=-1;
	}
	std::string voted="You can vote to Player";
	for(int i=0;i<player.size();i++){
		voted+=" "+std::to_string(i+1);
	}
	voted+=".\n";
	for(int i=0;i<n;i++)
		if(isalive[i]){
			(*allclient)[i].print("Please vote!\n"+voted);
			(*allclient)[i].turn_on_input();
			std::string tgt=(*allclient)[i].recv();
			while(!isalive[tgt[0]-'1']){
				(*allclient)[i].print("Please choose another Player!, he is dead\n");
				(*allclient)[i].turn_on_input();
				tgt=(*allclient)[i].recv();
			}
			voteinfo[i]=tgt[0]-'1';
			num[tgt[0]-'1']+=1.0;
		}
	double maxx=0;//票数的最大值
	std::vector<int> maxnum;//票数最大的若干名玩家的编号
	if(have_police>=0&&isalive[have_police])
		num[voteinfo[have_police]]+=0.5;
	for(int i=0;i<n;i++)
		if(num[i]>maxx)
			maxx=num[i];
	for(int i=0;i<n;i++)
		if(abs(num[i]-maxx)<1e-9)
			maxnum.push_back(i);
	std::string s="";//需要打印的投票信息
	for(int i=0;i<n;i++)
		if(isalive[i])
			s+="Player "+std::to_string(i)+" votes to Player "+std::to_string(voteinfo[i])+".\n";
	
	for(int i=0;i<n;i++)//公布投票信息
		if(isalive[i])
			(*allclient)[i].print(s);

	if(maxnum.size()==1){
		(*allclient)[maxnum[0]].selfCharacter()->set_dead();
		isalive[maxnum[0]]=0;
		writelog(ALL,VOTE,maxnum[0]);
		for(int i=0;i<n;i++)
			if(isalive[i])
				(*allclient)[i].print("Player "+std::to_string(maxnum[0])+" has been voted out!\n");
		(*allclient)[maxnum[0]].print("You have been voted out!\n");
		if(!is_end()){
			if((*allclient)[maxnum[0]].selfCharacter()->type()==hunter){
				(*allclient)[maxnum[0]].print("You can choose to shoot or not.(Y/N)\n");
				(*allclient)[maxnum[0]].turn_on_input();
				std::string choice=(*allclient)[maxnum[0]].recv();
				if(choice=="Y"){
					bool end=!ht->begin();//考虑猎人杀人后结算
					if(end)
						return false;
				}
			}
			for(int i=0;i<n;i++)
				isalive[i]=!(*allclient)[i].selfCharacter()->is_dead();
			for(int i=0;i<n;i++)
				if(isalive[i]||i==maxnum[0])
					(*allclient)[i].print("Last words begin.\n");
			(*allclient)[maxnum[0]].print("Please input the words you want to say before you quit the game.\n:q + Enter represents ending.\n");
			std::string lwords="";
			while(true){
				(*allclient)[maxnum[0]].turn_on_input();
				lwords=(*allclient)[maxnum[0]].recv();
				if(lwords!=":q"){
					for(int i=0;i<n;i++)
						if(isalive[i]){
							(*allclient)[i].print(lwords+"\n");
						}
				}
				else break;
			}
			for(int i=0;i<n;i++)
				if(isalive[i]||i==maxnum[0])
					(*allclient)[i].print("Last words end.\n");
			(*allclient)[maxnum[0]].print("You quit the game.\n");
			return true;
		}
		else
			return false;
	}
	else{
		std::string tie="Player";
		for(int i=0;i<maxnum.size();i++)
			tie+=" "+std::to_string(maxnum[i]);
		tie+=" have the same number of votes.\nRound 2 Chat begins.\nAt the following order:\n\tPlayer";
		int r1=rand()%maxnum.size();
 		int r2=rand()%2;
		int order=0;
		if(r2==0)
			order=1;
		else
			order=-1;
		std::vector<int> chod;
		for(int i=0;i<maxnum.size();i++)
			chod.push_back((r1+i*order+maxnum.size())%maxnum.size());
		for(int i=0;i<maxnum.size();i++)
			tie+=" "+std::to_string(chod[i]);
		for(int i=0;i<n;i++)
			(*allclient)[i].print(tie);
		for(int i=0;i<chod.size();i++){
			(*allclient)[chod[i]].print("Your turn:\n");
			(*allclient)[chod[i]].turn_on_input();
			std::string 
		}
	}
}

bool Calculating::is_guarded(Character* _cha){
	int type = _cha -> type();
	if((*_log)[0]._geter == type){
		return true;
	}
	return false;
}

bool Calculating::is_saved(Character* _cha){
	int type = _cha -> type();
	if((*_log)[1]._geter == type && (*_log)[1]._act == 2){
		return true;
	}
	return false;
}

bool Calculating::is_poisoned(Character* _cha){
	int type = _cha -> type();
	if((*_log)[1]._geter == type && (*_log)[1]._act == 1){
		return true;
	}
	return false;
}

/*void Calculating::willing(Client* cli){//遗言函数
	cli -> print("you are dead, please leave your will");
					cli -> turn_on_input();
					_s = cli -> recv();
					cli -> turn_off_input();
					s.push_back(_s);
}*/



bool Calculating::calculatewolf(){//分警徽和飞警徽没有实现
	int sum = 0;
	int num = 0;
	std::vector<Client>& m = *allclient;
	for(auto i = 0; i < m.size(); i++){
			if(m[i].selfCharacter() -> type() == 1){
			sum++;
			if(m[i].selfCharacter() -> is_dead() == true){
				if(is_guarded(m[i].selfCharacter()) && is_saved(m[i].selfCharacter())){
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}

					//willing(&m[i]);
				}
				else if(is_guarded(m[i].selfCharacter())){
					m[i].selfCharacter() -> set_alive();//被守卫的人知道自己被守卫么？
				}
				else if(is_saved(m[i].selfCharacter())){
					m[i].selfCharacter() -> set_alive();
				}
				else{
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
			}
			else{
				if(is_poisoned(m[i].selfCharacter())){
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
			}
		}
	if(num == sum){
		return true;
	}
	return false;
}
}

bool Calculating::calculatepeo(){
	int sum = 0;
	int num = 0;
	std::vector<Client>& m = *allclient;
	for(auto i = 0; i < m.size(); i++){
		for(auto i = 0; i < m.size(); i++){
		if(m[i].selfCharacter() -> type() == 2){
			sum++;
			if(m[i].selfCharacter() -> is_dead() == true){
				if(is_guarded(m[i].selfCharacter()) && is_saved(m[i].selfCharacter())){
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
				else if(is_guarded(m[i].selfCharacter())){
					m[i].selfCharacter() -> set_alive();//被守卫的人知道自己被守卫么？
				}
				else if(is_saved(m[i].selfCharacter())){
					m[i].selfCharacter() -> set_alive();
				}
				else{
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
			}
			else{
				if(is_poisoned(m[i].selfCharacter())){
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
			}
		}
	}
	if(num == sum){
	return true;
	}
	return false;
}
}

bool Calculating::calculategod(){
	int sum = 0;
	int num = 0;
	std::vector<Client>& m = *allclient;
	for(auto i = 0; i < m.size(); i++){
		if((m[i].selfCharacter() -> type() != 1 )&&(m[i].selfCharacter() -> type() != 2) && (m[i].selfCharacter() -> type() != 3)){
			sum++;
			if(m[i].selfCharacter() -> is_dead() == true){
				if(is_guarded(m[i].selfCharacter()) && is_saved(m[i].selfCharacter())){
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
				else if(is_guarded(m[i].selfCharacter())){
					m[i].selfCharacter() -> set_alive();//被守卫的人知道自己被守卫么？
				}
				else if(is_saved(m[i].selfCharacter())){
					m[i].selfCharacter() -> set_alive();
				}
				else{
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
			}
			else{
				if(is_poisoned(m[i].selfCharacter())){
					num++;
					if(m[i].selfCharacter() -> is_police()){
						po -> begin();
					}
					//willing(&m[i]);
				}
			}
		}
	}
	if(num == sum){
		return true;
	}
	return false;
}
			
bool Calculating::func(){
	if(_calibra == 1){
		if(calculatewolf() || calculatepeo()){
			return false;
		}
		else{
			return _hun -> begin();
		}
	
	}
	else{
		if(calculatewolf()){
			return false;
		}
		else{
			return _hun -> begin();
		}
	}
}
bool Po_passing::func(){
	int n=allclient->size();
	bool isalive[n];
	for(int i=0;i<n;i++)
		isalive[i]=!(*allclient)[i].selfCharacter()->is_dead();
	for(int i=_log.size()-1;i>=0;i--){
		if(_log[i]._act==VOTE)
			isalive[_log[i]._geter]=1;
		else break;
	}
	for(int i=0;i<n;i++)
			if(isalive[i])
				(*allclient)[i].print("The Police Out!\n");

	(*allclient)[have_police].print("Please choose to pass the police to someone or not.(Y/N)\n");
	(*allclient)[have_police].turn_on_input();
	std::string pp="";
	pp=(*allclient)[have_police].recv();
	while(pp[0]!='Y'&&pp[0]!='N'){
		(*allclient)[have_police].print("Input again.\n");
		(*allclient)[have_police].turn_on_input();
		pp=(*allclient)[have_police].recv();
	}
	if(pp[0]=='N'){
		for(int i=0;i<n;i++)
			if(isalive[i])
				(*allclient)[i].print("The Police chooses to tear the pooolice sign.\n");
		have_police=-1;
		_rel_cli.clear();
	}
	else{
		(*allclient)[have_police].print("Please choose a Player to be the next Police.\n\tJust input player number.\n");
		std::string pass="";
		(*allclient)[have_police].turn_on_input();
		pass=(*allclient)[have_police].recv();
		while((*allclient)[pass[0]-'1'].selfCharacter()->is_dead()){
			(*allclient)[have_police].print("That Player is dead.\nPlease choose another Player.\n");
			pass=(*allclient)[have_police].recv();
		}
		for(int i=0;i<n;i++)
			if(isalive[i])
				(*allclient)[i].print("The Police chooses Player "+std::to_string(pass[0]-'1')+" to be the next Police.\n");
		have_police=pass[0]-'1';
		_rel_cli.clear();
		_rel_cli.push_back(&((*allclient)[have_police]));
	}
	return true;
}




