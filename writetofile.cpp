#include "mainwindow.h"
#include "ustavka.h"
#include "ui_mainwindow.h"
#include "options.h"
//#include "ui_options.h"
#include "messages.h"
#include "keyboard.h"
#include "Channels/channelOptions.h"
#include "defines.h"
#include "processwritetofile.h"
#include "Channels/group_channels.h"
//#include "extern.h"

extern QVector<double> X_Coordinates,Y_coordinates_Chanel_1,Y_coordinates_Chanel_2,Y_coordinates_Chanel_3,Y_coordinates_Chanel_4;
extern QVector<double> vectorsChannels[24];
extern QVector<double> X_Date_Coordinates;
extern QList<cGroupChannels*> listGroup;
extern QList<Ustavka *> listUstavok;
extern QList<cSteel*> listSteel;
extern typeSteelTech steelTech[];
extern QList<cMathChannel *> listMath;
extern QList<ChannelOptions*> listChannels;

void MainWindow::WriteGpio(int num, bool val)
{
    QFile gpio("/sys/class/gpio/export");
    gpio.open(QIODevice::WriteOnly);
    QTextStream outgpio(&gpio);
    outgpio << num;
    gpio.close();
    QFile filedir("/sys/class/gpio/gpio" + QString::number(num) + "/direction");
    filedir.open(QIODevice::WriteOnly);
    QTextStream outdir(&filedir);
    outdir << "out";
    filedir.close();
    QFile file("/sys/class/gpio/gpio" + QString::number(num) + "/value");
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << val;
    file.close();
}

void MainWindow::RefreshScreen()
{
//    QProcess process;

#ifndef Q_OS_WIN32
    //запрещаем бланк экрана
    QFile file("/sys/class/graphics/fb0/blank");
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << 0;
    file.close();

    //убираем курсор (хз как это происходит - получено методом тыка)
    file.setFileName("/sys/class/graphics/fb0/state");
    file.open(QIODevice::WriteOnly);
    out.setDevice(&file);
    out << 1;
    file.close();
#endif

    this->update(); // мы апдейтим нашу главную форму
}

//void Options::WriteSystemOptionsToFile()
//{
//    UpdateCurrentDisplayParametr();

//    QJsonObject systemoptions;
//    systemoptions["Time"] = GetNewTimeString();
//    systemoptions["Date"] = GetNewDateString();
//    systemoptions["Display"] = GetCurrentDisplayParametr();
//    systemoptions["Calibration"] = GetCalibration();
//    systemoptions["Resolution"] = GetNewDisplayResolution();
//    QString setstr = QJsonDocument(systemoptions).toJson(QJsonDocument::Compact);
//    QFile file(pathtosystemoptions);
//    file.open(QIODevice::ReadWrite);
//    file.resize(0); // clear file
//    QTextStream out(&file);
//    out << setstr;
//    file.close();
//}

//void Options::WriteAllChannelsOptionsToFile()
//{
//    QList<ChannelOptions *> ChannelsObjectsList;

//    ChannelsObjectsList.append(&options_channel1);
//    ChannelsObjectsList.append(&options_channel2);
//    ChannelsObjectsList.append(&options_channel3);
//    ChannelsObjectsList.append(&options_channel4);

//    QJsonObject channeljsonobj,options;
//    QJsonObject ustavkijsonobj;
//    QJsonArray settings, settingsUst;

//    foreach (ChannelOptions * Channel, ChannelsObjectsList) {
//            channeljsonobj["Type"] = Channel->GetSignalType();
//            channeljsonobj["Name"] = Channel->GetChannelName();
//            channeljsonobj["Units"] = Channel->GetUnitsName();
//            channeljsonobj["HigherLimit"] = Channel->GetHigherLimit();
//            channeljsonobj["LowerLimit"] = Channel->GetLowerLimit();
//            channeljsonobj["HigherMeasLimit"] = Channel->GetHigherMeasureLimit();
//            channeljsonobj["LowerMeasLimit"] = Channel->GetLowerMeasureLimit();
//            channeljsonobj["Period"] = Channel->GetMeasurePeriod();
//            channeljsonobj["State1HighMessage"] = Channel->GetState1HighMessage();
//            channeljsonobj["State1LowMessage"] = Channel->GetState1LowMessage();
//            channeljsonobj["State2HighMessage"] = Channel->GetState2HighMessage();
//            channeljsonobj["State2LowMessage"] = Channel->GetState2LowMessage();
//            channeljsonobj["MathString"] = Channel->GetMathString();
//            channeljsonobj["MathWork"] = Channel->IsChannelMathematical();
//            channeljsonobj["Diapason"] = Channel->GetDiapason();
//            channeljsonobj["Dempher"] = Channel->GetDempherValue();
//            channeljsonobj["RegistrationType"] = Channel->GetRegistrationType();
//            settings.append(channeljsonobj);
//        }


