#include "rpsvrthread.h"
#include "../lib/crc16.h"

const int ATTEMP_MAX = 3;

const char* ASW_CONNECT = "100";

const char* CMD_SETFILTER = "SETFILTER %1\n";
const char* ASW_SETFILTER = "101";

const char* CMD_CREATETAGLIST = "CREATETAGLIST\n";
const char* ASW_CREATETAGLIST = "103";

const char* CMD_GETTAGLIST = "GETTAGLIST %1\n";
const char* ASW_GETTAGLIST = "104";

const char* CMD_FIXALL = "FIXALL\n";
const char* ASW_FIXALL = "105";

const char* CMD_GETALL = "GETALL ";
const char* ASW_GETALL = "106";

const char* CMD_GETCHANGE = "GETCHG ";
const char* ASW_GETCHANGE = "107";

const char* CMD_WNM = "WNM %1 \n";
const char* ASW_WNM = "109";

const char* CMD_GETPROPS = "GETPROPS ";
const char* ASW_GETPROPS = "116";

const char* CMD_SETFLAG = "SETFLAG %1 \n";
const char* ASW_SETFLAG = "117";

const char* CMD_GETCRC = "GETCRC\n";
const char* ASW_GETCRC = "118";



//=============================================================================
RpSvrTag::RpSvrTag(QObject *parent):
    QObject(parent),
    valIn(0),
    valOut(0),
    needOut(false),
    changed(false),
    isIrregular(false)
{
}


//=============================================================================
RpSvrThread::RpSvrThread(QObject *parent) :
    QThread(parent),
    m_workSatte(),
    m_tagList(0),
    m_tagsListCorrect(false),
    m_needToReadProps(true),
    m_needToGetAll(true),
    m_firstPass(true),
    m_useCrcTags(true),
    m_tcpClient(0),
    m_hostName(""),
    m_hostPort(0),
    m_tagFilter("*")
{
    m_tagList = new RpSvrTagList();
}


void RpSvrThread::init(QString a_hostName, int a_hostPort, QString a_tagsFilter)
{
    m_hostName = a_hostName;
    m_hostPort = a_hostPort;
    m_tagFilter = a_tagsFilter;
    m_workSatte.set(Connecting);
}


void RpSvrThread::stop(){
    m_workSatte.set(Disconnecting);
}


//===============================================================================
//  Цикл
void RpSvrThread::run()
{
    QTime   cycleTime;
    QString Answer;
    int cycle_count;
    bool c;

    m_tcpClient = new QTcpSocket();
    m_tagsListCorrect = false;
    m_useCrcTags = true;


    cycle_count = 0;
    cycleTime.start();

    forever {

        switch (m_workSatte.get()) {

        case Connecting:
            // если не подключены, подключаемся...
            if (m_tcpClient->state() != QAbstractSocket::ConnectedState) {
                m_tcpClient->connectToHost(m_hostName, m_hostPort, QIODevice::ReadWrite);
                if (!(m_tcpClient->waitForConnected(10000))) {
                    connectionRefused(m_tcpClient->errorString());
                    m_workSatte.set(WaitReconnect);
                    break;
                }
                connectionEstablished();
            }

            if (!tcpGetAnswer(ASW_CONNECT, false, Answer, c))  {
                m_workSatte.set(WaitReconnect);
                break;
            }
            m_tagsListCorrect = false;
            m_firstPass = true;
            m_workSatte.set(Working);
            break;

        case Working:
            if (!tcpIsTagsListCorrect()) { m_workSatte.set(WaitReconnect); break; }
            // запись тегов
            if (!tcpTransferValue()) { m_workSatte.set(WaitReconnect); break; }
            // обновляем значения тегов
            if (!tcpRefreshValue()) { m_workSatte.set(WaitReconnect); break; }
            emit dataChanged();
            m_firstPass = false;
            break;

        case WaitReconnect:
            // отключаемся от сервера...
            m_tcpClient->disconnectFromHost();
            if (m_tcpClient->state() != QAbstractSocket::UnconnectedState)  m_tcpClient->waitForDisconnected(10000);
            if (m_workSatte.stateTime() > 5000) {
                m_workSatte.set(Connecting);
            }
            break;

        case Disconnecting:
            // отключаемся от сервера...
            m_tcpClient->disconnectFromHost();
            if (m_tcpClient->state() != QAbstractSocket::UnconnectedState)  m_tcpClient->waitForDisconnected(10000);
            m_workSatte.set(Stop);
            break;
        }

        if (m_workSatte.get() == Stop) break;

//        qDebug() << ".cycle" << cycle_count << cycleTime.elapsed();

        cycle_count++;

        while (cycleTime.elapsed() < 500) { msleep(1); }
        cycleTime.restart();

    }

    delete m_tcpClient;
}


