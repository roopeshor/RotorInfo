#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), isConnected(false) {
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	mainLayout = new QVBoxLayout(centralWidget);

	controlsLayout = new QHBoxLayout();

	serialPortComboBox = new QComboBox(this);
	controlsLayout->addWidget(new QLabel("Port:", this));
	controlsLayout->addWidget(serialPortComboBox);

	baudRateComboBox = new QComboBox(this);
	controlsLayout->addWidget(new QLabel("Baud Rate:", this));
	controlsLayout->addWidget(baudRateComboBox);

	refreshPortsButton = new QPushButton("Refresh Ports", this);
	controlsLayout->addWidget(refreshPortsButton);

	connectButton = new QPushButton("Connect", this);
	controlsLayout->addWidget(connectButton);

	mainLayout->addLayout(controlsLayout);

	// Data Display Area
	dataDisplayArea = new QPlainTextEdit(this);
	dataDisplayArea->setReadOnly(true);
	mainLayout->addWidget(dataDisplayArea);

	// Status Bar
	statusLabel = new QLabel("Disconnected", this);
	statusBar()->addWidget(statusLabel);

	serialPort = new QSerialPort(this);

	populateSerialPorts();
	populateBaudRates();

	connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
	connect(refreshPortsButton, &QPushButton::clicked, this, &MainWindow::onRefreshPortsButtonClicked);
	connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
	connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
	// but the SIGNAL/SLOT macro version is guaranteed to work with all Qt5 setups.
	// connect(serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleSerialError); // Modern syntax also works in Qt5


	setWindowTitle("Serial Port Reader");
	resize(600, 400);
}

MainWindow::~MainWindow() {
	if (serialPort->isOpen()) {
		serialPort->close();
	}
}

// Populates the serial port combo box with available serial ports.
void MainWindow::populateSerialPorts() {
	serialPortComboBox->clear();
	const auto infos = QSerialPortInfo::availablePorts();
	if (infos.isEmpty()) {
		serialPortComboBox->addItem("No ports found");
		serialPortComboBox->setEnabled(false);
		connectButton->setEnabled(false);
	} else {
		for (const QSerialPortInfo& info : infos) {
			serialPortComboBox->addItem(
				info.portName(),
				QVariant::fromValue(info.systemLocation())
			);
		}
		serialPortComboBox->setEnabled(true);
		connectButton->setEnabled(true);
	}
}

void MainWindow::populateBaudRates() {
	baudRateComboBox->clear();
	QList<qint32> baudRates = {
		QSerialPort::Baud9600,
		QSerialPort::Baud1200,
		QSerialPort::Baud2400,
		QSerialPort::Baud4800,
		QSerialPort::Baud19200,
		QSerialPort::Baud38400,
		QSerialPort::Baud57600,
		QSerialPort::Baud115200
	};

	for (qint32 baud : baudRates) {
		baudRateComboBox->addItem(QString::number(baud), QVariant::fromValue(baud));
	}
	baudRateComboBox->setCurrentIndex(0);
}


void MainWindow::onRefreshPortsButtonClicked() {
	populateSerialPorts();
	statusLabel->setText("Ports refreshed.");
}

void MainWindow::onConnectButtonClicked() {
	if (!isConnected) {
		if (
			serialPortComboBox->currentText() == "No ports found" ||
			serialPortComboBox->currentIndex() < 0
			) {
			QMessageBox::warning(this, "Connection Error", "No serial port selected or available.");
			return;
		}

		QString portName = serialPortComboBox->currentText();
		serialPort->setPortName(portName);

		qint32 baudRate = baudRateComboBox->currentData().toInt();
		serialPort->setBaudRate(baudRate);

		serialPort->setDataBits(QSerialPort::Data8);
		serialPort->setParity(QSerialPort::NoParity);
		serialPort->setStopBits(QSerialPort::OneStop);
		serialPort->setFlowControl(QSerialPort::NoFlowControl);

		if (serialPort->open(QIODevice::ReadOnly)) {
			isConnected = true;
			connectButton->setText("Disconnect");
			statusLabel->setText(
				QString("Connected to %1 at %2 baud")
				.arg(portName)
				.arg(QString::number(baudRate))
			);
			dataDisplayArea->clear();
			setUiControlsEnabled(false);
		} else {
			QMessageBox::critical(
				this,
				"Connection Error",
				"Failed to open port: " + serialPort->errorString()
			);
			statusLabel->setText("Connection failed: " + serialPort->errorString());
		}
	} else {
		serialPort->close();
		isConnected = false;
		connectButton->setText("Connect");
		statusLabel->setText("Disconnected");
		setUiControlsEnabled(true);
	}
}

void MainWindow::readSerialData() {
	if (serialPort->canReadLine()) {
		QByteArray lineData = serialPort->readLine();
		QString line = QString::fromUtf8(lineData);
		dataDisplayArea->insertPlainText(line);
	} else {
		QByteArray data = serialPort->readAll();
		dataDisplayArea->insertPlainText(QString::fromLatin1(data));
	}
	dataDisplayArea->verticalScrollBar()->setValue(
		dataDisplayArea->verticalScrollBar()->maximum()
	);
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
	if (error != QSerialPort::NoError) {
		QString errorMsg = serialPort->errorString();
		if (error == QSerialPort::ResourceError && isConnected) {
			serialPort->close();
			isConnected = false;
			connectButton->setText("Connect");
			statusLabel->setText("Disconnected (Error: Resource unavailable)");
			setUiControlsEnabled(true);
			QMessageBox::warning(this, "Serial Port Error", "Device may have been disconnected.");
		} else if (isConnected && error != QSerialPort::NoError) {
			statusLabel->setText("Error: " + errorMsg);
			QMessageBox::warning(this, "Serial Port Error", errorMsg);
		} else if (error != QSerialPort::NoError) {
			statusLabel->setText("Serial Port Error: " + errorMsg);
			QMessageBox::information(this, "Serial Port Info", "Serial Port Event: " + errorMsg);
		}
	}
}

void MainWindow::setUiControlsEnabled(bool enabled) {
	serialPortComboBox->setEnabled(enabled);
	baudRateComboBox->setEnabled(enabled);
	refreshPortsButton->setEnabled(enabled);
}
