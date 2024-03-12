#include "oscilloscopeform.h"
#include "ui_oscilloscopeform.h"
#include "commonView/waveitemwidget.h"
#include "common/cabinetmanagement.h"
#include "common/commondialog.h"
#include "common/commonclass.h"
#include "communication/businessinvoker.h"
#include "commonView/mycustomplot.h"
#include "commonView/qcustomplot.h"
#include "oscilldatasavethread.h"

using namespace QtCharts;

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::OscilloscopeForm),
    isCatch(false),
    isMove(false),
    isZoom(false),
    isRecovery(false),
    isShowPoint(false),
    autoCollectting(false)
{
    ui->setupUi(this);
    //设置样式
    ui->oscill_channel->setStyleSheet("#oscill_channel{border-image: url(:/images/images/oscill_list_bg.png);}");
    ui->frame_5->setStyleSheet("#frame_5{border-image: url(:/images/images/oscill_set_bg.png);}");
    ui->frame_6->setStyleSheet("#frame_6{border-image: url(:/images/images/oscill_set_bg.png);}");
    ui->oscill_charts->setStyleSheet("#oscill_charts{border-image: url(:/images/images/oscill_charts_bg.png);}");
    ui->frame_5->setVisible(false);
    ui->pushButton->setVisible(false);  //隐藏显示按钮
    this->m_businessRunning=false;
    this->acqStatus = STOP;

    this->initData();
    this->initUiView();

    //关联右键点击事件--菜单
    connect(ui->qcustomplot, &MyCustomPlot::customContextMenuRequested, this, &OscilloscopeForm::slotContextMenu);

    //数据自动保存线程
    this->autoSaveTask = new OscillDataSaveTask();
    connect(autoSaveTask, &OscillDataSaveTask::finished, autoSaveTask, &QObject::deleteLater);
    this->autoSaveTask->start();    //启动自动保存线程
}

OscilloscopeForm::~OscilloscopeForm()
{
    endBusiness();
    this->autoSaveTask->taskFinish();
    delete ui;
}

/**
 * @brief OscilloscopeForm::initOscillListView--初始化通道listView
 */
void OscilloscopeForm::initOscillListView()
{
    //初始化listView
    if(ui->listWidget->count()>0){
        ui->listWidget->clear();    //先清空通道列表
    }
    if(m_parameterList!=Q_NULLPTR){
        channelShowCount=0;
        for (int i=0; i<this->m_parameterList->size(); i++ ){
            ParameterClass *pc = this->m_parameterList->at(i);
            WaveItemWidget *item = new WaveItemWidget(this);
            item->setData(i, pc->fpgaID, pc->chName);
            QListWidgetItem* pItem = new QListWidgetItem();
            pItem->setSizeHint(QSize(210, 32));
            ui->listWidget->addItem(pItem);
            ui->listWidget->setItemWidget(pItem, item);
            channelShowCount++;
            connect(item, &WaveItemWidget::deleteItem, this, &OscilloscopeForm::onListItemDelete);
            connect(item, &WaveItemWidget::isShowWave, this, &OscilloscopeForm::onListItemShow);
            connect(item, &WaveItemWidget::zoomWave, this, &OscilloscopeForm::onListItemZoom);
        }
        if(channelShowCount==0){
            ui->toolButton_12->setIcon(QIcon(":/images/images/eys_close.svg"));
        }else{
            ui->toolButton_12->setIcon(QIcon(":/images/images/eys_open.svg"));
        }
    }
}

/**
 * @brief OscilloscopeForm::initOscillCharts--初始化图表
 */
