#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainActivity/mainform2.h"
#include "systemtestActivity/systemtestform2.h"
#include "aioActivity/aioform.h"
#include "dioActivity/dioform.h"
#include "communicationDataActivity/communicationdataform.h"
#include "communicationSetActivity/communicationset10form.h"
#include "communicationSetActivity/communicationset3_2form.h"
#include "faultlogActivity/faultlogform.h"
#include "firmwareupdateActivity/firmupdateform.h"
#include "oscilloscopeActivity/oscilloscopeform.h"
#include "parameterActivity/parameterform.h"
#include <parameterActivity/importparametersform.h>
#include "parameterActivity/exportparametersform.h"
#include "parameterActivity/parametercomparisonform.h"
#include "watercoolingActivity/watercoolingform.h"
#include "waveformActivity/waveform.h"
#include <cnav/cnavview.h>
#include <common/commonclass.h>
#include "common/commondialog.h"
#include "common/tool.h"
#include "login.h"
#include "communication/businessinvoker.h"
#include "otherActivity/otherform.h"

#include<QDateTime>
#include<QTimer>
#include<QtDebug>

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);    //运行时最大化窗口
    ui->verticalFrame->setStyleSheet("#verticalFrame{background-color: rgb(6, 24, 64);}");
    ui->stackedWidget->setFrameShape(QListWidget::NoFrame);
    ui->toolButton->setVisible(false);

    //    QImage *img = new QImage;
    //    img->load(QString(":/images/images/net-off.png"));
    //    QImage scaleding = img->scaled(ui->label_6->width(), ui->label_6->height(), Qt::KeepAspectRatio);
    ui->toolButton_3->setEnabled(false);
    ui->toolButton_3->setIcon(QIcon(":/images/images/net-off.png"));


    //初始化管理模块
    CabinetManagement::instance();

    //初始化后台模块---网络状态监听
    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::netStates, this, &MainWindow::on_NetStates);

    loadStackedWidget();
    initListView();
    startSystemTimeShow();
}

MainWindow::~MainWindow()
{
    this->releaseStackedWidget();
    this->stopSystemTimeShow();
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::netStates, this, &MainWindow::on_NetStates);
    delete CabinetManagement::instance();
    delete BusinessInvoker::instance();
    delete ui;
}

void MainWindow::on_NetStates(bool linked)
{
    MY_DEBUG;
    //    QImage *img = new QImage;
    //    if(linked){
    //        img->load(QString(":/images/images/net-on.png"));
    //    }else{
    //        img->load(QString(":/images/images/net-off.png"));
    //    }
    //    QImage scaleding = img->scaled(ui->label_6->width(), ui->label_6->height(), Qt::KeepAspectRatio);
    if(linked){
        ui->toolButton_3->setIcon(QIcon(":/images/images/net-on.png"));
    }else{
        ui->toolButton_3->setIcon(QIcon(":/images/images/net-off.png"));
    }
}

