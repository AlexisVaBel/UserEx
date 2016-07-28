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


    void on_btnCancel_clicked();

private:

    bool                m_bTCPErr;

    QString             m_strCode;
    QString             m_strTCPHost;
    QString             m_strTCPPort;
    QString             m_strDBPath;
    QString             m_strDBHost;
    QString             m_strTag;
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

    void readIni();
    void writeIni();

    void startListenTcp();


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
