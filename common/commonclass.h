#ifndef COMMONCLASS_H
#define COMMONCLASS_H
#include <QDebug>
#include <QString>
#define MY_DEBUG    qDebug()<<__LINE__<<__FUNCTION__
class CommonClass
{
public:
    CommonClass();
    /*QTableView 左上角样式*/
    const static QString TABLE_CornerButtonSTYLE;
    const static QString TABLE_STYLE;
    const static QString TABLE_HEADSTYLE;
    const static QString TABLE_HEAD_SECTIONSTYLE;
    const static QString LEVEL1_STYLE;
    const static QString LEVEL1_FOCUS_STYLE;
    const static QString LEVEL2_STYLE;
    const static QString LEVEL2_FOCUS_STYLE;
    const static QString ENABLE_STYLE;
    const static QString DISABLE_STYLE;

    const static int ACTIVITY_FRESH_DELAY;
};

#endif // COMMONCLASS_H
