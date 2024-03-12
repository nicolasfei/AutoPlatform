#ifndef MYWAITDIALOG_H
#define MYWAITDIALOG_H

#include <QDialog>
#include <QLabel>
#include<QPainter>
#include<QMovie>

class MyWaitDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MyWaitDialog(QWidget *parent = 0);
    ~MyWaitDialog();

private:
    QMovie *movie;
    QLabel *label;
};

#endif // MYWAITDIALOG_H
