#include "parameterform.h"
#include "ui_parameterform.h"
#include "common/commondialog.h"
#include "common/commonclass.h"
#include "common/parameterclass.h"
#include "common/cabinetmanagement.h"
#include "parameterdialog.h"
#include "communication/businessinvoker.h"
#include "communication/businessclass.h"
#include "common/timertask.h"
#include "loadparameterfileqthread.h"
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include "userManager/usermanager.h"

#define LEVEL_WIDTH (90)
#define LEVEL_HIGHT (20)

ParameterForm::ParameterForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::ParameterForm)
{
    ui->setupUi(this);
    ui->verticalFrame_2->setStyleSheet("#verticalFrame_2{border-image: url(:/images/images/parame_form_bg.png);}");
    ui->verticalFrame_3->setStyleSheet("#verticalFrame_3{border-image: url(:/images/images/parame_form_level_bg.png);}");
    this->m_businessRunning=false;

    ui->level1->setAlignment(Qt::AlignLeft);    //一级菜单
    ui->level2->setAlignment(Qt::AlignLeft);    //二级菜单

    this->initData();
    this->initUiView();

    //关联右键点击事件--菜单
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &ParameterForm::slotContextMenu);
    //关联表格双击修改参数
    connect(ui->tableView, &QTableView::doubleClicked, this, &ParameterForm::on_TableDoubleClicked);

    //监听参数下发到主控结构
    CabinetManagement *manager = CabinetManagement::instance();
    connect(manager, &CabinetManagement::parameterLoadResult, this, &ParameterForm::on_parameterLoadResult);
    connect(manager, &CabinetManagement::oscillListChanged, this, &ParameterForm::on_oscillListChanged);
}

ParameterForm::~ParameterForm()
{
    CabinetManagement *manager = CabinetManagement::instance();
    disconnect(manager, &CabinetManagement::parameterLoadResult, this, &ParameterForm::on_parameterLoadResult);
    delete ui;
}

void ParameterForm::initData()
{
    CabinetManagement *manager = CabinetManagement::instance();
    this->m_configFileMap = manager->getParameterConfigMap();
}

void ParameterForm::initUiView()
{
    if(this->m_configFileMap==Q_NULLPTR){
        return;
    }
    //先清空控件
    QLayoutItem *child;
    while ((child = ui->level1->itemAt(0)) != nullptr)
    {
        ui->level1->removeItem(child);
        delete child->widget();
        delete child;
        child = nullptr;
    }
    //组件节点
    int n=0;
    QString firstNode;
    QMap<QString, ParameterTable*>::Iterator it = this->m_configFileMap->begin();
    while (it != this->m_configFileMap->end())
    {
        //构造1级节点
        QPushButton *l1Button = new QPushButton(this);
        l1Button->setFixedSize(LEVEL_WIDTH,LEVEL_HIGHT);
        l1Button->setStyleSheet(CommonClass::LEVEL1_STYLE);
        l1Button->setText(it.key());
        l1Button->setWhatsThis(it.key());
        ui->level1->addWidget(l1Button);
        connect(l1Button, &QPushButton::clicked, this, &ParameterForm::loadLevel2);
        if(n==0){
            firstNode = it.key();
        }
        ++it;
        n++;
    }
    //构建自定义参数列表
    QPushButton *l1Button = new QPushButton(this);
    l1Button->setFixedSize(LEVEL_WIDTH,LEVEL_HIGHT);
    l1Button->setStyleSheet(CommonClass::LEVEL1_STYLE);
    l1Button->setText(CUSTOM_TABLE);
    l1Button->setWhatsThis(CUSTOM_TABLE);
    ui->level1->addWidget(l1Button);
    connect(l1Button, &QPushButton::clicked, this, &ParameterForm::loadLevel2);

    //初始化列表view
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(14);
    /*设置表格的标题*/
    QList<QString> titleList = this->m_configFileMap->value(firstNode)->getHeaderLabels();
    for (int i=0; i<titleList.size(); i++) {
        m_model->setHeaderData(i, Qt::Horizontal, titleList.at(i));
    }
    //    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置标题自适应宽度
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
    //    const int columnW[] = {160,200,120,500};
    //    const int columnI[] = {0,1,2,5};
    //    for (int i=0; i<sizeof(columnI)/sizeof(int); i++) {
    //        ui->tableView->horizontalHeader()->setSectionResizeMode(columnI[i], QHeaderView::Fixed);//对第0列单独设置固定宽度
    //        ui->tableView->setColumnWidth(columnI[i],columnW[i]);
    //    }

    showLevel2(firstNode);
    this->updateParametSaveStatus();
}

