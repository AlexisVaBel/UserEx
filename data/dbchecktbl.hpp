#ifndef DBMTH_H
#define DBMTH_H

#include "dbfirebird.hpp"

#include <QMap>

class dbProcsTbl:public dbfirebird
{
public:
    dbProcsTbl(QObject *parent = 0,QString strDB=0,QString strHost=0);
    void            checkForTables();
    void            checkForFields(QString strTable);
    bool            procsDML(QString strSQL);

    QStringList     checkForData(QString strTable,QString strField);
    QStringList     checkForDataWhere(QString strTable,QString strField,QString strCond);
private:
    QMap<QString,QStringList> m_mapFields;
    QStringList             m_lstTable;
};

#endif // DBMTH_H
