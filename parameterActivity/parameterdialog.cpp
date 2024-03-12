#include "parameterdialog.h"
#include "ui_parameterdialog.h"
#include "common/commondialog.h"
#include "common/commonclass.h"
#include "userManager/usermanager.h"
#include <QTableView>
#include <QHeaderView>
#include <QDebug>
#include <QInputDialog>

ParameterDialog::ParameterDialog(ParameterClass *arg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterDialog)
{
    ui->setupUi(this);
    this->setParameter(arg);

    switch (this->parameter->DTypeFlags)
    {
    case ParameterClass::FLOAT32_Type:
        ui->tabWidget->setTabText(1, QString::fromLocal8Bit("参数字编辑"));
        break;
    case ParameterClass::UINT16_Type:
    case ParameterClass::INT16_Type:
        this->isHaveBitConfig=true;
        this->initTabView(16);
        ui->tabWidget->setTabText(1, QString::fromLocal8Bit("二进制字编辑"));
        break;
    case ParameterClass::UINT32_Type:
    case ParameterClass::INT32_Type:
        this->isHaveBitConfig=true;
        this->initTabView(32);
        ui->tabWidget->setTabText(1, QString::fromLocal8Bit("二进制字编辑"));
        break;
    default:
        break;
    }
    ui->tabWidget->setTabText(0, QString::fromLocal8Bit("基本信息编辑"));
}

ParameterDialog::~ParameterDialog()
{
    delete ui;
}

/**
 * @brief ParameterDialog::initTabView
 * @param vaildBit--有效位数
 */
void ParameterDialog::initTabView(int vaildBit)
{
    CabinetManagement *manager = CabinetManagement::instance();
    QStringList des = manager->getParameterBitDescribe(this->parameter->fpgaID);
    BinarySetClass bsc(this->parameter->m_dataValue.bitSetValue, vaildBit, des);

    UserManager *uManager = UserManager::instance();
    //可以修改位描述--只需要高级管理权限
    bool canFix = (uManager->getCurrentUser()->getLevel()==User::Admin_User);
    //可以修改位的值--需要高级管理权限 并且参数可写
    bool canSet = (uManager->getCurrentUser()->getLevel()==User::Admin_User) & (this->parameter->rwFlags==ParameterClass::READ_WRITE);


    this->m_model = new BinarySetModel(bsc, 1, canSet, canFix, this);
    /*设置表格的标题*/
    const char* titles[3] = {"位序","置位","描述"};
    for (int i=0; i<3; i++) {
        m_model->setHeaderData(i, Qt::Horizontal, QString::fromLocal8Bit(titles[i]));
    }
    this->tableView = new QTableView(this);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);	//内容不可编辑
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows); //选择整行
    tableView->setSelectionMode(QAbstractItemView::SingleSelection); //只选择一行
    tableView->setContextMenuPolicy(Qt::CustomContextMenu); //可弹出右键菜单
    tableView->verticalHeader()->hide();//隐藏垂直表头
    tableView->setModel(m_model);
    //    ui->tableView->setStyleSheet(this->TABLE_STYLE);
    //设置每列的宽度--在setModel(m_model)之后设置才有效
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度
    const int columnW[] = {40,60};
    const int columnI[] = {0,1};
    for (int i=0; i<2; i++)
    {
        tableView->horizontalHeader()->setSectionResizeMode(columnI[i], QHeaderView::Fixed);//对第0列单独设置固定宽度
        tableView->setColumnWidth(columnI[i],columnW[i]);
    }
    connect(this->tableView, &QTableView::doubleClicked, this, &ParameterDialog::on_itemDoubleClicked);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);
    ui->widget->setLayout(layout);
}

/**
 * @brief ParameterDialog::setParameter--设置参数
 * @param arg 参数
 */
void ParameterDialog::setParameter(ParameterClass *arg)
{
    this->parameter = arg;
    ui->chName->setText(this->parameter->chName);
    ui->unit->setText(this->parameter->unit);
    ui->dataType->setText(this->parameter->getDataTypeName());
    ui->value->setText(this->parameter->value);
    ui->valueMin->setText(this->parameter->min);
    ui->valueMax->setText(this->parameter->max);
    ui->rw->setText(this->parameter->rw);
    ui->describe->setText(this->parameter->Enname);
    if(this->parameter->rw == "R"){
        ui->value->setDisabled(true);
    }
    //普通用户不能修改参数
    User *user = UserManager::instance()->getCurrentUser();
    if(user != Q_NULLPTR && user->getLevel() == User::Normal_User){
        ui->value->setDisabled(true);
    }
}

