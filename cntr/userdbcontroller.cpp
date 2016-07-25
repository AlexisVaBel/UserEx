#include "userdbcontroller.hpp"
#include <QDebug>
UserDBController::UserDBController(QObject *parent, QString strDb, QString strHost) : QObject(parent){
    m_iUserID=-1;
    m_iCardID=-1;
    m_iGroupID=-1;
    m_dbCon=new dbProcsTbl(this,strDb,strHost);
    m_bCanWork=m_dbCon->connectDB("SYSDBA","masterkey");
    m_dbCon->checkForTables();
}

QStringList UserDBController::getUsersData(){
    if (m_bCanWork) return m_dbCon->checkForData("USERS","NAME");
    else {
        QStringList lst;lst<<"NONE Con";
        return lst;
    }
}

QStringList UserDBController::getGroupData(){
    if (m_bCanWork) return m_dbCon->checkForData("GROUPS","NAME");
    else {
        QStringList lst;lst<<"NONE Con";
        return lst;
    }
}

QString UserDBController::getUserCard(QString strUser){
    if(!m_bCanWork)return QString("NONE Con");
    QStringList lstCardCode;
    if(loadAllUserInfo(strUser))
        lstCardCode=m_dbCon->checkForDataWhere("CARDS","CODE",QString(" where USER_ID=%1").arg(m_iUserID));// get card Code
    if(lstCardCode.isEmpty())return QString("NONE");
    return lstCardCode.at(0);
}

QString UserDBController::getUserGroup(QString strUser){
    if(!m_bCanWork)return QString("NONE");
    QStringList lstGroup;
    if(loadAllUserInfo(strUser))
            lstGroup=m_dbCon->checkForDataWhere("GROUPS","NAME",QString(" where id=%1").arg(m_iGroupID));
        else return QString("NONE");
    if(lstGroup.isEmpty())  return QString("NONE");
    return lstGroup.at(0);
}

bool UserDBController::loadAllUserInfo(QString strUser){
    if(!getUserId(strUser))return false;
    if(!getGroupId())return false;
    if(!getCardId())return false;
    return true;
}

bool UserDBController::getUserId(QString strUser){
    m_iUserID=-1;
    QStringList lstUserId=m_dbCon->checkForDataWhere("USERS","ID",QString(" where NAME=\'%1\'").arg(strUser));// get user Id
    if(lstUserId.isEmpty())return false;
    m_iUserID=lstUserId.at(0).toInt();
    return true;
}

bool UserDBController::getCardId(){
    m_iCardID=-1;
    QStringList lstCardId=m_dbCon->checkForDataWhere("CARDS","ID",QString(" where USER_ID=%1").arg(m_iUserID));
    if(lstCardId.isEmpty())return false;
    m_iCardID=lstCardId.at(0).toInt();
    return true;
}

bool UserDBController::getCardIdByName(QString str){
    m_iCardID=-1;
    QStringList lstCardId=m_dbCon->checkForDataWhere("CARDS","ID",QString(" where CODE=\'%1\'").arg(str));
    if(lstCardId.isEmpty())return false;
    m_iCardID=lstCardId.at(0).toInt();
    return true;
}

bool UserDBController::getGroupId(){
    m_iGroupID=-1;
    QStringList lstGroupId=m_dbCon->checkForDataWhere("USERGROUPS","GROUP_ID",QString(" where USER_ID=%1").arg(m_iUserID));
    if(lstGroupId.isEmpty())return false;
    m_iGroupID=lstGroupId.at(0).toInt();
    return true;
}

bool UserDBController::getGroupIdByName(QString str){
    m_iGroupID=-1;
    QStringList lstGroupId=m_dbCon->checkForDataWhere("GROUPS","ID",QString(" where NAME=\'%1\'").arg(str));
    if(lstGroupId.isEmpty())return false;
    m_iGroupID=lstGroupId.at(0).toInt();
    return true;
}

bool UserDBController::insertUser(QString strUser, QString strCard, QString strGroup){
//    qDebug()<<"===============================";
//    qDebug()<<strUser;
//    qDebug()<<strCard;
//    qDebug()<<strGroup;
    //user
    QString str=QString("insert into users (NAME,DESCR,PSW) values (\'%1\',\'%2\',\'%3\')").arg(strUser).arg(strUser).arg(strCard);
    if(!m_dbCon->procsDML(str))return false;
//    qDebug()<<"added user";
    if(!getUserId(strUser))return false;
//    qDebug()<<"user found";
    //card
    if(!getCardIdByName(strCard)){
//        qDebug()<<"card not found";
        str=QString("insert into cards (USER_ID,CODE) values (%1,\'%2\')").arg(m_iUserID).arg(strCard);
        if(!m_dbCon->procsDML(str))return false;
//        qDebug()<<"card inserted";
        if(!getCardId())return false;
//        qDebug()<<"card getted";
    }else{
//        qDebug()<<"card found";
        str=QString("update cards set user_id=%1 where id=%2").arg(m_iUserID).arg(m_iCardID);
        if(!m_dbCon->procsDML(str))return false;
//        qDebug()<<"card updated";
    }
    //group
    if(!getGroupIdByName(strGroup))return false;
    str=QString("insert into usergroups (USER_ID,GROUP_ID) values (%1,%2)").arg(m_iUserID).arg(m_iGroupID);
    if(!m_dbCon->procsDML(str))return false;
//    qDebug()<<"===============================";
    return true;
}

bool UserDBController::updateUser(QString strUser,QString strCard, QString strGroup){
    QString str;
    if(!getUserId(strUser))return false;
    if(!getCardIdByName(strCard)){
//        qDebug()<<"card not found";
        str=QString("insert into cards (USER_ID,CODE) values (%1,\'%2\')").arg(m_iUserID).arg(strCard);
        if(!m_dbCon->procsDML(str))return false;
//        qDebug()<<"card inserted";
        if(!getCardId())return false;
//        qDebug()<<"card getted";
    }else{
//        qDebug()<<"card found";
        str=QString("update cards set user_id=%1 where id=%2").arg(m_iUserID).arg(m_iCardID);
        if(!m_dbCon->procsDML(str))return false;
//        qDebug()<<"card updated";
    }
    if(!getGroupIdByName(strGroup))return false;
    if(!getGroupId())str=QString("insert into usergroups (USER_ID,GROUP_ID) values (%1,%2)").arg(m_iUserID).arg(m_iGroupID);
    else
        str=QString("update usergroups set GROUP_ID=%1 where USER_ID=%2").arg(m_iGroupID).arg(m_iUserID);
    if(!m_dbCon->procsDML(str))return false;
    return true;
}

bool UserDBController::bUserExists(QString strUser){
    return getUserId(strUser)>0;
}
