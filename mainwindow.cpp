// mainwindow.cpp
// Implementation of the MainWindow class.
// Contains the logic for UI setup, serial port handling, and event responses.
// This implementation is compatible with both Qt5 and Qt6.

#include "mainwindow.h"
#include <QDebug> // For debugging output

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isConnected(false) // Initialize base class and isConnected member
{
    // --- Initialize UI Elements ---
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget); // Set the central widget for the main window

    mainLayout = new QVBoxLayout(centralWidget); // Create the main layout

    // Controls Layout (for port selection, baud rate, and buttons)
    controlsLayout = new QHBoxLayout();

    serialPortComboBox = new QComboBox(this);
    controlsLayout->addWidget(new QLabel("Port:", this)); // Label for port combo box
    controlsLayout->addWidget(serialPortComboBox);

    baudRateComboBox = new QComboBox(this);
    controlsLayout->addWidget(new QLabel("Baud Rate:", this)); // Label for baud rate combo box
    controlsLayout->addWidget(baudRateComboBox);

    refreshPortsButton = new QPushButton("Refresh Ports", this);
    controlsLayout->addWidget(refreshPortsButton);

    connectButton = new QPushButton("Connect", this);
    controlsLayout->addWidget(connectButton);

    mainLayout->addLayout(controlsLayout); // Add controls layout to the main layout

    // Data Display Area
    dataDisplayArea = new QPlainTextEdit(this);
    dataDisplayArea->setReadOnly(true); // Make the text area read-only
    mainLayout->addWidget(dataDisplayArea);

    // Status Bar
    statusLabel = new QLabel("Disconnected", this);
    statusBar()->addWidget(statusLabel); // Add label to the status bar

    // --- Initialize Serial Port ---
    serialPort = new QSerialPort(this);

    // --- Populate UI Elements ---
    populateSerialPorts();
    populateBaudRates();

    // --- Connect Signals and Slots ---
    // UI Signals
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(refreshPortsButton, &QPushButton::clicked, this, &MainWindow::onRefreshPortsButtonClicked);

    // Serial Port Signals
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
    // The QSerialPort::errorOccurred signal signature is the same in Qt5 and Qt6
    connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
    // Alternatively, for Qt5, the modern syntax can also be used if your compiler supports it,
    // but the SIGNAL/SLOT macro version is guaranteed to work with all Qt5 setups.
    // connect(serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleSerialError); // Modern syntax also works in Qt5


    // --- Window Properties ---
    setWindowTitle("Qt5 Serial Port Reader"); // Updated title for clarity
    resize(600, 400); // Set initial window size
}

MainWindow::~MainWindow()
{
    // Destructor: Clean up resources
    if (serialPort->isOpen()) {
        serialPort->close(); // Ensure serial port is closed
    }
    // Qt's parent-child ownership system will handle deletion of UI elements
    // that were created with 'this' as parent.
}

// Populates the serial port combo box with available serial ports.
void MainWindow::populateSerialPorts()
{
    serialPortComboBox->clear(); // Clear existing items
    const auto infos = QSerialPortInfo::availablePorts(); // Get list of available ports
    if (infos.isEmpty()) {
        serialPortComboBox->addItem("No ports found");
        serialPortComboBox->setEnabled(false); // Disable if no ports
        connectButton->setEnabled(false);
    } else {
        for (const QSerialPortInfo &info : infos) {
            serialPortComboBox->addItem(info.portName(), QVariant::fromValue(info.systemLocation())); // Add port name and system location as user data
        }
        serialPortComboBox->setEnabled(true);
        connectButton->setEnabled(true);
    }
}

// Populates the baud rate combo box with common baud rates.
void MainWindow::populateBaudRates()
{
    baudRateComboBox->clear();
    // Add common baud rates
    QList<qint32> baudRates = {
        QSerialPort::Baud1200, QSerialPort::Baud2400, QSerialPort::Baud4800,
        QSerialPort::Baud9600, QSerialPort::Baud19200, QSerialPort::Baud38400,
        QSerialPort::Baud57600, QSerialPort::Baud115200
    };

    for (qint32 baud : baudRates) {
        baudRateComboBox->addItem(QString::number(baud), QVariant::fromValue(baud));
    }
    // In Qt5, setCurrentIndex might be more reliable if you want to ensure the first item corresponding to 9600 is selected.
    // Or find the index of "9600" and set it.
    int indexOf9600 = baudRateComboBox->findText("9600");
    if (indexOf9600 != -1) {
        baudRateComboBox->setCurrentIndex(indexOf9600);
    } else {
        baudRateComboBox->setCurrentIndex(0); // Default to the first item if 9600 not found
    }
}


