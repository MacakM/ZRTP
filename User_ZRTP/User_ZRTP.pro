#-------------------------------------------------
#
# Project created by QtCreator 2015-02-13T10:24:19
#
#-------------------------------------------------

QT       += core
QT       += network

QT       -= gui

TARGET = User_ZRTP
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH +=  ../ZRTP

LIBS += ../build-ZRTP-Desktop_Qt_5_1_1_MinGW_32bit-Debug/debug/libZRTP.a

LIBS += C:/OpenSSL-Win32/lib/MinGW/ssleay32.a

LIBS += C:/OpenSSL-Win32/lib/MinGW/libeay32.a

INCLUDEPATH += C:/OpenSSL-Win32/include

SOURCES += main.cpp \
    networkmanager.cpp \
    mycallbacks.cpp \
    parser.cpp \
    zrtptimeout.cpp \
    zrtpmessage.cpp \
    restarter.cpp

HEADERS += \
    networkmanager.h \
    mycallbacks.h \
    parser.h \
    zrtptimeout.h \
    zrtpmessage.h \
    restarter.h

