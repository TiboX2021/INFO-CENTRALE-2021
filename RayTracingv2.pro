TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        render.cpp \
        render_base.cpp

HEADERS += \
    render.h \
    render_base.h

INCLUDEPATH += "C:\eigen-3.3.9"
INCLUDEPATH += "C:\bitmap"
