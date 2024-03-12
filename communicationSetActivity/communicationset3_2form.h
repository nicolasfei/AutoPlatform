#ifndef COMMUNICATIONSET3_2FORM_H
#define COMMUNICATIONSET3_2FORM_H

#include <QWidget>

namespace Ui {
class CommunicationSet3_2Form;
}

class CommunicationSet3_2Form : public QWidget
{
    Q_OBJECT

public:
    explicit CommunicationSet3_2Form(QWidget *parent = nullptr);
    ~CommunicationSet3_2Form();

private:
    Ui::CommunicationSet3_2Form *ui;
};

#endif // COMMUNICATIONSET3_2FORM_H
