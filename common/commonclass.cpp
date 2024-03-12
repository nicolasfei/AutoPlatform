#include "commonclass.h"

/*QTableView 左上角样式*/
const QString CommonClass::TABLE_CornerButtonSTYLE = "QTableView QTableCornerButton::section {"
    //"background: red;"
    //"border: 2px outset red;"
    "color: red;"
    "background-color: rgb(64, 64, 64);"
    "border: 5px solid #f6f7fa;"
    "border-radius:0px;"
    "border-color: rgb(64, 64, 64);"
 "}";

const QString CommonClass::TABLE_STYLE = "QTableView {"
    "color: white;"                                       /*表格内文字颜色*/
    "font: 9px;"
    "gridline-color: transparent;"                              /*表格内框颜色*/
    "background-color: rgb(0, 0, 0, 0);"                /*表格内背景色*/
    "alternate-background-color: rgb(0, 0, 0, 0);"
    "selection-color: white;"                             /*选中区域的文字颜色*/
    "selection-background-color: rgb(77, 77, 77);"        /*选中区域的背景色*/
    "border: 0px groove gray;"
    "border-radius: 0px;"
//        "padding: 2px 4px;"
"}";

const QString CommonClass::TABLE_HEADSTYLE = "QHeaderView {"
    "color: white;"
    "font: 9px;"
    "background-color: rgb(108, 108, 108);"
    "border: 0px solid rgb(144, 144, 144);"
    "border:0px solid rgb(191,191,191);"
    "border-left-color: rgba(255, 255, 255, 0);"
    "border-top-color: rgba(255, 255, 255, 0);"
    "border-radius:0px;"
    "min-height:15px;"
"}";

//const QString CommonClass::TABLE_HEAD_SECTIONSTYLE = "QHeaderView::section {"
//    "color: white;"
//    "background-color: rgb(64, 64, 64);"
//    "border: 5px solid #f6f7fa;"
//    "border-radius:0px;"
//    "border-color: rgb(64, 64, 64);"
//"}";

const QString CommonClass::TABLE_HEAD_SECTIONSTYLE = "QHeaderView::section {"
    "color: white;"                                       /*表格内文字颜色*/
    //"gridline-color: transparent;"                              /*表格内框颜色*/
    "font: 9px;"
    "background:rgb(22,40,76);"                /*表格内背景色*/
//    "alternate-background-color: rgb(0, 0, 0, 0);"
//    "selection-color: white;"                             /*选中区域的文字颜色*/
//    "selection-background-color: rgb(77, 77, 77);"        /*选中区域的背景色*/
//    "border: 0px groove gray;"
//    "border-radius: 0px;"
//        "padding: 2px 4px;"
"}";

const QString CommonClass::LEVEL1_STYLE = "QPushButton{"                             // 正常状态样式
        "background-color: rgb(0, 0, 0, 0);"       // 背景色（也可以设置图片）
        "color: white;"                            // 字体颜色
        "font: bold 12px;"                         // 字体: 加粗 大小
        "border-style:outset;"                     // 定义一个3D突出边框，inset与之相反
        "text-align: center;"                      // 文本：左对齐
        "}";
const QString CommonClass::LEVEL1_FOCUS_STYLE = "QPushButton{"                             // 获取焦点状态
        "border-image: url(:/images/images/bg-level1.png);"
        "color: white;"
        "border-style:outset;"
        "font:bold 12px;"
        "text-align: center;"
        "}";
const QString CommonClass::LEVEL2_STYLE = "QPushButton{"                             // 正常状态样式
        "background-color: rgb(27, 51, 95);"       // 背景色（也可以设置图片）
        "color: white;"                            // 字体颜色
        "font: bold 10px;"                         // 字体: 加粗 大小
        "border-style:outset;"                     // 定义一个3D突出边框，inset与之相反
        "text-align: center;"                      // 文本：左对齐
        "}";
const QString CommonClass::LEVEL2_FOCUS_STYLE = "QPushButton{"                        // 点击状态
        "background-color: rgb(0, 0, 0, 0);"
        "color: rgb(240, 110, 35);"
        "border-style:outset;"
        "font:bold 10px;"
        "text-align: center;"
        "}";

const QString CommonClass::ENABLE_STYLE = "QPushButton{"                             // 使能状态样式
        "background-color: rgb(240, 110, 35);"       // 背景色（也可以设置图片）
        "color: white;"                            // 字体颜色
        "font: 9px;"                         // 字体: 加粗 大小
        "border-style:outset;"                     // 定义一个3D突出边框，inset与之相反
        "text-align: center;"                      // 文本：左对齐
        "}";
const QString CommonClass::DISABLE_STYLE = "QPushButton{"                        // 不使能状态
        "background-color: rgb(200, 200, 200);"
        "color: white;"
        "border-style:outset;"
        "font: 9px;"
        "text-align: center;"
        "}";

const int CommonClass::ACTIVITY_FRESH_DELAY = 500;      //页面数据刷新率

CommonClass::CommonClass()
{

}
