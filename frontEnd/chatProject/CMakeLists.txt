cmake_minimum_required(VERSION 3.5)

project(chatProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chatProject
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res.qrc
        logindialog.h logindialog.cpp logindialog.ui
        registerdialog.h registerdialog.cpp registerdialog.ui
        global.h global.cpp
        singleton.h
        httpmgr.h httpmgr.cpp
        config.ini
        timerbtn.h timerbtn.cpp
        clickedlabel.h clickedlabel.cpp
        resetdialog.h resetdialog.cpp resetdialog.ui
        tcpmgr.h tcpmgr.cpp
        usermgr.h usermgr.cpp
        chatdialog.h chatdialog.cpp chatdialog.ui
        clickedbtn.h clickedbtn.cpp
        customizeedit.h customizeedit.cpp
        chatuserlist.h chatuserlist.cpp
        chatuserwidget.h chatuserwidget.cpp chatuserwidget.ui
        listitembase.h listitembase.cpp
        loadingdialog.h loadingdialog.cpp loadingdialog.ui
        chatpage.h chatpage.cpp chatpage.ui
        chatview.h chatview.cpp
        chatitembase.h chatitembase.cpp
        bubbleframe.h bubbleframe.cpp
        textbubble.h textbubble.cpp
        picturebubble.h picturebubble.cpp
        messagetextedit.h messagetextedit.cpp
        statewidget.h statewidget.cpp
        searchlist.h searchlist.cpp
        userdata.h userdata.cpp
        adduseritem.h adduseritem.cpp adduseritem.ui


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET chatProject APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(chatProject SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(chatProject
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(chatProject PRIVATE Qt${QT_VERSION_MAJOR}::Widgets  Qt${QT_VERSION_MAJOR}::Network)


# 添加自定义命令，将 config.ini 复制到构建目录
configure_file(config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.chatProject)
endif()
set_target_properties(chatProject PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS chatProject
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chatProject)
endif()
