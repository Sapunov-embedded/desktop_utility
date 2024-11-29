QT       += core gui serialport printsupport sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17 #static

#CONFIG-=static


SOURCES += \
    applicationconfiguration.cpp \
    deviceinfostorage.cpp \
    dialog.cpp \
    exportcsv.cpp \
    exportdatafrombytes.cpp \
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
    globaldefines.h \
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






