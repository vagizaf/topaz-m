#include "updategraph.h"
#include "mainwindow.h"
#include "uartdriver.h"
#include "ui_mainwindow.h"
#include "messages.h"
#include "metrologicalcalc.h"
#include "mathresolver.h"

int a=0;int b=0;
int color1rgb[]={219, 37, 37};
int color2rgb[]={43, 37, 219};
int color3rgb[]={37, 219, 49};
int color4rgb[]={59, 30, 7};

/*int color2rgb[]={rand()%245+10, rand()%245+10, rand()%245+10};
int color3rgb[]={rand()%245+10, rand()%245+10, rand()%245+10};
int color4rgb[]={rand()%245+10, rand()%245+10, rand()%245+10};*/

QVector<double> xx1, yy1, yy2, yy3, yy4;

void MainWindow::updategraph()
{
    UartDriver UD;
    xx1.append(b);
    yy1.append(UD.channelinputbuffer[0]);
    yy2.append(UD.channelinputbuffer[1]);
    yy3.append(UD.channelinputbuffer[2]);
    yy4.append(UD.channelinputbuffer[3]);
    b++;

    //if (b>600)
    while (xx1.length()>600)
    {
        xx1.remove(0);
        yy1.remove(0);
        yy2.remove(0);
        yy3.remove(0);
        yy4.remove(0);
    }

    if (b>=300&&b<900)
    {
        ui->customPlot->xAxis->setRange(b-300, b+300);
    }
    
    if (b==1200)
    {
        b=0;
        ui->customPlot->xAxis->setRange(0, 600);
        xx1.clear();
        yy1.clear();
        yy2.clear();
        yy3.clear();
        yy4.clear();
    }
}

void MainWindow::updatepicture()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName("graph #1");
    ui->customPlot->graph()->setData(xx1, yy1);
    graphPen.setWidth(4);
    graphPen.setColor(QColor(color1rgb[0],color1rgb[1],color1rgb[2]));
    
    ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->addGraph();
    
    {ui->customPlot->graph()->setData(xx1, yy2);
        graphPen.setColor(QColor(color2rgb[0],color2rgb[1],color2rgb[2]));
        ui->customPlot->graph()->setPen(graphPen);
    }
    
    {ui->customPlot->addGraph();
        ui->customPlot->graph()->setData(xx1, yy3);
        graphPen.setColor(QColor(color3rgb[0],color3rgb[1],color3rgb[2]));
        ui->customPlot->graph()->setPen(graphPen);}
    
    {ui->customPlot->addGraph();
        ui->customPlot->graph()->setData(xx1, yy4);
        graphPen.setColor(QColor(color4rgb[0],color4rgb[1],color4rgb[2]));
        ui->customPlot->graph()->setPen(graphPen);
    }
    
    ui->customPlot->xAxis->setTickStep(60); // 60 secs btw timestamp

//    ui->customPlot->xAxis->setTickLength(1,2); // 60 secs btw timestamp

//    ui->customPlot->xAxis->setAutoTickStep(false);
//    ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//    ui->customPlot->xAxis->setTickLabels(false);

    QVector<double> Ticks;  // вектор с шагом в 30 градусов
    int i = 0;
    while(i<=360) {
        Ticks << i;
        i+=30;
    }
    ui->customPlot->xAxis->setAutoTicks(false); // выключаем автоматические отсчеты
    ui->customPlot->xAxis->setTickVector(Ticks);

    QVector<QString> Labels;
    Labels << "0" << QString::fromUtf8("π/6") << QString::fromUtf8("π/3")
           << QString::fromUtf8("π/2") << QString::fromUtf8("2π/3")
           << QString::fromUtf8("5π/6") << QString::fromUtf8("π")
           << QString::fromUtf8("7π/6") << QString::fromUtf8("4π/3")
           << QString::fromUtf8("3π/2") << QString::fromUtf8("5π/3")
           << QString::fromUtf8("11π/6") << QString::fromUtf8("2π");

    ui->customPlot->xAxis->setAutoTickLabels(false);
    ui->customPlot->xAxis->setTickVectorLabels(Labels);

    //    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
