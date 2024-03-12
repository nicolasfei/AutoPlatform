#ifndef PARAMETERTABLE_H
#define PARAMETERTABLE_H

#include "parameterclass.h"

/**
 * @brief The ParameterTable class--描述一个参数表
 */
class ParameterTable
{
public:
    /**
     * @brief ParameterTable
     * @param addrOffset---AddrOffset偏移地址值
     * @param wrOffset---WR寄存器ID偏移地址
     * @param rOffset---R寄存器ID偏移地址
     * @param wrFpgaOffset---FPGA（上位机WR）偏移地址
     * @param rFpgaOffset---FPGA（上位机R）偏移地址
     * @param filePath---参数文件路径
     * @param cabinetID---机柜id
     */
    ParameterTable(int addrOffset, int wrOffset, int rOffset,
                                 int wrFpgaOffset,int rFpgaOffset,QString filePath, int cabinetID);
    ~ParameterTable();

    QStringList getHeaderLabels();
    bool updateTableMap2File();

public:
    QStringList headerLabels;
    QString tableFilePath;
    QMap<QString, QList<ParameterClass*>*>* tableMap;
    QList<ParameterClass*>* m_list;
    bool isHaveError;
};

#endif // PARAMETERTABLE_H
