#include "mytracer.h"
#include "common/commonclass.h"

MyTracer::MyTracer(QCustomPlot *_plot, TracerType _type, QObject *parent)
    : QObject(parent),
      m_plot(_plot),
      m_type(_type)
{
    m_visible = true;
    m_tracer = Q_NULLPTR;// 跟踪的点
    m_label = Q_NULLPTR;// 显示的数值
    m_arrow = Q_NULLPTR;// 箭头
    if (m_plot)
    {
        QColor clrDefault(Qt::red);
        QBrush brushDefault(Qt::NoBrush);
        QPen penDefault(clrDefault);
        //        penDefault.setBrush(brushDefault);
        penDefault.setWidthF(0.5);

        m_tracer = new QCPItemTracer(m_plot);
        m_tracer->setStyle(QCPItemTracer::tsCircle);
        m_tracer->setPen(penDefault);
        m_tracer->setBrush(brushDefault);

        m_label = new QCPItemText(m_plot);
        m_label->setLayer("overlay");
        m_label->setClipToAxisRect(false);
        m_label->setPadding(QMargins(5, 5, 5, 5));
        m_label->setBrush(brushDefault);
        m_label->setPen(penDefault);
        m_label->position->setParentAnchor(m_tracer->position);
        //        m_label->setFont(QFont("宋体", 8));
        m_label->setFont(QFont("Arial", 8));
        m_label->setColor(clrDefault);
        m_label->setText("");

        m_arrow = new QCPItemLine(m_plot);
        QPen  arrowPen(clrDefault, 1);
        m_arrow->setPen(penDefault);
        m_arrow->setLayer("overlay");
        m_arrow->setClipToAxisRect(false);
        m_arrow->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状

        switch (m_type)
        {
        case XAxisTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            m_tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
            m_tracer->setSize(7);
            m_label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_arrow->end);
            m_arrow->start->setCoords(0, 20);//偏移量
            break;
        }
        case YAxisTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
            m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
            m_tracer->setSize(7);
            m_label->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_label->position);
            m_arrow->start->setCoords(-20, 0);//偏移量
            break;
        }
        case DataTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
            m_tracer->setSize(5);

            m_label->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_arrow->end);
            m_arrow->start->setCoords(20, 0);
            break;
        }
        default:
            break;
        }
        setVisible(false);
    }
}

MyTracer::~MyTracer()
{
    if(m_plot)
    {
        if (m_tracer)
            m_plot->removeItem(m_tracer);
        if (m_label)
            m_plot->removeItem(m_label);
        if (m_arrow)
            m_plot->removeItem(m_arrow);
    }
}

void MyTracer::setPen(const QPen &pen)
{
    if(m_tracer)
        m_tracer->setPen(pen);
    if(m_arrow)
        m_arrow->setPen(pen);
}

void MyTracer::setBrush(const QBrush &brush)
{
    if(m_tracer)
        m_tracer->setBrush(brush);
}

void MyTracer::setLabelPen(const QPen &pen)
{
    if(m_label)
    {
        m_label->setPen(pen);
        m_label->setBrush(Qt::NoBrush);
        m_label->setColor(pen.color());
    }
}

void MyTracer::setText(const QString &text)
{
    if(m_label)
        m_label->setText(text);
}

void MyTracer::setVisible(bool vis)
{
    m_visible = vis;
    if(m_tracer)
        m_tracer->setVisible(m_visible);
    if(m_label)
        m_label->setVisible(m_visible);
    if(m_arrow)
        m_arrow->setVisible(m_visible);
}

