#ifndef FAULTALARMDIALOG_H
#define FAULTALARMDIALOG_H

#include <QDialog>
#include "common/parameterclass.h"
#include "communication/businessresponse.h"

namespace Ui {
class FaultAlarmDialog;
}

class FaultAlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FaultAlarmDialog(QWidget *parent = nullptr);
    ~FaultAlarmDialog();

    void setShowData(QList<ParameterClass*> data);
private:
    Ui::FaultAlarmDialog *ui;
    QList<ParameterClass*> m_data;
    ParameterClass* currentPc;      //当前选中的参数
    QStringList currentDes;         //当前选中参数的位描述定义

    void showParameterInfo(uint32_t var);       //显示当前选中的信息

private slots:
    void on_contentItemClicked(const QModelIndex &index);
    /**
     * @brief queryMyParameterTask--数据查询任务
     */
    void queryMyParameterTask();
    /**
     * @brief mainFormQueryResponse--数据查询响应
     * @param success
     * @param m_Data
     * @param errorID
     * @param errorMsg
     */
    void myFormQueryResponse(BusinessResponse m_Data);
};

#endif // FAULTALARMDIALOG_H
