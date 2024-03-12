#ifndef DIODATACLASS_H
#define DIODATACLASS_H

#include <QString>

class DioDataClass{

public:
    DioDataClass()
    {

    }

    DioDataClass(bool forceDio,bool status,QString name,ushort rid)
    {
        this->forceDio = forceDio;
        this->status = status;
        this->name = name;
        this->rid = rid;
    }

    bool getStatus(){
        return status;
    }

    void setStatus(bool status){
        this->status = status;
    }

    QString getName(){
        return name;
    }

    void setName(QString name){
        this->name = name;
    }

    bool getForceDio(){
        return forceDio;
    }

    void setForceDio(bool force){
        this->forceDio = force;
    }

    ushort getRid(){
        return this->rid;
    }

    void setRid(ushort rid){
        this->rid = rid;
    }

    bool getCheckDio(){
        return this->checkDio;
    }

    void setCheckDio(bool checked){
        this->checkDio = checked;
    }
private:
    bool forceDio;      //是否是强制ID模式
    bool checkDio;      //是否被选中
    bool status;        //指示灯状态--开/关
    QString name;
    ushort rid;         //寄存器地址
};

#endif // DIODATACLASS_H
