include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED DIE_SCRIPT_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../die_script/die_script.cmake)
    set(DIE_WIDGET_SOURCES ${DIE_WIDGET_SOURCES} ${DIE_SCRIPT_SOURCES})
endif()
if (NOT DEFINED DIALOGTEXTINFO_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogtextinfo.cmake)
    set(DIE_WIDGET_SOURCES ${DIE_WIDGET_SOURCES} ${DIALOGTEXTINFO_SOURCES})
endif()
if (NOT DEFINED XDIALOGPROCESS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/xdialogprocess.cmake)
    set(DIE_WIDGET_SOURCES ${DIE_WIDGET_SOURCES} ${XDIALOGPROCESS_SOURCES})
endif()
if (NOT DEFINED DIALOGFINDTEXT_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogfindtext.cmake)
    set(DIE_WIDGET_SOURCES ${DIE_WIDGET_SOURCES} ${DIALOGFINDTEXT_SOURCES})
endif()
if (NOT DEFINED XCOMBOBOXEX_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xcomboboxex.cmake)
    set(DIE_WIDGET_SOURCES ${DIE_WIDGET_SOURCES} ${XCOMBOBOXEX_SOURCES})
endif()

set(DIE_WIDGET_SOURCES
    ${DIE_WIDGET_SOURCES}
    ${DIE_SCRIPT_SOURCES}
    ${DIALOGTEXTINFO_SOURCES}
    ${XDIALOGPROCESS_SOURCES}
    ${DIALOGFINDTEXT_SOURCES}
    ${XCOMBOBOXEX_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiehexviewer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiehexviewer.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiehexviewer.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescandirectory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescandirectory.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescandirectory.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescanprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescanprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiesignatures.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiesignatures.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiesignatures.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiesignatureselapsed.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiesignatureselapsed.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiesignatureselapsed.ui
    ${CMAKE_CURRENT_LIST_DIR}/die_highlighter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_highlighter.h
    ${CMAKE_CURRENT_LIST_DIR}/die_signatureedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_signatureedit.h
    ${CMAKE_CURRENT_LIST_DIR}/die_widget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_widget.h
    ${CMAKE_CURRENT_LIST_DIR}/die_widget.ui
    ${CMAKE_CURRENT_LIST_DIR}/dieoptionswidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dieoptionswidget.h
    ${CMAKE_CURRENT_LIST_DIR}/dieoptionswidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/diewidgetadvanced.cpp
    ${CMAKE_CURRENT_LIST_DIR}/diewidgetadvanced.h
    ${CMAKE_CURRENT_LIST_DIR}/diewidgetadvanced.ui
)
