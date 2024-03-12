#include "dioform.h"
#include "ui_dioform.h"
#include "diosavework.h"

#include "common/cabinetmanagement.h"
#include "common/commonclass.h"
#include "common/commondialog.h"
#include "common/timertask.h"
#include "communication/businessinvoker.h"
#include "parameterActivity/parameterdialog.h"
#include "userManager/user.h"
#include "userManager/usermanager.h"

DioForm::DioForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::DioForm)
{
    ui->setupUi(this);

    this->initData();
    this->initUiView();

    //关联表格双击修改参数
    connect(ui->tableView, &QTableView::doubleClicked, this, &DioForm::on_TableDoubleClicked);
}

DioForm::~DioForm()
{
    disconnect(ui->tableView, &QTableView::doubleClicked, this, &DioForm::on_TableDoubleClicked);
    delete ui;
}

/**
 * @brief DioForm::initData--初始化数据
 */
void DioForm::initData()
{
    if(this->m_parameterList==Q_NULLPTR){
        this->m_parameterList = new QList<ParameterClass*>();
    }else{
        if(this->m_parameterList->size()>0){
            this->m_parameterList->clear();
        }
    }
    CabinetManagement *manager = CabinetManagement::instance();
    //加载di
    QList<ParameterClass*> *di = manager->getDiFormParameters();
    if(di!=Q_NULLPTR){
        for(int i=0; i<di->size(); i++){
            this->m_parameterList->append(di->at(i));
        }
    }
    //加载do
    QList<ParameterClass*> *Do = manager->getDoFormParameters();
    if(Do!=Q_NULLPTR){
        for(int i=0; i<Do->size(); i++){
            this->m_parameterList->append(Do->at(i));
        }
    }
}

/**
 * @brief DioForm::initUiView--初始化ui view
 */
void DioForm::initUiView()
{
    if(this->m_parameterList==Q_NULLPTR){
        return;
    }

    //初始化列表view
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(14);
    /*设置表格的标题*/
    QStringList titleList = CabinetManagement::instance()->getParamterTitle();
    if(titleList.isEmpty()){
        return;
    }
    for (int i=0; i<titleList.size(); i++) {
        m_model->setHeaderData(i, Qt::Horizontal, titleList.at(i));
    }
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置标题自适应宽度
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);	//内容不可编辑
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); //选择整行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); //只选择一行
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu); //可弹出右键菜单
    ui->tableView->verticalHeader()->hide();//隐藏垂直表头
    ui->tableView->setModel(m_model);
    ui->tableView->setStyleSheet(CommonClass::TABLE_STYLE);

    //设置每列的宽度--在setModel(m_model)之后设置才有效
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度
    ui->tableView->horizontalHeader()->setStyleSheet(CommonClass::TABLE_HEAD_SECTIONSTYLE);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //所有列根据内容自动调整宽度
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setMinimumSectionSize(83);
}

/**
 * @brief DioForm::startBusiness--开始业务逻辑
 */
void DioForm::startBusiness()
{
    MY_DEBUG << "startBusiness";
    /**
     * @brief manager---普通用户不能导入参数
     */
    UserManager *userManager = UserManager::instance();
    if(userManager->getCurrentUser()->getLevel() == User::Admin_User && this->isHaveParametFix){
        //普通用户不能保存参数
        ui->pushButton->setEnabled(false);
        ui->pushButton->setStyleSheet(CommonClass::ENABLE_STYLE);
    }else{
        //高级用户可以保存参数
        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet(CommonClass::DISABLE_STYLE);
    }

    //检查参数是否成功加载
    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数失败，请检查文件！"));
        return;
    }

    //数据查询响应
    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &DioForm::on_parameterQueryResponse);       //绑定数据查询响应
    //唤醒modbus线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);

    //connect定时线程响应处理
    TimerTask *task = TimerTask::instance();
    connect(task, &TimerTask::taskIsTime, this, &DioForm::on_timerTaskTimeout);         //定时任务响应
    this->m_businessRunning=true;
    this->showParameTable();  //刷新页面
}