//    options["count"] = ChannelsObjectsList.length();
//    options["channels"] = settings;

//    foreach (Ustavka * ust, listUstavok)
//    {
//        ustavkijsonobj["UstavkaChannel"] = ust->getChannel();
//        ustavkijsonobj["StateHiValue"] = ust->getHiStateValue();
//        ustavkijsonobj["StateLowValue"] = ust->getLowStateValue();
//        ustavkijsonobj["lowHisteresis"] = ust->getHiHisteresis();
//        ustavkijsonobj["lowLowsteresis"] = ust->getLowHisteresis();
//        ustavkijsonobj["numRelayUp"] = ust->getnumRelayUp();
//        ustavkijsonobj["numRelayDown"] = ust->getnumRelayDown();
//        ustavkijsonobj["MessInHigh"] = ust->getMessInHigh();
//        ustavkijsonobj["MessNormHigh"] = ust->getMessNormHigh();
//        ustavkijsonobj["MessInLow"] = ust->getMessInLow();
//        ustavkijsonobj["MessNormLow"] = ust->getMessNormLow();

//        settingsUst.append(ustavkijsonobj);
//    }

//    options["countUst"] = listUstavok.length();
//    options["ustavki"] = settingsUst;

//    QString setstr = QJsonDocument(options).toJson(QJsonDocument::Compact);
//    QFile file(pathtooptions);
//    QTextStream out(&file);
//    file.open(QIODevice::ReadWrite);
//    file.resize(0); // clear file
//    out << setstr;
//    file.close();
//}

void MainWindow::WriteAllChannelsOptionsToFile()
{
//    QList<ChannelOptions *> ChannelsObjectsList;

//    ChannelsObjectsList.append(&channel1);
//    ChannelsObjectsList.append(&channel2);
//    ChannelsObjectsList.append(&channel3);
//    ChannelsObjectsList.append(&channel4);

//    QJsonObject channeljsonobj,options;
//    QJsonObject ustavkijsonobj;
//    QJsonArray settings, settingsUst;

//    foreach (ChannelOptions * Channel, ChannelsObjectsList) {
//            channeljsonobj["Type"] = Channel->GetSignalType();
//            channeljsonobj["Name"] = (Channel->GetChannelName());
//            channeljsonobj["Units"] = Channel->GetUnitsName();
//            channeljsonobj["HigherLimit"] = Channel->GetHigherLimit();
//            channeljsonobj["LowerLimit"] = Channel->GetLowerLimit();
//            channeljsonobj["HigherMeasLimit"] = Channel->GetHigherMeasureLimit();
//            channeljsonobj["LowerMeasLimit"] = Channel->GetLowerMeasureLimit();
//            channeljsonobj["Period"] = Channel->GetMeasurePeriod();
//            channeljsonobj["State1HighMessage"] = (Channel->GetState1HighMessage());
//            channeljsonobj["State1LowMessage"] = (Channel->GetState1LowMessage());
//            channeljsonobj["State2HighMessage"] = (Channel->GetState2HighMessage());
//            channeljsonobj["State2LowMessage"] = (Channel->GetState2LowMessage());
//            channeljsonobj["MathString"] = Channel->GetMathString();
//            channeljsonobj["MathWork"] = Channel->IsChannelMathematical();
//            channeljsonobj["Diapason"] = Channel->GetDiapason();
//            channeljsonobj["Dempher"] = Channel->GetDempherValue();
//            channeljsonobj["RegistrationType"] = Channel->GetRegistrationType();
//            settings.append(channeljsonobj);
//        }


//    options["count"] = ChannelsObjectsList.length();
//    options["channels"] = settings;

//    foreach (Ustavka * ust, listUstavok)
//    {
////        ustavkijsonobj["Num"] = ust->getNum();
//        ustavkijsonobj["Identifikator"] = ust->getIdentifikator();
//        ustavkijsonobj["UstavkaChannel"] = ust->getChannel();
//        ustavkijsonobj["TypeFix"] = ust->getTypeFix();
//        ustavkijsonobj["StateHiValue"] = ust->getHiStateValue();
////        ustavkijsonobj["StateLowValue"] = ust->getLowStateValue();
//        ustavkijsonobj["lowHisteresis"] = ust->getHiHisteresis();
////        ustavkijsonobj["lowLowsteresis"] = ust->getLowHisteresis();
//        ustavkijsonobj["numRelayUp"] = ust->getnumRelayUp();
////        ustavkijsonobj["numRelayDown"] = ust->getnumRelayDown();
//        ustavkijsonobj["MessInHigh"] = (ust->getMessInHigh());
//        ustavkijsonobj["MessNormHigh"] = (ust->getMessNormHigh());
////        ustavkijsonobj["MessInLow"] = (ust->getMessInLow());
////        ustavkijsonobj["MessNormLow"] = (ust->getMessNormLow());
//        ustavkijsonobj["KvitirUp"] = ust->getKvitirUp();
////        ustavkijsonobj["KvitirDown"] = ust->getKvitirDown();

//        settingsUst.append(ustavkijsonobj);
//    }

//    options["countUst"] = listUstavok.length();
//    options["ustavki"] = settingsUst;

//    QString setstr = QJsonDocument(options).toJson(QJsonDocument::Compact);
//    QFile file(pathtooptions);
//    QTextStream out(&file);
//    file.open(QIODevice::ReadWrite);
//    file.resize(0); // clear file
//    out << setstr;
//    file.close();
}

