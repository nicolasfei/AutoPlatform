#ifndef COMMUNICATIONDATAFORM_H
#define COMMUNICATIONDATAFORM_H

#include <QWidget>
#include "commonView/mybaseqwidget.h"

namespace Ui {
class CommunicationDataForm;
}

class CommunicationDataForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit CommunicationDataForm(QWidget *parent = nullptr);
    ~CommunicationDataForm();

private:
    Ui::CommunicationDataForm *ui;
};

#endif // COMMUNICATIONDATAFORM_H
