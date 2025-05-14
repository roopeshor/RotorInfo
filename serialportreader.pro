# serialportreader.pro
# This is the Qt project file.
# It tells qmake how to build your application.
# This .pro file is compatible with both Qt5 and Qt6.

QT       += core gui widgets serialport # Include core, gui, widgets, and serialport modules

TARGET = SerialPortReader # The name of the executable
TEMPLATE = app # Specifies that this is an application template

SOURCES += \
    main.cpp \
    mainwindow.cpp # Source files

HEADERS += \
    mainwindow.h # Header files

# Define the root build directory
BUILD_DIR = build

# Directory for object files (.o or .obj)
OBJECTS_DIR = $$BUILD_DIR/objects

# Directory for Meta Object Compiler (moc) generated files
MOC_DIR = $$BUILD_DIR/moc

# Directory for User Interface Compiler (uic) generated files
UI_DIR = $$BUILD_DIR/ui

# Directory for Qt Resource Compiler (rcc) generated files
RCC_DIR = $$BUILD_DIR/rcc

DESTDIR = $$BUILD_DIR/bin

# Default rules for deployment (can be adjusted as needed).
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $${BUILD_DIR}/linux
!isEmpty(target.path): INSTALLS += target