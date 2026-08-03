#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include <limits>


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
    int reward,SW,len;
    while (true){
        cout << "任务编辑：1" << endl << "查询现有任务：2" << endl << "退出程序：3" << endl;
        SW = getSafeInt("输入数字以操作：");
        cout << endl << endl;
        if (SW == 1){
            SW = getSafeInt("输入数字以发布任务(1)或完成任务(2):");
            cout << endl;
            if (SW == 1){
                cout << "任务名称：";
                cin.ignore(10000, '\n');
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
            else if (SW == 2){
                cout << "输入任务名称或ID（若有重复名称，将会删除第一个）";
                string input;
                char val;
                cin.ignore(10000, '\n');  // 清空到换行符
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
        }
        else if (SW == 2){
            len = unfishedMission.size();
            for(int i = 0;i < len;i++){
                cout << "任务：" << unfishedMission[i].name << "  报酬：" << unfishedMission[i].reward << "  ID：" << unfishedMission[i].id << endl;
            }
            cout << "所有任务打印完成，总数：" << len << endl << endl;
        }
        else if (SW == 3){
            break;
        }
        cout << endl;
    }
    cout << "程序退出" << endl;
}
