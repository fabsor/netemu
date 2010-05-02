TEMPLATE = app
TARGET = netemu_ui
QT += core \
    gui
HEADERS += SettingsWidgets/masterserversettings.h \
    settingsdialog.h \
    mainwindow.h
SOURCES += SettingsWidgets/masterserversettings.cpp \
    settingsdialog.cpp \
    mainwindow.cpp \
    main.cpp
FORMS += SettingsWidgets/masterserversettings.ui \
    settingsdialog.ui \
    mainwindow.ui
RESOURCES += 
INCLUDEPATH = ../netemu_sender/interface/;../netlib/headers
DEFINES += _NIX
LIBS += -L../netemu_sender/Debug/ \
    -lnetemu
LIBS += -L../netlib/Debug/ \
    -lnetlib
