include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../die_script/die_script.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogtextinfo.cmake)

set(DIE_WIDGET_SOURCES
    ${DIE_SCRIPT_SOURCES}
    ${DIALOGTEXTINFO_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescandirectory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescandirectory.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescanprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdiescanprocess.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialoglog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoglog.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogsignatures.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogsignatures.ui
    ${CMAKE_CURRENT_LIST_DIR}/die_highlighter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_signatureedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_widget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/die_widget.ui
)
