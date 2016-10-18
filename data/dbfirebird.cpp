#include "dbfirebird.hpp"
#include <unistd.h>
#include <ibase.h>
#include <string>
#include <QTableView>
#include <fstream>


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
    std::ofstream out;
    out.open("db.log");

    if(m_db.databaseName().isEmpty())
        m_db=QSqlDatabase::addDatabase("QIBASE","QIBASE_MAIN");
    m_db.setDatabaseName(m_strDB);
    m_db.setHostName(m_strHost);
    m_db.setUserName(m_strUser);
    m_db.setPassword(m_strPSW);

    for(QString str:m_db.drivers()){
        out<<" dbs are "<<str.toStdString().c_str()<<std::endl;
    }
    if(!m_db.open()){
        m_strError=m_db.lastError().text();
//        out<<m_strError.toStdString().c_str();
        out.close();
        return false;
    }
    out.close();
    return true;
}

void dbfirebird::disConnectDB(){
    if(m_db.isOpen()) m_db.close();
}


