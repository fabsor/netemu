TEMPLATE = app
TARGET = netemu_ui
QT += core \
    gui
HEADERS += Dialogs/connectdialog.h \
    Dialogs/settingsdialog.h \
    SettingsWidgets/masterserversettings.h \
    mainwindow.h
SOURCES += Dialogs/connectdialog.cpp \
    Dialogs/settingsdialog.cpp \
    SettingsWidgets/masterserversettings.cpp \
    mainwindow.cpp \
    main.cpp
FORMS += Dialogs/connectdialog.ui \
    Dialogs/settingsdialog.ui \
    SettingsWidgets/masterserversettings.ui \
    mainwindow.ui
RESOURCES += 
INCLUDEPATH = ../netemu_sender/interface/;../netlib/headers
DEFINES += _NIX
LIBS += -L../netemu_sender/Debug/ \
    -lnetemu
LIBS += -L../netlib/Debug/ \
    -lnetlib
