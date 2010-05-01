TEMPLATE = app
TARGET = netemu_ui
QT += core \
    gui
HEADERS += PreferencesWidgets/masterserverpreferences.h \
    preferencesdialog.h \
    mainwindow.h
SOURCES += PreferencesWidgets/masterserverpreferences.cpp \
    preferencesdialog.cpp \
    mainwindow.cpp \
    main.cpp
FORMS += PreferencesWidgets/masterserverpreferences.ui \
    preferencesdialog.ui \
    mainwindow.ui
RESOURCES += 
INCLUDEPATH = ../netemu_sender/interface/;../netlib/headers
DEFINES += _NIX
LIBS += -L../netemu_sender/Debug/ \
    -lnetemu
LIBS += -L../netlib/Debug/ \
    -lnetlib
