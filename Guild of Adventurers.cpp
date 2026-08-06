#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include <limits>
#include <fstream>


using namespace std;

class Mission {
public:
    Mission(string n, int r,short i){
        name = n;
        reward = r;
        id = i;
    }

    void print(){
        cout << name << "报酬：" << reward << endl;
    }

    string name;
    int reward;
    short id;
};

bool save(vector<Mission>& missions, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "无法打开文件进行保存！" << endl;
        return false;
    }
    for (const auto& mission : missions) {
        outFile << mission.name << "|" << mission.reward << "|" << mission.id << endl;
    }
    outFile << "*" << endl;
    outFile.close();
    return true;
}
vector<Mission> load( const string& filename, bool saveSpace[]) {
    ifstream inFile(filename);
    if (inFile) {
        cout << "检测到已保存文件，已加载任务数据" << endl;
    }
    else{  
        cout << "未检测到已保存文件，已创建新文件" << endl;
        ofstream newFile(filename);
        newFile << "*" << endl;
        newFile.close();
        inFile.open(filename);
        }
    vector<Mission> temp;
    string line;
    while (getline(inFile,line) && line != "*") {
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos1 != string::npos && pos2 != string::npos) {
            string name = line.substr(0, pos1);
            int reward = stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
            short id = stoi(line.substr(pos2 + 1));
            temp.emplace_back(name, reward, id);
            saveSpace[id - 1] = true;
        }
    }
    inFile.close();
    return temp;
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

const int MAX_MISSIONS = 1000;

int main(){
    vector<Mission> unfishedMission;
    bool saveSpace[MAX_MISSIONS] = {};
    string name;
    int reward,SW1,len;
    unfishedMission = load("missions.txt", saveSpace);
    while (true){
        cout << "任务编辑：1" << endl << "查询现有任务：2" << endl << "退出程序：3" << endl << "手动保存：4" << endl;
        SW1 = getSafeInt("输入数字以操作：");
        cout << endl << endl;
        if (SW1 == 1){
            int SW2;
            SW2 = getSafeInt("输入数字以发布任务(1)或完成任务(2)或修改任务(3):");
            cout << endl;
            if (SW2 == 1){
                cout << "任务名称：";
                getline(cin, name);
                reward = getSafeInt("任务报酬：");
                cout << endl;
                int id = 1;
                while(id <= MAX_MISSIONS && saveSpace[id - 1]){
                    id++;
                }     
                if (id > MAX_MISSIONS){
                    cout << "任务添加失败，任务列表已满" << endl;
                    continue;
                }
                else{         
                    Mission newMission(name,reward,id);
                    saveSpace[id - 1] = true;
                    unfishedMission.push_back(newMission);
                    cout << "任务[" << name << "]发布成功! 任务ID：" << id << endl;
                }
            }
            else if (SW2 == 2){
                cout << "输入任务名称或ID（若有重复名称，将会删除第一个）";
                string input;
                char val;
                getline(cin, input);
                int k = 0;
                for (int i = 0;i < input.length();i++){
                    val = input[i];
                    if (val >= '0' && val <= '9'){
                        k++;
                    }
                }
                if (k == static_cast<int>(input.length())){
                    int targetId;
                    targetId = stoi(input);
                    auto it = find_if(unfishedMission.begin(),unfishedMission.end(),
                        [&](const Mission& p) {return p.id == targetId;});
                    if (it != unfishedMission.end()){
                        short delId = it->id;
                        unfishedMission.erase(it);
                        saveSpace[delId - 1] = false;
                        cout << "任务结算成功" << endl;
                    }
                    else{
                        cout << "未找到任务，请检查输入" << endl;
                    }
                }
                else{
                    auto it = find_if(unfishedMission.begin(),unfishedMission.end(),
                        [&](const Mission& p) {return p.name == input;});
                    if (it != unfishedMission.end()){
                        short delId = it->id;
                        unfishedMission.erase(it);
                        saveSpace[delId - 1] = false;
                        cout << "任务结算成功" << endl;
                    }
                    else{
                        cout << "未找到任务，请检查输入" << endl;
                    }
                }
            }
            else if (SW2 == 3){
                int targetId;
                targetId = getSafeInt("输入任务ID：");
                auto it = find_if(unfishedMission.begin(),unfishedMission.end(),
                    [&](const Mission& p) {return p.id == targetId;});
                if (it != unfishedMission.end()){
                    cout << "任务名称：" << it->name << "  报酬：" << it->reward << endl;
                    cout << "输入新的任务名称：";
                    getline(cin, name);
                    reward = getSafeInt("输入新的任务报酬：");
                    it->name = name;
                    it->reward = reward;
                    cout << "任务修改成功" << endl;
                }
                else{
                    cout << "未找到任务，请检查输入" << endl;
                }
            }
        }
        else if (SW1 == 2){
            int SW2;
            SW2 = getSafeInt("选择排序方法：1.按ID升序 2.按ID降序 3.按报酬升序 4.按报酬降序 5.按名称排序 :");
            vector<Mission> temp = unfishedMission;
            if (SW2 == 1){
                sort(temp.begin(), temp.end(), [](const Mission& a, const Mission& b) {
                    return a.id < b.id;
                });
            }
            else if (SW2 == 2){
                sort(temp.begin(), temp.end(), [](const Mission& a, const Mission& b) {
                    return a.id > b.id;
                });
            }
            else if (SW2 == 3){
                sort(temp.begin(), temp.end(), [](const Mission& a, const Mission& b) {
                    return a.reward < b.reward;
                });
            }
            else if (SW2 == 4){
                sort(temp.begin(), temp.end(), [](const Mission& a, const Mission& b) {
                    return a.reward > b.reward;
                });
            }
            else if (SW2 == 5){
                sort(temp.begin(), temp.end(), [](const Mission& a, const Mission& b) {
                    return a.name < b.name;
                });
            }
            len = temp.size();
            for(int i = 0;i < len;i++){
                cout << "任务：" << temp[i].name << "  报酬：" << temp[i].reward << "  ID：" << temp[i].id << endl;
            }
            cout << "所有任务打印完成，总数：" << len << endl << endl;
        }
        else if (SW1 == 3){
            if (!save(unfishedMission, "missions.txt")) {
                cerr << "保存失败！是否直接退出？(y/n)" << endl;
                char choice;
                cin >> choice;
                if (choice == 'y' || choice == 'Y') {
                    break;
                }
            }
            else {
                cout << "任务已保存" << endl;
                break;
            }   
        }
        else if (SW1 == 4){
            if (!save(unfishedMission, "missions.txt")) {
                cerr << "保存失败！" << endl;
            }
            else {
                cout << "任务已保存" << endl;
            }   
        }
        else{
            cout << "输入错误，请重新输入" << endl;
        }
        cout << endl;
    }
    cout << "程序退出" << endl;
}
