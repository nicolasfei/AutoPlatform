#include "aioform.h"
#include "ui_aioform.h"

AIOForm::AIOForm(QWidget *parent) :
    MyBaseQWidget(parent),
    ui(new Ui::AIOForm)
{
    ui->setupUi(this);
    this->initUiView();
}

AIOForm::~AIOForm()
{
    delete ui;
}

void AIOForm::initUiView()      //初始化ui view
{
    int rowSpace = 40;
    int columnSpace = 20;
    ui->frame->setStyleSheet("#frame{border-image: url(:/images/images/aio-1.png);}");
    ui->frame_2->setStyleSheet("#frame_2{border-image: url(:/images/images/aio-2.png);}");
    ui->frame_3->setStyleSheet("#frame_3{border-image: url(:/images/images/aio-3.png);}");

    this->netModel = new AioModel();
    this->netModel->setColumnCount(2);
    for(int i=0;i<13;i++){
        this->netModel->add();
    }
    this->netDelegate = new AioItemDelegate();
    this->netDelegate->setAioModel(this->netModel);
    ui->tableView->setItemDelegate(this->netDelegate);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);	//内容不可编辑
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); //选择整行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); //只选择一行
    ui->tableView->setShowGrid(false);      //不显示网格
    ui->tableView->verticalHeader()->hide();//隐藏垂直表头
    ui->tableView->horizontalHeader()->hide();
//    ui->tableView->verticalHeader()->setDefaultSectionSize(rowSpace);        //设置行间距
//    ui->tableView->verticalHeader()->setMinimumSectionSize(rowSpace);        //设置行间距
//    ui->tableView->horizontalHeader()->setDefaultSectionSize(columnSpace);        //设置列间距
//    ui->tableView->horizontalHeader()->setMinimumSectionSize(columnSpace);        //设置列间距
    ui->tableView->setModel(this->netModel);
    //设置每列的宽度--在setModel(m_model)之后设置才有效
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度

    this->engineModel = new AioModel();
    this->engineModel->setColumnCount(2);
    for(int i=0;i<13;i++)
    {
        this->engineModel->add();
    }
    this->engineDelegate = new AioItemDelegate();
    this->engineDelegate->setAioModel(this->engineModel);
    ui->tableView_2->setItemDelegate(this->engineDelegate);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);	//内容不可编辑
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows); //选择整行
    ui->tableView_2->setSelectionMode(QAbstractItemView::SingleSelection); //只选择一行
    ui->tableView_2->setShowGrid(false);      //不显示网格
    ui->tableView_2->verticalHeader()->hide();//隐藏垂直表头
    ui->tableView_2->horizontalHeader()->hide();
//    ui->tableView_2->verticalHeader()->setDefaultSectionSize(rowSpace);        //设置行间距
//    ui->tableView_2->verticalHeader()->setMinimumSectionSize(rowSpace);        //设置行间距
//    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(columnSpace);        //设置列间距
//    ui->tableView_2->horizontalHeader()->setMinimumSectionSize(columnSpace);        //设置列间距
    ui->tableView_2->setModel(this->engineModel);
    //设置每列的宽度--在setModel(m_model)之后设置才有效
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度


    this->armModel = new AioModel();
    this->armModel->setColumnCount(3);
    for(int i=0;i<45;i++)
    {
        AioDataClass *data = new AioDataClass();
        data->setName(QString::fromLocal8Bit("保留"));
        data->setValue(QString::fromLocal8Bit("0.36587"));
        this->armModel->add(data);
    }
    this->armDelegate = new AioItemDelegate();
    this->armDelegate->setAioModel(this->armModel);
    ui->tableView_3->setItemDelegate(this->armDelegate);
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);	//内容不可编辑
    ui->tableView_3->setSelectionBehavior(QAbstractItemView::SelectRows); //选择整行
    ui->tableView_3->setSelectionMode(QAbstractItemView::SingleSelection); //只选择一行
    ui->tableView_3->setShowGrid(false);      //不显示网格
    ui->tableView_3->verticalHeader()->hide();//隐藏垂直表头
    ui->tableView_3->horizontalHeader()->hide();
//    ui->tableView_3->verticalHeader()->setDefaultSectionSize(rowSpace);        //设置行间距
//    ui->tableView_3->verticalHeader()->setMinimumSectionSize(rowSpace);        //设置行间距
//    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(columnSpace);        //设置列间距
//    ui->tableView_3->horizontalHeader()->setMinimumSectionSize(columnSpace);        //设置列间距
    ui->tableView_3->setModel(this->armModel);
    //设置每列的宽度--在setModel(m_model)之后设置才有效
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所有列都扩展自适应宽度，填充充满整个屏幕宽度=Stretch

}

void AIOForm::initData()        //初始化数据
{

}

void AIOForm::startBusiness()   //开始业务逻辑
{

}

void AIOForm::endBusiness()     //结束业务逻辑
{

}