void OscilloscopeForm::initOscillCharts()
{
    static QList<QColor> penColor = {QColor(Qt::red),QColor(Qt::green),QColor(Qt::blue),
                                     QColor(Qt::cyan),QColor(Qt::magenta),QColor(Qt::yellow),
                                     QColor(Qt::darkRed),QColor(Qt::darkGreen),QColor(Qt::darkBlue),
                                     QColor(Qt::darkCyan),QColor(Qt::darkMagenta),QColor(Qt::darkYellow),
                                     QColor(255,153,51),QColor(Qt::color1),QColor(Qt::darkGray),
                                     QColor(Qt::black),QColor(Qt::lightGray),QColor(Qt::gray)};
    //先清空曲线
    if(this->customPlot->graphCount()>0)
    {
        this->customPlot->clearGraphs();
    }
    //初始化曲线属性
    if(m_parameterList!=Q_NULLPTR){
        for (int i=0; i<this->m_parameterList->size(); i++ )
        {
            ParameterClass *pc = this->m_parameterList->at(i);
            //对应的曲线序列
            customPlot->addGraph();
            customPlot->graph(i)->setPen(QPen(penColor.at(i))); // 曲线的颜色
            customPlot->graph(i)->setName(QString::number(pc->fpgaID)+"-"+pc->chName);
            //customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 曲线与X轴包围区的颜色
            //数据点显示图案
            //customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,8));
            //自动调整范围
            //customPlot->graph(i)->rescaleAxes(true);
            //数据多选
            //customPlot->graph(i)->setSelectable(QCP::SelectionType::stWhole);
            //设置抗锯齿
            //customPlot->graph(i)->setAntialiasedFill(true);
            //曲线上的数据选中模式
            //customPlot->graph(i)->setSelectable(QCP::stWhole);
        }
        this->customPlot->replot(); //刷新图像
    }
}

void OscilloscopeForm::initOscillTableView()
{
    //    for (int i=0; i<this->m_parameterList->size(); i++ )
    //    {
    //        //设置通道table瞬时值
    //        this->model1->setItem(0, i+1, new QStandardItem(QString::number(i*2)));
    //        this->model1->item(0, i+1)->setTextAlignment(Qt::AlignCenter);            //设置单元格文本居中
    //        this->model1->item(0, i+1)->setForeground(QBrush(QColor(255, 252, 255))); //设置单元格文本颜色
    //        this->model1->item(0, i+1)->setFont( QFont( "Times", 10, QFont::Bold ));  //设置单元格文本加粗
    //        this->model1->setItem(1, i+1, new QStandardItem(QString::number(i+1)));
    //        this->model1->item(1, i+1)->setTextAlignment(Qt::AlignCenter);            //设置单元格文本居中
    //        this->model1->item(1, i+1)->setForeground(QBrush(QColor(255, 252, 255))); //设置单元格文本颜色
    //        this->model1->item(1, i+1)->setFont( QFont( "Times", 10, QFont::Bold ));  //设置单元格文本加粗
    //    }

    //yMax yMin输入值范围限定
    //    ui->lineEdit->setValidator(new QDoubleValidator(-100,100,3,this));
    //    ui->lineEdit_2->setValidator(new QDoubleValidator(-100,100,3,this));
}

/**
 * @brief OscilloscopeForm::configChannelParamete--配置通道参数
 */
void OscilloscopeForm::configChannelParamete()
{
    if(this->m_parameterList!=Q_NULLPTR && this->m_parameterList->size() > 0)
    {
        this->waitDialog->show();
        //        MY_DEBUG << "m_parameterList->size is " << m_parameterList->size();
        //        MY_DEBUG << "OscilloscopeForm configChannelParamete start";
        BusinessInvoker *invoker = BusinessInvoker::instance();
        BusinessClass req;
        req.setLevel(BusinessClass::TCP_IP_HANDLER);
        req.setCommandType(BusinessClass::OSCILL_CH_CONFIG);
        //        MY_DEBUG << "OscilloscopeForm configChannelParamete size " << this->m_parameterList->size();
        for(int i=0; i<this->m_parameterList->size(); i++)
        {
            ParameterClass *pc = this->m_parameterList->at(i);
            req.addData(pc->getRegisterData());
        }
        invoker->exec(req);
        //        MY_DEBUG << "OscilloscopeForm configChannelParamete finish";
    }
}

/**
 * @brief OscilloscopeForm::initUiView 初始化ui view
 */
