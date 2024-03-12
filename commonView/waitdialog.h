#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class WaitDialog;
}

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = nullptr);
    ~WaitDialog();
    void setHint(QString hint);

private:
    Ui::WaitDialog *ui;
    QMovie *movie;
};

#endif // WAITDIALOG_H
