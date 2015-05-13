TEMPLATE = lib
CONFIG += console
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

LIBS += C:/OpenSSL-Win32/lib/MinGW/ssleay32.a

LIBS += C:/OpenSSL-Win32/lib/MinGW/libeay32.a

INCLUDEPATH += C:/OpenSSL-Win32/include


SOURCES += \
    zrtp.cpp \
    stateengine.cpp \
    packethello.cpp \
    packethelloack.cpp \
    packetcommit.cpp \
    packetdhpart.cpp \
    packetconfirm.cpp \
    packetconf2ack.cpp \
    packeterror.cpp \
    packeterrorack.cpp

HEADERS += \
    Integers.h \
    zrtp.h \
    ZrtpCallback.h \
    stateengine.h \
    packet.h \
    packethello.h \
    packethello.h \
    packethelloack.h \
    packetcommit.h \
    packetdhpart.h \
    packetconfirm.h \
    packetconf2ack.h \
    packeterror.h \
    userinfo.h \
    packeterrorack.h