void OscilloscopeForm::initUiView()
{
    /*初始化示波器通道列表属性*/
    //ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setViewMode(QListView::IconMode);   //设置Item图标显示
    ui->listWidget->setDragEnabled(false);          //控件不允许拖动

    /*初始化示波器图表属性*/
    this->customPlot = ui->qcustomplot;
    connect(this->customPlot, &MyCustomPlot::yAxisUpdate, this, &OscilloscopeForm::on_yAxisUpdate);
    x_min=x_RangeLower, x_max=x_RangeUpper;
    y_min=y_RangeLower, y_max=y_RangeUpper;
    //显示鼠标跟踪曲线
    //    customPlot->showTracer(true);
    //

    QSharedPointer<QCPAxisTickerTime> xTicker(new QCPAxisTickerTime);
    xTicker->setTimeFormat("%h:%m:%s.%z");//精确到毫秒
    xTicker->setTickCount(4);

    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0,new QCPTextElement(customPlot,
                                                                 QString::fromLocal8Bit("动态示波器"), QFont("黑体", 8, QFont::Bold)));
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
    /*坐标设置*/
    //四边显示坐标轴
    //    customPlot->axisRect()->setupFullAxesBox();
    // make left and bottom axes always transfer their ranges to right and top axes:
    //connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    //give the axes some labels:
    customPlot->xAxis->setLabel("time(millisecond)");
    customPlot->yAxis->setLabel("value");
    customPlot->xAxis->setTicker(xTicker);
    customPlot->xAxis->setRange(x_RangeLower,x_RangeUpper);     //x轴范围
    customPlot->yAxis->setRange(y_RangeLower, y_RangeUpper);    //y轴范围

    //    customPlot->adjustSize();
    customPlot->setBackground(QBrush(Qt::white));
    //支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
    //    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom
    //                                /*|QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables*/);
    //    customPlot->xAxis->setBasePen(QPen(Qt::red, 12));//设置x轴基准线颜色和大小
    //    customPlot->yAxis->setBasePen(QPen(Qt::red, 12));//设置y轴基准线颜色和大小
    //    customPlot->rescaleAxes(true);
    //设置选框的边框和颜色
    //customPlot->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//虚线
    //customPlot->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//半透明浅蓝
    //customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
    //硬件加速开启
    customPlot->openGl();
    customPlot->replot();
    ui->lineEdit->setText(QString::number(y_RangeUpper));       //最大值
    ui->lineEdit_2->setText(QString::number(y_RangeLower));     //最小值

    /*初始化tableView*///--xy
    this->model2 = new QStandardItemModel;
    /*设置行字段名--表2*/
    QList<QString> colusList2 = {" ","1","2","3"};
    this->model2->setColumnCount(colusList2.size());   //列数
    for(int i=0; i<colusList2.size(); i++)
    {
        this->model2->setHeaderData(i, Qt::Horizontal, colusList2.at(i));
    }
    this->model2->setRowCount(2);
    this->model2->setItem(0, 0, new QStandardItem("X"));
    this->model2->item(0, 0)->setTextAlignment(Qt::AlignCenter);            //设置单元格文本居中
    this->model2->setItem(1, 0, new QStandardItem("Y"));
    this->model2->item(1, 0)->setTextAlignment(Qt::AlignCenter);            //设置单元格文本居中
    ui->tableView->setModel(this->model2);
    ui->tableView->setStyleSheet(CommonClass::TABLE_STYLE);
    ui->tableView->horizontalHeader()->setStyleSheet(CommonClass::TABLE_HEAD_SECTIONSTYLE);//"QHeaderView::section{background:rgb(22,40,76);}");
    ui->tableView->verticalHeader()->hide();
    ui->tableView->verticalHeader()->setDefaultSectionSize(15);     //行高
    ui->tableView->horizontalHeader()->setTabKeyNavigation(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);      //设置表格的单元为只读属性，即不能编辑
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);        //设置表格不可被选中

    /*初始化tableView--18路通道瞬时值*/
    this->model1 = new QStandardItemModel;
    /*设置行字段名--表1*/
    QList<QString> colusList1 = {"Ch","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18"};
    this->model1->setColumnCount(colusList1.size());   //列数
    for(int i=0; i<colusList1.size(); i++)
    {
        this->model1->setHeaderData(i, Qt::Horizontal, colusList1.at(i));
    }
    this->model1->setRowCount(2);
    this->model1->setItem(0, 0, new QStandardItem("X1"));
    this->model1->item(0, 0)->setTextAlignment(Qt::AlignCenter);            //设置单元格文本居中
    this->model1->setItem(1, 0, new QStandardItem("X2"));
    this->model1->item(1, 0)->setTextAlignment(Qt::AlignCenter);            //设置单元格文本居中
    ui->tableView_2->setStyleSheet(CommonClass::TABLE_STYLE);
    ui->tableView_2->horizontalHeader()->setStyleSheet(CommonClass::TABLE_HEAD_SECTIONSTYLE);//"QHeaderView::section{background:rgb(22,40,76);}");
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(15);     //行高
    ui->tableView_2->setModel(this->model1);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置表格的单元为只读属性，即不能编辑
    ui->tableView_2->setSelectionMode(QAbstractItemView::NoSelection);      //设置表格不可被选中
}

