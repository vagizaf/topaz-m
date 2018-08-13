#include "menu.h"
#include "ui_menu.h"
#include "defines.h"
#include "filemanager.h"
#include "keyboard.h"
#include <options.h>
#include <QFile>
#include <QString>

#define HEIGHT 768
#define WIDTH 1024
#define TIME_UPDATE DateLabelUpdateTimer

extern int dateindex;
extern int timeindex;
extern QStringList datestrings, timestrings;
extern QVector<double> X_Coordinates_archive, Y_coordinates_Chanel_1_archive, Y_coordinates_Chanel_2_archive, Y_coordinates_Chanel_3_archive, Y_coordinates_Chanel_4_archive;
extern QList<cSteel*> listSteel;
extern typeSteelTech steelTech[];
extern cChannelSlotController csc;
extern cSteelController ssc;
extern cSystemOptions systemOptions;  //класс хранения состемных опций


dMenu::dMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dMenu)
{


    ui->setupUi(this);

    ui->saveButton->setColorText(ColorBlue);
    ui->saveButton->setColorBg(QColor(0xff,0xff,0xff));
    ui->exitButton->setColorText(ColorBlue);
    ui->exitButton->setColorBg(QColor(0xff,0xff,0xff));

    QList<wButtonStyled *> buttons = ui->stackedWidget->findChildren<wButtonStyled *>();
    foreach(wButtonStyled * button, buttons)
    {
        button->setColorText(QColor(0xff,0xff,0xff));
        button->setColorBg(ColorButtonNormal);
        button->setAlignLeft();
    }

    //фильтр для поля редактирования даты
    ui->dateEdit->installEventFilter(this);

    ui->load->setHidden(true);
    mo.setFileName(pathtoloadgif);
    ui->load->setMovie(&mo);
    mo.start();

    connect(&timerLoad, SIGNAL(timeout()), this, SLOT(timeoutLoad()));

    QPixmap pix(pathtologotip);
    ui->label->setPixmap(pix);
    ui->label->setScaledContents(true);

    ui->stackedWidget->setCurrentIndex(0);
    ui->frameNameSubMenu->setHidden(true);

    connect(&tUpdateTime, SIGNAL(timeout()), this, SLOT(DateUpdate()));
    tUpdateTime.start(TIME_UPDATE);
    DateUpdate();

    updateSystemOptions();

//    //в зависимости от подключенных плат определям, что показывать или не показывать в меню
//    if(csc.isConnect())
//    {
//        ui->frameAnalogModes->show();
//        ui->radioButAnalogModes->setEnabled(true);
//        ui->radioButAnalogModes->setChecked(true);
//        ui->bAnalog->show();
//    }
//    else
//    {
//        ui->frameAnalogModes->hide();
//        ui->radioButAnalogModes->setEnabled(false);
//        ui->bAnalog->hide();
//    }
//    if(ssc.isConnect())
//    {
//        ui->radioButSteelModes->setEnabled(true);
//        ui->bSteel->show();
//        if(!csc.isConnect())
//        {
//            ui->frameSteelMode->show();
//            ui->radioButSteelModes->setChecked(true);
//        }
//        else
//        {
//           ui->frameSteelMode->hide();
//        }
//    }
//    else
//    {
//        ui->radioButSteelModes->setEnabled(false);
//        ui->frameSteelMode->hide();
//        ui->bSteel->hide();
//    }


}

bool dMenu::eventFilter(QObject *object, QEvent *event)
{
#ifndef Q_OS_WIN
    if ( (event->type() == QEvent::MouseButtonRelease) && \
         (object->property("enabled").toString() == "true") && \
         (QString::fromLatin1(object->metaObject()->className()) == "QDateTime") )
    {
//        QCalendarWidget cw;
//        cw.show();
        Options::olderprop = object->property("date").toString();
        keyboard kb(this);
        kb.setModal(true);
        kb.exec();
    }
#endif
    return QObject::eventFilter(object, event);
}

dMenu::~dMenu()
{
    delete ui;
}

void dMenu::on_exitButton_clicked()
{
    this->close();
}

void dMenu::on_saveButton_clicked()
{
    ui->load->setHidden(false);
//     засекаем время записи настроек в файл или ждать сигнал о завершении
    timerLoad.start(1000);
    //  запись файла //
    sysOptions.arrows = ui->arrowscheckBox->checkState();
    sysOptions.display = ui->modeGraf->currentIndex();
    sysOptions.display += (ui->modeBar->currentIndex() << 2);
    if(ui->radioButSteelModes->isChecked())
    {
        sysOptions.display = cSystemOptions::Steel;
    }
    sysOptions.autoscale = ui->autoscalecheckbox->isChecked();
    cFileManager::writeSystemOptionsToFile(pathtosystemoptions, &sysOptions);
    emit saveButtonSignal();
    //Окно закроется по сигналу таймаута
}

