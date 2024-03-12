#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QListWidgetItem>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QCloseEvent>

#include <cnav/cnavmodel.h>
#include <cnav/cnavdelegate.h>

#include "commonView/mybaseqwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTimer *myTimer;
    CNavModel* pNavModel;
    CNavDelegate* pDelegate;
    void initListView();
    void loadStackedWidget();
    void releaseStackedWidget();
    void startSystemTimeShow();
    void stopSystemTimeShow();

    //串口关闭时候触发
    void closeEvent(QCloseEvent *event) override;

signals:
    void showLogin();
    void quit();

public slots:
    void receiverShow();

protected slots:
    void timeUpdate();
    void select(const QModelIndex &index);
    void on_NetStates(bool);
private slots:
    void on_stackedWidget_currentChanged(int arg1);
    void on_toolButton_2_clicked();

private:
    Ui::MainWindow *ui;
    MyBaseQWidget *mainForm;
    MyBaseQWidget *systemTestForm;
    MyBaseQWidget *dioForm;
//    MyBaseQWidget *aIOForm;
    MyBaseQWidget *oscilloscopeForm;
    MyBaseQWidget *waterCoolingForm;
    MyBaseQWidget *parameterForm;
    MyBaseQWidget *importParametersForm;
    MyBaseQWidget *exportParametersForm;
    MyBaseQWidget *parameterComparisonForm;
    MyBaseQWidget *waveForm;
//    MyBaseQWidget *communicationDataForm;
    MyBaseQWidget *communicationSet10Form;
    MyBaseQWidget *firmwareUpdateForm;
    MyBaseQWidget *faultLogForm;
    MyBaseQWidget *otherForm;
    QList<MyBaseQWidget *> *formList;
};
#endif // MAINWINDOW_H
