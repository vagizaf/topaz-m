#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "options.h"
#include "messages.h"
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

QString inputstr = "";

QDateTime start(QDateTime::currentDateTime());

QString MainWindow::startdate = start.toString("dd/MM/yy");
QString MainWindow::starttime = start.toString("hh:mm:ss");

QString MainWindow::endtime = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowTitle(tr("VISION"));

    QProcess process1;
    QPixmap pix("/usr/inc/logo.jpg");
    ui->label->setPixmap(pix);
    ui->customPlot->xAxis->setRange(-8, 600);
    ui->customPlot->yAxis->setRange(-5, 110);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCaption()));

    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(updatepicture()));

    QTimer *timetouch = new QTimer(this);
    connect(timetouch, SIGNAL(timeout()), this, SLOT(touchupdate()));

    tmr = new QTimer();
    tmr->setInterval(500);

    QTimer *tmrarchive = new QTimer(this);
    tmrarchive->setInterval(5000);

    connect(tmrarchive, SIGNAL(timeout()), this, SLOT(WriteArchiveToFile()));
    tmrarchive->start(5000);

    QTimer *closetimer = new QTimer(this);

    connect(tmr, SIGNAL(timeout()), this, SLOT(updategraph()));
    connect(tmr, SIGNAL(timeout()), this, SLOT(updatevalue()));

    tmr->start(100);
    timer->start(1111);
    timer2->start(201);
    timetouch->start(5000);

    QThread *thread= new QThread();
    UartDriver *my = new UartDriver();
    UartDriver *UD = new UartDriver();

    my->moveToThread(thread);

    connect(thread, SIGNAL(started()), my, SLOT(readuart()));

    channeltimer1 = new QTimer();
    channeltimer1->setInterval(100);
    channeltimer2 = new QTimer();
    channeltimer2->setInterval(300);
    channeltimer3 = new QTimer();
    channeltimer3->setInterval(2000);
    channeltimer4 = new QTimer();
    channeltimer4->setInterval(5000);

    connect(channeltimer1, SIGNAL(timeout()), this, SLOT(UpdateDataChannel1()) );
    connect(channeltimer2, SIGNAL(timeout()), this, SLOT(UpdateDataChannel2()) );
    connect(channeltimer3, SIGNAL(timeout()), this, SLOT(UpdateDataChannel3()) );
    connect(channeltimer4, SIGNAL(timeout()), this, SLOT(UpdateDataChannel4()) );

    channeltimer1->start(100);
    channeltimer2->start(100);
    channeltimer3->start(100);
    channeltimer4->start(100);

    thread->start();

    QProcess process;
    process.startDetached("sudo cpufreq-set -f 300MHz"); // max freq on
    process.startDetached("sudo cpufreq-set --governor powersave"); // min perfomance on
    process.startDetached("xinput set-prop 7 \"Evdev Axis Calibration\" 3383 3962 234 599"); // вручную ввели координаты тача

    MessageWrite mr ("Programm Started");

    ch1.readoptionsfromfile(1);
    ch2.readoptionsfromfile(2);
    ch3.readoptionsfromfile(3);
    ch4.readoptionsfromfile(4);
}

MainWindow::~MainWindow()
{
    MessageWrite mr1 ("Programm Closed");
    mr1.WriteAllLogToFile();
    delete ui;
}

void MainWindow::on_dial_sliderMoved(int position)
{
    ui->lcdNumber->display(ui->dial->value());
}

void MainWindow::on_lcdNumber_overflow()
{
}

void MainWindow::on_dial_actionTriggered(int action)
{
}

void MainWindow::updateCaption()
{
    QDateTime local(QDateTime::currentDateTime());
    ui->time_label->setText(local.time().toString() + local.date().toString(" dd.MM.yyyy "));
}

void MainWindow::touchupdate()
{
    QProcess process;
    process.startDetached("xinput set-prop 7 \"Evdev Axis Calibration\" " + Options::calibrationprm); // каждую секунду вводим координаты тача вдруг чтобы не отвалился
}

