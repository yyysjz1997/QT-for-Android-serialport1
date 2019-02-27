#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTime>
#include <QDialog>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_clearButton_clicked();
    void on_sendButton_clicked();
    void on_openButton_clicked();
    void Read_Data();
    void Send_Data();

    void on_btn_savedata_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer *ptimer;
    int num;//定时器发送时间

};

#endif // MAINWINDOW_H
