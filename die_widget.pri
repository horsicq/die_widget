QT       += concurrent scripttools

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialoginfo.ui \
    $$PWD/dialoglog.ui \
    $$PWD/dialogsignatures.ui \
    $$PWD/die_widget.ui

HEADERS += \
    $$PWD/dialoginfo.h \
    $$PWD/dialoglog.h \
    $$PWD/dialogsignatures.h \
    $$PWD/die_highlighter.h \
    $$PWD/die_signatureedit.h \
    $$PWD/die_widget.h

SOURCES += \
    $$PWD/dialoginfo.cpp \
    $$PWD/dialoglog.cpp \
    $$PWD/dialogsignatures.cpp \
    $$PWD/die_highlighter.cpp \
    $$PWD/die_signatureedit.cpp \
    $$PWD/die_widget.cpp

!contains(XCONFIG, die_script) {
    XCONFIG += die_script
    include($$PWD/../die_script/die_script.pri)
}