void MyTracer::updatePosition(double xValue, double yValue)
{
    if (!m_visible)
    {
        setVisible(true);
        m_visible = true;
    }

    if (yValue > m_plot->yAxis->range().upper)
        yValue = m_plot->yAxis->range().upper;

    switch (m_type)
    {
    case XAxisTracer:
    {
        m_tracer->position->setCoords(xValue, 1);
        m_label->position->setCoords(0, 15);
        m_arrow->start->setCoords(0, 15);
        m_arrow->end->setCoords(0, 0);
        setText(QString::number(xValue));
        break;
    }
    case YAxisTracer:
    {
        m_tracer->position->setCoords(0, yValue);
        m_label->position->setCoords(-20, 0);
        //        m_arrow->start->setCoords(20, 0);
        //        m_arrow->end->setCoords(0, 0);
        setText(QString::number(yValue));
        break;
    }
    case DataTracer:
    {
        m_tracer->position->setCoords(xValue, yValue);
        m_label->position->setCoords(20, 0);
        setText(QString("x:%1,y:%2").arg(xValue).arg(yValue));
        break;
    }
    default:
        break;
    }
}

MyTraceLine::MyTraceLine(QCustomPlot *_plot, LineType _type, QObject *parent)
    : QObject(parent),
      m_type(_type),
      m_plot(_plot)
{
    //    m_lineLeft = Q_NULLPTR;
    //    m_lineRight = Q_NULLPTR;
    //    m_lineUp = Q_NULLPTR;
    //    m_lineDown = Q_NULLPTR;
    m_lineV = Q_NULLPTR;
    m_lineH = Q_NULLPTR;
    initLine();
}

MyTraceLine::~MyTraceLine()
{
    if(m_plot)
    {
        //        if (m_lineLeft)
        //            m_plot->removeItem(m_lineLeft);
        //        if (m_lineRight)
        //            m_plot->removeItem(m_lineRight);

        //        if (m_lineUp)
        //            m_plot->removeItem(m_lineUp);
        //        if (m_lineDown)
        //            m_plot->removeItem(m_lineDown);

        if (m_lineV)
            m_plot->removeItem(m_lineV);
        if (m_lineH)
            m_plot->removeItem(m_lineH);
    }
}

void MyTraceLine::initLine()
{
    if(m_plot)
    {
        MY_DEBUG << "xAxis lower is " << m_plot->xAxis->range().lower;
        MY_DEBUG << "xAxis upper is " << m_plot->xAxis->range().upper;
        MY_DEBUG << "yAxis lower is " << m_plot->yAxis->range().lower;
        MY_DEBUG << "yAxis upper is " << m_plot->yAxis->range().upper;
        QPen linesPen(Qt::red, 1, Qt::DashLine);
        if(VerticalLine == m_type || Both == m_type)
        {
            m_lineV = new QCPItemStraightLine(m_plot);//垂直线
            m_lineV->setLayer("overlay");
            m_lineV->setPen(linesPen);
            m_lineV->setClipToAxisRect(true);
            m_lineV->point1->setCoords(0, 0);
            m_lineV->point2->setCoords(0, 0);
        }

        if(HorizonLine == m_type || Both == m_type)
        {
            m_lineH = new QCPItemStraightLine(m_plot);//水平线
            m_lineH->setLayer("overlay");
            m_lineH->setPen(linesPen);
            m_lineH->setClipToAxisRect(true);
            m_lineH->point1->setCoords(0, 0);
            m_lineH->point2->setCoords(0, 0);
        }
        //        if(LeftLine == m_type || All == m_type)
        //        {
        //            linesPen.setColor(Qt::red);
        //            m_lineLeft = new QCPItemStraightLine(m_plot);//垂直线--左
        //            m_lineLeft->setLayer("overlay");
        //            m_lineLeft->setPen(linesPen);
        //            m_lineLeft->setClipToAxisRect(true);
        //            m_lineLeft->point1->setCoords(m_plot->xAxis->range().lower+10, m_plot->yAxis->range().lower);
        //            m_lineLeft->point2->setCoords(m_plot->xAxis->range().lower+10, m_plot->yAxis->range().upper);
        //        }
        //        if(RightLine == m_type || All == m_type)
        //        {
        //            linesPen.setColor(Qt::blue);
        //            m_lineRight = new QCPItemStraightLine(m_plot);//垂直线--右
        //            m_lineRight->setLayer("overlay");
        //            m_lineRight->setPen(linesPen);
        //            m_lineRight->setClipToAxisRect(true);
        //            m_lineRight->point1->setCoords(m_plot->xAxis->range().upper-10, m_plot->yAxis->range().lower);
        //            m_lineRight->point2->setCoords(m_plot->xAxis->range().upper-10, m_plot->yAxis->range().upper);
        //        }

        //        if(UpLine == m_type || All == m_type)
        //        {
        //            linesPen.setColor(Qt::green);
        //            m_lineUp = new QCPItemStraightLine(m_plot);//水平线--上
        //            m_lineUp->setLayer("overlay");
        //            m_lineUp->setPen(linesPen);
        //            m_lineUp->setClipToAxisRect(true);
        //            m_lineUp->point1->setCoords(m_plot->xAxis->range().lower, m_plot->yAxis->range().upper-10);
        //            m_lineUp->point2->setCoords(m_plot->xAxis->range().upper, m_plot->yAxis->range().upper-10);
        //        }
        //        if(DownLine == m_type || All == m_type)
        //        {
        //            linesPen.setColor(Qt::yellow);
        //            m_lineDown = new QCPItemStraightLine(m_plot);//水平线--下
        //            m_lineDown->setLayer("overlay");
        //            m_lineDown->setPen(linesPen);
        //            m_lineDown->setClipToAxisRect(true);
        //            m_lineDown->point1->setCoords(m_plot->xAxis->range().lower, m_plot->yAxis->range().lower+10);
        //            m_lineDown->point2->setCoords(m_plot->xAxis->range().upper, m_plot->yAxis->range().lower+10);
        //        }
    }
}

