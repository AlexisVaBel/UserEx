#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QCompleter>
#include <QVBoxLayout>

#include <QThread>
#include <QMessageBox>
#include <QHeaderView>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),
    m_ui(new Ui::MainWindow){

    m_userCntr=new UserDBController(this,"E:\\DataBase\\SHPFZ\\USERS.FDB","localhost");


    prepareView();
    prepareSiSlo();

    prepareUsersData();
    startListenTcp();
    prepareTbl();
}

MainWindow::~MainWindow(){
    delete m_ui;    
    delete m_rpsvr;
    delete m_userCntr;
}

void MainWindow::prepareView(){
    m_ui->setupUi(this);
    m_ui->centralWidget->setLayout(m_ui->gridLayout);
    m_ui->lblCardAct->setFont(QFont("Digital-7",14,QFont::Bold));
    m_ui->lblCardNew->setFont(QFont("Digital-7",14,QFont::Bold));
    m_ui->lblCardAct->setText("----");
    m_ui->lblCardNew->setText("----");
}

void MainWindow::prepareUsersData(){
    prepareUserList();
    prepareGroupList();
}

void MainWindow::prepareUserList(){
    QCompleter *completer=new QCompleter(m_userCntr->getUsersData(),this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_ui->edtUser->setCompleter(completer);
}

void MainWindow::prepareGroupList(){
    m_ui->boxGroup->addItems(m_userCntr->getGroupData());
}

void MainWindow::slOkPushed(){
    QString strUser=m_ui->edtUser->text();
    if(!m_userCntr->bUserExists(strUser)){
        QMessageBox::StandardButton reply=QMessageBox::question(this,QString::fromLocal8Bit("Подтверждение"),QString::fromLocal8Bit("Добавить нового пользователя"),
                                                                QMessageBox::Yes|QMessageBox::No);
        if(reply==QMessageBox::Yes){
            if(m_userCntr->insertUser(strUser,m_ui->lblCardNew->text(),m_ui->boxGroup->currentText())){
                QMessageBox::information(this,"Added user",strUser);
                prepareUserList();
                slUserSetted();
            }else{
                QMessageBox::information(this,"Error adding user",strUser);
            }
        };
    }else{
        QMessageBox::StandardButton reply=QMessageBox::question(this,QString::fromLocal8Bit("Подтверждение"),QString::fromLocal8Bit("Обновить пользователя"),
                                                                QMessageBox::Yes|QMessageBox::No);
        if(reply==QMessageBox::Yes){
            if(m_userCntr->updateUser(m_ui->edtUser->text(),m_ui->lblCardNew->text(),m_ui->boxGroup->currentText())){
                QMessageBox::information(this,"Updated user","str");
                slUserSetted();
            }else{
                QMessageBox::information(this,"Error updating user",strUser);
            }
        };
    }

}

void MainWindow::slCancelPushed(){
    close();
}



void MainWindow::prepareTbl(){
    m_modelCards = new QStringListModel();
    m_modelCards->setStringList(m_lstCards);
    m_lstCards.append(QString::fromLocal8Bit("Карта"));

    m_ui->lstCards->setModel(m_modelCards);
    m_ui->lstCards->setAlternatingRowColors(true);
    m_ui->lstCards->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::prepareSiSlo(){
    connect(m_ui->edtUser,SIGNAL(editingFinished()),this,SLOT(slUserSetted()));
    connect(m_ui->lstCards,SIGNAL(clicked(QModelIndex)),this,SLOT(slCardSelected(QModelIndex)));
    connect(m_ui->btnOK,SIGNAL(clicked()),this,SLOT(slOkPushed()));
}

void MainWindow::startListenTcp(){
    QThread *thr=new QThread(this);
    m_rpsvr =new RpSvrThread();
    m_rpsvr->init("localhost",3033,"*");
    m_rpsvr->moveToThread(thr);
    connect(thr,SIGNAL(started()),m_rpsvr,SLOT(startWork()));
    connect(m_rpsvr,SIGNAL(dataChanged()),this,SLOT(slUpdateTcpData()));
    connect(m_rpsvr,SIGNAL(connectionEstablished()),this,SLOT(slTcpEstablished()));
    connect(m_rpsvr,SIGNAL(connectionRefused(QString)),this,SLOT(slTcpRefused(QString)));
    thr->start();
}


void MainWindow::slUserSetted(){
    QString strUser=m_ui->edtUser->text();
    m_ui->lblCardAct->setText((m_userCntr->getUserCard(strUser)));
    m_ui->lblCardNew->setText("----");
    m_ui->boxGroup->setCurrentText(m_userCntr->getUserGroup(strUser));
}

void MainWindow::slUpdateTcpData(){
    int iCnt=0;
    int iCode[5]={0,0,0,0,0};

    QString strTagCode;
    QString strCode=QString::fromLocal8Bit("PS01.Code");

    while(iCnt<5){
        strTagCode=strCode+QString::number(iCnt);
        iCode[iCnt++]=m_rpsvr->getTagValue(strTagCode);
    };
    strCode="PS01.SYSTEM.ErrorCount";    
    if((m_rpsvr->getTagValue(strCode))==1)
        m_ui->lblStatPSDin->setText("<font color=red>"+QString::fromLocal8Bit("Ошб связь")+"<//font>");
    else
        m_ui->lblStatPSDin->setText(QString::fromLocal8Bit("OK"));
    strTagCode="";
    for(int i=0;i<5;i++)
        strTagCode.append(QString("%1").arg(iCode[i],2,16,QChar('0')));

    if(!m_lstCards.contains(strTagCode)){
        m_lstCards.append(strTagCode);
        m_modelCards->setStringList(m_lstCards);
        m_ui->lstCards->update();
    }
}


void MainWindow::slTcpEstablished(){
     m_ui->lblStatPSDin->setText("<font color=green>"+QString::fromLocal8Bit("OK")+"<//font>");
}

void MainWindow::slTcpRefused(QString str){    
    QMessageBox::information(this,"",str);
    m_ui->lblStatPSDin->setText("<font color=red>"+QString::fromLocal8Bit("TCP err")+"<//font>");
}

void MainWindow::slCardSelected(QModelIndex idx){
    m_strCode=idx.data().toString();
    m_ui->lblCardNew->setText(m_strCode);
}

void MainWindow::on_btnOK_clicked(){

}
