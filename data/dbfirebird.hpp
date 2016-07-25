#ifndef DBFIREBIRD_HPP
#define DBFIREBIRD_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>


static const QString    DEF_RUSER   ="SYSDBA";
static const QString    DEF_PSW     ="masterkey";

class dbfirebird : public QObject
{
    Q_OBJECT
public:
    explicit        dbfirebird(QObject *parent = 0,QString strDB=0,QString strHost=0);

    bool            connectDB(QString strUser,QString strPsw);
    void            disConnectDB();

protected:
    QString         m_strDB;
    QString         m_strUser;
    QString         m_strPSW;
    QString         m_strHost;
    QString         m_strSQL;

    QString         m_strError;

    QSqlDatabase    m_db;

    bool     tryToConnect();
};

#endif // DBFIREBIRD_HPP
