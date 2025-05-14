// main.cpp
// Main entry point for the Qt application.
// Initializes the QApplication and shows the MainWindow.
// This file is compatible with both Qt5 and Qt6.

#include "mainwindow.h"
#include <QApplication> // Manages GUI application-wide resources

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Create the application object
    MainWindow w;               // Create the main window object
    w.show();                   // Show the main window
    return a.exec();            // Start the application's event loop
}