/**
 * @brief OscilloscopeForm::initData--初始化数据
 */
void OscilloscopeForm::initData()
{

}

/**
 * @brief OscilloscopeForm::startBusiness--开始业务逻辑
 */
void OscilloscopeForm::startBusiness()
{
    MY_DEBUG << "OscilloscopeForm startBusiness";
    //更新数据
    CabinetManagement *manager = CabinetManagement::instance();
    this->m_parameterList = manager->getOscilloscopeList(manager->getCurrentCabinetID());
    if(this->m_parameterList==Q_NULLPTR){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取参数失败，请检查参数文件"));
        //        return;
    }
    this->initOscillListView();
    this->initOscillCharts();
    this->initOscillTableView();

    ui->toolButton_8->setIcon(QPixmap(":/images/images/start.svg"));
    ui->toolButton_8->setDisabled(true);
    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &OscilloscopeForm::on_dataResponse);
    //唤醒数据接收线程
    invoker->wakeup(BusinessClass::TCP_IP_HANDLER);
    //    invoker->suspend(BusinessClass::MODBUS_HANDLER);
    //    invoker->suspend(BusinessClass::FTP_HANDLER);
    this->m_businessRunning=true;
    this->configChannelParamete();
}

/**
 * @brief OscilloscopeForm::endBusiness --结束业务逻辑
 */
void OscilloscopeForm::endBusiness()
{
    MY_DEBUG << "OscilloscopeForm endBusiness";
    //    ui->toolButton_8->setIcon(QPixmap(":/images/images/start.svg"));
    //    ui->toolButton_8->setDisabled(true);
    if(this->m_parameterList==Q_NULLPTR){
        return;
    }
    if(acqStatus == START)
    {
        this->stopACQ();        //停止示波器
        acqStatus = STOP;
    }
    //解除绑定信号与槽---需要发送不再接收下位机示波器数据的请求(待完成)
    this->m_businessRunning=false;
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &OscilloscopeForm::on_dataResponse);
    //挂起数据接收线程
    //invoker->suspend(BusinessClass::TCP_IP_HANDLER);
    //退出自动保存线程
    //    if(this->autoSaveTask!=NULL){
    //        this->autoSaveTask->taskFinish();
    ////        this->autoSaveTask=NULL;
    //    }
    this->waitDialog->hide();
}

/**
 * @brief on_dataResponse 下位机数据响应
 * @param success
 * @param m_Data
 * @param errorID
 * @param errorMsg
 */
