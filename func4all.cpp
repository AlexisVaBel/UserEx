#include "func4all.h"
#include "QMessageBox"
#include "QString"
#include "QColor"
#include "QDebug"

#include <QtWidgets>


void ShowMessage (QString Text, QString WindowTitle="Message")
{
    QMessageBox* msgBox;
    msgBox = new QMessageBox();
    msgBox->setWindowTitle(WindowTitle);
    msgBox->setText(Text);
    msgBox->show();
}

QString inttostr (int A)
{
    QString B;
    B.setNum(A, 10);
    return B;
}


double DateTime2double (QDateTime A)
{
    return A.toTime_t();
}

QDateTime double2DateTime (double A)
{
    return QDateTime::fromTime_t(floor(A));
}

QString doubleTime2string (double A)
{
    return (QDateTime::fromTime_t(floor(A))).toString("yy-MM-dd hh:mm:ss");
}




QColor intToQColor (int A)
{
    QColor B;
    int r1,r2,r3;

    r1 = A % 256;
    A = int (A / 256);

    r2 = A % 256;
    A = int (A / 256);

    r3 = A % 256;

    B.setRgb(r1, r2, r3);
    return B;
}

QString QColortohex (QColor A)
{
    QString haxadecimal;

    uint decimal = A.red()*256*256
                 + A.green()*256
                 + A.blue();

    haxadecimal.setNum(decimal, 16);

    while ( haxadecimal.count() < 6 )
    {
        haxadecimal = "0" + haxadecimal;
    };
    return haxadecimal;
}


void moveToCenterScreen(QWidget *wd, int sizePercent)
{
    QDesktopWidget *dw = QApplication::desktop();

    QRect sr = dw->availableGeometry();

    QRect fr = wd->frameGeometry();
    if (sizePercent > 0) {
        fr.setSize(sr.size() * sizePercent / 100);
    }
    fr.moveCenter(QPoint(sr.width()/2, sr.height()/2));
    wd->setGeometry(fr);

}

void moveToCenterOfWidget(QWidget *wd, QWidget *parent, int sizePercent)
{
    if ((!wd) || (!parent)) return;

    QRect sr = parent->frameGeometry();
    QRect fr = wd->frameGeometry();

    if (sizePercent > 0) {
        fr.setSize(sr.size() * sizePercent / 100);
    }
    fr.moveCenter(QPoint(sr.width()/2 + sr.left(),
                         sr.height()/2 + sr.top()));
    wd->setGeometry(fr);
}

QPixmap loadPixmapWithMask(QString name)
{
    QPixmap img;

    img.load(name);
    img.setMask(img.createHeuristicMask());

    return img;
}