void dMenu::updateSystemOptions()
{
    cFileManager::readSystemOptionsFromFile(pathtosystemoptions, &sysOptions);
    ui->arrowscheckBox->setChecked(sysOptions.arrows);
    ui->modeBar->setCurrentIndex((sysOptions.display >> 2) & 4);
    ui->modeGraf->setCurrentIndex(sysOptions.display & 3);
    if(ssc.isConnect() && csc.isConnect())
    {
        ui->groupBoxTypePribor->show();
        if(sysOptions.display == cSystemOptions::Steel)
        {
            ui->radioButAnalogModes->setChecked(false);
            ui->radioButSteelModes->setChecked(true);
            ui->frameAnalogModes->hide();
            ui->frameSteelMode->show();
        } else {
            ui->radioButSteelModes->setChecked(false);
            ui->radioButAnalogModes->setChecked(true);
        }
    }
    else
    {
        ui->groupBoxTypePribor->hide();
    }
    ui->autoscalecheckbox->setChecked(sysOptions.autoscale);
}

void dMenu::timeoutLoad()
{
    this->close();
}

void dMenu::on_bWork_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->nameSubMenu->setText("РАБОТА");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bBack_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
//    ui->nameSubMenu->setText("");
    ui->frameNameSubMenu->setHidden(true);
}


void dMenu::on_bSettings_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bBackFromSettings_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
//    ui->nameSubMenu->setText("");
    ui->frameNameSubMenu->setHidden(true);
}

void dMenu::on_bBackFromSystem_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
//    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bBackFromInputs_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
//    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bInputs_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->nameSubMenu->setText("ВХОДЫ");
//    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bSystem_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->nameSubMenu->setText("СИСТЕМА");
}

void dMenu::DateUpdate() // каждую секунду обновляем значок времени
{
    QDateTime local(QDateTime::currentDateTime());
    QString str = "<html><head/><body><p align=\"center\"><span style=\" font-size:22pt; color:#ffffff;\">" \
                  + local.time().toString(timestrings.at(timeindex)) + \
                  "</span><span style=\" color:#ffffff;\"><br/></span>" \
                  "<span style=\" font-size:17pt; color:#ffffff;\">" \
                  + local.date().toString(datestrings.at(dateindex)) + \
                  "</span></p></body></html>";
    ui->date_time->setText(str);
}


void dMenu::on_bBackChannels_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->nameSubMenu->setText("ВХОДЫ");
}

void dMenu::on_bAnalog_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    ui->nameSubMenu->setText("АНАЛОГ. ВХОДЫ");
}

void dMenu::openSettingsChannel(int num, int page)
{

    if(page == 0)
    {
        //проверка на наличие такого номера канала
        if((num <= 0) || (num > listChannels.size())) return;

        dialogSetingsChannel = new dSettings(listChannels, listUstavok, num, page);
        dialogSetingsChannel->exec();
        dialogSetingsChannel->deleteLater();
        //sendConfigChannelsToSlave();
    }
    else if(page == 4)
    {
        //проверка на наличие такого номера входной группы
        if((num <= 0) || (num > listSteel.size())) return;
        dialogSetingsChannel = new dSettings(listChannels, listUstavok, num, page);
//        dialogSetingsChannel->addSteel(listSteel.at(num), steelTech);
        dialogSetingsChannel->exec();
        dialogSetingsChannel->deleteLater();

    }

}

void dMenu::on_bChannel1_clicked()
{
    openSettingsChannel(1);
}

void dMenu::on_bChannel2_clicked()
{
    openSettingsChannel(2);
}

void dMenu::on_bChannel3_clicked()
{
    openSettingsChannel(3);
}

void dMenu::on_bChannel4_clicked()
{
    openSettingsChannel(4);
}

void dMenu::on_bApplication_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->nameSubMenu->setText("ПРИЛОЖЕНИЯ");
}

void dMenu::on_bBackApplications_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
}

void dMenu::on_bUstavki_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
    ui->nameSubMenu->setText("УСТАВКИ");
}

void dMenu::on_bBackApplications_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->nameSubMenu->setText("ПРИЛОЖЕНИЯ");
}


