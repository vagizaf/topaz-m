#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "options.h"
#include "keyboard.h"
#include "dialog.h"

#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QtMath>
#include <QFile>
#include <QDataStream>
#include <QtScript/QScriptEngine>
#include <QtSerialPort/QtSerialPort>
#include <QPainterPath>
#include <QPainter>
#include <QDateTime>
#include <QCloseEvent>
#include <QMessageBox>
#include <QtWidgets>
#include <QThread>
#include <QPoint>
#include <channel1.h>
#include <uartdriver.h>

extern QString inputstr ;

double UartDriver::channelinputbuffer[] = {27.22,33.87,57.89,81.11};

void UartDriver::readuart()
{
    while (1)
    {
        QSerialPort serial;
        serial.setPortName("/dev/ttyS1"); //usart1
        if (serial.open(QIODevice::ReadWrite))
        {
            serial.setBaudRate(QSerialPort::Baud9600);
            serial.setDataBits(QSerialPort::Data8);
            serial.setParity(QSerialPort::NoParity);
            serial.setStopBits(QSerialPort::OneStop);
            serial.setFlowControl(QSerialPort::NoFlowControl);
            {
                // start stuff
                serial.write("a");
                while (serial.waitForBytesWritten(500))
                    ;

                serial.write("b");
                while (serial.waitForBytesWritten(500))
                    ;
                // end stuff

                QByteArray requestData;
                while (serial.waitForReadyRead(10))
                    requestData = serial.readAll();
                inputstr = QTextCodec::codecForMib(106)->toUnicode(requestData);

                if ((serial.bytesAvailable())>0)
                {
                    while (1)
                    {
                        while (serial.waitForReadyRead(10))
                            requestData = serial.readAll();
                        inputstr = QTextCodec::codecForMib(106)->toUnicode(requestData);
                        serial.write("c");
                        while (serial.waitForBytesWritten(20))
                            ;
                    }
                }
                while (serial.waitForReadyRead(10))
                    requestData = serial.readAll();
            }
            serial.close();
        }
    }
}

void UartDriver::writechannelvalue(int channel, double value)
{
    this->channelinputbuffer[channel] = value;
//    qDebug() << "writechannelvalue";
}

void UartDriver::just()
{
    qDebug() << "just";
}


double UartDriver::readchannelvalue(int channelnumber)
{
    return channelinputbuffer[channelnumber];
}

void UartDriver()
{
}