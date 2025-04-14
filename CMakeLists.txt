cmake_minimum_required(VERSION 3.5)

project(SparkPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core LinguistTools Multimedia X11Extras)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core LinguistTools Multimedia X11Extras)
find_package(Dtk REQUIRED COMPONENTS Widget Core)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ffmpeg REQUIRED IMPORTED_TARGET libavcodec libavformat libavfilter libavutil libswscale sdl2 SDL2_mixer x11)

find_package(CURL REQUIRED)

set(TS_FILES SparkPlayer_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        sparkplayer.cpp
        sparkplayer.h
        sparkplayer.ui

        SparkMediaControler.h
        SparkMediaControler.cpp
        SparkSettings.h
        SparkSettings.cpp
        SparkAIAPI.h
        SparkAIAPI.cpp
        SparkGlobalHotkey.h
        SparkGlobalHotkey.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SparkPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SparkPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(SparkPlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SparkPlayer
            ${PROJECT_SOURCES}
            include.h
            widgets/TimeLine.h widgets/TimeLine.cpp
            widgets/MainPage.h widgets/MainPage.cpp
            widgets/FlowLayout.h widgets/FlowLayout.cpp
            widgets/ImageBox.h widgets/ImageBox.cpp
            widgets/TitleBar.h widgets/TitleBar.cpp
            widgets/MediaListButton.h widgets/MediaListButton.cpp
            widgets/MediaBox.h widgets/MediaBox.cpp
            widgets/VideoBox.h widgets/VideoBox.cpp
            widgets/VolumeBox.h widgets/VolumeBox.cpp
            widgets/PlayListBox.h widgets/PlayListBox.cpp
            widgets/PlayListButton.h widgets/PlayListButton.cpp
            widgets/SpeedBox.h widgets/SpeedBox.cpp
            utils/ImageTools.h utils/ImageTools.cpp
            utils/Path.h utils/Path.cpp
            utils/Codec.h utils/Codec.cpp
            utils/qhotkey.h utils/qhotkey.cpp utils/qhotkey_p.h utils/qhotkey_x11.cpp
            
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(SparkPlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::X11Extras
    Dtk::Widget
    Dtk::Core
    PkgConfig::ffmpeg
    CURL::libcurl
)
target_link_options(SparkPlayer PRIVATE -Wl,-rpath,"$$ORIGIN/lib")
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER fun.sakuraspark.SparkPlayer)
endif()
set_target_properties(SparkPlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SparkPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# 警用debug
add_definitions(-DQT_NO_DEBUG_OUTPUT)
# 新增：构建后复制images文件夹到build目录(可执行文件所在目录)
add_custom_command(TARGET SparkPlayer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:SparkPlayer>/images
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SparkPlayer)
endif()
