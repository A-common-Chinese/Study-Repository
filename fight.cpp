#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;

class Environment {
public:
    Environment(string n, int a, int d, int b) : name(n), atkPlus(a), defPlus(d), actPointdemand(b), id(envNextID++) {}
    string getName() const {
        return name;
    }
    int getAtkPlus() const {
        return atkPlus;
    }
    int getDefPlus() const {
        return defPlus;
    }
    int getActPointdemand() const {
        return actPointdemand;
    }
    int getId() const {
        return id;
    }
protected:
    string name;
    int atkPlus;
    int defPlus;
    int actPointdemand;
    short id;
private:
    static int envNextID;
};

class Character {
private:
    static int chrNextID;
protected:
    string name;
    short id;
    int hp;
    int maxHp;
    int atk;
    int def;
    int hl;
    bool isHide;
public:
    int pastAtk;
    int pastDef;
    Character(string n, int h, int a, int d, int l, bool SW) : name(n), id(chrNextID++), hp(h), maxHp(h), atk(a), def(d), hl(l), pastAtk(a), pastDef(d), isHide(SW) {}
    virtual ~Character() {}
    virtual void attack(Character& target) = 0;
    virtual string getClassName() const = 0;
    virtual void healAllies (vector<Character*>& allies) {}
    virtual void aoeAttack (vector<Character*> & enemies) {}
    virtual void takeDamageT1(int damage) {
        if (hp <= 0){
            hp = 0;
        }  
        else{
            int actualDamage = damage - def;
            if (actualDamage < 0) actualDamage = 1;
            hp -= actualDamage;
            if (hp < 0) hp = 0;
        }
    }
    virtual void takeDamageT2(int damage) {
        if (hp <= 0){
            hp = 0;
        }
        else{
            int actualDamage = damage;
            hp -= actualDamage;
            if (hp < 0) hp = 0;
        }
    }
    virtual void healSelf (int amount) {
        hp += amount;
        if (hp > maxHp) hp = maxHp;
    }
    virtual void healOneAlly (Character& ally, int amount) {
        ally.healSelf(amount);
    }
    bool isAlive() const {
        return hp > 0;
    }
    string getName() const {
        return name;
    }
    int getHp() const {
        return hp;
    }
    int getMaxHp() const {
        return maxHp;
    }
    int getAtk() const {
        return atk;
    }
    int getDef() const {
        return def;
    }
    int getId() const {
        return id;
    }
    int getPastAtk() const {
        return pastAtk;
    }
    int getPastDef() const {
        return pastDef;
    }
    void loadEnvEff(int atkBuff, int defBuff) {
        pastAtk = atk;
        atk += atkBuff;
        pastDef = def;
        def += defBuff;
    }
    void resetStats() {
        atk = pastAtk;
        def = pastDef;
    }
    int getHealAmount() const {
        return hl;
    }
    bool getHideInf() const {
        return isHide;
    }
}; 
int Character :: chrNextID = 1; // 全局角色ID计数器
int Environment :: envNextID = 1; // 全局环境ID计数器

class Player{
private:
    string name;
public:
    Player(string n) : name(n) {}
    vector<Character*> selfCharacter;

    void addCharacter(Character* character) {
        selfCharacter.push_back(character);
    }
    void clearCharacters() {
        selfCharacter.clear();
    }
    string getName() const {
        return name;
    }
};

class Warrior : public Character {
public:
    Warrior(string n, int h, int a, int d, int l, bool SW) : Character(n, h, a, d, l,SW) {}
    void attack(Character& target) override {
        target.takeDamageT1(atk + 5);
    }
    virtual string getClassName() const override{
        return "Warrior";
    }
}; 

class Ranger : public Character {
public:
    Ranger(string n, int h, int a, int d, int l, bool SW) : Character(n, h, a, d, l, SW) {}
    void attack(Character& target) override {   
        target.takeDamageT1(atk);
        int temp;
        temp = atk * 20 / 100;
        target.takeDamageT1(temp); // 额外伤害为攻击力的20%,但是取整数,且无法无视防御
    }
    virtual string getClassName() const override{
        return "Ranger";
    }
};