void MyTraceLine::updatePosition(double xValue, double yValue)
{
    if(VerticalLine == m_type || Both == m_type)
    {
        if(m_lineV)
        {
            m_lineV->point1->setCoords(xValue, m_plot->yAxis->range().lower);
            m_lineV->point2->setCoords(xValue, m_plot->yAxis->range().upper);
        }
    }

    if(HorizonLine == m_type || Both == m_type)
    {
        if(m_lineH)
        {
            m_lineH->point1->setCoords(m_plot->xAxis->range().lower, yValue);
            m_lineH->point2->setCoords(m_plot->xAxis->range().upper, yValue);
        }
    }
}

void MyTraceLine::updatePosition(LineDirection dir, double xValue, double yValue)
{
    //    switch(dir)
    //    {
    //    case Left:
    //        if(LeftLine == m_type || All == m_type)
    //        {
    //            m_lineLeft->point1->setCoords(xValue, m_plot->yAxis->range().lower);
    //            m_lineLeft->point2->setCoords(xValue, m_plot->yAxis->range().upper);
    //        }
    //        break;
    //    case Right:
    //        if(RightLine == m_type || All == m_type)
    //        {
    //            m_lineRight->point1->setCoords(xValue, m_plot->yAxis->range().lower);
    //            m_lineRight->point2->setCoords(xValue, m_plot->yAxis->range().upper);
    //        }
    //        break;
    //    case Up:
    //        if(UpLine == m_type || All == m_type)
    //        {
    //            m_lineUp->point1->setCoords(m_plot->xAxis->range().lower, yValue);
    //            m_lineUp->point2->setCoords(m_plot->xAxis->range().upper, yValue);
    //        }
    //        break;
    //    case Down:
    //        if(DownLine == m_type || All == m_type)
    //        {
    //            m_lineDown->point1->setCoords(m_plot->xAxis->range().lower, yValue);
    //            m_lineDown->point2->setCoords(m_plot->xAxis->range().upper, yValue);
    //        }
    //        break;
    //    default:
    //        break;
    //    }
}
