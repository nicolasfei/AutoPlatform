#include "faultalarmdialog.h"
#include "ui_faultalarmdialog.h"
#include "common/cabinetmanagement.h"
#include "communication/businessinvoker.h"
#include "common/timertask.h"
#include "common/commondialog.h"
#include <QStringListModel>
#include <QAbstractItemView>

FaultAlarmDialog::FaultAlarmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FaultAlarmDialog)
{
    ui->setupUi(this);
}

FaultAlarmDialog::~FaultAlarmDialog()
{
    MY_DEBUG;
    //解除绑定数据查询响应
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &FaultAlarmDialog::myFormQueryResponse);
    //挂起线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);
    //解除定时任务绑定
    TimerTask *task = TimerTask::instance();
    disconnect(task, &TimerTask::taskIsTime, this, &FaultAlarmDialog::queryMyParameterTask);
    disconnect(ui->listView, &QListView::clicked, this, &FaultAlarmDialog::on_contentItemClicked);
    delete ui;
}

/**
 * @brief FaultAlarmDialog::setShowData--设置要显示的数据
 * @param data--数据
 */
void FaultAlarmDialog::setShowData(QList<ParameterClass*> data)
{
    MY_DEBUG;
    if(data.size()==0){
        return;
    }
    this->m_data = data;
    QStringListModel *model = new QStringListModel;
    QStringList mList;
    for(int i=0; i<m_data.size(); i++){
        ParameterClass* item = m_data.at(i);
        QString name = item->chName;//+"-"+QString::number(item->registerId);
        mList.append(name);
    }
    model->setStringList(mList);
    ui->listView->setModel(model);
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    connect(ui->listView, &QListView::clicked, this, &FaultAlarmDialog::on_contentItemClicked);

    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &FaultAlarmDialog::myFormQueryResponse); //绑定数据查询响应
    //唤醒数据接收线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);

    //定时任务绑定
    TimerTask *task = TimerTask::instance();
    connect(task, &TimerTask::taskIsTime, this, &FaultAlarmDialog::queryMyParameterTask);
}

/**
 * @brief FaultAlarmDialog::on_contentItemClicked--内容项被用户点击
 * @param index--点击项
 */
void FaultAlarmDialog::on_contentItemClicked(const QModelIndex &index)
{
    this->currentPc = this->m_data.at(index.row());
    //找到对应的位描述信息
    CabinetManagement *manager = CabinetManagement::instance();
    this->currentDes = manager->getParameterBitDescribe(currentPc->fpgaID);
    //显示数据
    showParameterInfo(this->currentPc->regData.des.uintValue);
}

/**
 * @brief showParameterInfo--显示当前选中的信息
 */
void FaultAlarmDialog::showParameterInfo(uint32_t var)
{
    QStringListModel *model = new QStringListModel;
    QStringList mList;
    for(int i=0; i<currentDes.size(); i++){
        QString des = QString::fromLocal8Bit("第") + QString::number(i) + QString::fromLocal8Bit("位：") +
                QString::number(var&1<<i) + "\t" + currentDes.at(i);
        mList.append(des);
    }
    model->setStringList(mList);
    ui->listView_2->setModel(model);
    ui->listView_2->setEditTriggers(QListView::NoEditTriggers);
}

/**
 * @brief queryMyParameterTask--数据查询任务
 */
void FaultAlarmDialog::queryMyParameterTask()
{
    if(this->currentPc==NULL){
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        return;
    }
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    ParameterClass::RegisterData reg = this->currentPc->getRegisterData();
    reg.rw = 0;
    bc.addData(reg);
    invoker->exec(bc);
}

/**
 * @brief mainFormQueryResponse--数据查询响应
 * @param success
 * @param m_Data
 * @param errorID
 * @param errorMsg
 */
void FaultAlarmDialog::myFormQueryResponse(BusinessResponse resp)
{
    if(resp.type!=BusinessClass::REGISTER_RW){
        return;
    }
    if(!resp.result){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("查询数据失败：") + resp.errorMsg);
        return;
    }

    QList<ParameterClass::RegisterData> dataList = resp.getRegisterData();
    for (int i=0; i<dataList.size(); i++){
        ParameterClass::RegisterData rData = dataList.at(i);
        if(rData.fpgaID == currentPc->fpgaID){
            this->showParameterInfo(rData.des.uintValue);
            break;
        }
    }
}
