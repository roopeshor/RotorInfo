cmake_minimum_required(VERSION 3.16)

project(rotorinfo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)
qt_standard_project_setup()

file(GLOB UI_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.ui")
qt6_wrap_ui(UI_HEADERS ${UI_FILES})

qt_add_executable(rotorinfo
	src/mainwindow.h
	src/mainwindow.cpp
	src/main.cpp
	${UI_HEADERS}
)

target_link_libraries(rotorinfo PRIVATE Qt6::Widgets Qt6::SerialPort)

set_target_properties(rotorinfo PROPERTIES
	WIN32_EXECUTABLE ON
	MACOSX_BUNDLE ON
)
if (WIN32)
  set_target_properties(rotorinfo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/win"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/win"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/win"
  )
else()
  set_target_properties(rotorinfo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/linux"
  )
endif()