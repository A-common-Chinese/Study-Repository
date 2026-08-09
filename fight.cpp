#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
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
public:
    int pastAtk;
    int pastDef;
    Character(string n, int h, int a, int d, int l) : name(n), id(chrNextID++), hp(h), maxHp(h), atk(a), def(d), hl(l), pastAtk(a), pastDef(d) {}
    virtual ~Character() {}
    virtual void attack(Character& target) = 0;
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
    Warrior(string n, int h, int a, int d, int l) : Character(n, h, a, d, l) {}
    void attack(Character& target) override {
        target.takeDamageT1(atk + 5);
    }
}; 

class Mage : public Character {
public:
    Mage(string n, int h, int a, int d, int l) : Character(n, h, a, d, l) {}
    void attack(Character& target) override {
        target.takeDamageT1(atk);
        int temp;
        temp = atk * 25 / 100;
        target.takeDamageT2(temp); // 额外伤害为攻击力的25%,但是取整数
    }
    void healAllies(vector<Character*>& allies) {
        for (auto& ally : allies) {
            if (ally->isAlive()) {
                ally->healSelf(hl);
            }
        }
    }
};

void endBattle(Player& p1, Player& p2) {
    cout << "战斗结束！" << endl;
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
    }
    for (const auto& character : p2.selfCharacter) {
        character->resetStats();  
    } 
    p1.clearCharacters();
    p2.clearCharacters();
};

