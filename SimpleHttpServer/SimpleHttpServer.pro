TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
#CONFIG += qt core network
QT += network core
SOURCES += \
        HttpServer.cpp \
        main.cpp

HEADERS += \
    HttpServer.h