void MainWindow::updatevalue()
{
    if (inputstr!="")
    {
        QString message =inputstr.left(4);
        QString valuestring = message.left(3);
        QString letterstring = message.right(1);
        double value1 = valuestring.toDouble();
        if (value1<=1000)
            if (value1>0)
            {
                if (letterstring=="a")
                {
                    ui->dial->setValue(value1);
                    ui->lcdNumber->display(value1);}
                if (letterstring=="b")
                {
                    //ui->lcdNumber_2->display(value1);
                }
            }
    }
}

void MainWindow::paintEvent(QPaintEvent *e)
{
}

void MainWindow::on_pushButton_clicked()
{
    QProcess process;
    process.startDetached("sudo poweroff");
    QApplication::exit();
}

void MainWindow::on_pushButton_2_clicked()
{
    Options options;
    options.setModal(true);
    options.exec();
    //читаем параметры каналов прямо после закрытия окна настроек и перехода в меню режима работы
    ch1.readoptionsfromfile(1);
    ch2.readoptionsfromfile(2);
    ch3.readoptionsfromfile(3);
    ch4.readoptionsfromfile(4);
    //    qDebug() << "readoptionsfromfile" ;
}

void MainWindow::on_pushButton_2_pressed()
{
    ui->customPlot->clearGraphs();
}
/*
void MainWindow::on_checkBox_stateChanged(int arg1)
{
    QFile filedir("/sys/class/gpio/gpio69/direction");

    filedir.open(QIODevice::WriteOnly);
    QTextStream outdir(&filedir);

    outdir << "out";
    filedir.close();

    QFile file("/sys/class/gpio/gpio69/value");
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);

    if (1)
    {
        out << "1";
    }

    else
    {
        out << "0";
    }

    file.close();
}
*/
double MainWindow::returnmathresult(double dval)
{
    QString inn = ui->textEdit->toPlainText();
    QScriptEngine myEngine;
    QString vaal = QString::number(dval);
    QString replaced=inn;
    replaced.replace(QString("sin"), QString("Math.sin"));
    replaced.replace(QString("cos"), QString("Math.cos"));
    replaced.replace(QString("sqrt"), QString("Math.sqrt"));
    replaced.replace(QString("pow"), QString("Math.pow"));
    replaced.replace(QString("x"), QString(vaal));
    double Result = myEngine.evaluate(replaced).toNumber();
    return Result;
}

void MainWindow::on_textEdit_textChanged()
{
    QString inn = ui->textEdit->toPlainText();
    QScriptEngine myEngine;
    QString vaal = QString::number(ui->dial->value());
    QString replaced=inn;
    replaced.replace(QString("sin"), QString("Math.sin"));
    replaced.replace(QString("cos"), QString("Math.cos"));
    replaced.replace(QString("sqrt"), QString("Math.sqrt"));
    replaced.replace(QString("pow"), QString("Math.pow"));
    replaced.replace(QString("x"), QString(vaal));
    double Result = myEngine.evaluate(replaced).toNumber();
}

void MainWindow::on_dial_valueChanged(int value)
{
}

void MainWindow::on_pushButton_3_clicked()
{
    QApplication::exit();
}

void MainWindow::delay(int n)
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::focusChanged(QWidget* , QWidget* )
{
}

void MainWindow::on_pushButton_4_clicked()
{
    mr.WriteAllLogToFile();
    Messages messages;
    messages.setModal(true);
    messages.exec();
}

void MainWindow::on_customPlot_destroyed()
{
}

void MainWindow::on_radioButton_clicked()
{
    QProcess process;
    process.startDetached("sudo cpufreq-set -f 1000MHz"); // max freq on
    process.startDetached("sudo cpufreq-set --governor performance"); // max perfomance on
}

void MainWindow::on_radioButton_2_clicked()
{
    QProcess process;
    process.startDetached("sudo cpufreq-set -f 300MHz"); // max freq on
    process.startDetached("sudo cpufreq-set --governor powersave"); // min perfomance on
}
