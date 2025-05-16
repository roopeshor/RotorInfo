#include "mainwindow.h"
#include "../build/ui_mainwindow.h" // Include the generated UI header
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), isConnected(false) {
	ui->setupUi(this); // Initialize the UI from the .ui file

	serialPort = new QSerialPort(this);

	populateSerialPorts();
	populateBaudRates();

	connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
	connect(ui->refreshPortsButton, &QPushButton::clicked, this, &MainWindow::onRefreshPortsButtonClicked);
	connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
	connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));

	ui->statusbar->showMessage("Disconnected");
}

MainWindow::~MainWindow() {
	if (serialPort->isOpen()) {
		serialPort->close();
	}
	delete ui;
}

void MainWindow::populateSerialPorts() {
	ui->serialPortComboBox->clear();
	const auto infos = QSerialPortInfo::availablePorts();
	if (infos.isEmpty()) {
		ui->serialPortComboBox->addItem("No ports found");
		ui->serialPortComboBox->setEnabled(false);
		ui->connectButton->setEnabled(false);
	} else {
		for (const QSerialPortInfo& info : infos) {
			ui->serialPortComboBox->addItem(
				info.portName(),
				QVariant::fromValue(info.systemLocation())
			);
		}
		ui->serialPortComboBox->setEnabled(true);
		ui->connectButton->setEnabled(true);
	}
}

void MainWindow::populateBaudRates() {
	ui->baudRateComboBox->clear();
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
		ui->baudRateComboBox->addItem(QString::number(baud), QVariant::fromValue(baud));
	}
	ui->baudRateComboBox->setCurrentIndex(0);
}

void MainWindow::onRefreshPortsButtonClicked() {
	populateSerialPorts();
	ui->statusbar->showMessage("Ports refreshed.");
}

void MainWindow::onConnectButtonClicked() {
	if (!isConnected) {
		if (
			ui->serialPortComboBox->currentText() == "No ports found" ||
			ui->serialPortComboBox->currentIndex() < 0
			) {
			QMessageBox::warning(this, "Connection Error", "No serial port selected or available.");
			return;
		}

		QString portName = ui->serialPortComboBox->currentText();
		serialPort->setPortName(portName);

		qint32 baudRate = ui->baudRateComboBox->currentData().toInt();
		serialPort->setBaudRate(baudRate);

		serialPort->setDataBits(QSerialPort::Data8);
		serialPort->setParity(QSerialPort::NoParity);
		serialPort->setStopBits(QSerialPort::OneStop);
		serialPort->setFlowControl(QSerialPort::NoFlowControl);

		if (serialPort->open(QIODevice::ReadOnly)) {
			isConnected = true;
			ui->connectButton->setText("Disconnect");
			ui->statusbar->showMessage(
				QString("Connected to %1 at %2 baud")
				.arg(portName)
				.arg(QString::number(baudRate))
			);
			setUiControlsEnabled(false);
		} else {
			QMessageBox::critical(
				this,
				"Connection Error",
				"Failed to open port: " + serialPort->errorString()
			);
			ui->statusbar->showMessage("Connection failed: " + serialPort->errorString());
		}
	} else {
		serialPort->close();
		isConnected = false;
		ui->connectButton->setText("Connect");
		ui->statusbar->showMessage("Disconnected");
		setUiControlsEnabled(true);
	}
}

void MainWindow::readSerialData() {
	QByteArray data = serialPort->readAll();
	QString line = QString::fromUtf8(data);
	if (line == "\n") {
		ui->R1degLabel->setText(serialLine);
		serialLine.clear();
	} else if (line != "\r") {
		serialLine.append(data);
	}
}
void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
	if (error != QSerialPort::NoError) {
		QString errorMsg = serialPort->errorString();
		if (error == QSerialPort::ResourceError && isConnected) {
			serialPort->close();
			isConnected = false;
			ui->connectButton->setText("Connect");
			ui->statusbar->showMessage("Disconnected (Error: Resource unavailable)");
			setUiControlsEnabled(true);
			QMessageBox::warning(this, "Serial Port Error", "Device may have been disconnected.");
		} else if (isConnected && error != QSerialPort::NoError) {
			ui->statusbar->showMessage("Error: " + errorMsg);
			QMessageBox::warning(this, "Serial Port Error", errorMsg);
		} else if (error != QSerialPort::NoError) {
			ui->statusbar->showMessage("Serial Port Error: " + errorMsg);
			QMessageBox::information(this, "Serial Port Info", "Serial Port Event: " + errorMsg);
		}
	}
}

void MainWindow::setUiControlsEnabled(bool enabled) {
	ui->serialPortComboBox->setEnabled(enabled);
	ui->baudRateComboBox->setEnabled(enabled);
	ui->refreshPortsButton->setEnabled(enabled);
}