/**
 * @brief DioForm::endBusiness--结束业务逻辑
 */
void DioForm::endBusiness()
{
    MY_DEBUG << "endBusiness";
    if(this->m_parameterList==Q_NULLPTR){
        return;
    }
    this->waitDialog->hide();
    this->m_businessRunning=false;

    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &DioForm::on_parameterQueryResponse);       //绑定数据查询响应
    //挂起modbus线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);

    TimerTask *task = TimerTask::instance();
    disconnect(task, &TimerTask::taskIsTime, this, &DioForm::on_timerTaskTimeout);
}

/**
 * @brief DioForm::on_pushButton_clicked--保存参数按钮点击响应
 */
void DioForm::on_pushButton_clicked()
{
    //下发到下位机，保存文件
    BusinessInvoker *invoker = BusinessInvoker::instance();
    //先检查与主控的网络链接情况
    if(!invoker->isLink(BusinessClass::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络未连接，请检查!"));
        return;
    }
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    ParameterClass *pc = CabinetManagement::instance()->getSaveLowerMachineREG();
    ParameterClass::RegisterData reg = pc->getRegisterData();
    reg.rw = 1;
    reg.src.uintValue=1;
    bc.addData(reg);
    invoker->exec(bc);

    //保存数据到本地文件
    DioSaveWork *workerThread = new DioSaveWork();
    connect(workerThread, &DioSaveWork::saveFinish, this, [=](){
        this->isHaveParametFix=false;
        this->updateParametSaveStatus();
    });
    connect(workerThread, &DioSaveWork::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

/**
 * @brief DioForm::on_TableDoubleClicked--tableview item 被点击
 * @param index
 */
void DioForm::on_TableDoubleClicked(const QModelIndex &index)
{
    currIndex = index;
    if (currIndex.isValid())
    {
        ParameterDialog* dialog = new ParameterDialog(this->m_parameterList->at(this->currIndex.row()), this);
        connect(dialog, &ParameterDialog::ParameterUpdate, this, &DioForm::on_parameterFixed);
        dialog->setCurrentTab(0);
        dialog->setWindowTitle(QString::fromLocal8Bit("参数编辑"));
        dialog->showNormal();
    }
}

/**
 * @brief DioForm::on_parameterFixed--参数数据被修改
 */
void DioForm::on_parameterFixed()
{
    ParameterClass* pc = this->m_parameterList->at(this->currIndex.row());
    //下发到下位机
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检测！"));
        return;
    }
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    ParameterClass::RegisterData reg = pc->getRegisterData();
    reg.rw = 1;
    bc.addData(reg);
    invoker->exec(bc);
}

/**
 * @brief DioForm::on_parameterQueryResponse--参数查询响应
 * @param success
 * @param data
 * @param errorID
 * @param errorMsg
 */
void DioForm::on_parameterQueryResponse(BusinessResponse data)
{
    //    MY_DEBUG << "ParameterForm::myFormQueryResponse--> " << success;
    switch(data.type){
    case BusinessClass::REGISTER_RW:{
        if(!data.result){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("参数查询失败:")+ data.errorMsg);
            return;
        }
        QList<ParameterClass::RegisterData> dataList = data.getRegisterData();
        if(dataList.size()!=this->m_parameterList->size()){
            if(dataList.size()==1){
                ParameterClass::RegisterData data = dataList.at(0);
                if(data.rw==1){
                    if(data.addr == CabinetManagement::instance()->getSaveLowerMachineREG()->registerId){
                        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("参数保存成功!"));
                    }else{
                        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("参数修改成功!"));
                        this->isHaveParametFix=true;
                        this->updateParametSaveStatus();
                    }
                }
            }
            return;
        }
        for (int i=0; i<dataList.size(); i++)
        {
            ParameterClass::RegisterData rData = dataList.at(i);

            QString dataShow;
            switch(rData.type)
            {
            case ParameterClass::VOID_Type:
                break;
            case ParameterClass::STRING_Type:
                break;
            case ParameterClass::INT16_Type:
            {
                dataShow = QString::number(rData.des.shortValue);
            }
                break;
            case ParameterClass::UINT16_Type:
            {
                dataShow = QString::number(rData.des.ushortValue);
            }
                break;
            case ParameterClass::INT32_Type:
            {
                dataShow = QString::number(rData.des.intValue);
            }
                break;
            case ParameterClass::UINT32_Type:
            {
                dataShow = QString::number(rData.des.uintValue);
            }
                break;
            case ParameterClass::FLOAT32_Type:
            {
                dataShow = QString::number(rData.des.floatValue);
            }
                break;
            }
            QStandardItem* item = new QStandardItem();         //创建标准项对象
            item->setText(dataShow);
            m_model->setItem(i, 6, item);		//在模型中设置标准项
        }
    }
        break;
    default:
        MY_DEBUG << "this is not register response";
        return;
    }
}