class Mage : public Character {
public:
    Mage(string n, int h, int a, int d, int l, bool SW) : Character(n, h, a, d, l, SW) {}
    void attack(Character& target) override {
        target.takeDamageT1(atk);
        int temp;
        temp = atk * 25 / 100;
        target.takeDamageT2(temp); // 额外伤害为攻击力的25%,但是取整数,真实伤害
    }
    void healAllies(vector<Character*>& allies) {
        for (auto& ally : allies) {
            if (ally->isAlive()) {
                ally->healSelf(hl);
            }
        }
    }
    void aoeAttack (vector<Character*>& enemies) {
        for (auto enemy : enemies){
            if (enemy->isAlive()){
                enemy->takeDamageT2(atk * 20 / 100); //0.2伤害的AOE攻击，真伤
            }
        }
    }
    virtual string getClassName() const override{
        return "Mage";
    }
};

class President : public Character {
public:
    President(string n, int h, int a, int d, int l, bool SW) : Character(n, h, a, d, l, SW) {}
    void attack(Character& target) override {
        target.takeDamageT2(atk);
    }
    void healAllies(vector<Character*>& allies) {
        for (auto& ally : allies) {
            if (ally->isAlive()) {
                ally->takeDamageT1(50);
                if (ally->isAlive()) {
                    ally->healSelf(hl*10);
                }
                else{
                    cout << ally->getName() << "死于" << name << "的经济援助" << endl;
                    ally->takeDamageT1(0);
                }
            }
        }
    }
    virtual string getClassName() const override{
        return "President";
    }
};

bool hasAlive(const vector<Character*>& team) {
    for (auto* c : team) {
        if (c->isAlive()) return true;
    }
    return false;
}

void endBattle(Player& p1, Player& p2) {
    cout << endl << "战斗结束！" << endl;
    cout << p1.getName() << "的角色：" << endl;
    for (const auto& character : p1.selfCharacter) {
        cout << character->getName() << " - HP: " << character->getHp() << "/" << character->getMaxHp() << endl;
    }
    cout << p2.getName() << "的角色：" << endl;
    for (const auto& character : p2.selfCharacter) {
        cout << character->getName() << " - HP: " << character->getHp() << "/" << character->getMaxHp() << endl;
    };
    for (const auto& character : p1.selfCharacter) {
        character->resetStats();
        character->healSelf(character->getMaxHp() - character->getHp()); // 战斗结束后恢复血量
    }
    for (const auto& character : p2.selfCharacter) {
        character->resetStats();
        character->healSelf(character->getMaxHp() - character->getHp()); // 战斗结束后恢复血量
    }
    p1.clearCharacters();
    p2.clearCharacters();
    cout << endl; // 添加一个空行以分隔战斗结果和下一轮战斗
};

int getSafeInt(const string& prompt);
Character* selectCharacter(const string& playerName, const string& roleLabel, unordered_map<int, Character*>& characterObjects, unordered_set<int>& selectedIDs) {
    cout << playerName << "，请选择" << roleLabel << "：" << endl;
    // 显示可选列表
    vector<pair<int, Character*>> available;
    for (const auto& pair : characterObjects) {
        int id = pair.first;
        Character* c = pair.second;
        if (!c->getHideInf() && selectedIDs.find(id) == selectedIDs.end()) {
            available.push_back({id, c});
            cout << id << ". " << c->getName() << "|职业：" << c->getClassName() <<"|攻击：" << c->getAtk() << " 防御：" << c->getDef() << " 血量：" << c->getMaxHp() << " 治疗：" << c->getHealAmount() << endl;
        }
    }
    if (available.empty()) {
        cout << "没有可用角色！" << endl;
        return nullptr;
    }

    while (true) {
        int choice = getSafeInt("输入角色ID：");
        auto it = characterObjects.find(choice);
        if (it != characterObjects.end() &&  
            selectedIDs.find(choice) == selectedIDs.end()) {
            // 有效选择
            selectedIDs.insert(choice);
            cout << "角色『" << it->second->getName() << "』 选择成功！" << endl;
            return it->second;
        }
        cout << "无效ID或角色已被选择，请重新选择。" << endl;
    }
}

