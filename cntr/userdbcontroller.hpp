#ifndef USERDBCONTROLLER_H
#define USERDBCONTROLLER_H

#include <QObject>
#include <QStringList>

#include <data/dbchecktbl.hpp>

class UserDBController : public QObject
{
    Q_OBJECT
public:
    explicit UserDBController(QObject *parent = 0,QString strDb="",QString strHost="");
    QStringList   getUsersData();
    QStringList   getGroupData();
    QString       getUserCard(QString strUser);
    QString       getUserGroup(QString strUser);
    bool          insertUser(QString strUser,QString strCard,QString strGroup);
    bool          updateUser(QString strUser, QString strCard, QString strGroup);
    bool          bUserExists(QString strUser);
    bool          canWork(){return m_bCanWork;}

signals:

public slots:
private:
    int          m_iUserID;
    int          m_iGroupID;
    int          m_iCardID;
    bool         m_bCanWork;
    dbProcsTbl  *m_dbCon;

    bool        loadAllUserInfo(QString strUser);
    bool        getUserId(QString strUser);
    bool        getCardId();
    bool        getGroupId();
    bool        getGroupIdByName(QString str);
    bool        getCardIdByName(QString str);
};

#endif // USERDBCONTROLLER_H
