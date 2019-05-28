#pragma once
class Nocopyable {
protected:
    Nocopyable() {  }
    ~Nocopyable() {  }
private:
    Nocopyable(const Nocopyable&) {  }
    const Nocopyable& operator+(const Nocopyable&);   
};
//基本思想：把构造函数和析构函数设置protected权限，这样子类可以调用
//但是外边的类不能调用。
//关键是把复制构造函数和复制赋值函数做成了private，这就意味着除非子类自己
//定义的copy构造和赋值函数，否则在子类没有定义的情况下，外面的调用者
//是不能通过赋值和拷贝构造等手段来产生一个新的子类对象的。