//    textTicker->addTick(10, "a bit\nlow");

/*
    QCPItemText *groupTracerText = new QCPItemText(ui->customPlot);
    groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    groupTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    groupTracerText->position->setCoords(1.0, 0.20); // lower right corner of axis rect

    groupTracerText->setText("Fixed positions in\nwave packet define\ngroup velocity vg");*/
//    groupTracerText->setTextAlignment(Qt::AlignLeft);
//    groupTracerText->setFont(QFont(font().family(), 9));
//    groupTracerText->setPadding(QMargins(10,10,20,20));

//    QCPItemText *textLabel = new QCPItemText(ui->customPlot);
//    ui->customPlot->addItem(textLabel);
//    textLabel->setText("Date Dummy");
////    textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
////    textLabel->setPen(QPen(Qt::black)); // show black border around text
////    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
////    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
//    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
//    textLabel->position->setCoords(100, 20); // place positi
////    ui->customPlot->addItem(textLabel);

    //    ui->customPlot->xAxis->setDateTimeSpec(Qt::OffsetFromUTC);
    //    ui->customPlot->xAxis->setDateTimeForma("hh:mm:ss");// dd.MM.yyyy
    ui->customPlot->replot();
}

void MainWindow::UpdateDataChannel1()
{
    UartDriver UD;
    ModBus modbus;
    mathresolver mathres;
    double currentdata;
    
    UD.needtoupdatechannel[0] = 1;
    
    currentdata = modbus.DataChannelRead(ModBus::UniversalChannel1);
    if (ch1.IsMathematical())
    {
        currentdata = mathres.Solve(ch1.GetMathString(), currentdata); // + mathres.Solve("sin(x)*10", currentdata); //sqrt(abs(x))+20
    }
    UD.writechannelvalue(1,currentdata);
    
    //qDebug() << QString::number(ModBus::UniqueIDAddress);
    if ((currentdata>=ch1.GetState1Value() ) && ( ch1.HighState1Setted == false ))
    {
        ch1.LowState1Setted = false;
        ui->listWidget->addItem(ch1.GetState1HighMessage());
        ui->listWidget->scrollToBottom();
        ch1.HighState1Setted = true;
        mr.LogMessageWrite (ch1.GetChannelName() + ":" + ch1.GetState1HighMessage());
    }
    
    if ((currentdata<ch1.GetState1Value() ) && ( ch1.LowState1Setted == false ))
    {
        ch1.LowState1Setted = true;
        ui->listWidget->addItem(ch1.GetState1LowMessage());
        ui->listWidget->scrollToBottom();
        ch1.HighState1Setted = false;
        mr.LogMessageWrite (ch1.GetChannelName() + ":" + ch1.GetState1LowMessage());
    }
    
    int period = ch1.GetMeasurePeriod()*1000;
    channeltimer1->setInterval(period);
}

void MainWindow::UpdateDataChannel111()
{
    UartDriver::needtoupdatechannel[0] = 1;
    int period = ch1.GetMeasurePeriod()*1000;
    channeltimer1->setInterval(period);
}

void MainWindow::UpdateDataChannel222()
{
    UartDriver::needtoupdatechannel[1] = 1;
    int period = ch2.GetMeasurePeriod()*1000;
    channeltimer2->setInterval(period);
}

void MainWindow::UpdateDataChannel333()
{
    UartDriver::needtoupdatechannel[2] = 1;
    int period = ch3.GetMeasurePeriod()*1000;
    channeltimer3->setInterval(period);
}

void MainWindow::UpdateDataChannel444()
{
    UartDriver::needtoupdatechannel[3] = 1;
    int period = ch4.GetMeasurePeriod()*1000;
    channeltimer4->setInterval(period);
}