void dMenu::on_bDiagnost_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->nameSubMenu->setText("ДИАГНОСТИКА");
}

void dMenu::on_bBackDiagnostika_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
}

void dMenu::addChannels(QList<ChannelOptions *> channels, QList<Ustavka*> ustavki)
{
    foreach (ChannelOptions * ch, channels) {
        listChannels.append(ch);
    }
    foreach (Ustavka * ust, ustavki) {
        listUstavok.append(ust);
    }
}

void dMenu::selectPageWork()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->nameSubMenu->setText("РАБОТА");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bUstavka_1_clicked()
{
    openSettingsChannel(1, 3);
}

void dMenu::on_bUstavka_2_clicked()
{
    openSettingsChannel(2, 3);
}

void dMenu::on_bUstavka_3_clicked()
{
    openSettingsChannel(3, 3);
}

void dMenu::on_bUstavka_4_clicked()
{
    openSettingsChannel(4, 3);
}

void dMenu::on_bSteel1_clicked()
{
    openSettingsChannel(1, 4);
}

void dMenu::on_bSteel2_clicked()
{
    openSettingsChannel(2, 4);
}

void dMenu::on_bSteel3_clicked()
{
    openSettingsChannel(3, 4);
}

void dMenu::on_bSteel4_clicked()
{
    openSettingsChannel(4, 4);
}

void dMenu::on_bBackDateTime_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->nameSubMenu->setText("СИСТЕМА");
}

void dMenu::on_bExpert_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->nameSubMenu->setText("ЭКСПЕРТ");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bBackExpert_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->frameNameSubMenu->setHidden(true);
}

void dMenu::on_bAnaliz_clicked()
{
    UpdateAnalyze();
    ui->stackedWidget->setCurrentIndex(11);
    ui->nameSubMenu->setText("АНАЛИЗ");
}

void dMenu::on_bBackExpert_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->nameSubMenu->setText("РАБОТА");
}

void dMenu::on_bOptions_clicked()
{
    ui->stackedWidget->setCurrentIndex(12);
    ui->nameSubMenu->setText("ОПЦИИ");
}

void dMenu::on_bBackSystemOptions_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->nameSubMenu->setText("СИСТЕМА");
}

void dMenu::on_bModeDiagram_clicked()
{
    ui->stackedWidget->setCurrentIndex(13);
    ui->nameSubMenu->setText("ОТОБРАЖЕНИЕ");
}

void dMenu::on_bBackOtobrazhenie_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->nameSubMenu->setText("РАБОТА");
}


void dMenu::on_bBackSteel_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->nameSubMenu->setText("ВХОДЫ");
}

void dMenu::on_bEditDataTime_clicked()
{
    QDateTime local(QDateTime::currentDateTime());
    ui->timeEdit->setTime(local.time());
    ui->timeEdit->setDisplayFormat(timestrings.at(timeindex));
    ui->dateEdit->setDate(local.date());
    ui->dateEdit->setDisplayFormat(datestrings.at(dateindex));
    ui->DateFormat->clear();
    ui->DateFormat->addItems(datestrings);
    ui->DateFormat->setCurrentIndex(dateindex);
    ui->timeformat->clear();
    ui->timeformat->addItems(timestrings);
    ui->timeformat->setCurrentIndex(timeindex);
    ui->stackedWidget->setCurrentIndex(9);
    ui->nameSubMenu->setText("ДАТА/ВРЕМЯ");
}

void dMenu::on_bBackDateTimeSet_clicked()
{

#ifndef WIN32
    QProcess process;
    QDateTime newuidate = ui->dateEdit->dateTime();
    QTime newuitime = ui->timeEdit->time();

    QString newdate = QString::number(newuidate.date().year()) + "-" + QString::number(newuidate.date().month()) + "-" + QString::number(newuidate.date().day()) ;
    QString newtime = newuitime.toString();

    process.startDetached("date --set " + newdate);
    process.startDetached("date --set " + newtime); // max freq on

#endif
    dateindex = ui->DateFormat->currentIndex();
    timeindex = ui->timeformat->currentIndex();
    ui->timeEdit->setDisplayFormat(timestrings.at(timeindex));
    ui->dateEdit->setDisplayFormat(datestrings.at(dateindex));

}



