#include "parameterclass.h"
#include "common/commonclass.h"
#include <string.h>
#include <stdlib.h>
#include <QStringList>
#include <QDebug>

//数据类型（0--void*, 1--char*, 3--无符号整形, 4--有符号整形, 5--无符号short, 6--有符号short, 7--浮点型 ）
const QChar ParameterClass::VOID_DT = '0';
const QChar ParameterClass::STRING_DT = '1';
const QChar ParameterClass::INT16_DT = '5';
const QChar ParameterClass::UINT16_DT = '6';
const QChar ParameterClass::INT32_DT = '4';
const QChar ParameterClass::UINT32_DT = '3';
const QChar ParameterClass::FLOAT32_DT = '7';
const QString ParameterClass::READ_ONLY_DT = QString::fromLocal8Bit("R");
const QString ParameterClass::READ_WRITE_DT = QString::fromLocal8Bit("WR");
const int ParameterClass::classCount = 13;

/**
 * @brief ParameterClass
 * @param addrOffset---AddrOffset偏移地址值
 * @param wrOffset---WR寄存器ID偏移地址
 * @param rOffset---R寄存器ID偏移地址
 * @param wrFpgaOffset---FPGA（上位机WR）偏移地址
 * @param rFpgaOffset---FPGA（上位机R）偏移地址
 * @param line---参数数据
 */
ParameterClass::ParameterClass(int addrOffset, int wrOffset, int rOffset,
                               int wrFpgaOffset,int rFpgaOffset, QString line)
{
    this->addrOffset = addrOffset;
    this->wrOffset = wrOffset;
    this->rOffset = rOffset;
    this->wrFpgaOffset = wrFpgaOffset;
    this->rFpgaOffset = rFpgaOffset;
    if(!line.isEmpty())
    {
        //qDebug() << "ParameterClass is "<< line << endl;
        displayRole = line.split(",");//一行中的单元格以，区分
        if(displayRole.length()<14)
        {
            MY_DEBUG << "Parameter error:"<<displayRole <<" length is " << displayRole.length() << endl;
            return;
        }
        else
        {
            int i=0;
            bool ok=false;
            if(i<displayRole.size())
                addOffse = displayRole.at(i++).trimmed();
            addOffse = QString::number(addOffse.toUInt(&ok)+addrOffset);

            if(i<displayRole.size())
                group = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                item = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                chName = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                unit = displayRole.at(i++).trimmed();

            if(i<displayRole.size()){
                typeData = displayRole.at(i++).trimmed();
                type = typeData.at(0);
            }
            if(i<displayRole.size())
                value = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                min = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                max = displayRole.at(i++).trimmed();

            if(i<displayRole.size())
                Enname = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                rw = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                rw2 = displayRole.at(i++).trimmed();
            if(i<displayRole.size())
                registerId = displayRole.at(i++).trimmed().toUShort();
            if(i<displayRole.size())
                fpgaID = displayRole.at(i++).trimmed().toUShort();
            if(i<displayRole.length()){
                if(i<displayRole.size())
                    isOsc = displayRole.at(i++).trimmed().compare("TRUE", Qt::CaseInsensitive) == 0;        //不区分大小写
                if(i<displayRole.size())
                    custom = displayRole.at(i++).trimmed().compare("TRUE", Qt::CaseInsensitive) == 0;
            }else{
                isOsc=false;
                custom=false;
            }

            bool isOK;
            if(this->type==(VOID_DT)){
                this->DTypeFlags = VOID_Type;
            }else if(this->type==(STRING_DT)){
                this->DTypeFlags = STRING_Type;
            }else if(this->type==(INT16_DT)){
                this->DTypeFlags = INT16_Type;
                this->m_dataValue.shortValue = this->value.toShort(&isOK);
                this->minValue.shortValue = this->min.toShort();
                this->maxValue.shortValue = this->max.toShort();
//                if(isOK)
//                    this->bitSetValue = m_dataValue.shortValue;
            }else if(this->type==(UINT16_DT)){
                this->DTypeFlags = UINT16_Type;
                this->m_dataValue.ushortValue = this->value.toUShort(&isOK);
                this->minValue.ushortValue = this->min.toUShort();
                this->maxValue.ushortValue = this->max.toUShort();
//                if(isOK){
//                    this->bitSetValue = m_dataValue.ushortValue;
//                }
            }else if(this->type==(INT32_DT)){
                this->DTypeFlags = INT32_Type;
                this->m_dataValue.intValue = this->value.toInt(&isOK);
                this->minValue.intValue = this->min.toInt();
                this->maxValue.intValue = this->max.toInt();
//                if(isOK)
//                    this->bitSetValue = this->m_dataValue.intValue&0xFFFF;
            }else if(this->type==(UINT32_DT)){
                this->DTypeFlags = UINT32_Type;
                this->m_dataValue.uintValue = this->value.toUInt(&isOK);
                this->minValue.uintValue = this->min.toUInt();
                this->maxValue.uintValue = this->max.toUInt();
//                if(isOK){
//                    this->bitSetValue = this->m_dataValue.uintValue&0xFFFF;
//                }
            }else if(this->type==(FLOAT32_DT)){
                this->DTypeFlags = FLOAT32_Type;
                this->m_dataValue.floatValue = this->value.toFloat(&isOK);
                this->minValue.floatValue = this->min.toFloat();
                this->maxValue.floatValue = this->max.toFloat();
            }

            if(this->rw.compare("R")==0){
                this->rwFlags = READ_ONLY;
                this->registerId+=rOffset;
                this->fpgaID+=rFpgaOffset;
            }else{
                this->rwFlags = READ_WRITE;
                this->registerId+=wrOffset;
                this->fpgaID+=wrFpgaOffset;
            }


            //初始化寄存器变量
            memset(&regData,0,sizeof(RegisterData));
            regData.addr = this->registerId;
            regData.type = this->DTypeFlags;
            regData.fpgaID = this->fpgaID;
            regData.cabinetID = this->cabinetID;
            regData.rwFlags = this->rwFlags==READ_ONLY?0:1;
            regData.src = this->m_dataValue;
            switch (this->DTypeFlags){
            case ParameterClass::VOID_Type:
                regData.nb = 2;
                break;
            case ParameterClass::STRING_Type:
                regData.nb = 2;
                break;
            case ParameterClass::FLOAT32_Type:
                regData.nb = 2;
                break;
            case ParameterClass::UINT32_Type:
                regData.nb = 2;
                break;
            case ParameterClass::INT32_Type:
                regData.nb = 2;
                break;
            case ParameterClass::UINT16_Type:
                regData.nb = 1;
                break;
            case ParameterClass::INT16_Type:
                regData.nb = 1;
                break;
            default:
                break;
            }
        }
    }
}

