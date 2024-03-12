#ifndef AIODATACLASS_H
#define AIODATACLASS_H

#include <QString>

class AioDataClass{

public:
    AioDataClass()
    {

    }

    AioDataClass(QString name, ushort rid, QString value)
    {
        AioDataClass();
        this->name = name;
        this->rid = rid;
        this->value = value;
    }

    QString getName(){
        return name;
    }

    void setName(QString name){
        this->name = name;
    }

    ushort getRid(){
        return this->rid;
    }

    void setRid(ushort rid){
        this->rid = rid;
    }

    QString getValue(){
        return this->value;
    }

    void setValue(QString value){
        this->value = value;
    }

private:
    QString name;       //变量名字
    ushort rid;         //寄存器地址
    QString value;      //值
public:
    static const int itemCount=2;      //元素个数
};
#endif // AIODATACLASS_H
