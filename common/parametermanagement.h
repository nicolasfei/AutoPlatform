//#ifndef PARAMETERMANAGEMENT_H
//#define PARAMETERMANAGEMENT_H

//#include <QObject>
//#include <iostream>
//#include <memory> // shared_ptr
//#include <mutex>  // mutex
//#include <QMap>
//#include <QTextStream>
//#include <QDebug>
//#include <QFile>
//#include <QTextCodec>
//#include <QMutexLocker>
//#include <QFileSystemWatcher>

//#include "parameterclass.h"
//#include "parametertable.h"

//class ParameterManagement:public QObject
//{
//    Q_OBJECT
//public:
//    ~ParameterManagement();
//    static ParameterManagement* instance(){
//        static QMutex mutex;
//        static QScopedPointer<ParameterManagement> inst;
//        if (Q_UNLIKELY(!inst)) {
//            mutex.lock();
//            if (!inst) {
//                inst.reset(new ParameterManagement);
//            }
//            mutex.unlock();
//        }
//        return inst.data();
//    }
//    //添加参数文件到目录
//    bool addParameterFile(QString fileName);
//    //获取fileConfig
//    QMap<QString, ParameterTable*>* getFileConfigMap();
//    //获取table
//    //ParameterTable* getFileTables(QString level1, QString level2);
//    //添加Parameter到故障录波
//    void addParameter2FaultRecording(ParameterClass* p);
//    //删除Parameter到故障录波
//    void removeParameter2FaultRecording(ParameterClass* p);
//    //添加Parameter到示波器
//    void addParameter2Oscilloscope(ParameterClass* p);
//    //删除Parameter到示波器
//    void removeParameter2Oscilloscope(ParameterClass* p);
//    //是否还能加到示波器
//    bool canAddOscilloscope();
//    //是否还能加到故障录波
//    bool canAddFaultRecording();

//    QList<ParameterClass*> *getOscilloscopeList()
//    {
//        return this->oscilloscopeList;
//    }

//    QList<ParameterClass*> *getFaultRecording()
//    {
//        return this->faultRecordingList;
//    }

//    QList<ParameterClass*>* getMainFormParameterList();

//private:
//    //初始化ParameterManagement
//    void initParameterFile(QString fileDirPath);
//    //初始化文件系统监听
//    void initQFileSystemWatcher();


//private:
//    //QMap<QString, QList<QString>*>* dirFileMap;         //配置文件夹对应的文件夹下的配置文件
//    /**
//     * @brief configFile
//     * QString 配置文件名字
//     * QString 参数组号
//     * QList 参数列表
//     * ParameterClass 参数
//     */
//    QMap<QString, ParameterTable*> *fileConfig;      //配置文件的描述
//    //QList<ParameterTable*>* tables;
//    QFileSystemWatcher *m_FileWatcher;               //文件系统监听器
//    bool isMyChangeDirLevel=false;          //是否是我自己改变了目录
//    bool isMyChangeFILE=false;              //是否是我自己改变了文件内容
//    QList<ParameterClass*> *oscilloscopeList;    //示波器列表
//    QList<ParameterClass*> *faultRecordingList;  //故障录波列表,修改为读取配置文件来获得

//    QList<ParameterClass*> *mainFormParameterList;  //main页面需要的ParameterClass list
//private:
//    explicit ParameterManagement(QObject *parent = nullptr);        //禁止构造函数。
//    ParameterManagement(const ParameterManagement &);               //禁止拷贝构造函数。
//    ParameterManagement & operator=(const ParameterManagement &);   //禁止赋值拷贝函数。

//    signals:
//    void tableFileChanged(const QString filePath);
//    void oscillListChanged();       //示波器列表发生改变
//    void faultListChanged();        //故障录波列表发生改变

//private slots:
//    void on_tableFileChanged(const QString &path);      //table 文件内容发生改变
//    void on_dirLevelChanged(const QString &path);       //级目录发生改变
//};
//#endif // PARAMETERMANAGEMENT_H
