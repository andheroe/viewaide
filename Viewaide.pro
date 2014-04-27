#-------------------------------------------------
#
# Project created by QtCreator 2013-06-04T10:03:53
#
#-------------------------------------------------

QT += core gui
QT += multimedia multimediawidgets
QT += webkitwidgets
QT += widgets

macx {
   QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden
   QMAKE_CXXFLAGS_DEBUG += -fvisibility=hidden
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
   QMAKE_MAC_SDK = macosx10.8
   QMAKE_CFLAGS += -mmacosx-version-min=10.8
   QMAKE_CXXFLAGS += -mmacosx-version-min=10.8
}

TARGET = Viewaide
TEMPLATE = app

######################################## MAC libs

unix {
INCLUDEPATH += ../Viewaide/MAC_opencv/include

LIBS += -L/opt/local/lib \
-lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_objdetect \
}

######################################## WIN libs

win32 {
INCLUDEPATH += ../Viewaide/WIN_opencv/include
LIBS += -L../Viewaide/WIN_opencv\bin \
-llibopencv_core245d \
-llibopencv_highgui245d \
-llibopencv_imgproc245d \
-llibopencv_objdetect245d \

INCLUDEPATH += ../Viewaide/videoInput/Include
LIBS += ../Viewaide/videoInput/libvideoInput.a
LIBS += -L../Viewaide/videoInput \
-lddraw\
-ldxguid\
-lole32\
-loleaut32\
-lstrmbasd\
-lstrmbase\
-lstrmiids\
-luuid
}

SOURCES += main.cpp\
    main_window.cpp \
    camstream.cpp \
    algorithms.cpp \
    mylabel.cpp \
    mywidget.cpp \
    specialthread.cpp \
    downloadfile.cpp \
    updateapp.cpp \
    regandlogin.cpp \
    connectwithserver.cpp

HEADERS  += \
    main_window.h \
    camstream.h \
    algorithms.h \
    mylabel.h \
    mywidget.h \
    specialthread.h \
    downloadfile.h \
    updateapp.h \
    regandlogin.h \
    connectwithserver.h

FORMS    += \
    main_window.ui \
    form.ui \
    regandlogin.ui

RESOURCES += \
    res.qrc

TRANSLATIONS += \
    Viewaide_ru.ts

RC_FILE = icon.rc