// Slot called when the "Refresh Ports" button is clicked.
void MainWindow::onRefreshPortsButtonClicked()
{
    populateSerialPorts();
    statusLabel->setText("Ports refreshed.");
}

// Slot called when the "Connect" / "Disconnect" button is clicked.
void MainWindow::onConnectButtonClicked()
{
    if (!isConnected) { // If currently disconnected, try to connect
        if (serialPortComboBox->currentText() == "No ports found" || serialPortComboBox->currentIndex() < 0) {
            QMessageBox::warning(this, "Connection Error", "No serial port selected or available.");
            return;
        }

        QString portName = serialPortComboBox->currentText();
        serialPort->setPortName(portName);

        // Get selected baud rate
        qint32 baudRate = baudRateComboBox->currentData().toInt();
        serialPort->setBaudRate(baudRate);

        // Set other standard serial port parameters (can be made configurable later)
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (serialPort->open(QIODevice::ReadWrite)) { // Try to open the port
            isConnected = true;
            connectButton->setText("Disconnect");
            statusLabel->setText(QString("Connected to %1 at %2 baud").arg(portName).arg(QString::number(baudRate)));
            dataDisplayArea->clear(); // Clear previous data
            setUiControlsEnabled(false); // Disable port/baud selection while connected
        } else {
            QMessageBox::critical(this, "Connection Error", "Failed to open port: " + serialPort->errorString());
            statusLabel->setText("Connection failed: " + serialPort->errorString());
        }
    } else { // If currently connected, disconnect
        serialPort->close();
        isConnected = false;
        connectButton->setText("Connect");
        statusLabel->setText("Disconnected");
        setUiControlsEnabled(true); // Re-enable port/baud selection
    }
}

// Slot called when data is available to be read from the serial port.
void MainWindow::readSerialData()
{
    if (serialPort->canReadLine()) { // Check if a full line can be read
         QByteArray lineData = serialPort->readLine();
         // Attempt to convert to string, assuming UTF-8 or Latin-1.
         // For binary data, you'd display hex or handle differently.
         dataDisplayArea->insertPlainText(QString::fromUtf8(lineData));
    } else {
        QByteArray data = serialPort->readAll(); // Read all available data
        dataDisplayArea->insertPlainText(QString::fromLatin1(data)); // Or use fromUtf8 if you expect UTF-8
    }
    // Scroll to the bottom to show the latest data
    dataDisplayArea->verticalScrollBar()->setValue(dataDisplayArea->verticalScrollBar()->maximum());
}

// Slot called when a serial port error occurs.
void MainWindow::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) { // Only handle actual errors
        QString errorMsg = "Serial port error: " + serialPort->errorString();
        // If the port closes unexpectedly (e.g., device unplugged)
        if (error == QSerialPort::ResourceError && isConnected) {
            errorMsg += ". Device may have been disconnected.";
            serialPort->close(); // Ensure port is marked as closed
            isConnected = false;
            connectButton->setText("Connect");
            statusLabel->setText("Disconnected (Error: Resource unavailable)");
            setUiControlsEnabled(true);
            QMessageBox::warning(this, "Serial Port Error", errorMsg);
        } else if (isConnected && error != QSerialPort::NoError) { // Other errors while connected
             // Avoid showing "NoError" as an error message if the port was closed intentionally
            statusLabel->setText("Error: " + serialPort->errorString());
            QMessageBox::warning(this, "Serial Port Error", errorMsg);
        } else if (error != QSerialPort::NoError) { // Errors when not connected or other general errors
            statusLabel->setText("Serial Port Error: " + serialPort->errorString());
             QMessageBox::information(this, "Serial Port Info", "Serial Port Event: " + serialPort->errorString());
        }
        qDebug() << "Serial Port Error Enum:" << error << "String:" << serialPort->errorString();
    }
}

// Enables or disables UI controls typically modified before/after connection.
void MainWindow::setUiControlsEnabled(bool enabled)
{
    serialPortComboBox->setEnabled(enabled);
    baudRateComboBox->setEnabled(enabled);
    refreshPortsButton->setEnabled(enabled);
}