bool RpSvrThread::tcpIsTagsListCorrect()
{
    if (m_tagsListCorrect)  return true;

    QString Answer;
    QString cmd;
    bool c_ok;
    int i;

    // устанавливаем фильтр
    cmd = QString(CMD_SETFILTER).arg(m_tagFilter);
    if (!tcpDoCommand(ASW_SETFILTER, cmd.toLocal8Bit().data(), Answer)) return false;
    if (Answer!= "OK") return false;

    if (!tcpDoCommand(ASW_CREATETAGLIST, CMD_CREATETAGLIST, Answer)) return false;
    i = Answer.toInt(&c_ok, 16);
    if (!c_ok) return false;
    if (!tagsListCreate(i)) return false;

    if (!tcpGetTagNames()) return false;

    m_tagsListCorrect = true;
    m_needToReadProps = true;
    m_needToGetAll = true;
    m_firstPass = true;
    return true;
}


bool RpSvrThread::tcpGetTagNames()
{
    QString     answer, cmd;
    bool        isAll;

    int tagnum = 0;
    bool result = false;
    while (!result) {
        cmd = QString(CMD_GETTAGLIST).arg(QString::number(tagnum, 16).toUpper());
        m_tcpClient->write(cmd.toLocal8Bit().data());
        if (!tcpGetAnswer(ASW_GETTAGLIST, true, answer, isAll)) break;

        if (!tcpReciveTagNames(answer, tagnum)) break;

        result = isAll;
    }
    return result;
}


bool RpSvrThread::tcpReciveTagNames(QString &answer, int &tagNum) {
    int i, tnum, cnt;
    bool c_Ok;

    // количество тегов
    i = answer.indexOf("#");
    cnt = answer.mid(0, i).toInt(&c_Ok, 16);
    if (!c_Ok) return false;
    answer.remove(0, i+1);

    // номер первого в посылке
    i = answer.indexOf("!");
    tnum = answer.mid(0, i).toInt(&c_Ok, 16);
    if (!c_Ok) return false;
    if (tnum != tagNum) return false;
    answer.remove(0, i+1);


    i = 0;
    while ((i < cnt) && (tnum < m_tagList->count())) {
        m_tagList->at(tnum)->tagName = answer.section(';', i, i);
        tnum++;
        i++;
    }
    tagNum = tnum;

    if (i != cnt) return false;

    return  true;
}


bool RpSvrThread::tcpRefreshValue()
{
    if (m_needToReadProps) {
        m_needToReadProps = !tcpReciveValues(CMD_GETPROPS, ASW_GETPROPS);
    }

    if (m_needToGetAll) {
        return tcpReciveValues(CMD_GETALL, ASW_GETALL);
    } else {
        return tcpReciveValues(CMD_GETCHANGE, ASW_GETCHANGE);
    }
}


bool RpSvrThread::tcpReciveValues(QString getCmd, QString getAnsw)
{
    QString answerStr, crcTags;

    // режим - принимаем значения тегов, а не тип
    bool modeValues = (getCmd == CMD_GETALL) || (getCmd == CMD_GETCHANGE);

    int attempt = 0;
    bool reciveAll = false;
    while (attempt < ATTEMP_MAX) {
        attempt++;

        if (modeValues) {   // фиксируем значения
            m_tcpClient->write(CMD_FIXALL);
            if (!tcpGetAnswer(ASW_FIXALL, false, answerStr, reciveAll)) continue;

            if (m_useCrcTags) {
                m_tcpClient->write(CMD_GETCRC);
                if (tcpGetAnswer(ASW_GETCRC, false, answerStr, reciveAll)) {
                    crcTags = answerStr;
                } else {
                    crcTags = "";
                    if (answerStr.mid(0,3) == "400") {
                        m_useCrcTags = false;
                        tcpGetAnswer(ASW_GETCRC, false, answerStr, reciveAll);
                    }
                }
            }
        }
        reciveAll = tcpReciveValuesEx(getCmd, getAnsw, modeValues);
        if (reciveAll) break;
    }

    if (!reciveAll) return false;

    if (modeValues) m_needToGetAll = false;

    if ((m_useCrcTags) && (modeValues)) {
        QByteArray strCrcH, strCrcL;
        int v;
        for (int i=0; i<m_tagList->count(); i++) {
            v = m_tagList->at(i)->valIn;
            strCrcH.append(v >> 8);
            strCrcL.append(v & 0xFF);
        }
        int crcH = Crc16((unsigned char *)strCrcH.data(), strCrcH.count());
        int crcL = Crc16((unsigned char *)strCrcL.data(), strCrcL.count());

        answerStr = QString::number(crcH, 16)+QString::number(crcL, 16);

        if (crcTags.toUpper() != answerStr.toUpper()) m_needToGetAll = true;
    }

    return true;
}


