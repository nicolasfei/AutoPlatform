#include "waveform.h"
#include "ui_waveform.h"
#include "common/cabinetmanagement.h"
#include "common/commonclass.h"
#include "common/commondialog.h"
#include "commonView/waveitemwidget.h"
#include "commonView/listchooisedialog.h"
#include "communication/businessinvoker.h"
#include "loadwavefileqthread.h"

#include <QVector>

WaveForm::WaveForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::WaveForm),
    isCatch(false),
    isMove(false),
    isZoom(false),
    isRecovery(false),
    isShowPoint(false),
    channelShowCount(0)
{
    ui->setupUi(this);
    //设置背景色
    ui->listFrame_2->setStyleSheet("#listFrame{border-image: url(:/images/images/waveg-2.png);}");
    ui->charFrame->setStyleSheet("#charFrame{border-image: url(:/images/images/waveg-3.png);}");
    ui->gridFrame->setStyleSheet("#gridFrame{border-image: url(:/images/images/waveg-1.png);}");
    ui->verticalFrame_3->setStyleSheet("#verticalFrame_3{border-image: url(:/images/images/waveg-5.png);}");
    ui->horizontalFrame_8->setStyleSheet("#horizontalFrame_8{border-image: url(:/images/images/waveg-4.png);}");
    ui->verticalFrame_2->setStyleSheet("#verticalFrame_2{border-image: url(:/images/images/oscill_list_bg.png);}");

    ui->horizontalFrame_2->setVisible(false);       //下方表操作隐藏
    ui->horizontalFrame_4->setVisible(true);    //放大缩小等3个按键隐藏
    ui->toolButton_9->setVisible(true);
    ui->toolButton_2->setVisible(false);        //保存案件隐藏
    ui->horizontalFrame_7->setVisible(false);       //隐藏天数快捷键
    ui->toolButton_11->setVisible(false);       //隐藏通道列表一键隐藏显示
    ui->pushButton->setVisible(false);      //隐藏显示通道列表按钮

    this->m_businessRunning=false;
    //    this->m_yValueList = new QList<QVector<double>*>;
    //this->waitDialog->setHint(QString::fromLocal8Bit("文件数据加载中..."));
    this->initData();
    this->initUiView();

    //关联右键点击事件--菜单
    connect(ui->customPlot, &MyCustomPlot::customContextMenuRequested, this, &WaveForm::slotContextMenu);
}

WaveForm::~WaveForm()
{
    delete ui;
}

/**
 * @brief WaveForm::initUiView--初始化ui view
 */
void WaveForm::initUiView()
{
    /*初始化示波器通道列表属性*/
    //    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setViewMode(QListView::IconMode);   //设置Item图标显示
    ui->listWidget->setDragEnabled(false);          //控件不允许拖动

    /*初始化示波器图表属性*/
    this->customPlot = ui->customPlot;
    //显示鼠标跟踪曲线
    //    customPlot->showTracer(true);

    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0,new QCPTextElement(customPlot,
                                                                 QString::fromLocal8Bit("故障录波分析器"),
                                                                 QFont("黑体", 12, QFont::Bold)));
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    /*坐标设置*/
    //四边显示坐标轴
    customPlot->axisRect()->setupFullAxesBox();
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    //give the axes some labels:
    customPlot->xAxis->setLabel("time(millisecond)");
    customPlot->yAxis->setLabel("value");
    customPlot->xAxis->setRange(this->m_xMin, this->m_xMax);    //x轴范围
    customPlot->yAxis->setRange(this->m_yMin, this->m_yMax);    //y轴范围
    customPlot->setBackground(QBrush(Qt::white));
    //支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
    //    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
    //                                QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    //    customPlot->xAxis->setBasePen(QPen(Qt::red, 12));//设置x轴基准线颜色和大小
    //    customPlot->yAxis->setBasePen(QPen(Qt::red, 12));//设置y轴基准线颜色和大小
    //    customPlot->rescaleAxes(true);
    //    //设置选框的边框和颜色
    //    customPlot->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//虚线
    //    customPlot->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//半透明浅蓝
    //    customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
    //硬件加速开启
    customPlot->openGl();
    customPlot->replot();
    //显示y轴最大最小值
    ui->lineEdit_12->setText(QString::number(m_yMin));
    ui->lineEdit_11->setText(QString::number(m_yMax));
}

