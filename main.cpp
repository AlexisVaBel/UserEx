#include <QApplication>
#include <QFontDatabase>
#include "view/mainwindow.hpp"


int main(int argc,char ** argv){
    QApplication app(argc,argv);
    QFontDatabase::addApplicationFont("fonts/digital-7.ttf");
    QFontDatabase::addApplicationFont("fonts/highspeed.ttf");
    QFontDatabase::addApplicationFont("fonts/crystal.ttf");

    MainWindow *wnd=new MainWindow();    
    wnd->setWindowIcon(QIcon(":/img/uexicon2.png"));
    wnd->show();
    return app.exec();
}