void dMenu::on_bResetToDefault_clicked()
{
    //замена файлов настроек
    //настройки каналов
    QFile::remove(pathtooptions + QString(".backup"));
    QFile::rename(pathtooptions, pathtooptions + QString(".backup"));
    QFile::copy(pathtooptionsdef, pathtooptions);
    //системные настройки
    QFile::remove(pathtosystemoptions + QString(".backup"));
    QFile::rename(pathtosystemoptions, pathtosystemoptions + QString(".backup"));
    QFile::copy(pathtosystemoptionsdef, pathtosystemoptions);
    //чтение и применение настроек из новых файлов
    cFileManager::readChannelsSettings(pathtooptions, listChannels, listUstavok);
    updateSystemOptions();
    emit saveButtonSignal();

}





void dMenu::UpdateAnalyze()
{
    double averagechannel_1 , averagechannel_2 , averagechannel_3,averagechannel_4 , sum;

    averagechannel_1 = averagechannel_2 = averagechannel_3 = averagechannel_4 = 0.0;

    averagechannel_1 = mathresolver::dGetAverageValue(Y_coordinates_Chanel_1_archive);
    averagechannel_2 = mathresolver::dGetAverageValue(Y_coordinates_Chanel_2_archive);
    averagechannel_3 = mathresolver::dGetAverageValue(Y_coordinates_Chanel_3_archive);
    averagechannel_4 = mathresolver::dGetAverageValue(Y_coordinates_Chanel_4_archive);

    double minimumchannel_1 = mathresolver::dGetMinimumValue(Y_coordinates_Chanel_1_archive);
    double maximumchannel_1 = mathresolver::dGetMaximumValue(Y_coordinates_Chanel_1_archive);

    double minimumchannel_2 = mathresolver::dGetMinimumValue(Y_coordinates_Chanel_2_archive);
    double maximumchannel_2 = mathresolver::dGetMaximumValue(Y_coordinates_Chanel_2_archive);

    double minimumchannel_3 = mathresolver::dGetMinimumValue(Y_coordinates_Chanel_3_archive);
    double maximumchannel_3 = mathresolver::dGetMaximumValue(Y_coordinates_Chanel_3_archive);

    double minimumchannel_4 = mathresolver::dGetMinimumValue(Y_coordinates_Chanel_4_archive);
    double maximumchannel_4 = mathresolver::dGetMaximumValue(Y_coordinates_Chanel_4_archive);

    ui->analizenameChannel_1->setText( listChannels.at(0)->GetChannelName() \
                                        + " [" + listChannels.at(0)->GetUnitsName() + "]" );
    ui->analizenameChannel_2->setText( listChannels.at(1)->GetChannelName() \
                                       + " [" + listChannels.at(1)->GetUnitsName() + "]" );
    ui->analizenameChannel_3->setText( listChannels.at(2)->GetChannelName() \
                                       + " [" + listChannels.at(2)->GetUnitsName() + "]" );
    ui->analizenameChannel_4->setText( listChannels.at(3)->GetChannelName() \
                                       + " [" + listChannels.at(3)->GetUnitsName() + "]" );

    ui->analizeavgvaluechannel_1->setText(QString::number(averagechannel_1, 'f', 2));
    ui->analizeavgvaluechannel_2->setText(QString::number(averagechannel_2, 'f', 2));
    ui->analizeavgvaluechannel_3->setText(QString::number(averagechannel_3, 'f', 2));
    ui->analizeavgvaluechannel_4->setText(QString::number(averagechannel_4, 'f', 2));

    ui->analizeminvaluechannel_1->setText(QString::number(minimumchannel_1));
    ui->analizeminvaluechannel_2->setText(QString::number(minimumchannel_2));
    ui->analizeminvaluechannel_3->setText(QString::number(minimumchannel_3));
    ui->analizeminvaluechannel_4->setText(QString::number(minimumchannel_4));

    ui->analizemaxvaluechannel_1->setText(QString::number(maximumchannel_1));
    ui->analizemaxvaluechannel_2->setText(QString::number(maximumchannel_2));
    ui->analizemaxvaluechannel_3->setText(QString::number(maximumchannel_3));
    ui->analizemaxvaluechannel_4->setText(QString::number(maximumchannel_4));
}






void dMenu::on_bSteel_clicked()
{
    ui->stackedWidget->setCurrentIndex(14);
    ui->nameSubMenu->setText("СТАЛЬ");
}





void dMenu::on_radioButAnalogModes_clicked()
{
    ui->frameAnalogModes->show();
    ui->frameSteelMode->hide();
}

void dMenu::on_radioButSteelModes_clicked()
{
    ui->frameAnalogModes->hide();
    ui->frameSteelMode->show();
}