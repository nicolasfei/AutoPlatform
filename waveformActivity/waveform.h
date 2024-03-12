#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QtCharts>
#include <QWidget>
#include "commonView/mybaseqwidget.h"
#include "commonView/mycustomplot.h"
#include "commonView/waitdialog.h"
#include "commonView/mywaitdialog.h"
#include "communication/businessresponse.h"

namespace Ui {
class WaveForm;
}

class WaveForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit WaveForm(QWidget *parent = nullptr);
    ~WaveForm();

public:
    void initUiView();      //初始化ui view
    void initData();        //初始化数据
    void startBusiness();   //开始业务逻辑
    void endBusiness();     //结束业务逻辑

private:
    void loadWaveFile();    //导入波形文件
    void buildChannelList(QList<QString> *chanName);     //组件通道列表
    void buildCurveChart(QList<QString> *chanName, QVector<double> *xValue, QList<QVector<double>*> *yValue);
    void updateCentralAxis(double yMin);

private slots:
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();

    void on_channelListItemDelete(uint32_t id);
    void on_channelListItemShow(uint32_t id, bool show);
    void on_channelListItemZoom(uint32_t id);
    void on_channelListItemFixName(uint32_t id, QString newName);

    void on_toolButton_8_clicked();

    void on_dataResponse(BusinessResponse data);

    void on_toolButton_3_clicked();

    void on_toolButton_10_clicked();

    void on_toolButton_4_clicked();

    void on_toolButton_5_clicked();

    void on_pushButton_5_clicked();

    void on_toolButton_9_clicked();

    void on_waveDataResponse(bool success, bool coverOk, QList<QString> *chanName, QVector<double> *xValue, QList<QVector<double>*>* datas);

    void on_fileSelectResult(QList<QString>* list);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_toolButton_11_clicked();

    void slotContextMenu(QPoint pos);

private:
    Ui::WaveForm *ui;
    MyCustomPlot *customPlot;
    QList<QVector<double>*> *m_yValueList;

    bool isCatch;       //是否处于捕捉状态
    bool isMove;        //是否处于移动状态
    bool isZoom;        //是否处于缩放状态
    bool isRecovery;    //是否处于还原状态
    bool isShowPoint;   //是否突出显示数据点

    double m_yMin=-1000;
    double m_yMax=1000;
    double m_xMin=1750;
    double m_xMax=2250;
    double m_xMiddle=2000;

    int channelShowCount;   //记录了有多少个通道处于显示状态
};

#endif // WAVEFORM_H
