QT       += core gui
QT       += serialport
QT       +=printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#CONFIG+=debug QMAKE_CXXFLAGS+=-fsanitize=address QMAKE_LFLAGS+=-fsanitize=address


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

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Add libatomic for MinGW
#win32: LIBS += -LC:/msys64/mingw64/lib -latomic
#RC_ICONS = myappico.ico
QMAKE_LFLAGS += -static -static-libgcc