void ParameterClass::updateValue(QString value)
{
    bool isOK=false;
    this->value = value;
    switch(this->DTypeFlags){
    case VOID_Type:
        this->m_dataValue.uintValue = this->value.toUInt(&isOK);
//        if(isOK){
//            this->bitSetValue = this->m_dataValue.uintValue&0xFFFF;
//        }
        break;
    case STRING_Type:
        this->m_dataValue.uintValue = this->value.toUInt(&isOK);
//        if(isOK){
//            this->bitSetValue = this->m_dataValue.uintValue&0xFFFF;
//        }
        break;
    case INT16_Type:
        this->m_dataValue.shortValue = this->value.toShort(&isOK);
//        if(isOK)
//            this->bitSetValue = m_dataValue.shortValue;
        break;
    case UINT16_Type:
        this->m_dataValue.ushortValue = this->value.toUShort(&isOK);
//        if(isOK){
//            this->bitSetValue = m_dataValue.ushortValue;
//        }
        break;
    case INT32_Type:
        this->m_dataValue.intValue = this->value.toInt(&isOK);
//        if(isOK)
//            this->bitSetValue = this->m_dataValue.intValue&0xFFFF;
        break;
    case UINT32_Type:
        this->m_dataValue.uintValue = this->value.toUInt(&isOK);
//        if(isOK){
//            this->bitSetValue = this->m_dataValue.uintValue&0xFFFF;
//        }
        break;
    case FLOAT32_Type:
        this->m_dataValue.floatValue = this->value.toFloat(&isOK);
        break;
    default:
        break;
    }

    regData.src = this->m_dataValue;    //更新寄存器值
}

ParameterClass::RegisterData ParameterClass::getRegisterData()
{
    return this->regData;
}

QString ParameterClass::getText()
{
    QString result;
    result.append(QString::number((addOffse.toInt()-this->addrOffset))).append(",")
            .append(group).append(",")
            .append(item).append(",")
            .append(chName).append(",")
            .append(unit).append(",")
            .append(typeData).append(",")
            .append(value).append(",")
            .append(min).append(",")
            .append(max).append(",")
            .append(Enname).append(",")
            .append(rw).append(",")
            .append(rw2).append(",");
    if(this->rwFlags==READ_ONLY){
        result.append(QString::number(registerId-rOffset)).append(",")
                .append(QString::number(fpgaID-rFpgaOffset)).append(",");
    }else{
        result.append(QString::number(registerId-wrOffset)).append(",")
                .append(QString::number(fpgaID-wrFpgaOffset)).append(",");
    }
    result.append(isOsc?"TRUE":"FALSE").append(",")
            .append(custom?"TRUE":"FALSE").append("\n");
    return result;/*addOffse+","+group+","+item+","+chName+","+unit
            +","+type+","+value+","+min+","+max+","+Enname+","+rw
            +","+rw2+","+QString::number(registerId)+","+QString::number(fpgaID)
            +","+(isOsc?"TRUE":"FALSE")+","+(custom?"TRUE":"FALSE")+"\n";*/
}

void ParameterClass::updateRWType(QString rw)
{
    if(rw.compare("r")==0){
        this->rw = "R";
        this->rwFlags = READ_ONLY;
    }else if(rw.compare("rw")==0){
        this->rw = "RW";
        this->rwFlags = READ_WRITE;
    }
}

void ParameterClass::setOscilloscope(bool os)
{
    this->isOsc=os;
}

void ParameterClass::setCustom(bool value)
{
    this->custom = value;
}

bool ParameterClass::isAddOscilloscope()
{
    return this->isOsc;
}

bool ParameterClass::isAddCustom()
{
    return this->custom;
}

void ParameterClass::setCabinetID(int cabinetID)
{
    this->cabinetID = cabinetID;
}

QString ParameterClass::getDataTypeName()
{
    QString value = QString::fromLocal8Bit("未知");
    switch (this->DTypeFlags)
    {
    case ParameterClass::VOID_Type:
        value = QString::fromLocal8Bit("无类型");
        break;
    case ParameterClass::STRING_Type:
        value = QString::fromLocal8Bit("字符串型");
        break;
    case ParameterClass::FLOAT32_Type:
        value = QString::fromLocal8Bit("浮点型");
        break;
    case ParameterClass::UINT32_Type:
        value = QString::fromLocal8Bit("无符号整型");
        break;
    case ParameterClass::INT32_Type:
        value = QString::fromLocal8Bit("整型");
        break;
    case ParameterClass::UINT16_Type:
        value = QString::fromLocal8Bit("无符号短整形");
        break;
    case ParameterClass::INT16_Type:
        value = QString::fromLocal8Bit("短整形");
        break;
    default:
        value = QString::fromLocal8Bit("未知");
        break;
    }
    return value;
}
