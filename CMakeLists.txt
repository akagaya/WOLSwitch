cmake_minimum_required(VERSION 3.21)

project(WOLSwitch VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
qt_standard_project_setup()
set(CMAKE_AUTOUIC ON)

# GUI に依存しない部分 (テスト対象)
add_library(wolcore STATIC
    src/wol.cpp
    src/wol.h
    src/hostlist.cpp
    src/hostlist.h
)
target_include_directories(wolcore PUBLIC src)
target_link_libraries(wolcore PUBLIC Qt6::Core)

qt_add_executable(WOLSwitch WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
)
target_link_libraries(WOLSwitch PRIVATE wolcore Qt6::Widgets Qt6::Network)
set_target_properties(WOLSwitch PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME WOLSwitch
    MACOSX_BUNDLE_GUI_IDENTIFIER io.github.akagaya.WOLSwitch
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    qt_add_executable(tst_wolcore tests/tst_wolcore.cpp)
    target_link_libraries(tst_wolcore PRIVATE wolcore Qt6::Test)
    add_test(NAME tst_wolcore COMMAND tst_wolcore)
endif()
