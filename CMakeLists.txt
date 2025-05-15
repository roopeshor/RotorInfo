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