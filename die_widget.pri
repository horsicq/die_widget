QT       += concurrent
lessThan(QT_MAJOR_VERSION, 6): QT += scripttools

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogdiehexviewer.h \
    $$PWD/dialogdiescandirectory.h \
    $$PWD/dialogdiescanprocess.h \
    $$PWD/dialogdiesignatures.h \
    $$PWD/dialogdiesignatureselapsed.h \
    $$PWD/die_highlighter.h \
    $$PWD/die_signatureedit.h \
    $$PWD/die_widget.h \
    $$PWD/dieoptionswidget.h \
    $$PWD/diewidgetadvanced.h

SOURCES += \
    $$PWD/dialogdiehexviewer.cpp \
    $$PWD/dialogdiescandirectory.cpp \
    $$PWD/dialogdiescanprocess.cpp \
    $$PWD/dialogdiesignatures.cpp \
    $$PWD/dialogdiesignatureselapsed.cpp \
    $$PWD/die_highlighter.cpp \
    $$PWD/die_signatureedit.cpp \
    $$PWD/die_widget.cpp \
    $$PWD/dieoptionswidget.cpp \
    $$PWD/diewidgetadvanced.cpp

FORMS += \
    $$PWD/dialogdiehexviewer.ui \
    $$PWD/dialogdiescandirectory.ui \
    $$PWD/dialogdiesignatures.ui \
    $$PWD/dialogdiesignatureselapsed.ui \
    $$PWD/die_widget.ui \
    $$PWD/dieoptionswidget.ui \
    $$PWD/diewidgetadvanced.ui

!contains(XCONFIG, die_script) {
    XCONFIG += die_script
    include($$PWD/../die_script/die_script.pri)
}

!contains(XCONFIG, dialogtextinfo) {
    XCONFIG += dialogtextinfo
    include($$PWD/../FormatDialogs/dialogtextinfo.pri)
}

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/../FormatDialogs/xdialogprocess.pri)
}

!contains(XCONFIG, dialogfindtext) {
    XCONFIG += dialogfindtext
    include($$PWD/../FormatDialogs/dialogfindtext.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/die_widget.cmake
