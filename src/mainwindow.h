// mainwindow.h
// Header file for the MainWindow class.
// Declares the main window's structure, UI elements, and slots.
// This header file is compatible with both Qt5 and Qt6.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QTextStream>

// Forward declaration of the generated UI class
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void onConnectButtonClicked();
  void onRefreshPortsButtonClicked();
  void onpprChanged();
  void readSerialData();
  void changeRecordingMode();
  void handleSerialError(QSerialPort::SerialPortError error);
	
	private:
  void recordToLogFile(const QString& data);
  void extractData(QString serialData);
  void populateSerialPorts();
  void populateBaudRates();
  void setUiControlsEnabled(bool enabled);

  Ui::MainWindow* ui; // Pointer to the generated UI object
  QSerialPort* serialPort;
  bool isConnected;

	QString serialLine;
	float ppr = 1000;
	double rpm1 = 0;
	double rpm2 = 0;
	float deg1 = 0;
	float deg2 = 0;
	long pulseCount1 = 0;
	long pulseCount2 = 0;
	long lastTimeFrame = 0;
	long lastPulseCount1 = 0;
	long lastPulseCount2 = 0;
	bool doRecord = false;
	QString logPath = "data.csv";
	QFile logFile;
	QTextStream logStream;
};

#endif // MAINWINDOW_H