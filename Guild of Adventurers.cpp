#include<iostream>
#include<vector>
#include<string>
#include<algorithm>


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

int main(){
    vector<Mission> unfishedMission;
    bool saveSpace[1000] = {};
    string name;
    int reward,SW,len;
    while (true){
        cout << "任务编辑：1" << endl << "查询现有任务：2" << endl << "退出程序：3" << endl << "输入数字以操作：";
        cin >> SW;
        cout << endl << endl;
        if (SW == 1){
            cout << "输入数字以发布任务(1)或完成任务(2):";
            cin >> SW;
            cout << endl;
            if (SW == 1){
                cout << "任务名称：";
                cin >> name;
                cout << "任务报酬：";
                cin >> reward;
                cout << endl;
                int id = 1;
                while(id <= 1000 && saveSpace[id - 1]){
                    id++;
                }     
                if (id > 1000){
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
                len = unfishedMission.size();
                cout << "输入任务名称或ID（若有重复名称，将会删除第一个）";
                string input;
                char val;
                cin >> input;
                int k = 0;
                for (int i = 0;i < len;i++){
                    val = input[i];
                    if (val >= '0' && val <= '9'){
                        k++;
                    }
                }
                if (k == len){
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