int getSafeInt(const string& prompt);
void battleOneVSone(Player& p1, Player& p2, Environment& env, unordered_map<int, Character*>& characterObjects) {
    int atkBuff = env.getAtkPlus();
    int defBuff = env.getDefPlus();
    int p1ActPoint = 10;
    int p2ActPoint = 10;
    int actPointDemand = env.getActPointdemand();
    cout << p1.getName() << "，请选择你的角色(P1)：" << endl;
    for (const auto& pair : characterObjects) {
        if (pair.second->getName() != "God") { // GOD为隐藏彩蛋
            cout << pair.first << ". " << pair.second->getName() << endl;
        }
    }

    int choice;
    Character* c1 = nullptr;
    while (true) {
        choice = getSafeInt("输入角色ID：");
        auto it = characterObjects.find(choice);
        if (it != characterObjects.end()) {
            c1 = it->second;
            c1->loadEnvEff(atkBuff, defBuff);
            break;
        }
        cout << "无效ID，请重新选择。" << endl;
    }
    cout << p2.getName() << "，请选择你的角色(P2)：" << endl;
    for (const auto& pair : characterObjects) {
        if (pair.second->getName() != "God") { // GOD为隐藏彩蛋，如果玩家直接输入ID选择God，则可以使用隐藏彩蛋
            cout << pair.first << ". " << pair.second->getName() << endl;
        }
    }

    Character* c2 = nullptr;
    while (true) {
        choice = getSafeInt("输入角色ID：");
        auto it = characterObjects.find(choice);
        if (it != characterObjects.end()) {
            c2 = it->second;
            c2->loadEnvEff(atkBuff, defBuff);
            break;
        }
        cout << "无效ID，请重新选择。" << endl;
    }
    p1.addCharacter(c1);
    p2.addCharacter(c2);

    while (c1->isAlive() && c2->isAlive()) {
        while (true) {
            if (!c1->isAlive()) {
                break;
            }
            cout << p1.getName() << "的回合!" << "行动点: " << p1ActPoint << endl;
            int actionChoice = getSafeInt("选择行动: 1. 攻击 2. 治疗 3. 结束回合");
            if (actionChoice == 1) {
                if (p1ActPoint >= actPointDemand) {
                    c1->attack(*c2);
                    p1ActPoint -= actPointDemand;
                    cout << c1->getName() << " 攻击了 " << c2->getName() << "!" << endl;
                    cout << c2->getName() << " 的剩余HP: " << c2->getHp() << "/" << c2->getMaxHp() << endl;
                    
                } else {
                    cout << "行动点不足，无法攻击。" << endl;
                }
                if (!c2->isAlive()) {
                    cout << c2->getName() << " 被击败了！" << endl;
                    break;
                }
            } else if (actionChoice == 2) {
                if (p1ActPoint >= actPointDemand) {
                    c1->healSelf(c1->getHealAmount());
                    p1ActPoint -= actPointDemand;
                    cout << c1->getName() << " 治疗了自己 " << c1->getHealAmount() << " 点 HP！" << endl;
                    cout << c1->getName() << " 的剩余HP: " << c1->getHp() << "/" << c1->getMaxHp() << endl;
                } else {
                    cout << "行动点不足，无法治疗。" << endl;
                }
            } else if (actionChoice == 3) {
                cout << p1.getName() << " 结束了回合。" << endl;
                p1ActPoint += 10; // 重置行动点
                break; // 结束回合后跳出循环，进入玩家2的回合
            } else {
                cout << "无效选择，请重新选择。" << endl;
                continue;
            }
        }
        while (true) {
            if (!c2->isAlive()) {
                break;
            }
            cout << p2.getName() << "的回合!" << "行动点: " << p2ActPoint << endl;
            int actionChoice = getSafeInt("选择行动: 1. 攻击 2. 治疗 3. 结束回合");
            if (actionChoice == 1) {
                if (p2ActPoint >= actPointDemand) {
                    c2->attack(*c1);
                    p2ActPoint -= actPointDemand;
                    cout << c2->getName() << " 攻击了 " << c1->getName() << "!" << endl;
                    cout << c1->getName() << " 的剩余HP: " << c1->getHp() << "/" << c1->getMaxHp() << endl;
                } else {
                    cout << "行动点不足，无法攻击。" << endl;
                }
                if (!c1->isAlive()) {
                    cout << c1->getName() << " 被击败了！" << endl;
                    break;
                }
            } else if (actionChoice == 2) {
                if (p2ActPoint >= actPointDemand) {
                    c2->healSelf(c2->getHealAmount());
                    p2ActPoint -= actPointDemand;
                    cout << c2->getName() << " 治疗了自己 " << c2->getHealAmount() << " 点 HP！" << endl;
                    cout << c2->getName() << " 的剩余HP: " << c2->getHp() << "/" << c2->getMaxHp() << endl;
                } else {
                    cout << "行动点不足，无法治疗。" << endl;
                }
            } else if (actionChoice == 3) {
                cout << p2.getName() << " 结束了回合。" << endl;
                p2ActPoint += 10; // 重置行动点
                break; // 结束回合后跳出循环，回到玩家1的回合
            } else {
                cout << "无效选择，请重新选择。" << endl;
                continue;
            }
        }
    }
    endBattle(p1, p2);
};

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
    Environment stndForest("Forest", 0, 0, 1);
    Environment migcForest("Magic Forest", 5, 2, 1);
    Warrior arthur("King Arthur", 100, 20, 10, 5);
    Mage merlin("Merlin", 80, 25, 5, 10);
    Mage OPcharacter("God", 1000, 100, 50, 20);
    vector<int> CharacterIDList = {arthur.getId(), merlin.getId(), OPcharacter.getId()};
    vector<int> EnvironmentIDList = {stndForest.getId(), migcForest.getId()};
    unordered_map<int, string> characterMap = {
        {arthur.getId(), arthur.getName()},
        {merlin.getId(), merlin.getName()},
        {OPcharacter.getId(), OPcharacter.getName()}
    };
    unordered_map<int, Character*> characterObjects = {
        {arthur.getId(), &arthur},
        {merlin.getId(), &merlin},
        {OPcharacter.getId(), &OPcharacter}
    };
    unordered_map<int, string> environmentMap = {
        {stndForest.getId(), stndForest.getName()},
        {migcForest.getId(), migcForest.getName()}
    };
    unordered_map<int, Environment*> environmentObjects = {
        {stndForest.getId(), &stndForest},
        {migcForest.getId(), &migcForest}
    };


    while (true) {
        short battleChoice,chartChoice;
        string playerName;
        cout << "欢迎来到战斗模拟器！" << endl;

        cout << "请输入玩家1名称：";
        getline(cin, playerName);
        Player p1(playerName);
        cout << "请输入玩家2名称：";
        getline(cin, playerName);
        Player p2(playerName);

        bool validChoice;
        while (true) {
            battleChoice = getSafeInt("选择游戏模式：1.『1 vs 1』 2. 『退出游戏』：");
            if (battleChoice == 2) {
                cout << "退出游戏。" << endl;
                return 0;
            }
            else if (battleChoice < 1 || battleChoice > 2) {
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
            enmChoice = getSafeInt("");    //已经存在提升信息，不再单独提示
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
        if (selectedEnv->getAtkPlus() != 0 || selectedEnv->getDefPlus() != 0) {
            cout << "，攻击力+" << selectedEnv->getAtkPlus() 
                << "，防御力+" << selectedEnv->getDefPlus();
        }
        cout << endl;

        if (battleChoice == 1) {
            battleOneVSone(p1, p2, *selectedEnv, characterObjects);
        }
    }
}