#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H
#include <QWidget>
#include "qcustomplot.h"
#include "mytracer.h"

class MyCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    MyCustomPlot(QWidget *parent = 0);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);

public:
    ///
    /// \brief 设置是否显示鼠标追踪器
    /// \param show:是否显示
    ///
    void showTracer(bool show)
    {
        m_isShowTracer = show;
        if(m_xTracer)
            m_xTracer->setVisible(m_isShowTracer);
        if(m_yTracer)
            m_yTracer->setVisible(m_isShowTracer);

        foreach (MyTracer *tracer, m_dataTracers)
        {
            if(tracer)
                tracer->setVisible(m_isShowTracer);
        }

        if(m_lineTracer)
            m_lineTracer->setVisible(m_isShowTracer);
    }

    ///
    /// \brief 是否显示鼠标追踪器
    /// \return
    ///
    bool isShowTracer(){return m_isShowTracer;};

    //zoom out
    void zoomOut()
    {
        this->yAxis->scaleRange(zoomRatio<5?zoomRatio+=0.5:zoomRatio, this->yAxis->range().center());
        this->xAxis->scaleRange(zoomRatio<5?zoomRatio+=0.5:zoomRatio, this->xAxis->range().center());
    }
    //zoom in
    void zoomIn()
    {
        this->yAxis->scaleRange(zoomRatio>-2?zoomRatio-=0.5:zoomRatio, this->yAxis->range().center());
        this->xAxis->scaleRange(zoomRatio>-2?zoomRatio-=0.5:zoomRatio, this->xAxis->range().center());
    }

signals:
    void yAxisUpdate(float y_val);
private:
    bool m_isShowTracer;//是否显示追踪器（鼠标在图中移动，显示对应的值）
    MyTracer *m_xTracer=NULL;//x轴
    MyTracer *m_yTracer=NULL;//y轴
    QList<MyTracer *> m_dataTracers;//
    MyTraceLine  *m_lineTracer;//直线

    double zoomRatio=1;
};

#endif // MYCUSTOMPLOT_H
