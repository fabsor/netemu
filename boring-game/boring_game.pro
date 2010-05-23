TEMPLATE = app
TARGET = boring_game 
INCLUDEPATH += ../netemu_sender/interface/;../netlib/headers

QT        += core gui opengl 

HEADERS   += boring_game.h
SOURCES   += main.cpp \
    boring_game.cpp
FORMS     += boring_game.ui    
RESOURCES +=
unix { 
    DEFINES += _NIX
    LIBS += -L../netemu_sender/Debug/ \
        -lnetemu
    LIBS += -L../netlib/Debug/ \
        -lnetlib
}