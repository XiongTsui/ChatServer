#include <iostream>
using namespace std;

class test
{
private:
    /* data */
public:
    test(/* args */){
        cout << "fa" << endl;
    };
    ~test(){
        cout << "fa over" << endl;
    };
};

class test1:public test
{
private:
    /* data */
public:
    test1(/* args */){
        cout << "son1" << endl;
    };
    ~test1(){
        cout << "son1 over" << endl;
    };
};


int main(){
    test1* t1 = new test1();
    test1* t2 = new test1();
    delete t1;
    delete t2;
}