//void MessageWrite::WriteAllLogToFile()
//{
//    QJsonObject archive;
//    Options opt;
//    double maxmes  = opt.GetLogMessagesLimit();

//    mMessQueue.lock();
//    while (messagesqueue.count()>maxmes) // удаляем все значения что были раньше чем нужно
//    {
//        messagesqueue.removeFirst();
//    }
//    archive["messages"] = messagesqueue;
//    archive["totalmsg"] = messagesqueue.count();
//    mMessQueue.unlock();

//    QString setstr = QJsonDocument(archive).toJson(QJsonDocument::Compact);

//    QThread * threadWriteFile = new QThread;
//    cProcessWriteToFile * wtf = new cProcessWriteToFile;
////    wtf->writeToFile(pathtomessages, setstr);
////    connect(threadWriteFile, SIGNAL(started()), wtf, SLOT(process()));
////    connect(wtf, SIGNAL(finished()), threadWriteFile, SLOT(quit()));
////    connect(wtf, SIGNAL(finished()), wtf, SLOT(deleteLater()));
////    connect(threadWriteFile, SIGNAL(finished()), threadWriteFile, SLOT(deleteLater()));
////    wtf->moveToThread(threadWriteFile);
////    threadWriteFile->start();

////    QFile file(pathtomessages);
////    while(file.isOpen());   //подождать пока файл не закроется другим потоком
////    file.open(QIODevice::ReadWrite);
////    file.resize(0); // clear file
////    QTextStream out(&file);
////    out << setstr;
////    file.close();

//    opt.deleteLater();
//}



//void MessageWrite::LogAddMessage(QString nm)
//{
//    QJsonObject themessage;
//    QDateTime local (QDateTime::currentDateTime());
//    themessage["T"] = local.time().toString();
//    themessage["D"] = local.date().toString("dd/MM/yy");
//    themessage["M"] = nm;
//    mMessQueue.lock();
//    messagesqueue.append(themessage);
//    mMessQueue.unlock();
//}

//void MessageWrite::LogClear()
//{
//    QFile file(pathtomessages);
//    while(file.isOpen());   //подождать пока файл не закроется другим потоком
//    file.open(QIODevice::ReadWrite);
//    file.resize(0); // clear file
//    file.close();
//}