/**
 * @brief ParameterForm::startBusiness--开始业务逻辑
 */
void ParameterForm::startBusiness()
{
    MY_DEBUG << "startBusiness";
    /**
     * @brief manager---普通用户不能导入参数
     */
    UserManager *userManager = UserManager::instance();
    if(userManager->getCurrentUser()->getLevel() == User::Normal_User){
        //普通用户不能导入参数列表
        ui->pushButton->setEnabled(false);
        ui->pushButton->setStyleSheet(CommonClass::DISABLE_STYLE);
        //普通用户不能保存参数
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_2->setStyleSheet(CommonClass::DISABLE_STYLE);
    }else{
        //高级用户可以导入参数列表
        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet(CommonClass::ENABLE_STYLE);
    }


    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数失败，请检查参数文件"));
        return;
    }

    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &ParameterForm::myFormQueryResponse);       //绑定数据查询响应
    //唤醒modbus线程
    invoker->wakeup(BusinessClass::MODBUS_HANDLER);
    //唤醒ftp线程
    invoker->wakeup(BusinessClass::FTP_HANDLER);
    //connect定时线程响应处理
    TimerTask *task = TimerTask::instance();
    connect(task, &TimerTask::taskIsTime, this, &ParameterForm::queryMyParameterTask);         //定时任务响应
    this->m_businessRunning=true;
    this->showParameTable(curLevel1Tag, curLevel2Tag);  //刷新页面
}

/**
 * @brief ParameterForm::endBusiness--结束业务逻辑
 */
void ParameterForm::endBusiness()
{
    MY_DEBUG << "endBusiness";
    if(this->m_parameterList==Q_NULLPTR){
        return;
    }
    this->waitDialog->hide();
    this->m_businessRunning=false;
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &ParameterForm::myFormQueryResponse);       //绑定数据查询响应
    //挂起modbus线程
    invoker->suspend(BusinessClass::MODBUS_HANDLER);
    //挂起ftp线程
    invoker->suspend(BusinessClass::FTP_HANDLER);
    TimerTask *task = TimerTask::instance();
    disconnect(task, &TimerTask::taskIsTime, this, &ParameterForm::queryMyParameterTask);
}

/**
 * @brief ParameterForm::release--释放资源
 */
void ParameterForm::release()
{

}


/**
 * @brief ParameterForm::loadLevel2--加载2级菜单
 */
void ParameterForm::loadLevel2()
{
    QPushButton* pBtn= qobject_cast<QPushButton*>(sender());
    QString  tag= pBtn->whatsThis();
    showLevel2(tag);
}

/**
 * @brief compareLevelData
 * @param l1
 * @param l2
 * @return
 */
bool compareLevelData(const QString &l1, const QString &l2)
{
    if(l1.toInt() < l2.toInt())
    {
        return true;
    }
    return false;
}

/**
 * @brief ParameterForm::showLevel2--显示2级菜单
 * @param tag
 */
