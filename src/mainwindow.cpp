#include "mainwindow.h"
#include "../build/ui_mainwindow.h" // Include the generated UI header
// #include <QDebug>
// #include <cmath>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), isConnected(false) {
	ui->setupUi(this); // Initialize the UI from the .ui file

	serialPort = new QSerialPort(this);

	populateSerialPorts();
	populateBaudRates();

	connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
	connect(ui->refreshPortsButton, &QPushButton::clicked, this, &MainWindow::onRefreshPortsButtonClicked);
	connect(ui->recordButton, &QPushButton::clicked, this, &MainWindow::changeRecordingMode);
	connect(ui->ppr, &QSpinBox::textChanged, this, &MainWindow::onpprChanged);
	connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
	connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
	ui->ppr->setMinimum(0);
	ui->ppr->setValue(1000);
	ui->ppr->setButtonSymbols(QAbstractSpinBox::NoButtons);

	ui->statusBar->showMessage("Disconnected");
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
	ui->statusBar->showMessage("Ports refreshed.");
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
			ui->statusBar->showMessage(
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
			ui->statusBar->showMessage("Connection failed: " + serialPort->errorString());
		}
	} else {
		serialPort->close();
		isConnected = false;
		ui->connectButton->setText("Connect");
		ui->statusBar->showMessage("Disconnected");
		setUiControlsEnabled(true);
	}
}

void MainWindow::readSerialData() {
	QByteArray data = serialPort->readAll();
	QString line = QString::fromUtf8(data);
	if (line == "\n") {
		this->extractData(serialLine);
		serialLine.clear();
	} else if (line != "\r") {
		serialLine.append(data);
	}
}

/**
 * Serial structure:
 * <long timeStamp>|<state> <pulse>|<state> <pulse>\r\n
 */
void MainWindow::extractData(QString serialData) {
	QStringList datas = serialData.split("|");
	if (datas.length() == 3) {
		long timeStamp = datas.at(0).toLong();
		QStringList rotor1 = datas.at(1).split(" ");
		QStringList rotor2 = datas.at(2).split(" ");

		/////// 1 ////////
		// QChar status = rotor1.at(0)[0];
		pulseCount1 = rotor1.at(1).toLong();
		deg1 = (pulseCount1 / ppr * 360);
		// deg = std::fmod(deg, 360.0);
		rpm1 = double(pulseCount1 - lastPulseCount1) /
			(double(timeStamp - lastTimeFrame) / 1000.0) * 1000.0 / ppr * 60.0;
		lastPulseCount1 = pulseCount1;

		/////// 2 ////////
		pulseCount2 = rotor2.at(1).toLong();
		deg2 = (pulseCount2 / ppr * 360);
		rpm2 = double(pulseCount2 - lastPulseCount2) /
			(double(timeStamp - lastTimeFrame) / 2000.0) * 1000.0 / ppr * 60.0;
		lastPulseCount2 = pulseCount2;

		lastTimeFrame = timeStamp;

		ui->R1degLabel->setText(QString::number(deg1, 'f', 2) + "°");
		ui->R1rpmLabel->setText(QString::number(rpm1, 'f', 1) + " rpm");
		ui->R2degLabel->setText(QString::number(deg2, 'f', 2) + "°");
		ui->R2rpmLabel->setText(QString::number(rpm2, 'f', 1) + " rpm");

	} else {
		ui->statusBar->showMessage(QString("Failed to extract"));
		QTimer::singleShot(1000, [&]() {ui->statusBar->showMessage("");});

	}
}

void MainWindow::changeRecordingMode() {
	doRecord = !doRecord;
}
void MainWindow::onpprChanged() {
	ppr = ui->ppr->value();
	ui->statusBar->showMessage(QString("Changed ppr to %1").arg(ppr));
}
void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
	if (error != QSerialPort::NoError) {
		QString errorMsg = serialPort->errorString();
		if (error == QSerialPort::ResourceError && isConnected) {
			serialPort->close();
			isConnected = false;
			ui->connectButton->setText("Connect");
			ui->statusBar->showMessage("Disconnected (Error: Resource unavailable)");
			setUiControlsEnabled(true);
			QMessageBox::warning(this, "Serial Port Error", "Device may have been disconnected.");
		} else if (isConnected && error != QSerialPort::NoError) {
			ui->statusBar->showMessage("Error: " + errorMsg);
			QMessageBox::warning(this, "Serial Port Error", errorMsg);
		} else if (error != QSerialPort::NoError) {
			ui->statusBar->showMessage("Serial Port Error: " + errorMsg);
			QMessageBox::information(this, "Serial Port Info", "Serial Port Event: " + errorMsg);
		}
	}
}

void MainWindow::setUiControlsEnabled(bool enabled) {
	ui->serialPortComboBox->setEnabled(enabled);
	ui->baudRateComboBox->setEnabled(enabled);
	ui->refreshPortsButton->setEnabled(enabled);
}