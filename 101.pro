QT       += core gui serialport printsupport sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 static

SOURCES += \
    applicationconfiguration.cpp \
    deviceinfostorage.cpp \
    dialog.cpp \
    exportcsv.cpp \
    exportdatafrombytes.cpp \
    exportpdf.cpp \
    grapthics.cpp \
    journal.cpp \
    logging.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    serialportmanager.cpp \
    users.cpp

HEADERS += \
    applicationconfiguration.h \
    deviceinfostorage.h \
    dialog.h \
    exportcsv.h \
    exportdatafrombytes.h \
    exportpdf.h \
    grapthics.h \
    ini.h \
    journal.h \
    logging.h \
    mainwindow.h \
    qcustomplot.h \
    serialportmanager.h \
    users.h

FORMS += \
    dialog.ui \
    grapthics.ui \
    mainwindow.ui \
    users.ui

RESOURCES += icons.qrc
RC_FILE += icon.rc

QMAKE_LFLAGS += -static #-static-libgcc -static-libstdc++

LIBS += -LC:/Qt/5.12.12/static/lib \
        -lQt5Core \
        -lQt5Gui \
        -lQt5Widgets \
        -lQt5SerialPort \
        -lqtharfbuzz \
        -lz \
        -lpcre2-16 \
        -ldouble-conversion \
       -ljpeg \
        -lpng16




