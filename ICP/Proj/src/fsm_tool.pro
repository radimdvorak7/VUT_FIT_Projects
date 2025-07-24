QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fsm_tool
TEMPLATE = app

CONFIG += c++17
CONFIG += sdk_no_version_check

SOURCES += \
    automaton_model.cpp \
    main.cpp \
    main_window.cpp \
    state.cpp \
    transition.cpp

HEADERS += \
    automaton_model.h \
    main_window.h \
    state.h \
    transition.h

FORMS += \
    main_window.ui

# Deployment settings
unix:!macx {
    target.path = /usr/local/bin
    INSTALLS += target
}

win32 {
    RC_ICONS = app.ico
}