bool RpSvrThread::tcpReciveValuesEx(QString getCmd, QString getAnsw, bool modeValues)
{
    QString answerStr, s;
    bool reciveAll, c;

    int n, k, k1, k2, v, iv;
    char c1, c2;

    int tNum = 0;
    do {
        s = getCmd + QString::number(tNum, 16).toUpper() + "\r\n";
        m_tcpClient->write(s.toLocal8Bit().data());
        if (!tcpGetAnswer(getAnsw.toLocal8Bit().data(), true, answerStr, reciveAll)) return false;

        k = answerStr.indexOf("#");
        n = answerStr.mid(0, k).toInt(&c, 16);
        if (!c) return false;
        if (n == 0) return true;
        answerStr.remove(0, k);
        answerStr.append(";");

        c2 = ' ';
        k2 = 0;
        for (k=0; k<answerStr.count(); k++) {
            if ((answerStr[k] == '#') || (answerStr[k] == '!') || (answerStr[k] == ';')) {
                c1 = c2;
                k1 = k2;
                c2 = answerStr.at(k).toLatin1();
                k2 = k;

                s = answerStr.mid(k1+1, k2-k1-1);
                iv = 1;
                if (s[0] == '-') {
                    iv = -1;
                    s.remove(0, 1);
                }
                if (s == "") {
                    v = 0;
                } else {
                    v = iv * s.toInt(&c, 16);
                    if (!c) v = 0;
                }

                if ((c1 == '#') && (c2 == '!')) { tNum = v; }

                if (    ((c1 == '!') && (c2 == ';'))
                     || ((c1 == '!') && (c2 == '#'))
                     || ((c1 == ';') && (c2 == '#'))
                     || ((c1 == ';') && (c2 == ';')) ) {
                    if (tNum < m_tagList->count()) {
                        if (modeValues) {
                            m_tagList->at(tNum)->valIn = v;
                            m_tagList->at(tNum)->changed = true;
                        } else {
                            m_tagList->at(tNum)->isIrregular = (v != 0);
                        }
                        tNum++;
                    }
                }
            }
        }
    } while (!reciveAll);


    return true;
}


bool RpSvrThread::tcpTransferValue()
{
    QString s, answer;
    unsigned short crc;
    bool c_ok;

    for (int i=0; i<m_tagList->count(); i++) {

        RpSvrTag *tag = m_tagList->at(i);

        if (tag->needOut) {
            s = QString::number(i, 16).toUpper()
                    + " " + QString::number(tag->valOut, 16).toUpper();
            crc = Crc16((unsigned char*)s.toLocal8Bit().constData(), s.count());
            s = QString(CMD_WNM).arg(s + " " + QString::number(crc, 16).toUpper());

            if (tcpDoCommand(ASW_WNM, s.toLocal8Bit().data(), answer)) {
                if (answer == "!") tag->needOut = false;
            } else {
                return false;
            }
        }

        if ((tag->isIrregular) && (tag->useFlag != UfNone)) {
            s = QString::number(i, 16).toUpper()
                    + " " + QString::number(tag->useFlag, 16).toUpper();
            crc = Crc16((unsigned char*)s.toLocal8Bit().constData(), s.count());
            s = QString(CMD_SETFLAG).arg(s + " " + QString::number(crc, 16).toUpper());
            if (tcpDoCommand(ASW_SETFLAG, s.toLocal8Bit().data(), answer)) {
                if (answer == "!") tag->useFlag = UfNone;
            } else {
                return false;
            }
        }
    }
    return true;
}


