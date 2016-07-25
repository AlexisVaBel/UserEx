#include "dbchecktbl.hpp"
#include <QDebug>
#include <QSqlQuery>
#include <iostream>

dbProcsTbl::dbProcsTbl(QObject *parent, QString strDB, QString strHost):dbfirebird(parent,strDB,strHost){

}

void dbProcsTbl::checkForTables(){
    m_lstTable=m_db.tables();
    m_mapFields.clear();
    foreach(QString strTable,m_lstTable)checkForFields(strTable);
}

void dbProcsTbl::checkForFields(QString strTable){
    QSqlQuery *qry=new QSqlQuery(m_db);
    QStringList lstFields;
    QString strSql="select rdb$field_name from rdb$relation_fields ";
    strSql.append("where rdb$relation_name=\'"+strTable+"\'");
    qry->prepare(strSql);
    if (!qry->exec()){
        std::cout<<qry->lastError().text().toUtf8().data()<<std::endl;
        return;
    }
    while(qry->next()){
        lstFields.append(QString::fromLocal8Bit(qry->value(0).toString().toUtf8().data()));
    }
    qry->clear();
    m_mapFields.insert(strTable,lstFields);
}

bool dbProcsTbl::procsDML(QString strSQL){
    QSqlQuery *qry=new QSqlQuery(m_db);
    qDebug()<<strSQL;
    qry->prepare(strSQL);
    if (!qry->exec()){
        return false;
    }
    qry->finish();
    return true;
}


QStringList dbProcsTbl::checkForDataWhere(QString strTable, QString strField, QString strCond){
    QStringList lst,lstOut;
    QSqlQuery *qry=new QSqlQuery(m_db);
    if(!(m_lstTable.contains(strTable)))return lstOut;
    lst=m_mapFields.value(strTable);
    if(lst.contains(strField))return lstOut;
    QString strSql=QString("select %1 from %2 ").arg(strField).arg(strTable);
    strSql.append(strCond);
    qry->prepare(strSql);
    if (!qry->exec()){
        std::cout<<qry->lastError().text().toUtf8().data()<<std::endl;
        std::cout<<qry->lastQuery().toStdString().data()<<std::endl;
        return lstOut;
    };
    while(qry->next()){
        lstOut.append(qry->value(0).toString());
    };
    return lstOut;
}



QStringList dbProcsTbl::checkForData(QString strTable, QString strField){
    return checkForDataWhere(strTable,strField," ");
}
