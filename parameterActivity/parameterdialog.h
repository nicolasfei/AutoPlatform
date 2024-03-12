#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H
#include <QTableView>
#include <QDialog>
#include <common/parameterclass.h>
#include <QStandardItemModel>

#include "binarysetmodel.h"

namespace Ui {
class ParameterDialog;
}

class ParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterDialog(ParameterClass *arg, QWidget *parent = nullptr);
    ~ParameterDialog();
    void setTitle(QString title);
    void setCurrentTab(int tab);

private slots:
    //确定按钮
    void on_buttonBox_accepted();
    //列表项被双击了
    void on_itemDoubleClicked(const QModelIndex &index);
signals:
    void ParameterUpdate();         //参数更新信号
private:
    Ui::ParameterDialog *ui;
    ParameterClass *parameter;
    BinarySetModel *m_model;
    QTableView *tableView;
    bool isDataUpdate=false;
    bool isHaveBitConfig=false;
private:
    void setParameter(ParameterClass *arg);
    void initTabView(int vaildBit);
};

#endif // PARAMETERDIALOG_H
