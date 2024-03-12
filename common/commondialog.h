#ifndef COMMONDIALOG_H
#define COMMONDIALOG_H

#include <QObject>
#include <QMessageBox>
#include "waitdialog.h"

class CommonDialog
{
public:
    CommonDialog();

    static void showErrorDialog(QWidget *parent, QString msg)
    {
        QMessageBox errorBox(parent);
        errorBox.setWindowTitle(QString::fromLocal8Bit("错误！"));
        errorBox.setText(msg);
        errorBox.setStandardButtons(QMessageBox::Ok | QMessageBox:: Cancel);//对话框上包含的按钮
        errorBox.exec();
    }

    static void showSuccessDialog(QWidget *parent, QString msg)
    {
        QMessageBox successBox(parent);
        successBox.setWindowTitle(QString::fromLocal8Bit("成功！"));
        successBox.setText(msg);
        successBox.setStandardButtons(QMessageBox::Ok | QMessageBox:: Cancel);//对话框上包含的按钮
        successBox.exec();
    }
};

#endif // COMMONDIALOG_H