void ParameterForm::showLevel2(QString tag)
{
    if(this->m_configFileMap==Q_NULLPTR){
        return;
    }
    //先清空控件
    QLayoutItem *child=NULL;
    if(ui->level2->count()>0){
        while ((child = ui->level2->itemAt(0)) != nullptr)
        {
            ui->level2->removeItem(child);
            delete child->widget();
            delete child;
            child = nullptr;
        }
    }
    QList<QString> level;

    if(this->m_configFileMap->contains(tag))
    {
        QMap<QString, QList<ParameterClass*>*>* tableMap = m_configFileMap->value(tag)->tableMap;
        if(tableMap!=NULL){
            level = tableMap->keys();
            qSort(level.begin(), level.end(), compareLevelData);    //排序
            for (int i = 0; i < tableMap->size(); i++)
            {
                //加入2级节点
                QPushButton *l2Button = new QPushButton(this);
                l2Button->setFixedSize(LEVEL_WIDTH-50,LEVEL_HIGHT);
                l2Button->setStyleSheet(CommonClass::LEVEL2_STYLE);
                l2Button->setText("Group"+level.at(i));
                l2Button->setWhatsThis(tag+","+level.at(i));
                ui->level2->addWidget(l2Button);
                if(i==0)
                {
                    l2Button->setStyleSheet(CommonClass::LEVEL2_FOCUS_STYLE);
                }
                connect(l2Button, &QPushButton::clicked, this, &ParameterForm::loadParameTable);
            }
        }
    }

    //加载自定义参数分组
    if(tag.compare(CUSTOM_TABLE)==0)
    {
        QPushButton *l2Button = new QPushButton(this);
        l2Button->setStyleSheet(CommonClass::LEVEL2_FOCUS_STYLE);
        l2Button->setText("Group"+QString::fromLocal8Bit(CUSTOM_TABLE));
        l2Button->setWhatsThis(tag+","+QString::fromLocal8Bit(CUSTOM_TABLE));
        ui->level2->addWidget(l2Button);
        connect(l2Button, &QPushButton::clicked, this, &ParameterForm::loadParameTable);
        level.append(CUSTOM_TABLE);
    }
    //修改level1按钮背景
    for (int cc = ui->level1->layout()->count()-1; cc >= 0; cc--)
    {
        QLayoutItem *it = ui->level1->layout()->itemAt(cc);
        QPushButton *button = qobject_cast<QPushButton *>(it->widget());
        if (button != NULL)
        {
            if(button->text().compare(tag)==0)
                button->setStyleSheet(CommonClass::LEVEL1_FOCUS_STYLE);
            else
                button->setStyleSheet(CommonClass::LEVEL1_STYLE);
        }
    }
    if(!level.isEmpty()){
        showParameTable(tag, level.at(0));
    }
}

/**
 * @brief ParameterForm::loadParameTable--加载参数列表
 */
void ParameterForm::loadParameTable()
{
    QPushButton* pBtn= qobject_cast<QPushButton*>(sender());
    QString tag= pBtn->whatsThis();
    if(tag.isEmpty()){
        qDebug() << "参数为空" << endl;
        return;
    }
    QStringList arg = tag.split(",");
    showParameTable(arg.at(0), arg.at(1));
}

/**
 * @brief ParameterForm::showParameTable--显示table数据
 * @param level1Tag
 * @param level2Tag
 */
