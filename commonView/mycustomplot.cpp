#include "mycustomplot.h"

MyCustomPlot::MyCustomPlot(QWidget *parent)
    :QCustomPlot(parent)
    ,m_isShowTracer(false)
    ,m_xTracer(Q_NULLPTR)
    ,m_yTracer(Q_NULLPTR)
    ,m_dataTracers(QList<MyTracer *>())
    ,m_lineTracer(Q_NULLPTR)
{
    //    //右键菜单自定义
        this->setContextMenuPolicy(Qt::CustomContextMenu);
    //    //信号连接槽函数
//        connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    m_xTracer = new MyTracer(this, MyTracer::XAxisTracer);//x轴
    m_yTracer = new MyTracer(this, MyTracer::YAxisTracer);//y轴
}

void MyCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    if(m_isShowTracer)
    {
        //当前鼠标位置（像素坐标）
        int x_pos = event->pos().x();
        int y_pos = event->pos().y();

        //像素坐标转成实际的x,y轴的坐标
        float x_val = this->xAxis->pixelToCoord(x_pos);
        float y_val = this->yAxis->pixelToCoord(y_pos);

        m_xTracer->updatePosition(x_val, y_val);
        m_yTracer->updatePosition(x_val, y_val);

        int nTracerCount = m_dataTracers.count();
        int nGraphCount = graphCount();
        if(nTracerCount < nGraphCount)
        {
            for(int i = nTracerCount; i < nGraphCount; ++i)
            {
                MyTracer *tracer = new MyTracer(this, MyTracer::DataTracer);
                m_dataTracers.append(tracer);
            }
        }
        else if(nTracerCount > nGraphCount)
        {
            for(int i = nGraphCount; i < nTracerCount; ++i)
            {
                MyTracer *tracer = m_dataTracers[i];
                if(tracer)
                {
                    tracer->setVisible(false);
                }
            }
        }

        for (int i = 0; i < graphCount(); ++i)
        {
            MyTracer *tracer = m_dataTracers[i];
            if(this->graph(i)->visible()){      //只显示显示的曲线，隐藏的曲线不显示捕获点
//                MyTracer *tracer = m_dataTracers[i];
//                if(!tracer)
//                    tracer = new MyTracer(this, MyTracer::DataTracer);
                tracer->setVisible(true);
                tracer->setPen(this->graph(i)->pen());
                tracer->setBrush(Qt::NoBrush);
                tracer->setLabelPen(this->graph(i)->pen());
                auto iter = this->graph(i)->data()->findBegin(x_val);
                double value = iter->mainValue();
                //double value = this->graph(i)->data()->findBegin(x_val)->value;
                tracer->updatePosition(x_val, value);
            }else{
                tracer->setVisible(false);
            }
        }

        if(Q_NULLPTR == m_lineTracer)
            m_lineTracer = new MyTraceLine(this,MyTraceLine::Both);//直线
        m_lineTracer->updatePosition(x_val, y_val);

        this->replot();//曲线重绘
        emit this->yAxisUpdate(y_val);
    }
}