// пишет архив в файл каждые (ArchiveUpdateTimer) сек...
void MainWindow::WriteArchiveToFile()
{


    // и начинаем архивацию
    QJsonObject archivechannel1;
    QJsonObject archivechannel2;
    QJsonObject archivechannel3;
    QJsonObject archivechannel4;
    QJsonArray valuesarray1;
    QJsonArray valuesarray2;
    QJsonArray valuesarray3;
    QJsonArray valuesarray4;
    QJsonObject archive;
    QJsonArray archives;

    for(int y=0; y<vectorsChannels[0].size(); y++)
        valuesarray1.append(QString::number( vectorsChannels[0].at(y), 'f', 3)); // округляем до 3 знаков после запятой
    for(int y=0; y<vectorsChannels[1].size(); y++)
        valuesarray2.append(QString::number( vectorsChannels[1].at(y), 'f', 3)); // округляем до 3 знаков после запятой
    for(int y=0; y<vectorsChannels[2].size(); y++)
        valuesarray3.append(QString::number( vectorsChannels[2].at(y), 'f', 3)); // округляем до 3 знаков после запятой
    for(int y=0; y<vectorsChannels[3].size(); y++)
        valuesarray4.append(QString::number( vectorsChannels[3].at(y), 'f', 3)); // округляем до 3 знаков после запятой

    cGroupChannels * g = listGroup.at(curGroupChannel);
    QString channel1period = "";
    QString channel2period = "";
    QString channel3period = "";
    QString channel4period = "";
    if(g->typeInput[0] == 1)
    {
        channel1period = QString::number(listChannels.at(g->channel[0])->GetMeasurePeriod(), 'f', 1);
    }
    if(g->typeInput[1] == 1)
    {
        channel2period = QString::number(listChannels.at(g->channel[1])->GetMeasurePeriod(), 'f', 1);
    }
    if(g->typeInput[2] == 1)
    {
        channel3period = QString::number(listChannels.at(g->channel[2])->GetMeasurePeriod(), 'f', 1);
    }
    if(g->typeInput[3] == 1)
    {
        channel4period = QString::number(listChannels.at(g->channel[3])->GetMeasurePeriod(), 'f', 1);
    }

    archivechannel1["size"] = valuesarray1.size();
    archivechannel1["values"] = valuesarray1;
    archivechannel1["name"] = "AI4ch1";
    archivechannel1["T"] = channel1period;

    archivechannel2["size"] = valuesarray2.size();
    archivechannel2["values"] = valuesarray2;
    archivechannel2["name"] = "AI4ch2";
    archivechannel2["T"] = channel2period;

    archivechannel3["size"] = valuesarray3.size();
    archivechannel3["values"] = valuesarray3;
    archivechannel3["name"] = "AI4ch3";
    archivechannel3["T"] = channel3period;

    archivechannel4["size"] = valuesarray4.size();
    archivechannel4["values"] = valuesarray4;
    archivechannel4["name"] = "AI4ch4";
    archivechannel4["T"] = channel4period;

    archives.append(archivechannel1);
    archives.append(archivechannel2);
    archives.append(archivechannel3);
    archives.append(archivechannel4);

    QDateTime CurrentDateTime(QDateTime::currentDateTime());
    archive["Archives"] = archives;
    archive["StartDate"] = MainWindow::startdate;
    archive["StartTime"] = MainWindow::starttime;
    archive["EndDate"] = CurrentDateTime.toString("dd/MM/yy");
    archive["EndTime"] = CurrentDateTime.toString("hh:mm:ss");

    MainWindow::startdate = CurrentDateTime.toString("dd/MM/yy");
    MainWindow::starttime = CurrentDateTime.toString("hh:mm:ss");

    QString setstr = QJsonDocument(archive).toJson(QJsonDocument::Compact);

    QString pathtologs = pathtolog;
    // создаем файл с именем текстового файла архива
    pathtologs.append(CurrentDateTime.toString("ddMMyy"));
    pathtologs.append(CurrentDateTime.toString("hhmmss"));
    pathtologs.append(".txt");
    QFile file(pathtologs);
    file.open(QIODevice::ReadWrite);
    file.resize(0); // clear file
    QTextStream out(&file);
    out << setstr;
    file.close();
}

