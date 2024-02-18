QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainview.cpp \
    mainwindow.cpp \
    technicianview.cpp

HEADERS += \
    confighelper.h \
    goodosc.h \
    mainview.h \
    mainwindow.h \
    technicianview.h

FORMS += \
    mainwindow.ui \
    technicianview.ui


win32:INCLUDEPATH += \
    "C:/Program Files/NDI/NDI 5 SDK/Include"

win32:LIBS += \
    "C:/Program Files/NDI/NDI 5 SDK/Lib/x64/Processing.NDI.Lib.x64.lib" \
    "C:/Program Files/NDI/NDI 5 SDK/Bin/x64/Processing.NDI.Lib.x64.dll"

linux:INCLUDEPATH += \
    "/home/pi/NDI SDK for Linux/include"

linux:LIBS += \
    "/home/pi/NDI SDK for Linux/lib/aarch64-rpi4-linux-gnueabi/libndi.so" \
    "/usr/local/lib/libpigpio.so"



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../../Program Files/NDI/NDI 5 SDK/Lib/release/' -lProcessing.NDI.Lib.x64
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../../Program Files/NDI/NDI 5 SDK/Lib/debug/' -lProcessing.NDI.Lib.x64

