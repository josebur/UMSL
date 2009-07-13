# -------------------------------------------------
# Project created by QtCreator 2009-06-12T19:13:25
# -------------------------------------------------
QT += sql
TARGET = UMSL
TEMPLATE = app
LIBS += -lphidget21
SOURCES += main.cpp \
    mainwindow.cpp \
    scene.cpp \
    study.cpp \
    studysceneeditor.cpp \
    studyscenedelegate.cpp \
    studyscenemodel.cpp \
    studylistmodel.cpp \
    studytimeline.cpp \
    phidget.cpp
HEADERS += mainwindow.h \
    scene.h \
    study.h \
    queries.h \
    studysceneeditor.h \
    studyscenedelegate.h \
    studyscenemodel.h \
    studylistmodel.h \
    studytimeline.h \
    phidget.h
FORMS += mainwindow.ui \
    studysceneeditor.ui
RESOURCES += usml.qrc
