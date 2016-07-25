#ifndef FUNC4ALL_H
#define FUNC4ALL_H

#include <QtGui>


void moveToCenterScreen(QWidget *wd, int sizePercent=0);
void moveToCenterOfWidget(QWidget *wd, QWidget *parent, int sizePercent=0);

QPixmap loadPixmapWithMask(QString name);

QColor intToQColor (int A);
QString inttostr (int A);
void ShowMessage (QString Text, QString WindowTitle);
QString QColortohex (QColor A);
double DateTime2double (QDateTime A);
QString doubleTime2string (double A);



#endif // FUNC4ALL_H
