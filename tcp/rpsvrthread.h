#ifndef RPSVRTHREAD_H
#define RPSVRTHREAD_H

#include <QThread>
#include <QtNetwork>
#include <QStringList>
#include "func4all.h"

typedef enum{
    UfNone=0,
    UfInUse,
    UfUpdated
}TypeTagFlags ;

typedef enum{
      type_errNone=0,
      type_errFormat,
      type_errCRC,
      type_errTagNum,
      type_errTagCnt,
      type_errTagFst,
      type_errTagMark
}TypeRpSvrTCPErr;

class RpSvrTag : public QObject
{
    Q_OBJECT
public:
    RpSvrTag(QObject *parent = 0);
    virtual ~RpSvrTag() {}

    QString         tagName;
    int             valIn;
    int             valOut;
    bool            needOut;
    bool            changed;

    // флаги дл€ иррегул€рных, тегов
    TypeTagFlags    useFlag;
    bool            isIrregular;
};

typedef QList<RpSvrTag*> RpSvrTagList;







class RpSvrThread : public QThread
{
    Q_OBJECT
public:
    explicit RpSvrThread(QObject *parent = 0);

    void                init(QString a_hostName, int a_hostPort, QString a_tagsFilter);
    void                stop();


    enum WorkStates { Stop, Working, Connecting, WaitReconnect, Disconnecting };
    WorkStates          workState() { return m_workSatte.get(); }

    int                 tagCount();
    int                 getTagIndex(QString &aName);
    int                 getTagValue(QString &aName);
    int                 getTagValue(int aIndex);
    QString             getTagName(int aIndex);

    void                setTagValue(int aIndex, int aValue);

    bool                isTagsListCorrect();
public slots:
    void                startWork();
signals:
    void                dataChanged();
    void                connectionEstablished();
    void                connectionRefused(QString str);

protected:
    void                run();

private:
    class WorkState {
    private:
        QMutex      m_mutex;
        QTime       m_stateTime;
        WorkStates  m_state;
    public:
        void set(WorkStates value) {
            m_mutex.lock();
            m_state = value;
            m_stateTime.start();
            m_mutex.unlock();
        }
        WorkStates get() {
            m_mutex.lock();
            WorkStates value = m_state;
            m_mutex.unlock();
            return value;
        }
        int stateTime() {
            return m_stateTime.elapsed();
        }
    };
    WorkState           m_workSatte;


    RpSvrTagList        *m_tagList;
    bool                m_tagsListCorrect;
    bool                m_needToReadProps;
    bool                m_needToGetAll;
    bool                m_firstPass;
    bool                m_useCrcTags;

    bool    tagsListCreate(int aItemsCount);


    QTcpSocket          *m_tcpClient;
    char                m_tcpClientBuffer[1024];
    QString             m_hostName;
    int                 m_hostPort;
    // дл€ выборки тегов, относ€щихс€ к данному проекту
    QString             m_tagFilter;

    bool    tcpIsTagsListCorrect();

    bool    tcpGetTagNames();
    bool    tcpReciveTagNames(QString &answer, int &tagNum);

    bool    tcpRefreshValue();
    bool    tcpReciveValues(QString getCmd, QString getAnsw);
    bool    tcpReciveValuesEx(QString getCmd, QString getAnsw, bool modeValues);

    bool    tcpTransferValue();

    bool    tcpGetAnswer(const char *cmdCode, bool chekCRC, QString &answerStr, bool &reciveAll);
    bool    tcpDoCommand(const char *cmdCode, const char *cmd, QString &answer);
};

#endif // RPSVRTHREAD_H
