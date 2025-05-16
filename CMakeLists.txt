<<<<<<< HEAD
cmake_minimum_required(VERSION 3.16)

project(RotorSpeed VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RotorSpeed
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RotorSpeed APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RotorSpeed SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RotorSpeed
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(RotorSpeed PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::SerialPort)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RotorSpeed)
endif()
set_target_properties(RotorSpeed PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RotorSpeed
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RotorSpeed)
endif()
=======
cmake_minimum_required(VERSION 3.16)
project(RotorSpeed VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Gui SerialPort Widgets)

qt_standard_project_setup()

qt_add_executable(RotorSpeed
    main.cpp
    mainwindow.cpp mainwindow.h
)
target_link_libraries(RotorSpeed PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
)

include_directories(.)

install(TARGETS RotorSpeed
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider platform-specific deployment scripts if needed
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin") # macOS
    qt_generate_deploy_app_script(
        TARGET RotorSpeed
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    # You might want to add specific deployment steps for Linux here
    # For example, creating a .desktop file and copying necessary libraries
    message(STATUS "Considering Linux-specific deployment steps...")
endif()
>>>>>>> 730723f6615a1e96558d4bc0d76e0281cce1bb47
