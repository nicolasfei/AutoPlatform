#ifndef MYTRACER_H
#define MYTRACER_H

#include <QObject>
#include "qcustomplot.h"

class MyTracer : public QObject
{
    Q_OBJECT
public:
    enum TracerType
    {
        XAxisTracer=0,//依附在x轴上显示x值
        YAxisTracer,//依附在y轴上显示y值
        DataTracer//在图中显示x,y值
    };
    explicit MyTracer(QCustomPlot *_plot, TracerType _type, QObject *parent = Q_NULLPTR);
    ~MyTracer();
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text);
    void setLabelPen(const QPen &pen);
    void updatePosition(double xValue, double yValue);
    void setVisible(bool m_visible);

protected:
    bool m_visible;//是否可见
    QCustomPlot *m_plot;//图表
    TracerType m_type;//类型
    QCPItemTracer *m_tracer;//跟踪的点
    QCPItemText *m_label;//显示的数值
    QCPItemLine *m_arrow;//箭头

signals:

};

///
/// \brief The XxwCrossLine class:用于显示鼠标移动过程中的鼠标位置的直线
///
class MyTraceLine : public QObject
{
public:
    enum LineType
    {
        //        LeftLine=0,     //垂直线左
        //        UpLine,         //水平线上
        //        RightLine,      //垂直线右
        //        DownLine,       //水平线下
        //        All             //同时显示水平和垂直线
        VerticalLine=0,//垂直线
        HorizonLine, //水平线
        Both//同时显示水平和垂直线
    };
    enum LineDirection
    {
        Left=0,     //左
        Up,         //上
        Right,      //右
        Down        //下
    };
    explicit MyTraceLine(QCustomPlot *_plot, LineType _type = Both, QObject *parent = Q_NULLPTR);
    ~MyTraceLine();
    void initLine();
    void updatePosition(double xValue, double yValue);
    void updatePosition(LineDirection dir, double xValue, double yValue);

    void setVisible(bool vis)
    {
        if(m_lineV)
            m_lineV->setVisible(vis);
        if(m_lineH)
            m_lineH->setVisible(vis);
        //        if(m_lineLeft)
        //            m_lineLeft->setVisible(vis);
        //        if(m_lineUp)
        //            m_lineUp->setVisible(vis);
        //        if(m_lineRight)
        //            m_lineRight->setVisible(vis);
        //        if(m_lineDown)
        //            m_lineDown->setVisible(vis);
    }

protected:
    bool m_visible;//是否可见
    LineType m_type;//类型
    QCustomPlot *m_plot;//图表
    QCPItemStraightLine *m_lineV; //垂直线
    QCPItemStraightLine *m_lineH; //水平线
    QCPItemStraightLine *m_lineLeft; //垂直线--左
    QCPItemStraightLine *m_lineUp; //水平线--上
    QCPItemStraightLine *m_lineRight; //垂直线--右
    QCPItemStraightLine *m_lineDown; //水平线--下
};

#endif // MYTRACER_H
