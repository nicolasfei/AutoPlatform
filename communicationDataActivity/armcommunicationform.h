#ifndef ARMCOMMUNICATIONFORM_H
#define ARMCOMMUNICATIONFORM_H

#include <QWidget>

namespace Ui {
class ArmCommunicationForm;
}

class ArmCommunicationForm : public QWidget
{
    Q_OBJECT

public:
    explicit ArmCommunicationForm(QWidget *parent = nullptr);
    ~ArmCommunicationForm();

private:
    Ui::ArmCommunicationForm *ui;
};

#endif // ARMCOMMUNICATIONFORM_H