/**
 * @brief WaveForm::initData--初始化数据
 */
void WaveForm::initData()
{

}

/**
 * @brief WaveForm::startBusiness--开始业务逻辑
 */
void WaveForm::startBusiness()
{
    MY_DEBUG << "startBusiness";
    this->m_businessRunning=true;
    BusinessInvoker *invoker = BusinessInvoker::instance();
    connect(invoker, &BusinessInvoker::response, this, &WaveForm::on_dataResponse);
    this->m_businessRunning=true;
    //唤醒modbus线程
    invoker->wakeup(BusinessClass::FTP_HANDLER);
    //    invoker->suspend(BusinessClass::TCP_IP_HANDLER);
    //    invoker->suspend(BusinessClass::MODBUS_HANDLER);
}

/**
 * @brief WaveForm::endBusiness--结束业务逻辑
 */
void WaveForm::endBusiness()
{
    MY_DEBUG << "endBusiness";
    this->waitDialog->hide();
    //解除绑定信号与槽---需要发送不再接收下位机示波器数据的请求(待完成)
    this->m_businessRunning=false;
    BusinessInvoker *invoker = BusinessInvoker::instance();
    disconnect(invoker, &BusinessInvoker::response, this, &WaveForm::on_dataResponse);
    //挂起线程
    invoker->suspend(BusinessClass::FTP_HANDLER);
}

/**
 * @brief WaveForm::buildChannelList--组件通道列表
 * @param chanName name
 */
void WaveForm::buildChannelList(QList<QString> *chanName)
{
    //初始化listView
    if(ui->listWidget->count()>0){
        ui->listWidget->clear();    //先清空通道列表
    }

    CabinetManagement *manager = CabinetManagement::instance();
    QStringList *nameList = manager->getWaveChannelNameList();

    int channelSize = qMin(nameList->size(), chanName->size());

    for (int i=0; i<channelSize; i++ ){
        WaveItemWidget *item = new WaveItemWidget(this);
        QString tmp = nameList->at(i).split(":").at(1);
        item->setData(i, i, tmp, true, true);
        QListWidgetItem* pItem = new QListWidgetItem();
        pItem->setSizeHint(QSize(210, 32));
        ui->listWidget->addItem(pItem);
        ui->listWidget->setItemWidget(pItem, item);
        connect(item, &WaveItemWidget::deleteItem, this, &WaveForm::on_channelListItemDelete);
        connect(item, &WaveItemWidget::isShowWave, this, &WaveForm::on_channelListItemShow);
        connect(item, &WaveItemWidget::fixName, this, &WaveForm::on_channelListItemFixName);
        //connect(item, &WaveItemWidget::zoomWave, this, &WaveForm::on_channelListItemZoom);
    }
}

/**
 * @brief WaveForm::buildCurveChart--组件曲线图表
 * @param chanName
 * @param xValue
 * @param yValue
 */
