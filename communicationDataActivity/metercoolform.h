#ifndef METERCOOLFORM_H
#define METERCOOLFORM_H

#include <QWidget>

namespace Ui {
class MeterCoolForm;
}

class MeterCoolForm : public QWidget
{
    Q_OBJECT

public:
    explicit MeterCoolForm(QWidget *parent = nullptr);
    ~MeterCoolForm();

private:
    Ui::MeterCoolForm *ui;
};

#endif // METERCOOLFORM_H