void playerTurn(Player& player, Character* self, vector<Character*>& allies, vector<Character*>& enemies, int& actPoint, int actCost);
void battleNvsN(Player& p1, Player& p2, Environment& env, unordered_map<int, Character*>& characterObjects, short type) {
    int atkBuff = env.getAtkPlus();
    int defBuff = env.getDefPlus();
    int p1ActPoint = type * 2; //初始化AP
    int p2ActPoint = type * 2;
    int actPointDemand = env.getActPointdemand();
    unordered_set<int> selectedIDs;
    vector<Character*> team1 = {};
    vector<Character*> team2 = {};

    for (int i = 0; i < type; ++i) {
        Character* c = selectCharacter(p1.getName(), "第" + to_string(i+1) + "个角色(P1)", characterObjects, selectedIDs);
        if (!c) return;
        c->loadEnvEff(atkBuff, defBuff);
        p1.addCharacter(c);
        team1.push_back(c);
    }

    for (int i = 0; i < type; ++i) {
        Character* c = selectCharacter(p2.getName(), "第" + to_string(i+1) + "个角色(P2)", characterObjects, selectedIDs);
        if (!c) return;
        c->loadEnvEff(atkBuff, defBuff);
        p2.addCharacter(c);
        team2.push_back(c);
    }
    cout << endl;

    // 主战斗循环
    while (true) {
        // 检查胜负
        bool team1Alive = false, team2Alive = false;
        for (auto* c : team1) if (c->isAlive()) team1Alive = true;
        for (auto* c : team2) if (c->isAlive()) team2Alive = true;
        if (!team1Alive || !team2Alive) break;

        // 玩家1回合：每个存活角色行动一次
        while (true){
            for (auto* self : team1) {
                if (self->isAlive()) {
                    playerTurn(p1, self, team1, team2, p1ActPoint, actPointDemand);
                }
            }

            if (p1ActPoint >= actPointDemand + 1 && hasAlive(team1)) {
                cout << "剩余行动点: " << p1ActPoint << "，是否消耗1点再开一轮？(y/n) ";
                char choice;
                cin >> choice;
                cin.ignore(); // 清空缓冲区
                if (choice == 'y' || choice == 'Y') {
                    p1ActPoint -= 1;
                    continue; // 继续下一轮
                }
            }
            if (p1ActPoint < actPointDemand && hasAlive(team1)){
                cout << "行动点不足！ ";
            }
            cout << "剩余行动点: " << p1ActPoint << " |将被保留至下一轮" << endl;
            p1ActPoint += type * actPointDemand; // 每回合+AP
            break;
        }
        // 检查胜负
        team1Alive = false; team2Alive = false;
        for (auto* c : team1) if (c->isAlive()) team1Alive = true;
        for (auto* c : team2) if (c->isAlive()) team2Alive = true;
        if (!team1Alive || !team2Alive) break;

        // 玩家2回合：每个存活角色行动一次
        while (true){
            for (auto* self : team2) {
                if (self->isAlive()) {
                    playerTurn(p2, self, team2, team1, p2ActPoint, actPointDemand);
                }
            }

            if (p2ActPoint >= actPointDemand + 1 && hasAlive(team2)) {
                cout << "剩余行动点: " << p2ActPoint << "，是否消耗1点再开一轮？(y/n) ";
                char choice;
                cin >> choice;
                cin.ignore(); // 清空缓冲区
                if (choice == 'y' || choice == 'Y') {
                    p2ActPoint -= 1;
                    continue; // 继续下一轮
                }
            }
            if (p2ActPoint < actPointDemand && hasAlive(team2)){
                cout << "行动点不足！ ";
            }
            cout << "剩余行动点: " << p2ActPoint << " |将被保留至下一轮" << endl;
            p2ActPoint += type * actPointDemand; // 每回合+AP
            break;
        }
    }
    endBattle(p1, p2);
}