void MainWindow::loadStackedWidget()
{
    this->formList = new QList<MyBaseQWidget *>();

    this->mainForm = new MainForm2(this);

    this->systemTestForm = new SystemTestForm2(this);

    this->dioForm = new DioForm(this);
    //this->aIOForm = new AIOForm(this);
    this->oscilloscopeForm = new OscilloscopeForm(this);
    this->waterCoolingForm = new WaterCoolingForm(this);

    this->parameterForm = new ParameterForm(this);

    //    this->importParametersForm = new ImportParametersForm(this);
    //    this->exportParametersForm = new ExportParametersForm(this);
    //    this->parameterComparisonForm = new ParameterComparisonForm(this);
    this->waveForm = new WaveForm(this);

    //this->communicationDataForm = new CommunicationDataForm(this);
    this->communicationSet10Form = new CommunicationSet10Form(this);

    this->firmwareUpdateForm = new FirmUpdateForm(this);
    this->faultLogForm = new FaultLogForm(this);
    this->otherForm = new OtherForm(this);

    this->formList->append(this->mainForm);
    this->formList->append(this->systemTestForm);
    this->formList->append(this->dioForm);
    //this->formList->append(this->aIOForm);
    this->formList->append(this->oscilloscopeForm);
    this->formList->append(this->waterCoolingForm);
    this->formList->append(this->parameterForm);
    //    this->formList->append(this->importParametersForm);
    //    this->formList->append(this->exportParametersForm);
    //    this->formList->append(this->parameterComparisonForm);
    this->formList->append(this->waveForm);
    //this->formList->append(this->communicationDataForm);
    this->formList->append(this->communicationSet10Form);
    this->formList->append(this->firmwareUpdateForm);
    this->formList->append(this->faultLogForm);
    this->formList->append(this->otherForm);

    ui->stackedWidget->insertWidget(0,this->mainForm);
    ui->stackedWidget->insertWidget(1,this->systemTestForm);
    ui->stackedWidget->insertWidget(2,this->dioForm);
    //    ui->stackedWidget->insertWidget(3,this->aIOForm);
    ui->stackedWidget->insertWidget(3,this->oscilloscopeForm);
    ui->stackedWidget->insertWidget(4,this->waterCoolingForm);
    ui->stackedWidget->insertWidget(5,this->parameterForm);
    //    ui->stackedWidget->insertWidget(6,this->importParametersForm);
    //    ui->stackedWidget->insertWidget(7,this->exportParametersForm);
    //    ui->stackedWidget->insertWidget(8,this->parameterComparisonForm);
    ui->stackedWidget->insertWidget(6,this->waveForm);
    //    ui->stackedWidget->insertWidget(10,this->communicationDataForm);
    ui->stackedWidget->insertWidget(7,this->communicationSet10Form);
    ui->stackedWidget->insertWidget(8,this->firmwareUpdateForm);
    ui->stackedWidget->insertWidget(9,this->faultLogForm);
    ui->stackedWidget->insertWidget(10,this->otherForm);

    ui->stackedWidget->setCurrentIndex(0);
    //    this->formList->at(0)->startBusiness();
}

void MainWindow::releaseStackedWidget()
{
    for(int i=0; i<this->formList->size(); i++){
        delete this->formList->at(i);
    }
}


void MainWindow::startSystemTimeShow()
{
    myTimer= new QTimer(this);
    myTimer->start(1000);
    connect(myTimer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
}

void MainWindow::stopSystemTimeShow()
{
    disconnect(myTimer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    myTimer->stop();
    delete myTimer;
}

void MainWindow::timeUpdate()
{
    QDateTime Date_Time =QDateTime::currentDateTime();
    QString Time_system = Date_Time.toString("yyyy/MM/dd hh:mm:ss");
    ui->label_3->setText(Time_system);
}

void MainWindow::initListView()
{
    pNavModel = new CNavModel(this);
    pDelegate = new CNavDelegate(this);
    //    pNavModel->ReadConfig(QCoreApplication::applicationDirPath() + "/navigation.xml");
    ui->listView->setModel(pNavModel);
    ui->listView->setItemDelegate(pDelegate);
    connect(ui->listView, &CNavView::clicked, this, &MainWindow::select);
    //    connect(ui->listView, &CNavView::doubleClicked, pNavModel, &CNavModel::Collapse);
    QModelIndex qindex = pNavModel->index(0,0);   //默认选中 index
    ui->listView->setCurrentIndex(qindex);
    this->formList->at(0)->startBusiness();
}

void MainWindow::select(const QModelIndex & index)
{
    int cIn = ui->stackedWidget->currentIndex();
    //    MY_DEBUG << "index.row " << index.row() << "cIn is " << cIn;
    if(cIn!=index.row()){
        this->formList->at(cIn)->endBusiness();             //暂停当前页面的业务
        ui->stackedWidget->setCurrentIndex(index.row());    //加载新页面
        this->formList->at(index.row())->startBusiness();   //开始新页面的业务
    }
}

/**
 * @brief MainWindow::closeEvent--串口关闭时候触发
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    delete CabinetManagement::instance();
    delete BusinessInvoker::instance();
    for(int i=0; i<this->formList->size(); i++)
    {
        delete this->formList->at(i);
    }
    delete ui;
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{

}

/**
 * @brief MainWindow::on_toolButton_2_clicked--退出登录
 */
void MainWindow::on_toolButton_2_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("退出");
    msgBox.setText("确定退出登录吗?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
    {
        this->hide();
        emit showLogin();
//        this->deleteLater();
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
 * @brief MainWindow::receiverShow--显示当前页面
 */
void MainWindow::receiverShow()
{
    //先刷新当前页面
    int currentID = ui->stackedWidget->currentIndex();
    this->formList->at(currentID)->endBusiness();             //暂停当前页面的业务
//    ui->stackedWidget->setCurrentIndex(index.row());    //价值新页面
    this->formList->at(currentID)->startBusiness();   //开始新页面的业务
    this->show();
}

