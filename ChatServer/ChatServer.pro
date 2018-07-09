TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpserver.cpp \
    utils.cpp \
    chatserver.cpp

LIBS += -lpthread

HEADERS += \
    logger.h \
    tcpserverif.h \
    tcpserver.h \
    eventdata.h \
    utils.h \
    tcpobserverif.h \
    chatserver.h \
    chatprotocol.h
