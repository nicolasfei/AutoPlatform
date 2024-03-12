QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += charts
QT += xml
CONFIG += c++11
CONFIG += no_batch
//QMAKE_CXXFLAGS += /utf-8
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aioActivity/aiocommand.cpp \
    aioActivity/aioform.cpp \
    aioActivity/aiohandler.cpp \
    aioActivity/aioitemdelegate.cpp \
    aioActivity/aiomodel.cpp \
    aioActivity/compulsoryhanlder.cpp \
    cnav/cnavdelegate.cpp \
    cnav/cnavmodel.cpp \
    cnav/cnavview.cpp \
    common/cabinetmanagement.cpp \
    common/commonclass.cpp \
    common/commondialog.cpp \
    common/devicemanager.cpp \
    common/parameterclass.cpp \
    common/parametermanagement.cpp \
    common/parametertable.cpp \
    common/timertask.cpp \
    common/waitdialog.cpp \
    commonView/listchooisedialog.cpp \
    commonView/mylistchooicedialog.cpp \
    commonView/mybaseqwidget.cpp \
    commonView/mycustomplot.cpp \
    commonView/mytracer.cpp \
    commonView/mywaitdialog.cpp \
    commonView/qcustomplot.cpp \
    commonView/waitdialog.cpp \
    commonView/wavefileitemwidget.cpp \
    commonView/waveitemwidget.cpp \
    communication/businessclass.cpp \
    communication/businesshandler.cpp \
    communication/businessinvoker.cpp \
    communication/businessresponse.cpp \
    communication/ftp/adapter.c \
    communication/ftp/data_trans.c \
    communication/ftp/ftp_server.c \
    communication/ftp/myftpclientprocess.cpp \
    communication/ftp/myftpserver.cpp \
    communication/ftp/mytcpserver.cpp \
    communication/ftp/mytcpsocket.cpp \
    communication/ftp/wavebusiness.cpp \
    communication/ftp/waveclienthandler.cpp \
    communication/modbus/modbusbusiness.cpp \
    communication/modbus/src/modbus-data.c \
    communication/modbus/src/modbus-rtu.c \
    communication/modbus/src/modbus-tcp.c \
    communication/modbus/src/modbus.c \
    communication/tcp/oscillbusinesssigne.cpp \
    communicationDataActivity/armcommunicationform.cpp \
    communicationDataActivity/canopenform.cpp \
    communicationDataActivity/communicationdataform.cpp \
    communicationDataActivity/metercoolform.cpp \
    communicationSetActivity/communicationset10form.cpp \
    communicationSetActivity/communicationset3_2form.cpp \
    dioActivity/dioform.cpp \
    dioActivity/dioitemdelegate.cpp \
    dioActivity/diomodel.cpp \
    dioActivity/diosavework.cpp \
    faultlogActivity/faultlogform.cpp \
    firmwareupdateActivity/firmupdateform.cpp \
    firmwareupdateActivity/firmwareupdateform.cpp \
    login.cpp \
    main.cpp \
    mainActivity/faultalarmdialog.cpp \
    mainActivity/mainform2.cpp \
    mainwindow.cpp \
    oscilloscopeActivity/oscilldataqueue.cpp \
    oscilloscopeActivity/oscilldatasavetask.cpp \
    oscilloscopeActivity/oscilldatasavethread.cpp \
    oscilloscopeActivity/oscilloscopeform.cpp \
    otherActivity/otherform.cpp \
    parameterActivity/binarysetmodel.cpp \
    parameterActivity/exportparametersform.cpp \
    parameterActivity/importparametersform.cpp \
    parameterActivity/loadparameterfileqthread.cpp \
    parameterActivity/parametercomparisonform.cpp \
    parameterActivity/parameterdialog.cpp \
    parameterActivity/parameterform.cpp \
    parameterActivity/saveworker.cpp \
    syslog/logger.c \
    syslog/syslog-client.c \
    syslog/xgetopt.c \
    systemtestActivity/systemtestform2.cpp \
    userManager/user.cpp \
    userManager/usermanager.cpp \
    watercoolingActivity/watercoolingform.cpp \
    watercoolingActivity/watersavework.cpp \
    waveformActivity/loadwavefileqthread.cpp \
    waveformActivity/waveform.cpp

