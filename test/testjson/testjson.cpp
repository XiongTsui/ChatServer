#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
using namespace std;

//序列化代码 1
string func1(){
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "lisi";
    js["msg"] = "hello , whats wrong?";

    cout << js << endl;
    return js.dump();
}

//序列化代码 2
string func2(){
    json js;
    //添加数组
    js["id"] = {1,2,3,4,5};

    js["name"] = "zhangsan";

    js["msg"]["lisi"] = " i am lisi";

    js["msg"]["zhaosi"] = " i am zhaosi";

    js["msg1"] = {{"wangwu", "i am wangwu"} , {"liuqi" , "i am liuqi"}};
    //cout << js << endl;
    return js.dump();
}

//序列化代码 3
void func3(){
    json js;

    //序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    js["list"] = vec;

    //序列化一个map容器
    map<int , string> m;
    m.insert({1,"黄山"});
    m.insert({2,"岳山"});
    m.insert({3,"华山"});
    m.insert({4,"泰山"});

    js["monton"] = m;

    string aim = js.dump();

    cout << aim <<endl;

}

int main(){
    string recvBUf = func2();
    //数据的反序列化
    json jsbuf = json::parse(recvBUf);
    cout << jsbuf["id"]<<endl;
    cout << jsbuf["msg"] <<endl;
    cout << jsbuf["msg1"] <<endl;

    auto arr = jsbuf["id"];
    cout << arr[2] <<endl;
    return 0;
}