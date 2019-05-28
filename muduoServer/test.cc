#include <iostream>
#include "Nocopyable.h"

class me : public Nocopyable {
public:
    me() { std::cout <<"111111111111111111"<< std::endl; }
    me(int i) { std::cout <<"hello"<< std::endl; }
};
int main()
{
    me c1(4);
    me c2();
//    me c3(c1);
//    me c4 = c1;
    
    std::cout << "Hello world" << std::endl;
    return 0;
}