void OscilloscopeForm::on_dataResponse(BusinessResponse data)
{
    //MY_DEBUG << "success is " << success << "data.getResponseType() IS " << data.getResponseType();
    if(!data.result){
        this->waitDialog->hide();
        CommonDialog::showErrorDialog(this, data.errorMsg);
        return;
    }
    switch(data.type)
    {
    case BusinessClass::OSCILL_CH_CONFIG:
        this->waitDialog->hide();
        //        if(success){
        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("通道配置成功"));
        ui->toolButton_8->setDisabled(false);   //可以点击开始按钮接收波形数据
        //        }else{
        //            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("通道配置失败"));
        //            //继续配置通道参数到下位机
        //            this->configChannelParamete();
        //        }
        break;
    case BusinessClass::OSCILL_SAMPLE:
    {
        //        if(!success){
        //            CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("获取波形数据失败"));
        //        }else{
        //            qDebug() << "0->" << QDateTime::currentDateTime().toString("ss.zzz");
        double x_end = 0;
        double x_start=0;
        QList<BusinessResponse::OscillResponse> dataList = data.getOscillWaveData();
        //        if(dataList.size()!=this->customPlot->graphCount()){
        //            MY_DEBUG << "Oscill data is not match graph count";
        //        }
        //图像移动，以及删除图标范围外的数据
        double fullScreenCount = x_RangeUpper*100/data.getOscSampleTime() *0.2;
        int count = qMin(dataList.size(), this->customPlot->graphCount());
        y_max=0;
        for(int i=0; i<count; i++){
            BusinessResponse::OscillResponse item = dataList.at(i);
            QCPGraph *graph = this->customPlot->graph(i);
            graph->addData(item.x, item.y);
            //只记录显示的曲线的最大值
            if(graph->visible()){
                //记录最大值
                QVector<double>::iterator iter;
                for (iter=item.y.begin();iter!=item.y.end();iter++) {
                    y_max = qMax(y_max, qAbs(*iter));
                }
            }

            if(i==0){
                x_start = item.x.first();
            }
            if(i==count-1){
                x_end = item.x.last();
            }
            double new_x = item.x.first();
            if(graph->dataCount() > fullScreenCount){
                graph->data().data()->remove(0, new_x-fullScreenCount);
                x_min = new_x - fullScreenCount;
            }
            //                graph->rescaleAxes(true);
        }
        if(x_end > x_RangeUpper){
            x_max = x_min + fullScreenCount;    // x_end;
            //x_min = x_max-(x_RangeUpper-x_RangeLower);        //by bruce 2023-03-17
            this->customPlot->xAxis->setRange(x_min, x_max);
        }
//        if(x_start==0){
//            this->customPlot->xAxis->setRange(x_RangeLower, x_RangeUpper);    //by bruce
//        }

        this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新图像
        //            qDebug() << "1->" << QDateTime::currentDateTime().toString("ss.zzz");
        //如果启动了自动保存则保存到文件
        if(this->autoCollectting){
            this->autoSaveTask->inQueue(data);
        }
        this->oscillData = dataList;
    }
        break;
    default:
        break;
    }
}

void OscilloscopeForm::on_yAxisUpdate(float yValue)
{
    //接收数据状态不显示数值
    if(this->acqStatus==START){
        return;
    }
    //    MY_DEBUG << yValue;
    bool findRange;
    for(int i=0; i<this->customPlot->graphCount(); i++)
    {
        QCPDataContainer<QCPGraphData> *qs = this->customPlot->graph(i)->data().data();
        QCPRange valueRange = qs->valueRange(findRange);
        if(yValue > valueRange.maxRange)
        {
            MY_DEBUG << "out of range " << qs->constBegin()->valueRange().maxRange;
            continue;
        }
        for(int j=0; j<qs->size(); j++)
        {
            //            MY_DEBUG << qs->at(j)->mainValue();
            if(qAbs(qs->at(j)->mainValue()-yValue)<1)
            {
                MY_DEBUG << "find";
                double x = qs->at(j)->mainKey();
                this->model1->setItem(0, i+1, new QStandardItem(QString::number(x)));
                break;
            }
        }
    }
}

/**
 * @brief OscilloscopeForm::onListItemDelete--列表项删除
 * @param index
 */