void MainWindow::CreateMODBusConfigFile() // создает файл дескрипотор системы
{
    QJsonArray Devices;
    QJsonObject Device1;
    QJsonObject Device2;
    QJsonObject Device;

    QJsonArray Device1ReadCommands;
    QJsonArray Device1WriteCommands;
    QJsonArray Registration1,Registration2;

    QJsonArray Device2ReadCommands;
    QJsonArray Device2WriteCommands;

    // конфиг для платы 4AI
    Device1ReadCommands.append(0x04); // на чтение аналогового ввода? Команда 0x04
    Device1WriteCommands.append(0x06); // на запись аналогового вывода? Команда 0x06

    Registration1.append("Screen");
    Registration1.append("Memory");

    Device1["Address"] = 0x01;
    Device1["Name"] = "4AI";
    Device1["Speed"] = 9600;
    Device1["Other"] = "Other";
    Device1["Stuff"] = "Stuff";
    Device1["ReadCommands"] = Device1ReadCommands;
    Device1["WriteCommands"] = Device1WriteCommands;
    Device1["DataType"] = "float";
    Device1["Registration"] = Registration1;


    // конфиг для платы Relays
    Device2ReadCommands.append(0x01); //чтение дискретного вывода? Команда 0x01
    Device2WriteCommands.append(0x05); // на запись дискретного вывода? Команда 0x05
    Registration1.append("Memory");

    Device2["Address"] = 0x02;
    Device2["Name"] = "Relay";
    Device2["Speed"] = 115200;
    Device2["Other"] = "Relay";
    Device2["Stuff"] = "Module";
    Device2["ReadCommands"] = Device2ReadCommands;
    Device2["WriteCommands"] = Device2WriteCommands;
    Device2["DataType"] = "bool";
    Device2["Registration"] = Registration2;

    Devices.append(Device1);
    Devices.append(Device2);

    Device["Count"] = Devices.count();
    Device["Devices"] = Devices;

    QString setstr = QJsonDocument(Device).toJson(QJsonDocument::Compact);
    QFile file(pathtomodbusconfigfile);
    file.open(QIODevice::ReadWrite);
    file.resize(0); // clear file
    QTextStream out(&file);
    out << setstr;
    file.close();
}

// Vag: врменно---------------------
QJsonObject MainWindow::ObjectFromString(const QString& in)
{
    QJsonValue val(in);
    return val.toObject();
}
//----------------------------------------

void MainWindow::writeArchiveSteel(int steelNum)
{
    QJsonObject archive;
    QJsonArray archives;
    QJsonObject eds;
    QJsonObject temp;
    QJsonArray valEds;
    QJsonArray valTemp;
    cSteel * steel = listSteel.at(steelNum);

    for(int i = 0; i < steel->vectorEds.size(); i++)
    {
        valEds.append(QString::number(steel->vectorEds.at(i), 'f', 0));
    }
    for(int i = 0; i < steel->vectorTemp.size(); i++)
    {
        valTemp.append(QString::number(steel->vectorTemp.at(i), 'f', 0));
    }

    eds["name"] = "Eds";
    eds["size"] = steel->vectorEds.size();
    eds["values"] = valEds;

    temp["name"] = "Temp";
    temp["size"] = steel->vectorTemp.size();
    temp["values"] = valTemp;

    archives.append(eds);
    archives.append(temp);

//    QDateTime CurrentDateTime(QDateTime::currentDateTime());
    archive["Archives"] = archives;
    archive["Date"] = steel->timeUpdateData.toString("dd/MM/yy");
    archive["Time"] = steel->timeUpdateData.toString("hh:mm:ss");
    archive["Input"] = steelNum;
    archive["Technology"] = steel->technology->name;
    archive["Smelt"] = QString::number(steel->numSmelt);
    archive["Temp"] = QString::number(steel->temp, 'f', 2);
    archive["Eds"] = QString::number(steel->eds, 'f', 2);
    archive["OxActivity"] = QString::number(steel->ao, 'f', 2);
    archive["Carbon"] = QString::number(steel->cl, 'f', 2);
    archive["MassAl"] = QString::number(steel->alg, 'f', 2);


    QString setstr = QJsonDocument(archive).toJson(QJsonDocument::Compact);
    QString pathtologs = pathtolog;
    pathtologs.append("steel_");
    pathtologs.append(steel->timeUpdateData.toString("ddMMyy"));
    pathtologs.append(steel->timeUpdateData.toString("hhmmss"));
    pathtologs.append(".txt");
    QFile file(pathtologs);
    file.open(QIODevice::ReadWrite);
    file.resize(0); // clear file
    QTextStream out(&file);
    out << setstr;
    file.close();
}

