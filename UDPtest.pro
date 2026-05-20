QT += gui \
    widgets
QT += network xml

CONFIG += c++11 console network
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Plotter.cpp \
        datastorage.cpp \
        guipanel.cpp \
        launcher.cpp \
        main.cpp \
        mylineedit.cpp \
        myudp.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Plotter.h \
    datastorage.h \
    guipanel.h \
    launcher.h \
    mylineedit.h \
    myudp.h
