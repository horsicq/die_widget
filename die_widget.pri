QT       += concurrent
lessThan(QT_MAJOR_VERSION, 6): QT += scripttools

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialogdiescandirectory.ui \
    $$PWD/dialogdiescanprocess.ui \
    $$PWD/dialogelapsed.ui \
    $$PWD/dialoglog.ui \
    $$PWD/dialogsignatures.ui \
    $$PWD/die_widget.ui \
    $$PWD/dieoptionswidget.ui

HEADERS += \
    $$PWD/dialogdiescandirectory.h \
    $$PWD/dialogdiescanprocess.h \
    $$PWD/dialogelapsed.h \
    $$PWD/dialoglog.h \
    $$PWD/dialogsignatures.h \
    $$PWD/die_highlighter.h \
    $$PWD/die_signatureedit.h \
    $$PWD/die_widget.h \
    $$PWD/dieoptionswidget.h

SOURCES += \
    $$PWD/dialogdiescandirectory.cpp \
    $$PWD/dialogdiescanprocess.cpp \
    $$PWD/dialogelapsed.cpp \
    $$PWD/dialoglog.cpp \
    $$PWD/dialogsignatures.cpp \
    $$PWD/die_highlighter.cpp \
    $$PWD/die_signatureedit.cpp \
    $$PWD/die_widget.cpp \
    $$PWD/dieoptionswidget.cpp

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

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/die_widget.cmake