void WaveForm::buildCurveChart(QList<QString> *chanName, QVector<double> *xValue,
                               QList<QVector<double>*> *yValue)
{
    static QList<QColor> penColor = {QColor(Qt::red),QColor(Qt::green),QColor(Qt::blue),QColor(Qt::cyan),
                                     QColor(Qt::magenta),QColor(Qt::yellow),QColor(Qt::darkRed),QColor(Qt::darkGreen),
                                     QColor(Qt::darkBlue),QColor(Qt::darkCyan),QColor(Qt::darkMagenta),QColor(Qt::darkYellow),
                                     QColor(Qt::darkGray),QColor(Qt::black),QColor(Qt::lightGray),QColor(Qt::gray)};
    //先清空曲线
    if(this->customPlot->graphCount()>0)
    {
        this->customPlot->clearGraphs();
        //this->customPlot->replot(); //刷新图像
    }

    CabinetManagement *manager = CabinetManagement::instance();
    QStringList *nameList = manager->getWaveChannelNameList();

    int channelSize = qMin(nameList->size(), chanName->size());

    //初始化曲线属性
    for (int i=0; i<channelSize; i++ )
    {
        //对应的曲线序列
        customPlot->addGraph();
        customPlot->graph(i)->setPen(QPen(penColor.at((i%penColor.size())))); // 曲线的颜色
        customPlot->graph(i)->setName(QString::number(i) + "-" + nameList->at(i).split(":").at(1));

        //添加数据
        customPlot->graph(i)->setData(*xValue, *yValue->at(i), true);
    }
    //添加故障点竖线
    customPlot->addGraph();

    this->m_xMiddle = xValue->at(xValue->size()/2-1);
    this->updateCentralAxis(this->m_yMin);
    //设置X轴范围
    this->m_xMin = xValue->at(0);
    this->m_xMax = xValue->at(xValue->size()-1);
    customPlot->xAxis->setRange(m_xMin, m_xMax);     //x轴范围
    this->customPlot->replot(); //刷新图像
}

/**
 * @brief WaveForm::updateCentralAxis--更新中轴线
 */
void WaveForm::updateCentralAxis(double yMin)
{
    if(customPlot->graphCount()>0){
        QPen pen;
        pen.setColor(QColor(Qt::black));
        pen.setWidth(1);
        customPlot->graph(customPlot->graphCount()-1)->setPen(pen); // 曲线的颜色
        customPlot->graph(customPlot->graphCount()-1)->addData(this->m_xMiddle,std::numeric_limits<double>::max());
        customPlot->graph(customPlot->graphCount()-1)->addData(this->m_xMiddle,yMin /*std::numeric_limits<double>::min()*/);
    }
}

/**
 * @brief WaveForm::on_fileSelectResult--获取选择的文件
 * @param list
 */
void WaveForm::on_fileSelectResult(QList<QString>* list)
{
    this->waitDialog->show();
    BusinessInvoker *invoker = BusinessInvoker::instance();
    BusinessClass req;
    req.setLevel(BusinessClass::FTP_HANDLER);
    req.setCommandType(BusinessClass::WAVE_OBTAIN);
    req.setWaveData(*list);
    invoker->exec(req);
}

/**
 * @brief on_dataResponse 下位机数据响应
 * @param success
 * @param m_Data
 * @param errorID
 * @param errorMsg
 */
void WaveForm::on_dataResponse(BusinessResponse data)
{
    this->waitDialog->hide();
    if(!data.result){
        CommonDialog::showErrorDialog(this, data.errorMsg);
        return;
    }
    switch(data.type)
    {
    case BusinessClass::WAVE_QUERY:
    {
        QList<QString> waveList = data.m_waveData;
        ListChooiseDialog dialog(waveList);
        connect(&dialog, &ListChooiseDialog::selectResult, this, &WaveForm::on_fileSelectResult);
        dialog.exec();
    }
        break;
    case BusinessClass::WAVE_OBTAIN:
        CommonDialog::showSuccessDialog(this, QString::fromLocal8Bit("获取故障录波成功"));
        break;
    default:
        break;
    }
}

/**
 * @brief WaveForm::on_toolButton_clicked--实时波形导入
 */
void WaveForm::on_toolButton_clicked()
{
    this->loadWaveFile();
}

/**
 * @brief WaveForm::loadWaveFile--导入波形文件
 */
