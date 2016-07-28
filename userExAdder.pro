QT          +=  core sql widgets network
TARGET      =   userExAdder
TEMPLATE    =   app
CONFIG      +=  windows
CONFIG      +=  static

#QMAKE_CXXFLAGS+=-DIBPP_WINDOWS
#QMAKE_CXXFLAGS+=-D_WIN32

INCLUDEPATH+=D:\tools\Firebird\include
LIBS+=-LD:\tools\Firebird\lib -lfbclient_ms -lib_util_ms

SOURCES += \
    main.cpp \
    data/dbfirebird.cpp \
    data/dbchecktbl.cpp \
    view/mainwindow.cpp \
    func4all.cpp \
    lib/crc16.cpp \
    tcp/rpsvrthread.cpp \
    cntr/psconttroller.cpp \
    cntr/userdbcontroller.cpp

HEADERS += \
    data/dbchecktbl.hpp \
    view/mainwindow.hpp \
    data/dbfirebird.hpp \
    func4all.h \
    lib/crc16.h \
    tcp/rpsvrthread.h \
    cntr/psconttroller.hpp \
    cntr/userdbcontroller.hpp

FORMS += \
    view/mainwindow.ui

RESOURCES += \
    rsc.qrc

RC_FILE =userexa.rc
