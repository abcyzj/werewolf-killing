#include "process.h"
#include <iostream>
#include <cstdlib>
using namespace Werewolf;

std::vector<logging> Process::_log;
int Process::have_police = -1;
std::vector<logging>* Chat::_log = nullptr;

Process::Process(std::vector<Client>* _all):allclient(_all), _valid(false){
    std::cout<<"All clients have been loaded.\n";
    
}

Process :: ~Process() {}

bool Process :: begin()
{
    if(!valid())
        return true;
    else return func();
}

bool Process :: valid()
{
    return _valid;
}

void Process :: activate()
{
    _valid = 1;
}

int Process::get_size(){
    return _rel_cli.size();
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
std :: vector<logging>* Process::readlog(){
    return &_log;
}
void Process::writelog(Cha doer,Act act,int geter)
{
    _log.push_back(logging{doer,act,geter});
}

bool Guarding :: func()
{
    dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> who_i_guard(-1);    //将守卫的人置为-1
    if(!_rel_cli[0] -> selfCharacter() -> is_dead())
    {
        _rel_cli[0] -> print("Please input the player number you want to guard:\n");
        _rel_cli[0] -> turn_on_input();
        std::string res1 = _rel_cli[0]->recv();
        if ((*allclient)[res1[0] - '1'].selfCharacter() -> type() ==
            dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> last())
        {
            _rel_cli[0] -> print("You have guarded the same one as last night, please change\n");
            _rel_cli[0] -> print("Please input the player number you want to guard again:\n");
            _rel_cli[0] -> turn_on_input();
            std :: string res2 = _rel_cli[0] -> recv();
            (*allclient)[res2[0]-'1'].selfCharacter() -> guardfunc();
            dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> who_i_guard(res2[0] - '1');
            writelog(GUARD, GUARDING, res2[0] - '1');
        }
        else
        {
            (*allclient)[res1[0] - '1'].selfCharacter() -> guardfunc();
            dynamic_cast<Guard*>(_rel_cli[0] -> selfCharacter()) -> who_i_guard(res1[0] - '1');
            writelog(GUARD, GUARDING, res1[0] - '1');
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
                _rel_cli[i] -> print("please chat with you partner:");
                _rel_cli[i]->turn_on_input();
                std::string words=_rel_cli[i]->recv(10);
                for(int j=0;j<_rel_cli.size();j++)
                    if(isalive[j])
                        _rel_cli[j]->print(words);
            }
        for(int i=0;i<_rel_cli.size();i++)
            if(isalive[i]){
                _rel_cli[i]->print("Please input the player number you want to kill.\nPlease reach a consensus!!!\n");
                _rel_cli[i]->turn_on_input();
                std::string tgt=_rel_cli[i]->recv(10);
                if(i==0)
                    num=tgt[0]-'1';
                else if(num!=tgt[0]-'1')
                    flag=false;
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

bool Witching :: func() //女巫使用毒药或者解药
{
    if (! _rel_cli[0] -> selfCharacter() -> is_dead()) //女巫没死
    {
        int pos_num = dynamic_cast<Witch*>(_rel_cli[0] -> selfCharacter()) -> have_poison();
        int anti_num = dynamic_cast<Witch*>(_rel_cli[0] -> selfCharacter()) -> have_antidote();
        //告诉剩多少毒药和解药
        _rel_cli[0] -> print("You have " + std::to_string(pos_num) + " poison " + std :: to_string(anti_num) + " antidote\n");
        if (pos_num == 0 && anti_num == 0) return true;
        //读日志告诉女巫谁死谁活
        if ((*readlog())[readlog() -> size() - 1]._act == 0)
        {
            int have_dead = (*readlog())[readlog() -> size() - 1]._geter;
            _rel_cli[0] -> print("Player " + std::to_string(have_dead) + " have been killed by werewolves\n");
        }
        else _rel_cli[0] -> print("no one dead\n");
        
        //请女巫选择毒药和解药
        _rel_cli[0] -> print("Please choose to use poison or antidote, p or a\n");
        _rel_cli[0] -> turn_on_input();
        std::string drug = _rel_cli[0] -> recv();
        while(1)
        {
            if (drug == "p")
            {
                if (pos_num == 0)
                {
                    _rel_cli[0] -> print("ERROR, NO Posion left, please input again\n");
                    _rel_cli[0] -> turn_on_input();
                    drug = _rel_cli[0] -> recv();
                }
                else break;
            }
            
            if (drug == "a")
            {
                if (pos_num == 0)
                {
                    _rel_cli[0] -> print("ERROR, NO antidote left, please input again\n");
                    _rel_cli[0] -> turn_on_input();
                    drug = _rel_cli[0] -> recv();
                }
                else break;
            }
        }
        
        
        if (drug == "p")    //毒人
        {
            _rel_cli[0] -> print("Please Input the player you want to act on\n");
            _rel_cli[0] -> turn_on_input();
            std::string ans = _rel_cli[0] -> recv();
            int num = atoi(ans.c_str());
            writelog(WITCH, POISON, num - 1);
        }
        else
        {
            int have_dead = (*readlog())[readlog() -> size() - 1]._geter;
            writelog(WITCH, SAVE, have_dead);
        }
    }
    return true;
}

bool Predicting :: func()    //预言家进行身份检测
{
    if (! _rel_cli[0] -> selfCharacter() -> is_dead()) //预言家没死
    {
        _rel_cli[0] -> print("Please input the player number you want to see\n");
        _rel_cli[0] -> turn_on_input();
        std::string player = _rel_cli[0] -> recv();
        int t = (*allclient)[atoi(player.c_str()) - 1].selfCharacter() -> type();
        if (t == 1)
            _rel_cli[0] -> print("The identity of the player is werewolf\n");
        else
            _rel_cli[0] -> print("The identity of the player is goodman\n");
        writelog(SEER, PREDICT, atoi(player.c_str()) - 1);
    }
    return true;
}

Voting::Voting(std::vector<Client>* _all, Process* hunt,Process* Po_passing ):Process(_all), ht(hunt),Po_p(Po_passing){}
Po_passing::Po_passing(std::vector<Client>* _all):Process(_all){}
bool Voting::func(){
    int n=allclient->size();
    bool isalive[n];//”–À≠‘⁄≥°
    double num[n];//º«¬º√ø∏ˆ»À±ªÕ∂∆± ˝
    int voteinfo[n];//º«¬º√ø∏ˆ»ÀÕ∂∆±«Èøˆ
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
    if(have_police>=0){//æØ≥§πÈ∆±
        (*allclient)[have_police].print("Please vote a Player!\n");
        (*allclient)[have_police].turn_on_input();
        std::string pt=(*allclient)[have_police].recv();
        while(!isalive[pt[0]-'1']){
            (*allclient)[have_police].print("Please choose another Player!, he is dead\n");
            (*allclient)[have_police].turn_on_input();
            pt=(*allclient)[have_police].recv();
        }
        voteinfo[have_police]=pt[0]-'1';
        num[pt[0]-'1']+=1.5;
    }
    for(int i=0;i<n;i++)//∆‰”‡»ÀÕ∂∆±
        if(isalive[i]||i!=have_police){
            (*allclient)[i].print("Please vote!\n"+voted);
            if(have_police>=0)
                (*allclient)[i].print("The Police votes to Player "+std::to_string(voteinfo[have_police])+".\n");
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
    double maxx=0;//∆± ˝µƒ◊Ó¥Û÷µ
    std::vector<int> maxnum;//∆± ˝◊Ó¥Ûµƒ»Ù∏…√˚ÕÊº“µƒ±‡∫≈
    for(int i=0;i<n;i++)
        if(num[i]>maxx)
            maxx=num[i];
    for(int i=0;i<n;i++)
        if(abs(num[i]-maxx)<1e-9)
            maxnum.push_back(i);
    std::string s="";//–Ë“™¥Ú”°µƒÕ∂∆±–≈œ¢
    for(int i=0;i<n;i++)
        if(isalive[i])
            s+="Player "+std::to_string(i)+" votes to Player "+std::to_string(voteinfo[i])+".\n";
    
    for(int i=0;i<n;i++)//π´≤ºÕ∂∆±–≈œ¢
        if(isalive[i])
            (*allclient)[i].print(s);
    
    std::vector<int> deadnum;//À¿’ﬂ–≈œ¢
    if(maxnum.size()==1){
        (*allclient)[maxnum[0]].selfCharacter()->set_dead();
        isalive[maxnum[0]]=0;
        writelog(ALL,VOTE,maxnum[0]);
        deadnum.push_back(maxnum[0]);
        /*
         for(int i=0;i<n;i++)
         if(isalive[i])
         (*allclient)[i].print("Player "+std::to_string(maxnum[0])+" has been voted out!\n");
         (*allclient)[maxnum[0]].print("You have been voted out!\n");
         if(!is_end()){
         if((*allclient)[maxnum[0]].selfCharacter()->type()==hunter){
         bool end=!ht->begin();*/
        /*
         (*allclient)[maxnum[0]].print("You can choose to shoot or not.(Y/N)\n");
         (*allclient)[maxnum[0]].turn_on_input();
         std::string choice=(*allclient)[maxnum[0]].recv();
         if(choice=="Y"){
         bool end=!ht->begin();//øº¬«¡‘»À…±»À∫ÛΩ·À„*/
        /*
         if(end)
         return false;
         }
         }
         */
        /*
         for(int i=0;i<n;i++)
         isalive[i]=!(*allclient)[i].selfCharacter()->is_dead();//∏¸–¬isalive[]*/
        /*
         for(int i=0;i<n;i++)
         if(isalive[i]||i==maxnum[0])
         (*allclient)[i].print("Last words begin.\n");
         (*allclient)[maxnum[0]].print("Please input the words you want to say before you quit the game.\n:q + Enter represents ending.\n");
         std::string lwords="";
         while(true){
         (*allclient)[maxnum[0]].turn_on_input();
         lwords=(*allclient)[maxnum[0]].recv();
         if(lwords!=":q"){
         for(int j=0;i<n;j++)
         if(isalive[j]){
         (*allclient)[j].print(lwords+"\n");
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
         return false;*/
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
            chod.push_back(maxnum[(r1+i*order+maxnum.size())%maxnum.size()]);
        for(int i=0;i<maxnum.size();i++)
            tie+=" "+std::to_string(chod[i]);
        for(int i=0;i<n;i++)
            (*allclient)[i].print(tie);
        for(int i=0;i<chod.size();i++){
            for(int j=0;j<n;j++)
                if(isalive[j])
                    (*allclient)[j].print("Player "+ std::to_string(chod[i])+"'s turn:\n");
            (*allclient)[chod[i]].print("Please input the words you want to say.\n:q + Enter represents ending.\n");
            std::string saying="";
            while(true){
                (*allclient)[chod[i]].turn_on_input();
                saying=(*allclient)[chod[i]].recv();
                if(saying!=":q")
                    for(int j=0;j<n;j++)
                        if(isalive[j]&&j!=chod[i])
                            (*allclient)[j].print(saying);
                        else break;
            }
            for(int j=0;j<n;j++)
                (*allclient)[j].print("Player "+std::to_string(chod[i])+"has finished.\n");
        }
        for(int i=0;i<n;i++)
            (*allclient)[i].print("Round 2 Chat end.\nRound 2 Voting start.\n");
        int num2[n];//√ø∏ˆ»À±ªÕ∂∆±
        int voteinfo2[n];//√ø∏ˆ»ÀÕ∂∏¯À≠
        int maxx=0;//◊Ó∂‡∆± ˝
        bool canvote[n];
        for(int i=0;i<n;i++){
            num2[i]=0;
            voteinfo2[i]=-1;
            canvote[i]=1;
        }
        for(int i=0;i<maxnum.size();i++){
            canvote[maxnum[i]]=0;
        }
        std::string sec="You can only vote to Player";
        for(int i=0;i<maxnum.size();i++)
            sec+=" "+std::to_string(maxnum[i]);
        sec+=".\n";
        maxnum.clear();//«Âø’◊Ó∂‡∆± ˝µƒÕÊº“–Ú¡–
        for(int i=0;i<n;i++){
            if(isalive[i]&&canvote[i]){
                (*allclient)[i].print("Please vote!\n"+sec);
                (*allclient)[i].turn_on_input();
                std::string st=(*allclient)[i].recv();
                while(canvote[st[0]-'1']){
                    (*allclient)[i].print("You cannot vote that Player.\nPlease choose another Player.\n");
                    st=(*allclient)[i].recv();
                }
                voteinfo2[i]=st[0]-'1';
                num2[st[0]-'1']++;
            }
        }
        for(int i=0;i<n;i++)
            if(num2[i]>maxx)
                maxx=num2[i];
        for(int i=0;i<n;i++)
            if(num2[i]==maxx)
                maxnum.push_back(i);
        std::string secs="";//–Ë“™¥Ú”°µƒÕ∂∆±–≈œ¢
        for(int i=0;i<n;i++)
            if(isalive[i]&&canvote[i])
                secs+="Player "+std::to_string(i)+" votes to Player "+std::to_string(voteinfo2[i])+".\n";
        for(int i=0;i<n;i++)//π´≤ºÕ∂∆±–≈œ¢
            if(isalive[i])
                (*allclient)[i].print(s);
        for(int i=0;i<maxnum.size();i++){//À¿Õˆ≤Ÿ◊˜
            (*allclient)[maxnum[i]].selfCharacter()->set_dead();
            //	isalive[maxnum[i]]=0;
            writelog(ALL,VOTE,maxnum[i]);
            deadnum.push_back(maxnum[i]);
        }
    }
    //π´≤ºÀ¿’ﬂ–≈œ¢º∞Ω¯––∫Û–¯¥¶¿Ì
    std::string deathinfo="Player";
    for(int i=0;i<deadnum.size();i++)
        deathinfo+=" "+std::to_string(deadnum[i]);
    deathinfo+=" out!\n";
    for(int i=0;i<n;i++)
        if(isalive[i])
            (*allclient)[i].print(deathinfo);
    bool isend=is_end();
    if(isend)
        return false;
    else{//”Œœ∑≤¢Œ¥Ω· ¯ Ω¯––æØ≥§∑…æØª’∫Õ¡‘»Àø™«π
        bool flag1=0;
        bool flag2=0;
        for(int i=0;i<deadnum.size();i++)
            if(deadnum[i]==have_police){
                flag1=1;
                break;
            }
        if(flag1)//æØ≥§∑…æØª’
            Po_p->begin();
        
        for(int i=0;i<deadnum.size();i++)
            if((*allclient)[deadnum[i]].selfCharacter()->type()==hunter){
                flag2=deadnum[i];
                break;
            }
        if(flag2>=0){//¡‘»Àø™«π
            ht->begin();
            bool isend2=is_end();
            if(isend2)
                return false;
            else{//æØ≥§∑…æØª’
                int behunt=(*readlog())[readlog()->size()-1]._geter;
                if(behunt==have_police)
                    Po_p->begin();
            }
        }
        //“≈—‘≥¬ ˆ
        deadnum.clear();
        for(int i=_log.size()-1;i>=0;i--)
            if(_log[i]._act==VOTE||_log[i]._act==SHOOT)
                deadnum.push_back(_log[i]._geter);
            else
                break;
        for(int i=0;i<n;i++)
            isalive[i]=!(*allclient)[i].selfCharacter()->is_dead();
        for(int i=0;i<deadnum.size();i++)
            isalive[deadnum[i]]=1;
        for(int i=deadnum.size()-1;i>=0;i--){
            (*allclient)[deadnum[i]].print("Please input your last words.\n:q + Enter represents ending.\n");
            (*allclient)[deadnum[i]].turn_on_input();
            std::string lword="Player "+std::to_string(deadnum[i])+"'s last wotds start:\n";
            std::string ladd="";
            ladd=(*allclient)[deadnum[i]].recv();
            while(ladd!=":q"){
                lword+=ladd+"\n";
                (*allclient)[deadnum[i]].turn_on_input();
                ladd=(*allclient)[deadnum[i]].recv();
            }
            lword+="Last words end.\n";
            for(int j=0;j<n;j++)
                if(isalive[j]&&j!=deadnum[i])
                    (*allclient)[j].print(lword);
            (*allclient)[deadnum[i]].print("You quit the game!\n");
            isalive[deadnum[i]]=0;
        }
    }
    return true;
}
bool Voting::is_end(){
    int wolfnum=0;
    int villnum=0;
    int godnum=0;
    int n=allclient->size();
    for(int i=0;i<n;i++)
        if(!(*allclient)[i].selfCharacter()->is_dead()){
            int tp=(*allclient)[i].selfCharacter()->type();
            if(tp==wolf)
                wolfnum++;
            else if(tp==villager)
                villnum++;
            else godnum++;
        }
    if(wolfnum==0&&villnum!=0&&godnum!=0){
        for(int i=0;i<n;i++)
            (*allclient)[i].print("Good Man Win!\n");
        return true;
    }
    if(wolfnum==0&&(villnum==0||godnum==0)){
        for(int i=0;i<n;i++)
            (*allclient)[i].print("Tie Game!\n");
        return true;
    }
    if(wolfnum!=0&&(villnum==0||godnum==0)){
        for(int i=0;i<n;i++)
            (*allclient)[i].print("Wolf Win!\n");
    }
    if(wolfnum!=0&&villnum!=0&&godnum!=0)
        return false;
    
    return true;
}


bool Calculating::is_guarded(int i){
    if((*_log)[0]._geter == i){
        return true;
    }
    return false;
}

bool Calculating::is_saved(int i){
    if((*_log)[1]._geter == i && (*_log)[1]._act == 2){
        return true;
    }
    return false;
}

bool Calculating::is_poisoned(int i){
    if((*_log)[1]._geter == i && (*_log)[1]._act == 1){
        return true;
    }
    return false;
}

bool Calculating::calculatewolf(){//∑÷æØª’∫Õ∑…æØª’√ª”– µœ÷
    int sum = 0;
    int num = 0;
    std::vector<Client>& m = *allclient;
    for(auto i = 0; i < m.size(); i++){
        if(m[i].selfCharacter() -> type() == 1){
            sum++;
            if(m[i].selfCharacter() -> is_dead() == true){
                if(is_guarded(i) && is_saved(i)){
                    num++;
                    if(m[i].selfCharacter() -> is_police()){
                        _po -> begin();
                    }
                    
                    //willing(&m[i]);
                }
                else if(is_guarded(i)){
                    m[i].selfCharacter() -> set_alive();//±ª ÿŒ¿µƒ»À÷™µ¿◊‘º∫±ª ÿŒ¿√¥£ø
                }
                else if(is_saved(i)){
                    m[i].selfCharacter() -> set_alive();
                }
                else{
                    num++;
                    if(m[i].selfCharacter() -> is_police()){
                        _po -> begin();
                    }
                    //willing(&m[i]);
                }
            }
            else{
                if(is_poisoned(i)){
                    num++;
                    if(m[i].selfCharacter() -> is_police()){
                        _po -> begin();
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
    return true;
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
                    if(is_guarded(i) && is_saved(i)){
                        num++;
                        if(m[i].selfCharacter() -> is_police()){
                            _po -> begin();
                        }
                        //willing(&m[i]);
                    }
                    else if(is_guarded(i)){
                        m[i].selfCharacter() -> set_alive();//±ª ÿŒ¿µƒ»À÷™µ¿◊‘º∫±ª ÿŒ¿√¥£ø
                    }
                    else if(is_saved(i)){
                        m[i].selfCharacter() -> set_alive();
                    }
                    else{
                        num++;
                        if(m[i].selfCharacter() -> is_police()){
                            _po -> begin();
                        }
                        //willing(&m[i]);
                    }
                }
                else{
                    if(is_poisoned(i)){
                        num++;
                        if(m[i].selfCharacter() -> is_police()){
                            _po -> begin();
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
                if(is_guarded(i) && is_saved(i)){
                    num++;
                    if(m[i].selfCharacter() -> is_police()){
                        _po -> begin();
                    }
                    //willing(&m[i]);
                }
                else if(is_guarded(i)){
                    m[i].selfCharacter() -> set_alive();//±ª ÿŒ¿µƒ»À÷™µ¿◊‘º∫±ª ÿŒ¿√¥£ø
                }
                else if(is_saved(i)){
                    m[i].selfCharacter() -> set_alive();
                }
                else{
                    num++;
                    if(m[i].selfCharacter() -> is_police()){
                        _po -> begin();
                    }
                    //willing(&m[i]);
                }
            }
            else{
                if(is_poisoned(i)){
                    num++;
                    if(m[i].selfCharacter() -> is_police()){
                        _po -> begin();
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

bool check(int* a, int size, int k) //检测数组中有没有某个元素k
{
    for (int i = 0; i < size; i++)
        if (a[i] == k) return true;
    return false;
}

bool Po_electing :: func()  //选举警长
{
    have_police = -1; //没有警长，否则表示玩家编号
    int *num, cnt = 0;   //num中存竞选警长的人的编号，cnt表示竞选警长的人的个数
    int *tot_poll;  //记录每个竞选警长的玩家所得票数，与num相对应
    int max_poll = 0;   //表示最大票数，防止有重复要重新计票
    for (int i = 0; i < (*allclient).size(); i++)   //遍历询问想当警长的人
    {
        if (! (*allclient)[i].selfCharacter() -> is_dead())
        {
            (*allclient)[i].print("Do you want to be police, y or n?\n");
            (*allclient)[i].turn_on_input();
            std::string ans = (*allclient)[i].recv();
            if (ans[0] == 'y')
                num[cnt++] = i + 1; //记录下想当警长的人的编号
        }
    }
    
    for (int i = 0; i < cnt && num[i] != 0; i++)
    {
        (*allclient)[num[i]].print("Please input your Declaration of candidacy\n");
        (*allclient)[num[i]].turn_on_input();
        std::string words = (*allclient)[num[i]].recv(); //接受竞选宣言
        for (int j = 0; j < (*allclient).size(); j++)//将竞选宣言发给所有玩家
            (*allclient)[j].print("words from player " + std::to_string(num[i]) + " is" + words);
        
        
        for (int j = 0; j < cnt && num[j] != 0; j++)    //每位竞选者发言后询问是否有人退出竞选
        {
            (*allclient)[num[j]].print("Do you want to quit electing, q or n");
            (*allclient)[num[j]].turn_on_input();
            std::string ans = (*allclient)[num[j]].recv();   //得到信息是否退水
            if (ans[0] == 'q')
                num[j] = 0;  //退水则将编号置为0
        }
        
    }
    for (int i = 0; i < (*allclient).size(); i++)
    {
        if (! (*allclient)[i].selfCharacter() -> is_dead() && !check(num, cnt, i))  //没有死且不参加竞选
        {
            for (int j = 0; j < cnt; j++)
            {
                if (num[i])
                    (*allclient)[i].print("player involved in election include " + std::to_string(num[i]));
            }
            (*allclient)[i].print("Please input the player you want to choose to be the police\n");
            (*allclient)[i].turn_on_input();
            std::string ans = (*allclient)[i].recv();
            for (int l = 0; l < cnt; l++)
            {
                if (num[l] == atoi(ans.c_str()))    //找到相应的人并计票
                {
                    tot_poll[l] += 1;
                    break;
                }
            }
        }
    }
    int target = 0; //记录最大票数的人
    //票数相同再次发言并重新投票
    for (int i = 0; i < cnt; i++)   //开始寻找最大票数
    {
        if (tot_poll[i] > max_poll)
        {
            max_poll = tot_poll[i];
            target = i;
        }
    }
    std::vector<int> rep;    //计算有最大票数的人数
    for (int i = 0; i < cnt; i++)
    {
        if (tot_poll[i] == max_poll)
            rep.push_back(num[i]);
    }
    if(rep.size() == 1)    have_police = target;   //警长
    else    //有多人同票
    {
        for (int i = 0; i < (*allclient).size(); i++) //向每个人发送消息
        {
            for (int j = 0; j < rep.size(); j++)
            {
                (*allclient)[i].print("Player " + std::to_string(rep[j]) + " have the same poll, please say and vote again");
            }
        }
        for (int i = 0; i < rep.size(); i++) //同票者重新发言
        {
            (*allclient)[rep[i]].print("Please input your declaration again\n");
            (*allclient)[rep[i]].turn_on_input();
            std::string words = (*allclient)[rep[i]].recv();
            for (int j = 0; j < (*allclient).size(); i++)
                (*allclient)[j].print(words);
        }
        for (int i = 0; i < cnt; i++)
        {
            for (int j = 0; j < rep.size(); j++)
            {
                if (rep[j] == num[i])   break;
                num[i] = 0; //将第一次竞选的票数少的踢出，后来要进行投票
            }
        }
        for (int i = 0; i < cnt; i++)
            tot_poll[i] = 0;    //票数清空
        for (int i = 0; i < (*allclient).size(); i++)   //重新计票
        {
            if (!(*allclient)[i].selfCharacter() -> is_dead() && !check(num, cnt, i))  //没有死且不参加竞选
            {
                
                (*allclient)[i].print("Please input the player you want to choose to be the police again\n");
                (*allclient)[i].turn_on_input();
                std::string ans = (*allclient)[i].recv();
                for (int l = 0; l < cnt; l++)
                {
                    if (num[l] == atoi(ans.c_str()))    //找到相应的人并计票
                    {
                        tot_poll[l] += 1;
                        break;
                    }
                }
            }
        }//重新计票结束
        max_poll = 0, target = 0;
        for (int i = 0; i < cnt; i++)   //再次开始寻找最大票数
        {
            if (tot_poll[i] > max_poll)
            {
                max_poll = tot_poll[i];
                target = i;
            }
        }
        std::vector<int> rep2;    //计算有最大票数的人数
        for (int i = 0; i < cnt; i++)
        {
            if (tot_poll[i] == max_poll)
                rep2.push_back(num[i]);
        }
        if(rep2.size() == 1)    have_police = target;   //警长
        //else无警长
    }
    
    _valid = false; //执行完一次后直接不再可用（只竞选一次警长）
    return true;
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

Chat :: Chat(std::vector<Client>* _cli) : Process(_cli)
{
    size = _cli -> size();
}

void Chat :: read()//获取dead_num,dead_player[],
{
    
    int bite_man = -1;
    int poison_man = -1;
    int save_man = -1;
    int guard_man = -1;
    memset(dead_player, -1, sizeof(dead_player));
    _log = readlog();
    for(int i=0 ; i<_log -> size() ; i++)//判断有没有被咬或者被毒,被守卫，被救
    {
        if((*_log)[i]._act == BITE)
            bite_man = (*_log)[i]._geter;
        if((*_log)[i]._act == POISON)
            poison_man = (*_log)[i]._geter;
        if((*_log)[i]._act == SAVE)
            save_man = (*_log)[i]._geter;
        if((*_log)[i]._act == GUARDING)
            guard_man = (*_log)[i]._geter;
    }
    for(int i=0 ; i<size ; i++)//判断有没有被守卫或者被救
    {
        if((bite_man == i)&&(poison_man != i)&&(guard_man != i))//被咬，不被救，不被守卫
        {
            dead_num++;
            dead_player[dead_num]=i;
        }
        if((poison_man == i)&&(guard_man != i)) //  被毒，不被守卫
        {
            dead_num++;
            dead_player[dead_num]=i;
        }
        if((bite_man == i)&&(poison_man == i)&&(guard_man = i))//被咬，同守同救
        {
            dead_num++;
            dead_player[dead_num]=i;
        }
    }
}

void Chat :: right()
{
    read();
    for(int m=start_one+1; m < size; m++)
    {
        client[m].print("Please input your massages:");
        client[m].turn_on_input();
        std::string s = client[m].recv();
        client[m].turn_off_input();
        for(int k = 0; k < size; k++)
            client[k].print(s);
    }
    for(int m = 0 ; m <start_one ; m++)
    {
        client[m].print("Please input your massages:");
        client[m].turn_on_input();
        std::string s = client[m].recv();
        client[m].turn_off_input();
        for(int k = 0; k < size; k++)
            client[k].print(s);
    }
}

void Chat :: left()
{
    read();
    int size = client.size();
    if(start_one==0)
    {
        for(int m = size-1; m > 0; m--)
        {
            client[m].print("Please input your massages:");
            client[m].turn_on_input();
            std::string s = client[m].recv();
            client[m].turn_off_input();
            for(int k = 0; k < size; k++)
                client[k].print(s);
        }
        
    }
    else
    {
        for(int m = start_one-1; m>=0 ; m--)
        {
            client[m].print("Please input your massages:");
            client[m].turn_on_input();
            std::string s = client[m].recv();
            client[m].turn_off_input();
            for(int k = 0; k < size; k++)
                client[k].print(s);
        }
        for(int m = size-1; m >=start_one+1 ; m--)
        {
            client[m].print("Please input your massages:");
            client[m].turn_on_input();
            std::string s = client[m].recv();
            client[m].turn_off_input();
            for(int k = 0; k < size; k++)
                client[k].print(s);
        }
    }
}


bool Chat :: func()
{
    read();
    for(int i=0 ; i < size ; i++)//遗言环节
    {
        for(int j=1 ; j<=dead_num ; j++)
        {
            if(dead_player[j] == i)
            {
                client[i].print("Please input your last words:");
                client[i].turn_on_input();
                std::string s = client[i].recv();
                client[i].turn_off_input();
                for(int k = 0; k<size ; k++)
                    client[k].print(s);
            }
        }
    }
    for(int i=0 ; i < size ; i++)
    {
        if(have_police == i)//i是警长
        {
            police=1;//表示有警长存在
            if (dead_num==1)
            {
                start_one=dead_player[1];
                client[i].print("Please choose left or right:");
                client[i].turn_on_input();;
                std::string p = client[i].recv();
                if( p == "right")//向右
                    right();
                else
                    left();
            }
            else
            {
                start_one=i;
                client[i].print("Please choose left or right:");
                client[i].turn_on_input();
                std::string p = client[i].recv();
                if( p == "right")//向右
                    right();
                else
                    left();
            }
        }
        else
            continue;
    }
    if(police==0)//警长不存在的情况
    {
        if(dead_num==1)
        {
            start_one = dead_player[1];
            srand( (unsigned)time( NULL ) );
            int left_right = rand() % 2;//0为左，1为右
            if(left_right==1)
                right();
            else
                left();
        }
        else
        {
            srand( (unsigned)time( NULL ) );
            int t =  rand() % dead_num + 1;
            start_one = dead_player[t];
            int left_right = rand() % 2;
            if(left_right==1)
                right();
            else
                left();
        }
        
    }
    return true;
}

bool Hunting :: func()
{
    std::vector <Client>& tep_cli = *_cli;
    for(int i = 0 ; i < tep_cli.size() ; i++)
    {
        if(tep_cli[i].selfCharacter() -> type() == 3)//判断是不是猎人
        {
            tep_cli[i].print("Please choose a man you want to kill");
            tep_cli[i].turn_on_input();
            std::string s = tep_cli[i].recv();
            int x = std::atoi(s.c_str());//杀死第几客户端
            tep_cli[x].selfCharacter() ->set_dead();
        }
    }
    return true;
}
