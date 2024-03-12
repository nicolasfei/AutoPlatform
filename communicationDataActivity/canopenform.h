#ifndef CANOPENFORM_H
#define CANOPENFORM_H

#include <QWidget>

namespace Ui {
class CanopenForm;
}

class CanopenForm : public QWidget
{
    Q_OBJECT

public:
    explicit CanopenForm(QWidget *parent = nullptr);
    ~CanopenForm();

private:
    Ui::CanopenForm *ui;
};

#endif // CANOPENFORM_H
