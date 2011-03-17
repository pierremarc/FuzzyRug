#-------------------------------------------------
#
# Project created by QtCreator 2011-02-27T11:03:26
#
#-------------------------------------------------

QT       += core gui

TARGET = FuzzyRug
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    threadpattern.cpp \
    rugitem.cpp \
    threadpatterncollection.cpp \
    fuzzysaver.cpp \
    fuzzyloader.cpp \
    rugprinter.cpp \
    printrangpicker.cpp \
    patternimporter.cpp

HEADERS  += mainwindow.h \
    threadpattern.h \
    rugitem.h \
    threadpatterncollection.h \
    fuzzysaver.h \
    fuzzyloader.h \
    rugprinter.h \
    printrangpicker.h \
    patternimporter.h

FORMS    += mainwindow.ui \
    printrangpicker.ui