void MainWindow::UpdateDataChannel2()
{
    UartDriver UD;
    ModBus modbus;
    mathresolver mathres;
    
    double currentdata;
    double pressure;
    currentdata = modbus.DataChannel1Read(); // тоже покатит:  modbus.DataChannelRead(ModBus::UniversalChannel1);
    if (ch2.IsMathematical())
    {
        currentdata = mathres.Solve(ch2.GetMathString(), currentdata); // + mathres.Solve("sin(x)*10", currentdata); //sqrt(abs(x))+20
    }
    pressure = currentdata;
    UD.writechannelvalue(2,pressure);
    
    if ((pressure>=ch2.GetState1Value() ) && ( ch2.HighState1Setted == false ))
    {
        ch2.LowState1Setted = false;
        ui->listWidget->addItem(ch2.GetState1HighMessage());
        ui->listWidget->scrollToBottom();
        ch2.HighState1Setted = true;
        mr.LogMessageWrite (ch2.GetChannelName() + ":" + ch2.GetState1HighMessage());
    }
    
    if ((pressure<ch2.GetState1Value() ) && ( ch2.LowState1Setted == false ))
    {
        ch2.LowState1Setted = true;
        ui->listWidget->addItem(ch2.GetState1LowMessage());
        ui->listWidget->scrollToBottom();
        ch2.HighState1Setted = false;
        mr.LogMessageWrite (ch2.GetChannelName() + ":" + ch2.GetState1LowMessage());
    }
    
    int period = ch2.GetMeasurePeriod()*1000;
    channeltimer2->setInterval(period);
}

void MainWindow::UpdateDataChannel3()
{
    UartDriver UD;
    ModBus modbus;
    mathresolver mathres;
    double currentdata;
    
    currentdata = modbus.DataChannelRead(ModBus::UniversalChannel1);
    if (ch3.IsMathematical())
    {
        currentdata = mathres.Solve(ch3.GetMathString(), currentdata); // + mathres.Solve("sin(x)*10", currentdata); //sqrt(abs(x))+20
    }
    UD.writechannelvalue(3,currentdata);
    
    if ((currentdata>=ch3.GetState1Value() ) && ( ch3.HighState1Setted == false ))
    {
        ch3.LowState1Setted = false;
        ui->listWidget->addItem(ch3.GetState1HighMessage());
        ui->listWidget->scrollToBottom();
        ch3.HighState1Setted = true;
        mr.LogMessageWrite (ch3.GetChannelName() + ":" + ch3.GetState1HighMessage());
    }
    
    if ((currentdata<ch3.GetState1Value() ) && ( ch3.LowState1Setted == false ))
    {
        ch3.LowState1Setted = true;
        ui->listWidget->addItem(ch3.GetState1LowMessage());
        ui->listWidget->scrollToBottom();
        ch3.HighState1Setted = false;
        mr.LogMessageWrite (ch3.GetChannelName() + ":" + ch3.GetState1LowMessage());
    }
    
    int period = ch3.GetMeasurePeriod()*1000;
    channeltimer3->setInterval(period);
}

void MainWindow::UpdateDataChannel4()
{
    UartDriver UD;
    ModBus modbus;
    mathresolver mathres;
    double currentdata;
    
    currentdata = modbus.DataChannelRead(ModBus::UniversalChannel1);
    if (ch4.IsMathematical())
    {
        currentdata = mathres.Solve(ch4.GetMathString(), currentdata); // + mathres.Solve("sin(x)*10", currentdata); //sqrt(abs(x))+20
    }
    UD.writechannelvalue(4,currentdata);
    
    if ((currentdata>=ch4.GetState1Value() ) && ( ch4.HighState1Setted == false ))
    {
        ch4.LowState1Setted = false;
        ui->listWidget->addItem(ch4.GetState1HighMessage());
        ui->listWidget->scrollToBottom();
        ch4.HighState1Setted = true;
        mr.LogMessageWrite (ch4.GetChannelName() + ":" + ch4.GetState1HighMessage());
    }
    
    if ((currentdata<ch4.GetState1Value() ) && ( ch4.LowState1Setted == false ))
    {
        ch4.LowState1Setted = true;
        ui->listWidget->addItem(ch4.GetState1LowMessage());
        ui->listWidget->scrollToBottom();
        ch4.HighState1Setted = false;
        mr.LogMessageWrite (ch4.GetChannelName() + ":" + ch4.GetState1LowMessage());
    }
    
    int period = ch4.GetMeasurePeriod()*1000;
    channeltimer4->setInterval(period);
}
