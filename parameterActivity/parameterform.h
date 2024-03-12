#ifndef PARAMETERFORM_H
#define PARAMETERFORM_H

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <QStandardItemModel>
#include "saveworker.h"
#include "common/parametertable.h"
#include "commonView/mybaseqwidget.h"
#include "communication/businessresponse.h"
#include "common/cabinetmanagement.h"

namespace Ui {
class ParameterForm;
}

#define CUSTOM_TABLE    "custom"

class ParameterForm : public MyBaseQWidget
{
    Q_OBJECT

public:
    explicit ParameterForm(QWidget *parent = nullptr);
    ~ParameterForm();
    void initData() override;
    void initUiView() override;
    void startBusiness() override;   //开始业务逻辑
    void endBusiness() override;     //结束业务逻辑
    void release();         //释放资源

private:
    Ui::ParameterForm *ui;
    QStandardItemModel* m_model;
    ParameterTable* m_table;         //表
//    QList<ParameterClass*>* m_list;  //参数列表
    QMenu *popMenu; //菜单
    QModelIndex currIndex;

    const QString osIcon = ":/images/images/6-1.png";
    const QString frIcon = ":/images/images/7-1.png";
    const QString osfrIcon = ":/images/images/8-1.png";

    QMap<QString, ParameterTable*>* m_configFileMap;
    QString currentTable;

    //QTimer* m_taskTimer;    //任务定时器
    QMutex m_mutex;                  //任务处理锁
    //bool startQueryTask=true;      //开始查询任务
    bool isHaveParametFix=false;     //是否有参数被修改
    SaveWorker *saveWorker;

    QString curLevel1Tag;
    QString curLevel2Tag;

    QString currentDownParametFilePath;     //当前下载的参数文件路径
protected:
    void showLevel2(QString tag);
    void showParameTable(QString level1Tag, QString level2Tag);

    void suspendQueryTask();    //暂停查询任务
    void resumeQueryTask();     //恢复查询任务

    void saveParametToFile();
    void updateParametSaveStatus();

    void resetMasterContrl();   //重启主控

private slots:
    //加载2级菜单
    void loadLevel2();
    //加载参数列表
    void loadParameTable();
    //右键菜单响应函数
    void slotContextMenu(QPoint pos);
    //双击响应函数
    void on_TableDoubleClicked(const QModelIndex &index);
    //右键菜单--参数定义
    void slotParamDefine();
    //右键菜单--参数修改
    void slotParamFix();
    //右键菜单--加入示波器
    void slotAdd2Oscill();
    //右键菜单--加入自定义参数
    void slotAdd2Custom();


    //数据被修改
    void on_parameterFixed();

    //ParameterManagement table被修改
    //    void on_tableFileChanged(const QString filePath);

    /**
     * @brief queryMyParameterTask--数据查询任务
     */
    void queryMyParameterTask();
    /**
     * @brief mainFormQueryResponse--数据查询响应
     * @param success
     * @param m_Data
     * @param errorID
     * @param errorMsg
     */
    void myFormQueryResponse(BusinessResponse m_Data);
    /**
     * @brief on_pushButton_2_clicked--保存参数
     */
    void on_pushButton_2_clicked();
    /**
     * @brief on_saveFinish--参数保存完成
     */
    void on_saveFinish();
    void on_pushButton_clicked();
    /**
     * @brief on_parameterLoadResult--加载参数文件结果
     * @param success--成功与否
     * @param msg--msg
     */
    void on_parameterLoadResult(bool success, QString msg);
    /**
     * @brief on_oscillListChanged--动态示波器被修改
     */
    void on_oscillListChanged();
};

#endif // PARAMETERFORM_H