/**
 * @brief DioForm::on_timerTaskTimeout--定时查询任务响应
 */
void DioForm::on_timerTaskTimeout()
{
    if(!this->m_businessRunning)
    {
        return;
    }
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        return;
    }
    this->m_mutex.lock();
    if(this->m_parameterList->size()>0)
    {
        BusinessClass bc;
        bc.setLevel(BusinessClass::MODBUS_HANDLER);
        bc.setCommandType(BusinessClass::REGISTER_RW);
        for(int i=0; i<this->m_parameterList->size(); i++)
        {
            ParameterClass *pc = this->m_parameterList->at(i);
            ParameterClass::RegisterData reg = pc->getRegisterData();
            reg.rw = 0;
            bc.addData(reg);
        }
        invoker->exec(bc);
    }
    this->m_mutex.unlock();
}

//更新参数保存按钮状态
void DioForm::updateParametSaveStatus()
{
    User *user = UserManager::instance()->getCurrentUser();
    //高级用户才能保存参数，注意普通用户修改网络IP地址 也可以保存参数
    if(user->getLevel() == User::Admin_User &&isHaveParametFix){
        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet(CommonClass::ENABLE_STYLE);
    }else{
        ui->pushButton->setEnabled(false);
        ui->pushButton->setStyleSheet(CommonClass::DISABLE_STYLE);
    }
}

//刷新页面
void DioForm::showParameTable()
{
    this->m_businessRunning=false;       //暂停查询任务
    //清空数据
    if(m_model->rowCount()>0){
        m_model->removeRows(0,m_model->rowCount());     //不会清理表头
    }

    //tableView加载数据
    if(this->m_parameterList!=NULL){
        QStandardItem *item;	//创建标准项指针
        for (int i = 0; i < this->m_parameterList->size(); i++)
        {
            int j=0;
            m_model->insertRow(i);//在模型中插入行
            ParameterClass* pc = this->m_parameterList->at(i);

            item = new QStandardItem();         //创建标准项对象
            item->setText(pc->addOffse);        //设置标准项的内容
            if(pc->isAddOscilloscope()){
                item->setIcon(QIcon(":/images/images/6-1.png"));
            }
            if(pc->isAddCustom()){
                item->setIcon(QIcon(":/images/images/7-1.png"));
            }
            if(pc->isAddOscilloscope() && pc->isAddCustom())
            {
                item->setIcon(QIcon(":/images/images/8-1.png"));
            }
            m_model->setItem(i, j++, item);		//在模型中设置标准项

            item = new QStandardItem();
            item->setText(pc->group);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->item);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->chName);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->unit);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->typeData);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->value);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->min);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->max);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->Enname);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->rw);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(pc->rw2);
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(QString::number(pc->registerId));
            m_model->setItem(i, j++, item);

            item = new QStandardItem();
            item->setText(QString::number(pc->fpgaID));
            m_model->setItem(i, j++, item);
        }
    }

    this->m_businessRunning=true;       //恢复查询任务
}