void OscilloscopeForm::onListItemDelete(uint32_t id)
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
    QMessageBox msgBox;
    msgBox.setText(QString::fromLocal8Bit("曲线删除?"));
    msgBox.setInformativeText(QString::fromLocal8Bit("确定要删除该曲线吗？"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        for(int i=0; i<ui->listWidget->count(); i++){
            WaveItemWidget *wave = (WaveItemWidget *)ui->listWidget->itemWidget(ui->listWidget->item(i));
            if(wave->getId() == id){
                ui->listWidget->takeItem(i);
                CabinetManagement *manager = CabinetManagement::instance();
                manager->removeParameter2Oscilloscope(this->m_parameterList->at(i));
                //                this->m_parameterList = manager->getOscilloscopeList(manager->getCurrentCabinetID());
                //                this->m_parameterList->removeAt(i);
                break;
            }
        }
        for(int i=0; i<this->customPlot->graphCount(); i++){
            QString graphID = this->customPlot->graph(i)->name().split("-").at(0);
            if(graphID == QString::number(id)){
                this->customPlot->removeGraph(i);
                break;
            }
        }

        this->customPlot->replot();
        if(this->m_parameterList->size()==0){
            ui->toolButton_8->setDisabled(true);    //没有通道的情况下开始按钮不使能
        }else{
            this->configChannelParamete();    //删除后需要重新配置参数
        }
        //        ui->listWidget->takeItem(index);
        //        if(m_parameterList!=Q_NULLPTR){
        //            this->m_parameterList->removeAt(index);
        //            this->customPlot->removeGraph(index);
        //            this->customPlot->replot();
        //            this->configChannelParamete();    //删除后需要重新配置参数
        //        }
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
 * @brief OscilloscopeForm::onListItemShow--列表项显示/隐藏
 * @param index
 * @param show
 */
void OscilloscopeForm::onListItemShow(uint32_t id, bool show)
{
    for(int i=0; i<this->customPlot->graphCount(); i++){
        QString graphID = this->customPlot->graph(i)->name().split("-").at(0);
        if(graphID == QString::number(id)){
            this->customPlot->graph(i)->setVisible(show);
            if(show){
                channelShowCount++;
            }else{
                channelShowCount--;
            }
            if(channelShowCount==0){
                ui->toolButton_12->setIcon(QIcon(":/images/images/eys_close.svg"));
            }else{
                ui->toolButton_12->setIcon(QIcon(":/images/images/eys_open.svg"));
            }
            break;
        }
    }
    this->customPlot->replot();
}

/**
 * @brief OscilloscopeForm::onListItemZoom--列表项放大/缩小
 * @param index
 */
void OscilloscopeForm::onListItemZoom(uint32_t id)
{
    //    qDebug("list item %d Zoom\n\r", index);
}

/**
 * @brief OscilloscopeForm::startACQ--示波器开始
 */
void OscilloscopeForm::startACQ()
{
    MY_DEBUG;
    ui->toolButton_8->setIcon(QPixmap(":/images/images/stop.png"));
    //取消缩放状态
    ui->toolButton_2->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    isZoom=false;
    //取消移动状态
    ui->toolButton->setIcon(QIcon(":/images/images/move-true.png"));
    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    isMove=false;
    //取消突出显示数据点
    for(int i=0; i<this->customPlot->graphCount();i++){
        customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,0));
    }
    isShowPoint=false;
    //取消捕捉状态
    ui->toolButton_10->setIcon(QIcon(":/images/images/catch-true.png"));
    this->customPlot->showTracer(false);
    isCatch=false;

    BusinessInvoker *invoker = BusinessInvoker::instance();
    BusinessClass req;
    req.setLevel(BusinessClass::TCP_IP_HANDLER);
    req.setCommandType(BusinessClass::OSCILL_SAMPLE);
    invoker->exec(req);

}

/**
 * @brief OscilloscopeForm::stopACQ--示波器停止
 */
void OscilloscopeForm::stopACQ()
{
    MY_DEBUG;
    ui->toolButton_8->setIcon(QPixmap(":/images/images/start.svg"));
    BusinessInvoker *invoker = BusinessInvoker::instance();
    BusinessClass req;
    req.setLevel(BusinessClass::TCP_IP_HANDLER);
    req.setCommandType(BusinessClass::OSCILL_SAMPLE_STOP);
    invoker->exec(req);
}

/**
 * @brief OscilloscopeForm::开始运行按钮
 */
void OscilloscopeForm::on_toolButton_8_clicked()
{
    switch (this->acqStatus) {
    case STOP:
    {
        acqStatus = START;
        this->startACQ();
    }
        break;
    case START:
    {
        acqStatus = STOP;
        this->stopACQ();
    }
        break;
    default:
        break;
    }
}

/**
 * @brief OscilloscopeForm::保存按钮
 */
void OscilloscopeForm::on_toolButton_9_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }

    if(this->customPlot->graphCount()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("无示波器通道数据"));
        return;
    }
    if(this->oscillData.size()==0){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("无示波器通道数据"));
        return;
    }
    QString title = QString::fromLocal8Bit("动态示波器数据保存");
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    title,
                                                    "",
                                                    tr("csv(*.csv)"));
    if (!fileName.isNull()){
        //fileName是文件名
        this->waitDialog->show();
        OscillDataSaveThread *workerThread = new OscillDataSaveThread(this->oscillData, fileName);
        connect(workerThread, &OscillDataSaveThread::resultReady, this, [=](bool success, QString msg){
            this->waitDialog->hide();
            if(success){
                CommonDialog::showSuccessDialog(this, msg);
            }else{
                CommonDialog::showErrorDialog(this, msg);
            }
        });
        connect(workerThread, &OscillDataSaveThread::finished, workerThread, &QObject::deleteLater);
        workerThread->start();
    }else{
        //点的是取消
    }
}

