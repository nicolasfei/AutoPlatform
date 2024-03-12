#ifndef OSCILLOSCOPEFORM_H
#define OSCILLOSCOPEFORM_H

#include <QWidget>
#include <QtCharts>
#include <QStandardItemModel>
#include "common/parameterclass.h"
#include "commonView/mybaseqwidget.h"
#include "commonView/qcustomplot.h"
#include "commonView/mycustomplot.h"
#include "communication/businessresponse.h"
#include "oscilldatasavetask.h"

#define USE_QCustomPlot (1)
#define x_RangeLower (0)
#define x_RangeUpper (2500)           //ms/10
#define y_RangeLower (-500)
#define y_RangeUpper (500)

namespace Ui {
class OscilloscopeForm;
}

class OscilloscopeForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit OscilloscopeForm(QWidget *parent = nullptr);
    ~OscilloscopeForm();

    typedef enum {
        START=1,
        STOP=0,
    }ACQ_Status;

private:
    Ui::OscilloscopeForm *ui;
    QStandardItemModel *model1;      //曲线瞬时值
    QStandardItemModel *model2;      //德尔塔值
    OscillDataSaveTask *autoSaveTask;
    bool isCatch;       //是否处于捕捉状态
    bool isMove;        //是否处于移动状态
    bool isZoom;        //是否处于缩放状态
    bool isRecovery;    //是否处于还原状态
    bool isShowPoint;   //是否突出显示数据点
    int channelShowCount=0; //当前显示的通道个数
#if USE_QCustomPlot
    MyCustomPlot *customPlot;
    double zoomRatio = 1;
    double x_min, x_max;
    double y_min, y_max;
#elif


    QChart *chart;              //图表
    QValueAxis *axisX;      //X轴
    QValueAxis *axisY;      //Y轴
#endif
    ACQ_Status acqStatus;
    bool autoCollectting;       //是否在自动采集中。。。自动采集需要保存到文件中
    QList<BusinessResponse::OscillResponse> oscillData;
//    QList<BusinessResponse::OscillResponse> &oscill50ms;
private:
    void setQChart();       //设置曲线图
    void updateXAxis();     //更新X轴
    void updateYAxis();     //更新Y轴
    void setSerieVisibility(int serieID, bool show);    //设置某个曲线序列显示状态
    void removeSerie(int serieID);      //移除某个序列
    void zoomSerie(int serieID);        //缩放某个序列

    void initOscillListView();          //初始化通道listView
    void initOscillCharts();            //初始化图表
    void initOscillTableView();         //初始化表格
    void configChannelParamete();       //配置通道参数

    void startACQ();        //示波器开始
    void stopACQ();         //示波器停止
public:
    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑

private slots:
    void onListItemDelete(uint32_t id);
    void onListItemShow(uint32_t id,bool show);
    void onListItemZoom(uint32_t id);
    void on_toolButton_8_clicked();

    //右键菜单响应函数
    void slotContextMenu(QPoint pos);

    void on_toolButton_9_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void on_toolButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_toolButton_4_clicked();
    void on_toolButton_5_clicked();
    void on_toolButton_6_clicked();
    void on_toolButton_7_clicked();
    void on_pushButton_3_clicked();
    void on_toolButton_10_clicked();
    void on_toolButton_11_clicked();

    void on_dataResponse(BusinessResponse data);    //响应
    void on_yAxisUpdate(float yValue);  //鼠标移动，y值更新
    void on_pushButton_clicked();
    void on_toolButton_12_clicked();
};

#endif // OSCILLOSCOPEFORM_H
