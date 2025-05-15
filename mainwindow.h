// mainwindow.h
// Header file for the MainWindow class.
// Declares the main window's structure, UI elements, and slots.
// This header file is compatible with both Qt5 and Qt6.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>        // Base class for main application windows
#include <QSerialPort>        // For serial port communication
#include <QSerialPortInfo>    // For querying available serial ports
#include <QComboBox>          // Dropdown menu
#include <QPushButton>        // Push button
#include <QPlainTextEdit>     // Text area for displaying data
#include <QVBoxLayout>        // Vertical layout
#include <QHBoxLayout>        // Horizontal layout
#include <QLabel>             // Label for text
#include <QStatusBar>         // Status bar at the bottom of the window
#include <QMessageBox>        // For displaying messages/errors
#include <QScrollBar>         // For accessing the scrollbar of the text area
// Forward declaration of UI elements to avoid including their headers in the .h if only pointers are needed
// However, for direct member objects, includes are necessary as above.

class MainWindow : public QMainWindow
{
    Q_OBJECT // Macro for classes that use signals and slots

public:
    MainWindow(QWidget *parent = nullptr); // Constructor
    ~MainWindow();                         // Destructor

private slots:
    // Slots to handle UI interactions and serial port events
    void onConnectButtonClicked();      // Called when the connect/disconnect button is clicked
    void onRefreshPortsButtonClicked(); // Called when the refresh ports button is clicked
    void readSerialData();              // Called when data is available to be read from the serial port
    void handleSerialError(QSerialPort::SerialPortError error); // Called when a serial port error occurs

private:
    // Helper methods
    void populateSerialPorts();     // Fills the serial port combo box with available ports
    void populateBaudRates();       // Fills the baud rate combo box
    void setUiControlsEnabled(bool enabled); // Enables/disables UI controls during connection/disconnection

    // UI Elements
    QWidget *centralWidget;         // Central widget to hold the layout
    QVBoxLayout *mainLayout;        // Main vertical layout
    QHBoxLayout *controlsLayout;    // Horizontal layout for port and baud selection
    QComboBox *serialPortComboBox;  // Dropdown for selecting the serial port
    QComboBox *baudRateComboBox;    // Dropdown for selecting the baud rate
    QPushButton *connectButton;     // Button to connect/disconnect
    QPushButton *refreshPortsButton;// Button to refresh the list of serial ports
    QPlainTextEdit *dataDisplayArea;// Text area to display received data
    QLabel *statusLabel;            // Label in the status bar

    // Serial Port Object
    QSerialPort *serialPort;        // The serial port object for communication

    bool isConnected;               // Flag to track connection status
};

#endif // MAINWINDOW_H