void WaveForm::loadWaveFile()
{
    QString dir;
    CabinetManagement *manager = CabinetManagement::instance();
    CabinetManagement::CabinetDevice::NetConfig *config = manager->getNetConfig(manager->getCurrentCabinetID());
    //    dir.append(config->root + config->faultUploadDir).append(QString::number(CabinetManagement::instance()->getCurrentCabinetID()));
    dir.append(config->m_ftpFaultUploadDir);
    QString title = QString::fromLocal8Bit("请选择要导入的故障录波数据文件");
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                title,
                dir,
                tr("csv files (*.csv)"));

    if(!files.isEmpty()){
        //只能处理一个文件
        QString file = files.at(0);
        ui->lineEdit->setText(file);
        this->waitDialog->show();
        LoadWaveFileQThread *load = new LoadWaveFileQThread(file);
        connect(load, &LoadWaveFileQThread::waveDataResponse, this, &WaveForm::on_waveDataResponse);
        connect(load, &LoadWaveFileQThread::finished, load, &LoadWaveFileQThread::deleteLater);
        load->start();
    }
}

/**
 * @brief WaveForm::on_waveDataResponse--波形文件加载响应
 * @param success
 * @param coverOk
 * @param chanName
 * @param xValue
 * @param datas
 */
void WaveForm::on_waveDataResponse(bool success, bool coverOk, QList<QString> *chanName, QVector<double> *xValue, QList<QVector<double>*>* datas)
{
    MY_DEBUG;
    this->waitDialog->hide();
    if(!success){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("打开文件失败"));
        return;
    }
    if(!coverOk){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("读取文件数据有误：文件数据格式有误"));
        return;
    }
    //通道列表组建
    this->buildChannelList(chanName);
    //曲线图形组件
    this->buildCurveChart(chanName, xValue, datas);
    //记录加载了多少个通道数据
    this->channelShowCount = chanName->size();
    //显示通道列表一键隐藏显示
    if(this->channelShowCount>0)
        ui->toolButton_11->setIcon(QIcon(":/images/images/eys_open.svg"));
    else
        ui->toolButton_11->setIcon(QIcon(":/images/images/eys_close.svg"));
    ui->toolButton_11->setVisible(true);
    delete chanName;
    delete  xValue;
    delete  datas;
}

/**
 * @brief WaveForm::on_toolButton_2_clicked--保存波形文件--暂时放弃
 */
void WaveForm::on_toolButton_2_clicked()
{

}

/**
 * @brief WaveForm::on_channelListItemDelete--通道列表删除item
 * @param itemID
 */