/**
 * @brief OscilloscopeForm::自动采集 check box
 * @param arg1--是否勾选
 */
void OscilloscopeForm::on_checkBox_stateChanged(int arg1)
{
    this->autoCollectting=arg1;
}

/**
 * @brief OscilloscopeForm::on_toolButton_10_clicked--捕捉
 */
void OscilloscopeForm::on_toolButton_10_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }

    if(isCatch){
        //取消捕捉状态
        ui->toolButton_10->setIcon(QIcon(":/images/images/catch-true.png"));
        this->customPlot->showTracer(false);
        isCatch=false;
    }else{
        //设置捕捉状态
        ui->toolButton_10->setIcon(QIcon(":/images/images/catch-false.png"));
        this->customPlot->showTracer(true);
        isCatch=true;
    }
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    ui->toolButton_2->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->isZoom=false;

    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    ui->toolButton->setIcon(QIcon(":/images/images/move-true.png"));
    this->isMove=false;

    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief OscilloscopeForm::on_toolButton_11_clicked--突出显示点
 */
void OscilloscopeForm::on_toolButton_11_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }

    if(isShowPoint){
        for(int i=0; i<this->customPlot->graphCount();i++){
            customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,0));
        }
        isShowPoint=false;
    }else{
        for(int i=0; i<this->customPlot->graphCount();i++){
            customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,8));
        }
        isShowPoint=true;
    }
    customPlot->replot(QCustomPlot::rpImmediateRefresh);
}

/**
 * @brief OscilloscopeForm::图像平移
 */
void OscilloscopeForm::on_toolButton_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }

    if(isMove){
        //取消移动状态
        ui->toolButton->setIcon(QIcon(":/images/images/move-true.png"));
        this->customPlot->setInteraction(QCP::iRangeDrag,false);
        isMove=false;
    }else{
        //设置移动状态
        ui->toolButton->setIcon(QIcon(":/images/images/move-false.png"));
        this->customPlot->setInteraction(QCP::iRangeDrag,true);
        isMove=true;
    }
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    ui->toolButton_2->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->isZoom=false;

    this->customPlot->showTracer(false);
    ui->toolButton_10->setIcon(QIcon(":/images/images/catch-true.png"));
    this->isCatch=false;

    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief OscilloscopeForm::图像缩放
 */
void OscilloscopeForm::on_toolButton_2_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
    if(isZoom){
        //取消缩放状态
        ui->toolButton_2->setIcon(QIcon(":/images/images/zoom-true.png"));
        this->customPlot->setInteraction(QCP::iRangeZoom,false);
        isZoom=false;
    }else{
        //设置缩放状态
        ui->toolButton_2->setIcon(QIcon(":/images/images/zoom-false.png"));
        this->customPlot->setInteraction(QCP::iRangeZoom,true);
        isZoom=true;
    }
    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    ui->toolButton->setIcon(QIcon(":/images/images/move-true.png"));
    this->isMove=false;

    this->customPlot->showTracer(false);
    ui->toolButton_10->setIcon(QIcon(":/images/images/catch-true.png"));
    this->isCatch=false;
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief OscilloscopeForm::恢复
 */
void OscilloscopeForm::on_toolButton_3_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    ui->toolButton_2->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->isZoom=false;

    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    ui->toolButton->setIcon(QIcon(":/images/images/move-true.png"));
    this->isMove=false;

    this->customPlot->showTracer(false);
    ui->toolButton_10->setIcon(QIcon(":/images/images/catch-true.png"));
    this->isCatch=false;


    this->customPlot->xAxis->setRange(x_min, x_max);
    this->customPlot->yAxis->setRange(y_min, y_max);
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh);

    //更新Y轴最大最小值
    ui->lineEdit->setText(QString::number(y_RangeUpper));       //最大值
    ui->lineEdit_2->setText(QString::number(y_RangeLower));     //最小值
}

/**
 * @brief OscilloscopeForm::清除所有通道
 */
