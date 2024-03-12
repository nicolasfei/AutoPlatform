#ifndef PARAMETERCLASS_H
#define PARAMETERCLASS_H

#include <QString>
#include <QVariant>

class ParameterClass
{
public:
    typedef enum {
        VOID_Type=0,
        STRING_Type=1,
        INT16_Type=5,
        UINT16_Type=6,
        INT32_Type=3,
        UINT32_Type=4,
        FLOAT32_Type=7,
    }DataTypeFlags;

    typedef union Paramet_Value{
        void* voidValue;        //当前值
        uint8_t stringValue[4]; //当前值
        uint32_t uintValue;     //当前值
        int32_t intValue;       //当前值
        uint16_t ushortValue;   //当前值
        int16_t shortValue;     //当前值
        float_t floatValue;     //当前值
        uint32_t bitSetValue;   //标志用于参数修改的bitSet--32位仅仅对于整形变量有意义  //by bruce
        uint16_t valueBuf[2];    //buf
    }DataValue;

    //传递给modbus下发到下位机的数据
    typedef struct Register_Data{
        uint16_t addr;       //寄存器地址
        uint16_t fpgaID;     //fpga地址
        int nb;         //寄存器数量
        DataValue src;  //写寄存器
        DataValue des;  //读寄存器
        char rwFlags;   //读写权限
        int rw;         //读写标志 0-读 1-写
        DataTypeFlags type; //数据类型
        int cabinetID;  //机柜编号
    }RegisterData;

    DataTypeFlags DTypeFlags;


    typedef enum{
        READ_ONLY=0,
        READ_WRITE,
    }RWFlags;
    RWFlags rwFlags;        //读写biao标志位

    QString addOffse;   //地址偏移
    QString group;      //组ID
    QString item;       //itemID
    QString chName;     //通道名称
    QString unit;       //单位

    QChar type;       //数据类型（0--void*, 1--char*, 3--无符号整形, 4--有符号整形, 5--无符号short, 6--有符号short, 7--浮点型 ）
    QString typeData;   //数据类型完整名字

    QString value;          //当前值
    DataValue m_dataValue;    //值
    QString min;            //最小值
    DataValue minValue;     //值
    QString max;            //最大值
    DataValue maxValue;     //值

    QString Enname;
    QString rw;             //上位机读写类型（R--只读, RW--读写）
    QString rw2;            //下位机读写类型（R--只读, RW--读写）
    uint16_t registerId;     //寄存器ID
    uint16_t fpgaID;         //fpgaID
    QStringList displayRole;    //用于表格显示

    RegisterData regData;
    bool isOsc=false;   //是否加入示波器
    bool custom=false;    //是否加入自定义列表
    int cabinetID;      //机柜编号

    static const int classCount;
    static const QChar VOID_DT;
    static const QChar STRING_DT;
    static const QChar INT16_DT;
    static const QChar UINT16_DT;
    static const QChar INT32_DT;
    static const QChar UINT32_DT;
    static const QChar FLOAT32_DT;

    static const QString READ_ONLY_DT;
    static const QString READ_WRITE_DT;

    int addrOffset, wrOffset, rOffset, wrFpgaOffset, rFpgaOffset;
public:
    ParameterClass(int addrOffset, int wrOffset, int rOffset,
                   int wrFpgaOffset,int rFpgaOffset, QString line);
    RegisterData getRegisterData();
    void updateRWType(QString rw);
    QString getText();
    void setOscilloscope(bool os);
    void setCustom(bool cu);
    bool isAddOscilloscope();
    bool isAddCustom();
    void setCabinetID(int cabinetID);
    QString getDataTypeName();
    void updateValue(QString value);
};

#endif // PARAMETERCLASS_H