void WaveForm::on_channelListItemDelete(uint32_t id)
{
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
        this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
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
 * @brief WaveForm::on_channelListItemShow--通道列表显示/隐藏item
 * @param itemID
 * @param show
 */
void WaveForm::on_channelListItemShow(uint id, bool show)
{
    for(int i=0; i<this->customPlot->graphCount(); i++){
        QString graphID = this->customPlot->graph(i)->name().split("-").at(0);
        if(graphID == QString::number(id)){
            this->customPlot->graph(i)->setVisible(show);
            if(show){
                this->channelShowCount++;
            }else{
                this->channelShowCount--;
            }
            if(this->channelShowCount==0){
                ui->toolButton_11->setIcon(QIcon(":/images/images/eys_close.svg"));
            }else{
                ui->toolButton_11->setIcon(QIcon(":/images/images/eys_open.svg"));
            }
            break;
        }
    }
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief WaveForm::on_channelListItemFixName--参数列表项名字修改
 * @param id
 * @param newName
 */
void WaveForm::on_channelListItemFixName(uint32_t id, QString newName)
{
    //更新曲线显示
    for(int i=0; i<this->customPlot->graphCount(); i++){
        QString graphID = this->customPlot->graph(i)->name().split("-").at(0);
        if(graphID == QString::number(id)){
            this->customPlot->graph(i)->setName(QString::number(id)+":"+newName);
            break;
        }
    }
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新

    //更新文件
    CabinetManagement *manager = CabinetManagement::instance();
    manager->updateWaveChannel(id, newName);
}

/**
 * @brief WaveForm::on_channelListItemZoom--通道列表放大item--丢弃
 * @param itemID
 */
void WaveForm::on_channelListItemZoom(uint32_t itemName)
{

}


/**
 * @brief WaveForm::on_toolButton_8_clicked--获取下位机故障录波文件列表
 */
void WaveForm::on_toolButton_8_clicked()
{
    this->waitDialog->show();
    BusinessInvoker *invoker = BusinessInvoker::instance();
    CabinetManagement *manager = CabinetManagement::instance();
    CabinetManagement::CabinetDevice::NetConfig *config = manager->getNetConfig(manager->getCurrentCabinetID());
    QString ftpPath="";
    BusinessClass req;
    req.setLevel(BusinessClass::FTP_HANDLER);
    req.setCommandType(BusinessClass::WAVE_QUERY);
    ftpPath = "ftp://"+config->m_ftpIP + FAULT_DIR + QString::number(manager->getCurrentCabinetID())+"/";
    req.setFtpConfig(config->m_ftpUser, config->m_ftpLogin, ftpPath);
    invoker->exec(req);
}

/**
 * @brief WaveForm::on_toolButton_3_clicked--捕捉
 */
void WaveForm::on_toolButton_3_clicked()
{
    if(isCatch){
        //取消捕捉状态
        ui->toolButton_3->setIcon(QIcon(":/images/images/catch-true.png"));
        this->customPlot->showTracer(false);
        isCatch=false;
    }else{
        //设置捕捉状态
        ui->toolButton_3->setIcon(QIcon(":/images/images/catch-false.png"));
        this->customPlot->showTracer(true);
        isCatch=true;
    }
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    ui->toolButton_4->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->isZoom=false;

    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    ui->toolButton_10->setIcon(QIcon(":/images/images/move-true.png"));
    this->isMove=false;

    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief WaveForm::on_toolButton_10_clicked--移动
 */
void WaveForm::on_toolButton_10_clicked()
{
    if(isMove){
        //取消移动状态
        ui->toolButton_10->setIcon(QIcon(":/images/images/move-true.png"));
        this->customPlot->setInteraction(QCP::iRangeDrag,false);
        isMove=false;
    }else{
        //设置移动状态
        ui->toolButton_10->setIcon(QIcon(":/images/images/move-false.png"));
        this->customPlot->setInteraction(QCP::iRangeDrag,true);
        isMove=true;
    }
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    ui->toolButton_4->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->isZoom=false;

    this->customPlot->showTracer(false);
    ui->toolButton_3->setIcon(QIcon(":/images/images/catch-true.png"));
    this->isCatch=false;

    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief WaveForm::on_toolButton_4_clicked--缩放
 */
void WaveForm::on_toolButton_4_clicked()
{
    if(isZoom){
        //取消缩放状态
        ui->toolButton_4->setIcon(QIcon(":/images/images/zoom-true.png"));
        this->customPlot->setInteraction(QCP::iRangeZoom,false);
        isZoom=false;
    }else{
        //设置缩放状态
        ui->toolButton_4->setIcon(QIcon(":/images/images/zoom-false.png"));
        this->customPlot->setInteraction(QCP::iRangeZoom,true);
        isZoom=true;
    }
    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    ui->toolButton_10->setIcon(QIcon(":/images/images/move-true.png"));
    this->isMove=false;

    this->customPlot->showTracer(false);
    ui->toolButton_3->setIcon(QIcon(":/images/images/catch-true.png"));
    this->isCatch=false;
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}

/**
 * @brief WaveForm::on_toolButton_5_clicked--恢复
 */
void WaveForm::on_toolButton_5_clicked()
{
    this->customPlot->setInteraction(QCP::iRangeZoom,false);
    ui->toolButton_4->setIcon(QIcon(":/images/images/zoom-true.png"));
    this->isZoom=false;

    this->customPlot->setInteraction(QCP::iRangeDrag,false);
    ui->toolButton_10->setIcon(QIcon(":/images/images/move-true.png"));
    this->isMove=false;

    this->customPlot->showTracer(false);
    ui->toolButton_3->setIcon(QIcon(":/images/images/catch-true.png"));
    this->isCatch=false;

    customPlot->xAxis->setRange(this->m_xMin, this->m_xMax);    //x轴范围
    customPlot->yAxis->setRange(this->m_yMin, this->m_yMax);    //y轴范围
    this->updateCentralAxis(m_yMin);
    customPlot->replot(QCustomPlot::rpImmediateRefresh);
    //显示y轴最大最小值
    ui->lineEdit_12->setText(QString::number(m_yMin));
    ui->lineEdit_11->setText(QString::number(m_yMax));
}

/**
 * @brief WaveForm::on_pushButton_5_clicked--设置Y轴最大最小值
 */
void WaveForm::on_pushButton_5_clicked()
{
    bool isOk=false;
    double yMin = ui->lineEdit_12->text().toDouble(&isOk);
    if(!isOk){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("Y轴最小值输入有误"));
        return;
    }
    double yMax = ui->lineEdit_11->text().toDouble(&isOk);
    if(!isOk){
        CommonDialog::showErrorDialog(this, QString::fromLocal8Bit("Y轴最大值输入有误"));
        return;
    }
    if(yMin!=this->m_yMin || yMax!=this->m_yMax){
        //        this->m_yMin = yMin;
        //        this->m_yMax = yMax;
        customPlot->yAxis->setRange(yMin, yMax);    //y轴范围
        this->updateCentralAxis(yMin);
        customPlot->replot(QCustomPlot::rpImmediateRefresh);
    }
}

/**
 * @brief WaveForm::on_toolButton_9_clicked--曲线数据凸显
 */
void WaveForm::on_toolButton_9_clicked()
{
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
 * @brief WaveForm::on_pushButton_clicked--显示通道列表
 */
void WaveForm::on_pushButton_clicked()
{
    //隐藏 显示通道列表
    ui->pushButton->setVisible(false);
    //显示 通道列表
    ui->listFrame_2->setVisible(true);
}

/**
 * @brief WaveForm::on_pushButton_2_clicked--隐藏通道显示列表
 */
void WaveForm::on_pushButton_2_clicked()
{
    //显示 显示通道列表
    ui->pushButton->setVisible(true);
    //隐藏 通道列表
    ui->listFrame_2->setVisible(false);
}

/**
 * @brief WaveForm::on_toolButton_11_clicked--隐藏/显示所有通道图表
 */
void WaveForm::on_toolButton_11_clicked()
{
    bool show=false;
    if(this->channelShowCount==0){  //闭眼状态
        show=true;
    }
    for(int i=0; i<this->customPlot->graphCount()-1; i++){  //最后一条曲线是中轴先不用隐藏
        this->customPlot->graph(i)->setVisible(show);
    }
    for(int i=0; i<ui->listWidget->count(); i++){
        WaveItemWidget *wave = (WaveItemWidget *)ui->listWidget->itemWidget(ui->listWidget->item(i));
        wave->setShow(show);
    }
    if(show){
        this->channelShowCount=this->customPlot->graphCount();
        ui->toolButton_11->setIcon(QIcon(":/images/images/eys_open.svg"));
    }else{
        this->channelShowCount=0;
        ui->toolButton_11->setIcon(QIcon(":/images/images/eys_close.svg"));
    }
    this->customPlot->replot(QCustomPlot::rpImmediateRefresh); //刷新
}


/**
 * @brief OscilloscopeForm::slotContextMenu--右键菜单响应函数
 * @param pos
 */
void WaveForm::slotContextMenu(QPoint pos)
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
            connect(action, &QAction::triggered, this, &WaveForm::on_toolButton_9_clicked);
            break;
        case 1:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &WaveForm::on_toolButton_3_clicked);
            break;
        case 2:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &WaveForm::on_toolButton_10_clicked);
            break;
        case 3:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &WaveForm::on_toolButton_4_clicked);
            break;
        case 4:
            action->setText(QString::fromLocal8Bit(menuItem[i]));
            popMenu.addAction(action);
            connect(action, &QAction::triggered, this, &WaveForm::on_toolButton_5_clicked);
            break;
        }
    }
    popMenu.exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}
