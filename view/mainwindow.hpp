#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QWidget>
#include <QStatusBar>
#include <QLabel>
#include <QMainWindow>



#include "data/dbchecktbl.hpp"
#include "tcp/rpsvrthread.h"
#include <cntr/userdbcontroller.hpp>

static QString strPathDB    =QString("E:\\DataBase\\SHPFZ\\USERS.FDB");
static QString strHostDB    =QString("localhost");
static QString strHostTCP   =QString("localhost");
static QString strFiltTCP   ="*";
static int     iPortTCP     =3033;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
signals:

public slots:
    void    slOkPushed();
    void    slCancelPushed();

    void    slUserSetted();
    void    slUpdateTcpData();

    void    slTcpEstablished();
    void    slTcpRefused(QString str);
    void    slCardSelected(QModelIndex idx);
private slots:
    void on_btnOK_clicked();

private:

    QString             m_strCode;
    QStringList         m_lstCards;
    QStringListModel    *m_modelCards;


    Ui::MainWindow  *m_ui;

    RpSvrThread      *m_rpsvr;
    UserDBController *m_userCntr;

    void prepareUsersData();
    void prepareUserList();
    void prepareGroupList();

    void prepareTbl();
    void prepareView();
    void prepareSiSlo();

    void startListenTcp();

};

#endif // MAINWINDOW_H
