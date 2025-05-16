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
  void readSerialData();
  void handleSerialError(QSerialPort::SerialPortError error);

private:
  void populateSerialPorts();
  void populateBaudRates();
  void setUiControlsEnabled(bool enabled);

  Ui::MainWindow* ui; // Pointer to the generated UI object
  QSerialPort* serialPort;
  bool isConnected;

	QString serialLine;
};

#endif // MAINWINDOW_H