void ParameterForm::showParameTable(QString level1Tag, QString level2Tag)
{
    this->curLevel1Tag = level1Tag;
    this->curLevel2Tag = level2Tag;
    if(this->m_configFileMap==Q_NULLPTR){
        return;
    }
    this->suspendQueryTask();       //暂停查询任务
    //清空数据
    if(m_model->rowCount()>0)
    {
        m_model->removeRows(0,m_model->rowCount());     //不会清理表头
    }
    //获取数据
    if(this->m_configFileMap->contains(level1Tag))
    {
        this->m_table = this->m_configFileMap->value(level1Tag);
        this->currentTable = level1Tag;
        if(this->m_table!=NULL)
        {
            //添加数据到表格
            if(this->m_table->tableMap->contains(level2Tag))
            {
                this->m_mutex.lock();
                this->m_parameterList = this->m_table->tableMap->value(level2Tag);
                this->m_mutex.unlock();
                if(this->m_parameterList!=NULL)
                {
                    QStandardItem *item;	//创建标准项指针
                    for (int i = 0; i < this->m_parameterList->size(); i++)
                    {
                        int j=0;
                        m_model->insertRow(i);//在模型中插入行
                        ParameterClass* pc = this->m_parameterList->at(i);

                        item = new QStandardItem();         //创建标准项对象
                        item->setText(pc->addOffse);        //设置标准项的内容
                        if(pc->isAddOscilloscope()){
                            item->setIcon(QIcon(this->osIcon));
                        }
                        if(pc->isAddCustom()){
                            item->setIcon(QIcon(this->frIcon));
                        }
                        if(pc->isAddOscilloscope() && pc->isAddCustom())
                        {
                            item->setIcon(QIcon(this->osfrIcon));
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
            }
        }
    }
    if(level1Tag.compare(CUSTOM_TABLE) == 0)
    {
        CabinetManagement *manager = CabinetManagement::instance();
        this->m_mutex.lock();
        this->m_parameterList = manager->getCustomList(manager->getCurrentCabinetID());
        this->m_mutex.unlock();
        if(this->m_parameterList!=NULL)
        {
            QStandardItem *item;	//创建标准项指针
            for (int i = 0; i < this->m_parameterList->size(); i++)
            {
                int j=0;
                m_model->insertRow(i);//在模型中插入行
                ParameterClass* pc = this->m_parameterList->at(i);

                item = new QStandardItem();         //创建标准项对象
                item->setText(pc->addOffse);        //设置标准项的内容
                if(pc->isAddOscilloscope()){
                    item->setIcon(QIcon(this->osIcon));
                }
                if(pc->isAddCustom()){
                    item->setIcon(QIcon(this->frIcon));
                }
                if(pc->isAddOscilloscope() && pc->isAddCustom())
                {
                    item->setIcon(QIcon(this->osfrIcon));
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
    }

    //修改level2按钮背景
    for (int cc = ui->level2->layout()->count()-1; cc >= 0; cc--)
    {
        QLayoutItem *it = ui->level2->layout()->itemAt(cc);
        QPushButton *button = qobject_cast<QPushButton *>(it->widget());
        if (button != NULL)
        {
            if(button->text().mid(5).compare(level2Tag)==0)
                button->setStyleSheet(CommonClass::LEVEL2_FOCUS_STYLE);
            else
                button->setStyleSheet(CommonClass::LEVEL2_STYLE);
        }
    }
    this->currentTable = level1Tag;
    this->resumeQueryTask();       //恢复查询任务
}

/**
 * @brief ParameterForm::on_TableDoubleClicked--列表项被双击
 * @param index
 */
void ParameterForm::on_TableDoubleClicked(const QModelIndex &index)
{
    currIndex = index;
    if (currIndex.isValid())
    {
        this->slotParamDefine();
    }
}

/**
 * @brief ParameterForm::slotContextMenu--右键显示菜单
 * @param pos 鼠标位置
 */
void ParameterForm::slotContextMenu(QPoint pos)
{
    currIndex = ui->tableView->indexAt(pos);
    if (currIndex.isValid())
    {
        //表格每行点击右键菜单
        QMenu popMenu;
        const char* menuItem[4] = {"参数定义","参数修改","加入示波器","加入自定义列表"};  //,"下载参数"
        const char* menuItem2[4] = {"参数定义","参数修改","取消示波器","取消自定义列表"};     //,"下载参数"
        ParameterClass* pc = this->m_parameterList->at(this->currIndex.row());
        for(int i=0; i<4; i++)
        {
            QAction* action = new QAction();
            switch (i) {
            case 0:
                action->setText(QString::fromLocal8Bit(menuItem[i]));
                popMenu.addAction(action);
                connect(action, &QAction::triggered, this, &ParameterForm::slotParamDefine);
                break;
            case 1:
                action->setText(QString::fromLocal8Bit(menuItem[i]));
                popMenu.addAction(action);
                connect(action, &QAction::triggered, this, &ParameterForm::slotParamFix);
                break;
            case 2:
                if(pc->isAddOscilloscope()){
                    action->setText(QString::fromLocal8Bit(menuItem2[i]));
                }else{
                    action->setText(QString::fromLocal8Bit(menuItem[i]));
                }
                popMenu.addAction(action);
                connect(action, &QAction::triggered, this, &ParameterForm::slotAdd2Oscill);
                break;
            case 3:
                if(pc->isAddCustom()){
                    action->setText(QString::fromLocal8Bit(menuItem2[i]));
                }else{
                    action->setText(QString::fromLocal8Bit(menuItem[i]));
                }
                popMenu.addAction(action);
                connect(action, &QAction::triggered, this, &ParameterForm::slotAdd2Custom);
                break;
            case 4:
                action->setText(QString::fromLocal8Bit(menuItem[i]));
                popMenu.addAction(action);
                //connect(action, &QAction::triggered, this, &ParameterForm::slotParamDown);
                break;
            }
        }
        popMenu.exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
    }
}

//数据被修改
void ParameterForm::on_parameterFixed()
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

//右键菜单--参数定义
void ParameterForm::slotParamDefine()
{
    ParameterDialog* dialog = new ParameterDialog(this->m_parameterList->at(this->currIndex.row()), this);
    connect(dialog, &ParameterDialog::ParameterUpdate, this, &ParameterForm::on_parameterFixed);
    dialog->setCurrentTab(0);
    dialog->setWindowTitle(QString::fromLocal8Bit("参数编辑"));
    dialog->showNormal();
}

//右键菜单--参数修改
void ParameterForm::slotParamFix()
{
    ParameterDialog* dialog = new ParameterDialog(this->m_parameterList->at(this->currIndex.row()), this);
    connect(dialog, &ParameterDialog::ParameterUpdate, this, &ParameterForm::on_parameterFixed);
    dialog->setCurrentTab(1);
    dialog->setWindowTitle(QString::fromLocal8Bit("参数编辑"));
    dialog->showNormal();
}

//右键菜单--加入/取消示波器
void ParameterForm::slotAdd2Oscill()
{
    int j=0, i = this->currIndex.row();
    QStandardItem* item;
    ParameterClass* pc = this->m_parameterList->at(i);
    CabinetManagement *manager = CabinetManagement::instance();
    //这里是取消示波器
    if(pc->isAddOscilloscope())
    {
        pc->setOscilloscope(false);
        manager->removeParameter2Oscilloscope(pc);
    }else{
        //添加到示波器
        if(!manager->addParameter2Oscilloscope(pc)){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("动态示波器已满，请先删除！"));
            return;
        }
        pc->setOscilloscope(true);
    }

    //更新tableView项数据
    item = new QStandardItem();         //创建标准项对象
    item->setText(pc->addOffse);             //设置标准项的内容
    if(pc->isAddOscilloscope()){
        item->setIcon(QIcon(this->osIcon));
    }
    if(pc->isAddCustom()){
        item->setIcon(QIcon(this->frIcon));
    }
    if(pc->isAddOscilloscope() && pc->isAddCustom())
    {
        item->setIcon(QIcon(this->osfrIcon));
    }
    m_model->setItem(i, j, item);		//在模型中设置标准项
    this->isHaveParametFix=true;
    this->updateParametSaveStatus();
}

//右键菜单--加入/取消自定义参数
void ParameterForm::slotAdd2Custom()
{
    int j=0, i = this->currIndex.row();
    QStandardItem* item;
    ParameterClass* pc = this->m_parameterList->at(i);
    CabinetManagement *manager = CabinetManagement::instance();
    //这里是取消自定义参数
    if(pc->isAddCustom())
    {
        pc->setCustom(false);
        manager->removeParameter2Custom(pc);
    }else{
        //添加到自定义参数
        manager->addParameter2Custom(pc);
        pc->setCustom(true);
    }

    //更新界面
    if(this->currentTable.compare(CUSTOM_TABLE)==0 && !pc->isAddCustom()){
        m_model->removeRows(i, 1);
    }else{
        //更新tableView项数据
        item = new QStandardItem();         //创建标准项对象
        item->setText(pc->addOffse);             //设置标准项的内容
        if(pc->isAddOscilloscope()){
            item->setIcon(QIcon(this->osIcon));
        }
        if(pc->isAddCustom()){
            item->setIcon(QIcon(this->frIcon));
        }
        if(pc->isAddOscilloscope() && pc->isAddCustom())
        {
            item->setIcon(QIcon(this->osfrIcon));
        }
        m_model->setItem(i, j, item);		//在模型中设置标准项
    }
    this->isHaveParametFix=true;
    this->updateParametSaveStatus();
}

void ParameterForm::updateParametSaveStatus()
{
    User *user = UserManager::instance()->getCurrentUser();
    //高级用户才能保存参数，注意普通用户修改网络IP地址 也可以保存参数
    if(user->getLevel() == User::Admin_User &&isHaveParametFix){
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_2->setStyleSheet(CommonClass::ENABLE_STYLE);
    }else{
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_2->setStyleSheet(CommonClass::DISABLE_STYLE);
    }
}

/**
 * @brief ParameterForm::suspendQueryTask--暂停查询任务
 */
void ParameterForm::suspendQueryTask()
{
    this->m_businessRunning=false;
}

/**
 * @brief ParameterForm::resumeQueryTask--恢复查询任务
 */
void ParameterForm::resumeQueryTask()
{
    this->m_businessRunning=true;
}

/**
 * @brief MainForm::queryMyParameterTask
 */
void ParameterForm::queryMyParameterTask()
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

/**
 * @brief ParameterForm::resetMasterContrl--重启主控
 */
void ParameterForm::resetMasterContrl()
{
    BusinessInvoker *invoker = BusinessInvoker::instance();
    if(!invoker->isLink(BusinessClass::LEVEL::MODBUS_HANDLER)){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接失败"));
        return;
    }
    CabinetManagement *manager = CabinetManagement::instance();
    BusinessClass bc;
    bc.setLevel(BusinessClass::MODBUS_HANDLER);
    ParameterClass *pc = manager->getMasterContrlReset();
    ParameterClass::RegisterData reg = pc->getRegisterData();
    reg.rw = 1;
    reg.src.uintValue=1;
    bc.addData(reg);
    invoker->exec(bc);
}

/**
 * @brief ParameterForm::myFormQueryResponse
 * @param m_Data
 */
void ParameterForm::myFormQueryResponse(BusinessResponse data)
{
    //MY_DEBUG << "ParameterForm::myFormQueryResponse--> " << success;
    switch(data.type){
    case BusinessClass::REGISTER_RW:{
        if(!data.result){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据查询失败：")+ data.errorMsg);
            return;
        }
        QList<ParameterClass::RegisterData> dataList = data.getRegisterData();
        if(dataList.size()!=this->m_parameterList->size()){
            if(dataList.size()==1){
                ParameterClass::RegisterData data = dataList.at(0);
                if(data.rw==1){
                    if(data.addr == CabinetManagement::instance()->getSaveLowerMachineREG()->registerId){
                        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("参数保存成功！"));
                    }else{
                        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("参数修改成功！"));
                        this->isHaveParametFix=true;
                        this->updateParametSaveStatus();
                    }
                }
            }
            MY_DEBUG << "this is not current page";
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
        //主控单元收到参数下载启动
    case BusinessClass::PARAMETER_DOWN_START:{
        //发送参数文件配置帧
        BusinessInvoker *invoker = BusinessInvoker::instance();
        BusinessClass bc;
        bc.setLevel(BusinessClass::FTP_HANDLER);
        bc.setCommandType(BusinessClass::PARAMETER_DOWN);
        bc.setFilePath(currentDownParametFilePath);
        invoker->exec(bc);
    }
        break;
        //参数文件下载响应
    case BusinessClass::PARAMETER_DOWN:{
        MY_DEBUG;
        switch (data.respType) {
        //01表示“确认收到启动开始下载帧”；
        case 0x01:{
            //发送参数文件配置帧
            BusinessInvoker *invoker = BusinessInvoker::instance();
            BusinessClass bc;
            bc.setLevel(BusinessClass::FTP_HANDLER);
            bc.setCommandType(BusinessClass::PARAMETER_DOWN);
            bc.setFilePath(currentDownParametFilePath);
            invoker->exec(bc);
        }
            break;
            //02表示“账号/密码错误”；
        case 0x02:{
            this->waitDialog->hide();
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("发送文件下载启动帧失败：")+ data.errorMsg);
        }
            break;
            //03表示“路径不存在”；
        case 0x03:{
            this->waitDialog->hide();
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("发送文件下载启动帧失败：")+ data.errorMsg);
        }
            break;
            //04表示“确认登陆信息帧/确认升级变量正确”
        case 0x04:{
            //this->waitDialog->hide();
            //CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("发送文件下载启动帧失败：")+ data.errorMsg);
        }
            break;
            //05表示“crc文件校验错误”
        case 0x05:{
            this->waitDialog->hide();
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("发送文件下载启动帧失败：")+ data.errorMsg);
        }
            break;
            //06表示“回复下载成功”
        case 0x06:{
            //本地保存
            LoadParameterFileQThread *load = new LoadParameterFileQThread(currentDownParametFilePath);
            load->start();
        }
            break;
            //07表示“上传完成”
        case 0x07:{

        }
            break;
            //08标识“服务器不在线”
        case 0x08:{
            this->waitDialog->hide();
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("发送文件下载启动帧失败：")+ data.errorMsg);
        }
            break;
            //09表示“非法文件”
        case 0x09:{
            this->waitDialog->hide();
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("发送文件下载启动帧失败：")+ data.errorMsg);
        }
            break;
        default:
            break;
        }
    }
        break;
    default:
        MY_DEBUG << "this is not valid response";
        return;
    }
}