void playerTurn(Player& player, Character* self, vector<Character*>& allies,
                vector<Character*>& enemies, int& actPoint, int actCost) {
    if (!self->isAlive()) return;

    cout << "\n" << player.getName() << " 的 " << self->getName() << " 行动中！" << endl;
    cout << "当前行动点: " << actPoint << endl;

    cout << "选择行动: 1. 攻击 2. 治疗自己 3. 治疗队友 4. 跳过 ";
    if (self->getClassName() == "Mage" || self->getClassName() == "President"){
        cout << "5. 全体治疗 ";
    }
    if (self->getClassName() == "Mage"){
        cout << "6. AOE攻击 ";
    }
    cout << endl;
    int choice = getSafeInt("输入数字以行动:");

    if (choice == 1) {
        // --- 攻击（只能打前排） ---
        if (actPoint < actCost) {
            cout << "行动点不足，无法攻击。" << endl;
            return;
        }
        // 找敌方第一个存活角色（前排）
        Character* target = nullptr;
        for (auto* e : enemies) {
            if (e->isAlive()) {
                target = e;
                break;
            }
        }
        if (!target) {
            cout << "没有可攻击的敌人！" << endl;
            return;
        }
        self->attack(*target);
        actPoint -= actCost;
        cout << self->getName() << " 攻击了 " << target->getName() << "!" << endl;
        cout << target->getName() << " 的剩余HP: " << target->getHp() << "/" << target->getMaxHp() << endl;
        if (!target->isAlive()) {
            cout << target->getName() << " 被击败了！" << endl;
        }
    }
    else if (choice == 2) {
        // --- 治疗自己 ---
        if (actPoint < actCost) {
            cout << "行动点不足，无法治疗。" << endl;
            return;
        }
        self->healSelf(self->getHealAmount());
        actPoint -= actCost;
        cout << self->getName() << " 治疗了自己 " << self->getHealAmount() << " 点 HP！" << endl;
        cout << self->getName() << " 的剩余HP: " << self->getHp() << "/" << self->getMaxHp() << endl;
    }
    else if (choice == 3) {
        // --- 治疗队友 ---
        if (actPoint < actCost) {
            cout << "行动点不足，无法治疗。" << endl;
            return;
        }
        // 列出存活的队友（不包括自己）
        vector<Character*> aliveAllies;
        for (auto* a : allies) {
            if (a->isAlive() && a != self) aliveAllies.push_back(a);
        }
        if (aliveAllies.empty()) {
            cout << "没有需要治疗的队友！" << endl;
            return;
        }
        cout << "选择治疗目标：" << endl;
        for (size_t i = 0; i < aliveAllies.size(); ++i) {
            cout << i + 1 << ". " << aliveAllies[i]->getName()
                 << " (HP: " << aliveAllies[i]->getHp() << "/" << aliveAllies[i]->getMaxHp() << ")" << endl;
        }
        int targetChoice = getSafeInt("输入目标编号：");
        if (targetChoice < 1 || targetChoice > static_cast<int>(aliveAllies.size())) {
            cout << "无效选择。" << endl;
            return;
        }
        Character* target = aliveAllies[targetChoice - 1];
        self->healOneAlly(*target, self->getHealAmount());
        actPoint -= actCost;
        cout << self->getName() << " 治疗了 " << target->getName() << " " << self->getHealAmount() << " 点 HP！" << endl;
        cout << target->getName() << " 的剩余HP: " << target->getHp() << "/" << target->getMaxHp() << endl;
    } 
    else if (choice == 4) {
        cout << self->getName() << " 跳过行动。" << endl;
    } 
    else if (choice == 5 && (self->getClassName() == "Mage" || self->getClassName() == "President")) {
        // --- 治疗全部队友 ---
        if (actPoint < actCost) {
            cout << "行动点不足，无法治疗。" << endl;
            return;
        }

        vector<Character*> aliveAllies;
        for (auto* a : allies) {
            if (a->isAlive() && a != self) aliveAllies.push_back(a);
        }
        if (aliveAllies.empty()) {
            cout << "没有需要治疗的队友！" << endl;
            return;
        }
        self->healAllies(aliveAllies);
        actPoint -= actCost;
        cout << self->getName() << " 治疗了使用了全体治疗！ 友军全体增加HP！" << endl;
    }
    else if (choice == 6 && self->getClassName() == "Mage") {
        if (actPoint < actCost) {
            cout << "行动点不足，无法攻击。" << endl;
            return;
        }

        vector<Character*> aliveEnemies;
        for (auto* e : enemies) {
            if (e->isAlive()) aliveEnemies.push_back(e);
        }
        if (aliveEnemies.empty()) {
            cout << "敌人已经全部死亡！" << endl;
            return;
        }
        self->aoeAttack(aliveEnemies);
        actPoint -= actCost;
        cout << self->getName() << " 使用了范围攻击！" << endl;
        for (auto* e : aliveEnemies) {
            if (e->isAlive()) {
                cout << e->getName() << " 剩余 HP: " << e->getHp() << "/" << e->getMaxHp() << endl;
            }
        }   
    }
    else {
        cout << "无效选择。" << endl;
    }
}

int getSafeInt(const string& prompt) {
    int value;
    string leftover;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "请输入有效的整数！" << endl;
        } else {
            // 检查后面是否还跟着非法字符
            if (cin.peek() != '\n' && cin.peek() != EOF) {
                cin.ignore(10000, '\n');
                cout << "输入包含多余字符，请只输入数字！" << endl;
                continue;
            }
            cin.ignore(10000, '\n');
            return value;
        }
    }
}

