#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ptimer = new QTimer;

    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }
    //波特率下拉菜单默认115200
    ui->BaudBox->setCurrentIndex(6);
    //数据位下拉菜单默认8位
    ui->BitNumBox->setCurrentIndex(3);
    //校验位下拉菜单默认无
    ui->ParityBox->setCurrentIndex(0);
    //停止位下拉菜单默认为1
    ui->StopBox->setCurrentIndex(0);
    //关闭发送按钮的使能
    ui->sendButton->setEnabled(false);
    //关闭保存按钮的使能
    ui->btn_savedata->setEnabled(false);
    qDebug() << tr("成功启动！");
}

MainWindow::~MainWindow()
{
    delete ui;
    if(ptimer->isActive())
    {
        ptimer->stop();
    }
}

//清空接受窗口
void MainWindow::on_clearButton_clicked()
{
    ui->textEdit->clear();
}

//发送数据
void MainWindow::on_sendButton_clicked()
{
    qDebug()<<ui->le_time->text().toInt();
    qDebug()<<ui->textEdit_2->toPlainText();
    if(ui->rb_timesend->isChecked() == true)
    {
        this->num = ui->le_time->text().toInt();
        this->ptimer->start(num);
        connect(this->ptimer,SIGNAL(timeout()),this,SLOT(Send_Data()));
    }
    serial->write(ui->textEdit_2->toPlainText().toUtf8());
    //serial->write(ui->sendMsglineEdit->text().toLatin1());
}

//读取接收到的数据
void MainWindow::Read_Data()
{
    QByteArray buf;
    buf = serial->readAll();
    if(!buf.isEmpty())
    {
        QString str = ui->textEdit->toPlainText();
        str+=tr(buf);
        ui->textEdit->clear();
        ui->textEdit->append(str);
    }
    buf.clear();
}

void MainWindow::on_openButton_clicked()
{

    if(ui->openButton->text()==tr("打开串口"))
    {
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName(ui->PortBox->currentText());
        //设置波特率
        serial->setBaudRate(ui->BaudBox->currentText().toInt());
        //设置数据位数
        switch(ui->BitNumBox->currentIndex())
        {
            case 0: serial->setDataBits(QSerialPort::Data5); break;
            case 1: serial->setDataBits(QSerialPort::Data6); break;
            case 2: serial->setDataBits(QSerialPort::Data7); break;
            case 3: serial->setDataBits(QSerialPort::Data8); break;
            default: break;
        }
        //设置奇偶校验
        switch(ui->ParityBox->currentIndex())
        {
            case 0: serial->setParity(QSerialPort::NoParity); break;
            case 1: serial->setParity(QSerialPort::OddParity); break;
            case 2: serial->setParity(QSerialPort::EvenParity); break;
            default: break;
        }
        //设置停止位
        switch(ui->StopBox->currentIndex())
        {
            case 0: serial->setStopBits(QSerialPort::OneStop); break;
            case 1: serial->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: serial->setStopBits(QSerialPort::TwoStop); break;
            default: break;
        }

        //打开串口
        serial->open(QIODevice::ReadWrite);

        //设置流控制
        serial->setFlowControl(QSerialPort::NoFlowControl);

        //关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitNumBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->openButton->setText(tr("关闭串口"));
        ui->sendButton->setEnabled(true);
        ui->btn_savedata->setEnabled(true);

        //连接信号槽
        //QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);
        QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(Read_Data()));
    }
    else
    {
        //关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();

        //恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitNumBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->openButton->setText(tr("打开串口"));
        ui->sendButton->setEnabled(false);
        ui->btn_savedata->setEnabled(false);
        this->ptimer->stop();
    }
}

void MainWindow::Send_Data()
{
    serial->write(ui->textEdit_2->toPlainText().toLatin1());
}

void MainWindow::on_btn_savedata_clicked()
{
    if(ui->textEdit->toPlainText().isEmpty())
    {
         QMessageBox::critical(this,"错误","接收框为空，保存无效！","确定");
         return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("保存为"), tr("未命名.txt"));
    QFile file(filename);

    // if user cancel save, return null
    if(file.fileName().isEmpty())
    {
         QMessageBox::critical(this,"错误","请输入保存的文件名！","确定");
         return;
    }

    if(!file.open(QFile::WriteOnly | QIODevice::Text))
    {
         QMessageBox::warning(this, tr("保存文件"), tr("打开文件%1失败,无法保存\n%2").arg(filename).arg(file.errorString()), QMessageBox::Ok);
         return;
    }

    //write data to file
    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.close();
    setWindowTitle("saved: " + QFileInfo(filename).canonicalFilePath());
}