/**
 * @brief on_saveFinish--参数保存完成
 */
void ParameterForm::on_saveFinish()
{

}

/**
 * @brief ParameterForm::saveParametToFile--保存参数
 */
void ParameterForm::saveParametToFile()
{
    SaveWorker *workerThread = new SaveWorker();
    connect(workerThread, &SaveWorker::saveFinish, this, [=](){
        this->isHaveParametFix=false;
        this->updateParametSaveStatus();
    });
    connect(workerThread, &SaveWorker::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

/**
 * @brief ParameterForm::on_pushButton_2_clicked--保存参数按钮被点击
 */
void ParameterForm::on_pushButton_2_clicked()
{
    //保存数据到文件
    saveParametToFile();
}

/**
 * @brief ParameterForm::on_pushButton_clicked--加载参数文件
 */
void ParameterForm::on_pushButton_clicked()
{
    //默认打开我的文档路径
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    //    dir.append("./wave").append(CabinetManagement::instance()->getCurrentCabinetID());
    QString title = QString::fromLocal8Bit("请选择要导入的故障录波数据文件");
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                title,
                dir,
                "csv files (*.csv)");

    if(!files.isEmpty()){
        //只能处理一个文件
        this->currentDownParametFilePath = files.at(0);

        //先将参数文件下发到到下位机--先发送启动帧
        BusinessInvoker *invoker = BusinessInvoker::instance();
        if(!invoker->isLink(BusinessClass::FTP_HANDLER)){
            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("网络连接错误，请检查网络"));
            return;
        }
        BusinessClass bc;
        bc.setLevel(BusinessClass::FTP_HANDLER);
        bc.setCommandType(BusinessClass::PARAMETER_DOWN_START);
        bc.setFilePath(currentDownParametFilePath);
        invoker->exec(bc);
        MY_DEBUG;
        this->waitDialog->show();
    }
}