int main(){
    Environment stndForest("Forest", 0, 0, 2);
    Environment migcForest("Magic Forest", 5, 2, 1);
    Environment stndDesert("Desert", 1, 0, 3);

    Warrior arthur("King Arthur", 110, 19, 11, 7, false);  //名称 血量 攻击 防御 治疗
    Warrior lancelot("Sir Lancelot", 95, 24, 7, 5, false);
    Warrior guanyu("Guan Yu", 120, 16, 14, 6, false);
    Mage fakeOPcharacter("“God”", 100, 10, 15, -2, true); // 隐藏坑爹角色，属性极垃
    Mage merlin("Merlin", 75, 28, 7, 12, false);
    Mage GongsunSheng("Ruyun loong", 120, 15, 12, 8, false);
    Ranger robin("Robin Hood", 85, 26, 9, 5, false);
    Ranger jinke("Jin Ke", 80, 30, 6, 5, false);

    Mage OPcharacter("God", 1000, 100, 50, 20, true); // 隐藏彩蛋角色，属性极高
    Warrior saberArthur("Altria Pendragon", 1010, 150, 45, 15, true);
    Ranger theDesertFox("Rommel", 600, 250, 65, 30, true);
    President maga("Trump", 1500, 30, 60, 14, true);
    Ranger rangers("75th Ranger Regiment", 500, 300, 70, 30, true);

    vector<int> EnvironmentIDList = {stndForest.getId(), migcForest.getId(), stndDesert.getId()};
    unordered_map<int, Character*> characterObjects = {
        {arthur.getId(), &arthur},
        {lancelot.getId(), &lancelot},
        {guanyu.getId(), &guanyu},
        {merlin.getId(), &merlin},
        {fakeOPcharacter.getId(), &fakeOPcharacter},
        {GongsunSheng.getId(), &GongsunSheng},
        {OPcharacter.getId(), &OPcharacter},
        {robin.getId(), &robin},
        {jinke.getId(),&jinke},
        {saberArthur.getId(),&saberArthur},
        {theDesertFox.getId(),&theDesertFox},
        {maga.getId(),&maga},
        {rangers.getId(),&rangers}
    };
    unordered_map<int, string> environmentMap = {
        {stndForest.getId(), stndForest.getName()},
        {migcForest.getId(), migcForest.getName()},
        {stndDesert.getId(), stndDesert.getName()}
    };
    unordered_map<int, Environment*> environmentObjects = {
        {stndForest.getId(), &stndForest},
        {migcForest.getId(), &migcForest},
        {stndDesert.getId(), &stndDesert}
    };


    while (true) {
        short battleChoice,chartChoice;
        string playerName;
        cout << "欢迎来到战斗模拟器！" << endl << endl;

        cout << "请输入玩家1名称：";
        getline(cin, playerName);
        Player p1(playerName);
        cout << "请输入玩家2名称：";
        getline(cin, playerName);
        Player p2(playerName);
        cout << endl << "角色已就绪！" << endl << "玩家1: " << p1.getName() << " vs 玩家2: " << p2.getName() << endl << endl;

        bool validChoice;
        while (true) {
            battleChoice = getSafeInt("选择游戏模式：1.『1 vs 1』2. 『2 vs 2』 3. 『3 vs 3』 4. 『退出游戏』：");
            if (battleChoice == 4) {
                cout << "退出游戏。" << endl;
                return 0;
            }
            else if (battleChoice < 1 || battleChoice > 4) {
                cout << "无效选择，请重新选择。" << endl;
            }
            else {
                break;
            }
        }

        short enmChoice;
        while (true){
            validChoice = false;
            cout << "选择战斗环境: ";
            for (int i = 0; i < EnvironmentIDList.size(); ++i) {
                cout << EnvironmentIDList[i] << ". " << environmentMap[EnvironmentIDList[i]] << " ";
            }
            enmChoice = getSafeInt("");    //已经存在提示信息，不再单独提示
            for (int id : EnvironmentIDList) {
                if (enmChoice == id) {
                    validChoice = true;
                }
            }
            if (!validChoice) {
                cout << "无效选择，请重新选择。" << endl;
                continue;
            }
            else {
                break;
            }
        }

        Environment* selectedEnv = environmentObjects[enmChoice];
        cout << "选择了 " << selectedEnv->getName();
        cout << "，攻击力+" << selectedEnv->getAtkPlus() 
             << "，防御力+" << selectedEnv->getDefPlus()
             << "，行动消耗" << selectedEnv->getActPointdemand();
        cout << endl << endl;

        battleNvsN(p1, p2, *selectedEnv, characterObjects, battleChoice);

        cout << "回合已结束,是否继续游戏？(y/n)：";
        char continueChoice;
        cin >> continueChoice;
        cin.ignore(); // 清除输入缓冲区
        if (continueChoice == 'y' || continueChoice == 'Y') {
            continue; // 进入下一轮
        } else {
            cout << "感谢游戏！" << endl;
            return 0;
        }
        cout << endl << endl;
    }
}