bool RpSvrThread::tcpGetAnswer(const char *cmdCode, bool chekCRC, QString &answerStr, bool &reciveAll)
{
    QString s;
    int i, k;
    bool c, reciveComplite;

    // получаем ответ
    s.clear();
    reciveComplite = false;
    while (!reciveComplite) {
        if (!m_tcpClient->waitForReadyRead(1000)) {
            answerStr.clear();           //  ожидаемый ответ не пришёл...
            return false;
        }

        while (m_tcpClient->canReadLine()) {
            k = m_tcpClient->readLine(m_tcpClientBuffer, 1024);
            s += m_tcpClientBuffer;
        }

        i = s.indexOf("\r\n");

        if (i<0) continue; // посылка не окончена...

        while (i >= 0) {
            // ищем нужную строку
            answerStr = s.left(i);
            s.remove(0, i+2);

            // проверяем код ответа (должен быть равен коду команды)
            if (!strcmp(answerStr.section(" ", 0, 0).toLocal8Bit().data(), cmdCode)) {
                reciveComplite = true;
                answerStr.remove(0, answerStr.indexOf(" ")+1);
                break;   // всё приехало...
            }

            i = s.indexOf("\r\n");
        }
    }

    if (!chekCRC) return true;

    // проверяем CRC и признак конца посылки
    // проверка маркеров конца и продолжения посылок
    i = answerStr.indexOf("=");
    if(i<0){
        reciveAll = false;
        i = answerStr.indexOf("~");
    } else {
        reciveAll = true;
    }
    if (i<0) { return false; }

    k=answerStr.mid(i+1).toInt(&c,16);
    if (!c) { return false; }
    answerStr.truncate(i);
    i = Crc16((unsigned char*)answerStr.toLocal8Bit().constData(), answerStr.count());
    if (k != i) {return false; }

    return true;
}


bool RpSvrThread::tcpDoCommand(const char *cmdCode, const char *cmd, QString &answer)
{
    bool c;
    int i = 0;

    while (i < ATTEMP_MAX) {
        m_tcpClient->write(cmd);
        if (tcpGetAnswer(cmdCode, false, answer, c)) return true;
        i++;
    }
    return false;
}



//===============================================================================
//  Теги
bool RpSvrThread::tagsListCreate(int aItemsCount)
{
    RpSvrTag *tag;

    m_tagList->clear();

    while (aItemsCount > 0) {
        tag = new RpSvrTag();
        m_tagList->append(tag);
        aItemsCount--;
    }

    return true;
}


int RpSvrThread::tagCount()
{
    if (!m_tagsListCorrect) return 0;

    return m_tagList->count();
}

int RpSvrThread::getTagIndex(QString &aName)
{
    int i;

    if (!m_tagsListCorrect) return -2;

    for (i=0; i<m_tagList->count(); i++) {
        if (m_tagList->at(i)->tagName.compare(aName, Qt::CaseInsensitive) == 0) return i;
    }

    return -1;
}

int RpSvrThread::getTagValue(QString &aName)
{
    int i;

    i = getTagIndex(aName);

    return getTagValue(i);
}

int RpSvrThread::getTagValue(int aIndex)
{
    if ((!m_tagsListCorrect)
            || (aIndex < 0)
            || (aIndex >= m_tagList->count())) return 0;

    RpSvrTag *tag = m_tagList->at(aIndex);
    if (tag->isIrregular) tag->useFlag = UfInUse;

    return tag->valIn;
}

QString RpSvrThread::getTagName(int aIndex)
{
    if ((!m_tagsListCorrect)
            || (aIndex < 0)
            || (aIndex >= m_tagList->count())) return "";

    return m_tagList->at(aIndex)->tagName;
}


void RpSvrThread::setTagValue(int aIndex, int aValue)
{
    if (!m_tagsListCorrect) return;

    if (aIndex < m_tagList->count()) {
        RpSvrTag *tag = m_tagList->at(aIndex);
        tag->valOut = aValue;
        tag->needOut = true;
        if (tag->isIrregular) tag->useFlag = UfUpdated;
    }
}

bool RpSvrThread::isTagsListCorrect()
{
    return (m_tagsListCorrect && !m_firstPass);
}

void RpSvrThread::startWork()
{
    run();
}