void OscilloscopeForm::on_pushButton_4_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(QString::fromLocal8Bit("曲线删除?"));
    msgBox.setInformativeText(QString::fromLocal8Bit("确定要删除全部曲线吗？"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:{
        //删除曲线
        this->customPlot->clearGraphs();
        this->customPlot->replot();
        //清空列表
        //    this->m_parameterList->clear();
        ui->listWidget->clear();

        //删除所有示波器采集通道
        CabinetManagement *manager = CabinetManagement::instance();
        manager->removeAllParameter2Oscilloscope();

        //开始按钮不使能
        ui->toolButton_8->setDisabled(true);
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
 * @brief OscilloscopeForm::隐藏通道list
 */
void OscilloscopeForm::on_pushButton_5_clicked()
{
    ui->frame_3->setVisible(false);
    ui->pushButton->setVisible(true);
}

/**
 * @brief OscilloscopeForm::时基--加
 */
void OscilloscopeForm::on_toolButton_4_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
}

/**
 * @brief OscilloscopeForm::时基--减
 */
void OscilloscopeForm::on_toolButton_5_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
}

/**
 * @brief OscilloscopeForm::波形移动--左
 */
void OscilloscopeForm::on_toolButton_6_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
}

/**
 * @brief OscilloscopeForm::波形移动--右
 */
void OscilloscopeForm::on_toolButton_7_clicked()
{
    if(acqStatus==START)
    {
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("数据读取中，请先暂停"));
        return;
    }
}

static float yMinCurrSetV=0;
static float yMaxCurrSetV=0;
const float EPSINON = 0.00001f;
/**
 * @brief OscilloscopeForm::y轴范围值设置
 */
void OscilloscopeForm::on_pushButton_3_clicked()
{
    QString yMax = ui->lineEdit->text();
    QString yMin = ui->lineEdit_2->text();

    if(yMin.isEmpty() || yMax.isEmpty()){
        QMessageBox::information(this, QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("输入值为空！"),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    float yMinv = yMin.toFloat();
    float yMaxv = yMax.toFloat();
    if(abs(yMinCurrSetV- yMinv)<=EPSINON && abs(yMaxCurrSetV - yMaxv) <= EPSINON){
        QMessageBox::information(this, QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("输入值未改变！"),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    //更新Y轴设置
    customPlot->yAxis->setRange(yMinv,yMaxv);      //y轴范围
    customPlot->replot();
}

/**
 * @brief OscilloscopeForm::on_pushButton_clicked--显示通道列表
 */
void OscilloscopeForm::on_pushButton_clicked()
{
    ui->frame_3->setVisible(true);
    ui->pushButton->setVisible(false);
}

/**
 * @brief OscilloscopeForm::slotContextMenu--右键菜单响应函数
 * @param pos
 */
void OscilloscopeForm::slotContextMenu(QPoint pos)
{
    MY_DEBUG;
    //表格每行点击右键菜单
    QMenu popMenu;
    const char* menuItem[5] = {"凸显数据点","图表追踪","图表移动","图表缩放","图表恢复"};
    for(int i=0; i<5; i++)
    {
        QAction* action = new QAction();
        switch (i) {
        case 0:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &OscilloscopeForm::on_toolButton_11_clicked);
            break;
        case 1:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &OscilloscopeForm::on_toolButton_10_clicked);
            break;
        case 2:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &OscilloscopeForm::on_toolButton_clicked);
            break;
        case 3:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &OscilloscopeForm::on_toolButton_2_clicked);
            break;
        case 4:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &OscilloscopeForm::on_toolButton_3_clicked);
            break;
        }
    }
    popMenu.exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

/**
 * @brief OscilloscopeForm::on_toolButton_12_clicked--所有通道开关
 */
void OscilloscopeForm::on_toolButton_12_clicked()
{
    bool show=false;
    if(this->channelShowCount==0){  //闭眼状态
        show=true;
    }
    //隐藏/显示 曲线
    for(int i=0; i<this->customPlot->graphCount(); i++){
        this->customPlot->graph(i)->setVisible(show);
    }
    //隐藏/显示 list item
    for(int i=0; i<ui->listWidget->count(); i++){
        WaveItemWidget *wave = (WaveItemWidget *)ui->listWidget->itemWidget(ui->listWidget->item(i));
        wave->setShow(show);
    }
    if(show){
        this->channelShowCount=this->customPlot->graphCount();
        ui->toolButton_12->setIcon(QIcon(":/images/images/eys_open.svg"));
    }else{
        this->channelShowCount=0;
        ui->toolButton_12->setIcon(QIcon(":/images/images/eys_close.svg"));
    }
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

