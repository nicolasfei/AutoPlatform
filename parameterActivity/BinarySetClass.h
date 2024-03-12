#ifndef BINARYSETCLASS_H
#define BINARYSETCLASS_H

#include "common/cabinetmanagement.h"

#include <QString>
#include <QBitArray>

class BinarySetClass
{
public:
    BinarySetClass(uint32_t value, int validBit, QStringList describe)
    {
        this->validBitCount = qMin(32, validBit);
        bitValue.resize(validBitCount);
        for(int i=0; i<validBitCount; i++){
            bitValue.setBit(i, value>>i&0x01);
        }
        this->describe = describe;//
        this->isChange=false;
    }

public:
    QBitArray bitValue;
    QStringList describe;
    int validBitCount;      //有效位数
    bool isChange;

public:
    void updateBitValue(int index, bool bit)
    {
        if(index>this->validBitCount){
            return;
        }
        if(bitValue.at(index)==bit){
            return;
        }
        bitValue.setBit(index, bit);
        isChange=true;
    }

    void updateDescribe(int index, QString des)
    {
        if(index>this->validBitCount){
            return;
        }
        if(describe.at(index) == des){
            return;
        }
        describe.removeAt(index);
        describe.insert(index, des);
        isChange=true;
    }

    int validBit()
    {
        return this->validBitCount;
    }

    bool isUpdate()
    {
        return isChange;
    }

    uint32_t getBitValue()
    {
        uint32_t value=0;
        for(int i=0; i<validBitCount; i++){
            value = value<<1|(bitValue[validBitCount-1-i]?0x01:0);
        }
        return value;
    }

    QStringList getDescribe()
    {
        return describe;
    }
};
#endif // BINARYSETCLASS_H
