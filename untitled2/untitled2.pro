QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    untitled.h \
    untitled_global.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DEPENDPATH += $$PWD/.
#LIBS += /Users/yinxiangkai/Desktop/Study/QTVideo/untitled2/lib/libuntitled.dylib

#macx: LIBS += -LPWD/lib/ -llibuntitled.1
#INCLUDEPATH += $${DYLD_FRAMEWORK_PATH}/ \-llibuntitled.1
#DEPENDPATH += $$PWD/.

#LIBS += -L $${DYLD_FRAMEWORK_PATH}/lib/ -llibuntitled.1
    #$$PWD获取当前pro文件的目录
#INCLUDEPATH += -L$$PWD/
#LIBS += -L $${FFMPEG_HOME}/lib \
#        -lavcodec \
#        -lavutil