/**
 * @brief on_parameterLoadResult--加载参数文件结果
 * @param success--成功与否
 * @param msg--msg
 */
void ParameterForm::on_parameterLoadResult(bool success, QString msg)
{
    this->waitDialog->hide();
    if(!success){
        CommonDialog::showErrorDialog(this, msg);
        return;
    }
    //初始化界面
    this->initData();
    this->initUiView();
    //提示用户是否需要重启下位机，重启下位机后新的配置表才能生效
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("成功"));
    msgBox.setText(QString::fromLocal8Bit("配置文件加载成功，重启主控设备后，新的配置文件生效，同时请保证设备工作已经完成，重启设备不会造成故障！"));
    msgBox.setInformativeText(QString::fromLocal8Bit("确定要重启主控设备吗？"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:{
        // Save was clicked--发送重启命令到主控
        BusinessInvoker *invoker = BusinessInvoker::instance();
        BusinessClass bc;
        bc.setLevel(BusinessClass::MODBUS_HANDLER);
        ParameterClass* reset = CabinetManagement::instance()->getMasterContrlReset();
        ParameterClass::RegisterData reg = reset->getRegisterData();
        reg.rw = 1;
        reg.src.uintValue=1;
        bc.addData(reg);
        invoker->exec(bc);
    }
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        break;
    default:
        // should never be reached
        break;
    }
}

/**
 * @brief ParameterForm::on_oscillListChanged
 */
void ParameterForm::on_oscillListChanged()
{
    this->isHaveParametFix=true;
    this->updateParametSaveStatus();
}