HEADERS += \
    aioActivity/AioDataClass.h \
    aioActivity/aiocommand.h \
    aioActivity/aioform.h \
    aioActivity/aiohandler.h \
    aioActivity/aioitemdelegate.h \
    aioActivity/aiomodel.h \
    aioActivity/compulsoryhanlder.h \
    cnav/cnavdelegate.h \
    cnav/cnavmodel.h \
    cnav/cnavview.h \
    common/cabinetmanagement.h \
    common/commonclass.h \
    common/commondialog.h \
    common/devicemanager.h \
    common/parameterclass.h \
    common/parametermanagement.h \
    common/parametertable.h \
    common/timertask.h \
    common/tool.h \
    common/waitdialog.h \
    commonView/listchooisedialog.h \
    commonView/mylistchooicedialog.h \
    commonView/mybaseqwidget.h \
    commonView/mycustomplot.h \
    commonView/mytracer.h \
    commonView/mywaitdialog.h \
    commonView/qcustomplot.h \
    commonView/waitdialog.h \
    commonView/wavefileitemwidget.h \
    commonView/waveitemwidget.h \
    communication/businessclass.h \
    communication/businesshandler.h \
    communication/businessinvoker.h \
    communication/businessresponse.h \
    communication/ftp/adapter.h \
    communication/ftp/config.h \
    communication/ftp/data_trans.h \
    communication/ftp/ftp_server.h \
    communication/ftp/myftpserver.h \
    communication/ftp/myftpclientprocess.h \
    communication/ftp/mytcpserver.h \
    communication/ftp/mytcpsocket.h \
    communication/ftp/wavebusiness.h \
    communication/ftp/waveclienthandler.h \
    communication/modbus/config.h \
    communication/modbus/modbusbusiness.h \
    communication/modbus/src/modbus-private.h \
    communication/modbus/src/modbus-rtu-private.h \
    communication/modbus/src/modbus-rtu.h \
    communication/modbus/src/modbus-tcp-private.h \
    communication/modbus/src/modbus-tcp.h \
    communication/modbus/src/modbus-version.h \
    communication/modbus/src/modbus.h \
    communication/tcp/oscillbusinesssigne.h \
    communicationDataActivity/armcommunicationform.h \
    communicationDataActivity/canopenform.h \
    communicationDataActivity/communicationdataform.h \
    communicationDataActivity/metercoolform.h \
    communicationSetActivity/communicationset10form.h \
    communicationSetActivity/communicationset3_2form.h \
    dioActivity/DioDataClass.h \
    dioActivity/dioform.h \
    dioActivity/dioitemdelegate.h \
    dioActivity/diomodel.h \
    dioActivity/diosavework.h \
    faultlogActivity/faultlogform.h \
    firmwareupdateActivity/firmupdateform.h \
    firmwareupdateActivity/firmwareupdateform.h \
    login.h \
    mainActivity/faultalarmdialog.h \
    mainActivity/mainform2.h \
    mainwindow.h \
    oscilloscopeActivity/oscilldataqueue.h \
    oscilloscopeActivity/oscilldatasavetask.h \
    oscilloscopeActivity/oscilldatasavethread.h \
    oscilloscopeActivity/oscilloscopeform.h \
    otherActivity/otherform.h \
    parameterActivity/BinarySetClass.h \
    parameterActivity/binarysetmodel.h \
    parameterActivity/exportparametersform.h \
    parameterActivity/importparametersform.h \
    parameterActivity/loadparameterfileqthread.h \
    parameterActivity/parametercomparisonform.h \
    parameterActivity/parameterdialog.h \
    parameterActivity/parameterform.h \
    parameterActivity/saveworker.h \
    syslog/syslog.h \
    syslog/xgetopt.h \
    systemtestActivity/systemtestform2.h \
    userManager/user.h \
    userManager/usermanager.h \
    watercoolingActivity/watercoolingform.h \
    watercoolingActivity/watersavework.h \
    waveformActivity/loadwavefileqthread.h \
    waveformActivity/waveform.h

FORMS += \
    aioActivity/aioform.ui \
    commonView/listchooisedialog.ui \
    commonView/mylistchooicedialog.ui \
    commonView/waitdialog.ui \
    commonView/wavefileitemwidget.ui \
    commonView/waveitemwidget.ui \
    communicationDataActivity/armcommunicationform.ui \
    communicationDataActivity/canopenform.ui \
    communicationDataActivity/communicationdataform.ui \
    communicationDataActivity/metercoolform.ui \
    communicationSetActivity/communicationset10form.ui \
    communicationSetActivity/communicationset3_2form.ui \
    dioActivity/dioform.ui \
    faultlogActivity/faultlogform.ui \
    firmwareupdateActivity/firmupdateform.ui \
    firmwareupdateActivity/firmwareupdateform.ui \
    login.ui \
    mainActivity/faultalarmdialog.ui \
    mainActivity/mainform2.ui \
    mainwindow.ui \
    oscilloscopeActivity/oscilloscopeform.ui \
    otherActivity/otherform.ui \
    parameterActivity/exportparametersform.ui \
    parameterActivity/importparametersform.ui \
    parameterActivity/parametercomparisonform.ui \
    parameterActivity/parameterdialog.ui \
    parameterActivity/parameterform.ui \
    systemtestActivity/systemtestform2.ui \
    watercoolingActivity/watercoolingform.ui \
    waveformActivity/waveform.ui

TRANSLATIONS += \
    DongfangElectric_zh_CN.ts

LIBS += -Ldll -lws2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