/**
 * @brief ParameterDialog::on_itemDoubleClicked--列表项被双击了
 * @param index
 */
void ParameterDialog::on_itemDoubleClicked(const QModelIndex &index)
{
    if(index.column()==2){
        bool ok;
        QString text = QInputDialog::getText(NULL, QString::fromLocal8Bit("位定义编辑"),
                                             QString::fromLocal8Bit("请输入新的定义"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()){
            this->m_model->setData(index, text, Qt::DisplayRole);
        }
    }
}

/**
 * @brief ParameterDialog::setTitle--设置对话框标题
 * @param title 标题
 */
void ParameterDialog::setTitle(QString title)
{
    this->setWindowTitle(title);
}

/**
 * @brief ParameterDialog::setCurrentTab--设置当前显示那个tab
 * @param tab tab
 */
void ParameterDialog::setCurrentTab(int tab)
{
    ui->tabWidget->setCurrentIndex(tab);
}

/**
 * @brief ParameterDialog::on_buttonBox_accepted --确定按钮
 */
void ParameterDialog::on_buttonBox_accepted()
{
    bool isChange=false;

    //普通用户因不能修改参数，所有不用检测是否有变化
    UserManager *uManager = UserManager::instance();
    if(uManager->getCurrentUser()->getLevel()==User::Normal_User){
        return;
    }
    //查看是那个页面处于当前编辑之下
    switch(ui->tabWidget->currentIndex()){
    //基本信息编辑
    case 0:
    {
        MY_DEBUG << "-------------------------------------------base info edit";
        //参数信息
        QString curValue = ui->value->text().trimmed();
        //参数比较
//        if(curValue.compare(parameter->value)!=0)
//        {
            bool isOK=false;
            switch (parameter->DTypeFlags)
            {
            case ParameterClass::VOID_Type:
                isOK=true;
                break;
            case ParameterClass::STRING_Type:
                isOK=true;
                break;
            case ParameterClass::FLOAT32_Type:{
                float tmp = curValue.toFloat(&isOK);
                //            if(tmp<parameter->minValue.floatValue || tmp>parameter->maxValue.floatValue){
                //                isOK=false;
                //            }
            }
                break;
            case ParameterClass::INT32_Type:{
                int tmp = curValue.toInt(&isOK);
                //            if(tmp<parameter->minValue.intValue || tmp>parameter->maxValue.intValue){
                //                isOK=false;
                //            }
            }
                break;
            case ParameterClass::UINT32_Type:{
                uint32_t tmp = curValue.toUInt(&isOK);
                //            if(tmp<parameter->minValue.uintValue || tmp>parameter->maxValue.uintValue){
                //                isOK=false;
                //            }
            }
                break;
            case ParameterClass::INT16_Type:{
                uint16_t tmp = curValue.toShort(&isOK);
                //            if(tmp<parameter->minValue.shortValue || tmp>parameter->maxValue.shortValue){
                //                isOK=false;
                //            }
            }
                break;
            case ParameterClass::UINT16_Type:{
                uint16_t tmp = curValue.toUShort(&isOK);
                //            if(tmp<parameter->minValue.ushortValue || tmp>parameter->maxValue.ushortValue){
                //                isOK=false;
                //            }
            }
                break;
            default:
                break;
            }
            if(!isOK)
            {
                CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("变量值有误！"));
                return;
            }
            isChange = true;
            parameter->updateValue(curValue);
//        }
    }
        break;
    //二进制字编辑
    case 1:
    {
        if(this->m_model->isDataUpdate()){
            MY_DEBUG << "new bit value is " << this->m_model->getBitValue();
            parameter->updateValue(QString::number(this->m_model->getBitValue()));
            CabinetManagement::instance()->updateParameterBitDescribe(parameter->fpgaID, this->m_model->getDescribe());
            isChange = true;
        }
    }
        break;
    default:
        break;
    }
    if(isChange){
        emit ParameterUpdate();     //发送更新信号
    }
}

