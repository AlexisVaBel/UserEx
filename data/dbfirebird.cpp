#include "dbfirebird.hpp"
#include <unistd.h>
#include <ibase.h>
#include <string>
#include <QTableView>


dbfirebird::dbfirebird(QObject *parent, QString strDB, QString strHost) :
    QObject(parent),    
    m_strDB(strDB),
    m_strHost(strHost){
    m_strUser=DEF_RUSER;
    m_strPSW=DEF_PSW;
}

bool dbfirebird::connectDB(QString strUser, QString strPsw){
    m_strPSW=strPsw;
    m_strUser=strUser;
    if(!tryToConnect()){        
        return false;
    };
    return true;
}

bool dbfirebird::tryToConnect(){
    if(m_db.databaseName().isEmpty())
        m_db=QSqlDatabase::addDatabase("QIBASE","QIBASE_MAIN");
    m_db.setDatabaseName(m_strDB);
    m_db.setHostName(m_strHost);
    m_db.setUserName(m_strUser);
    m_db.setPassword(m_strPSW);
    if(!m_db.open()){
        m_strError=m_db.lastError().text();
//        qDebug()<<m_strError;
//        qDebug()<<m_db.drivers();
        return false;
    }    
    return true;
}

void dbfirebird::disConnectDB(){
//    qDebug()<<"disconnected";
    if(m_db.isOpen()) m_db.close();
}


