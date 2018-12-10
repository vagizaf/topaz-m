#include "menu.h"
#include "ui_menu.h"
#include "defines.h"
#include "filemanager.h"
#include "keyboard.h"
#include "expert_access.h"
//#include <options.h>
#include <QFile>
#include <QDir>
#include <QString>
#include "assert.h"
#include "usb_flash.h"
#include "Channels/group_channels.h"
#include "device_slot.h"
#include "ip_controller.h"


#define HEIGHT 768
#define WIDTH 1024
#define TIME_UPDATE DateLabelUpdateTimer
#define TIME_UPDATE_DEVICE_UI   500
#define DRIVE_UPDATE 500
#define TIME_UPD_DIAGNOSTIC     500

//cExpertAccess access;

extern int dateindex;
extern int timeindex;
extern QStringList datestrings, timestrings;
extern QVector<double> X_Coordinates_archive, Y_coordinates_Chanel_1_archive, Y_coordinates_Chanel_2_archive, Y_coordinates_Chanel_3_archive, Y_coordinates_Chanel_4_archive;
extern QList<cDevice*> listDevice;
extern QList<ChannelOptions *> listChannels;
extern QList<Ustavka *> listUstavok;
extern QList<cSteel*> listSteel;
extern QList<cRelay*> listRelais;
extern typeSteelTech steelTech[];
extern cChannelSlotController csc;
extern cSteelController ssc;
extern cSystemOptions systemOptions;  //класс хранения состемных опций
extern cUsbFlash * flash;
extern QList<cGroupChannels*> listGroup;
extern cIpController * ethernet;



dMenu::dMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dMenu)
{
//    QTime time;
//    time.start();
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);

    QString ver = CURRENT_VER;
    ui->name->setText(QString("<html><head/><body><p align=\"center\"><span style=\" color:#ffffff;\">MULTIGRAPH<br/>Ver. " + ver + "</span></p></body></html>"));

    ui->saveButton->setColorText(ColorBlue);
    ui->saveButton->setColorBg(QColor(0xff,0xff,0xff));
    ui->exitButton->setColorText(ColorBlue);
    ui->exitButton->setColorBg(QColor(0xff,0xff,0xff));

    ui->load->setHidden(true);
    mo.setFileName(pathtoloadgif);
    ui->load->setMovie(&mo);
    ui->progressBarLoadFiles->setHidden(true);
//    mo.start();       //замедляет скорость открытия окна на 250 мсек

    connect(&timerLoad, SIGNAL(timeout()), this, SLOT(timeoutLoad()));

    QPixmap pix(pathtologotip);
    ui->label->setPixmap(pix);
    ui->label->setScaledContents(true);

    ui->stackedWidget->setCurrentIndex(0);
    ui->frameNameSubMenu->setHidden(true);

    ui->stackedWidgetDIFunc->setCurrentIndex(0);

    connect(&tUpdateTime, SIGNAL(timeout()), this, SLOT(DateUpdate()));
    tUpdateTime.start(TIME_UPDATE);
    DateUpdate();

    connect(&tUpdateDiagnostic, SIGNAL(timeout()), this, SLOT(updateLabelDiagnostic()));
    connect(&tUpdateDiagnostic, SIGNAL(timeout()), this, SLOT(updateLabelModeling()));
    tUpdateDiagnostic.start(TIME_UPD_DIAGNOSTIC);

    //периодические обновления виджетов информации о платах
    connect(&tUpdateDeviceUI, SIGNAL(timeout()), this, SLOT(updateDevicesUI()));
    tUpdateDeviceUI.start(TIME_UPDATE_DEVICE_UI);
    curDiagnostDevice = 0;

    connect(flash, SIGNAL(newFlash(int)), this, SLOT(updateDriversWidgets()));

    updateSystemOptions();


    QList<wButtonStyled *> buttons = ui->stackedWidget->findChildren<wButtonStyled *>();
    foreach(wButtonStyled * button, buttons)
    {
        button->setColorText(QColor(0xff,0xff,0xff));
        button->setColorBg(ColorButtonNormal);
        button->setAlignLeft();
    }

    QScroller::grabGesture(ui->scrollAreaUstavki, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->scrollAreaDI, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->scrollAreaDigitalOutputs, QScroller::LeftMouseButtonGesture);

    log = new cLogger(pathtomessages, cLogger::UI);
    log->addMess("Menu > Open ", cLogger::SERVICE);

    //добавить виджеты групп каналов
    addWidgetGroup();
    addWidgetChannels();

    light = systemOptions.brightness;
    ui->volLight->setText(QString::number(light));
    ui->progressLight->setValue(light);

    QScroller::grabGesture(ui->listWidget->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->listDeviceErrors->viewport(), QScroller::LeftMouseButtonGesture);

//    connect(ethernet, SIGNAL(signalErrIp()), this, SLOT(slotIpErr()));
//    connect(ethernet, SIGNAL(signalStatus(bool,bool)), \
//            this, SLOT(slotUpdateEthernetStatus(bool,bool)));
    ui->ipAddr->installEventFilter(this);
    ui->netMask->installEventFilter(this);
    ui->gateWay->installEventFilter(this);

//    qDebug() << "Time start dMenu:" << time.elapsed();
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
        keyboard::olderprop = object->property("date").toString();
        keyboard kb(this);
        kb.setModal(true);
        kb.exec();
        object->setProperty("text",kb.getcustomstring() );
        object->setProperty("value",kb.getcustomstring() );
    }
#endif

    if ( (event->type() == QEvent::MouseButtonRelease) && \
         (object->objectName().contains("bModeling")))
    {
        if(QString::fromLatin1(object->metaObject()->className()) == "QPushButton")
        {
            QPushButton * widget = (QPushButton*)object;
            widget->setStyleSheet("background-color: rgb(230, 230, 230);\n"
                                  "color: rgb(0, 0, 0);\n"
                                  "border-radius: 0px;");
        }
        QStringList listParam = object->objectName().split('_');
        int num = listParam.at(2).toInt();
        if(listParam.at(1) == "On") listRelais.at(num)->setState(true);
        else if(listParam.at(1) == "Off") listRelais.at(num)->setState(false);
        //возвращать цвет кнопки

    }

    if ( (event->type() == QEvent::MouseButtonPress) && \
         (object->objectName().contains("bModeling")))
    {
        if(QString::fromLatin1(object->metaObject()->className()) == "QPushButton")
        {
            QPushButton * widget = (QPushButton*)object;
            //менять цвет кнопки
            widget->setStyleSheet("background-color: rgb(180, 180, 180);\n"
                                  "color: rgb(0, 0, 0);\n"
                                  "border-radius: 0px;");
        }
    }

#ifndef Q_OS_WIN
    if ( (event->type() == QEvent::MouseButtonPress) && \
         (object->property("enabled").toString() == "true") && \
         ((object->objectName() == "ipAddr") ||\
          (object->objectName() == "netMask") ||\
          (object->objectName() == "gateWay")))
    {
        if(QString::fromLatin1(object->metaObject()->className()) == "QLineEdit")
        {
            keyboard::olderprop = object->property("text").toString();
            keyboard kb(this);
            kb.setModal(true);
            kb.exec();
            object->setProperty("text",kb.getcustomstring() );
        }
    }
#endif

    return QObject::eventFilter(object, event);
}

dMenu::~dMenu()
{
//    disconnect(ethernet, SIGNAL(signalErrIp()), this, SLOT(slotIpErr()));
//    disconnect(ethernet, SIGNAL(signalStatus(bool,bool)), \
            this, SLOT(slotUpdateEthernetStatus(bool,bool)));
    delete ui;
}

void dMenu::on_exitButton_clicked()
{
    log->addMess("Menu > Cancel", cLogger::SERVICE);
    this->close();
}

void dMenu::on_saveButton_clicked()
{
    mo.start();
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
    sysOptions.brightness = light;
//    setBrightness(light);
    cFileManager::writeSystemOptionsToFile(pathtosystemoptions, &sysOptions);
    log->addMess("Menu > Save", cLogger::SERVICE);
    emit saveButtonSignal();
    //Окно закроется по сигналу таймаута
}

void dMenu::updateSystemOptions(QString path)
{
    cFileManager::readSystemOptionsFromFile(path, &sysOptions);
    ui->arrowscheckBox->setChecked(sysOptions.arrows);
    ui->modeBar->setCurrentIndex((sysOptions.display >> 2) % ui->modeBar->count());
    ui->modeGraf->setCurrentIndex(sysOptions.display & 3);
    if(ssc.isConnect() && (listChannels.size() > 0))
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
            ui->frameSteelMode->hide();
        }
    }
    else
    {
        ui->groupBoxTypePribor->hide();
    }
    ui->autoscalecheckbox->setChecked(sysOptions.autoscale);
}

void dMenu::addWidgetUstavki()
{
    clearLayout(ui->verticalLayoutUstavki);

    // генерация виджетов (кнопок) уставок
    int i = 0;
    foreach (Ustavka * u, listUstavok) {
        wButtonStyled * bUstavka = new wButtonStyled(ui->widgetScrollAreaUstavki);
        bUstavka->index = i+1;
        QString nameUstavka = u->getIdentifikator().size() ? (" | " + u->getIdentifikator()) : " ";
        bUstavka->setText("УСТАВКА " + QString::number(bUstavka->index) + nameUstavka);
        bUstavka->setMinimumSize(QSize(0, 70));
        bUstavka->setColorText(QColor(0xff,0xff,0xff));
        bUstavka->setColorBg(ColorButtonNormal);
        bUstavka->setAlignLeft();
        connect(bUstavka, SIGNAL(clicked(int)), this, SLOT(slotOpenSettings(int)));
        ui->verticalLayoutUstavki->addWidget(bUstavka);
        i++;
    }
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayoutUstavki->addItem(verticalSpacer);
}

void dMenu::addWidgetGroup()
{

    clearLayout(ui->verticalLayoutGroup);

    // генерация виджетов (кнопок) уставок
    int i = 0;
    foreach (cGroupChannels * group, listGroup) {
        wButtonStyled * bGroup = new wButtonStyled(ui->widgetScrollAreaGroup);
        bGroup->index = i+1;
        QString nameGroup = group->groupName.size() ? (" | " + group->groupName) : " ";
        bGroup->setText("ГРУППА " + QString::number(bGroup->index) + nameGroup);
        bGroup->setMinimumSize(QSize(0, 70));
        bGroup->setColorText(QColor(0xff,0xff,0xff));
        bGroup->setColorBg(ColorButtonNormal);
        bGroup->setAlignLeft();
        connect(bGroup, SIGNAL(clicked(int)), this, SLOT(slotOpenGroup(int)));
        ui->verticalLayoutGroup->addWidget(bGroup);
        i++;
    }
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayoutGroup->addItem(verticalSpacer);
}

void dMenu::addWidgetChannels()
{
    clearLayout(ui->verticalLayoutChannels);

    // генерация виджетов (кнопок) уставок
    int i = 0;
    foreach (ChannelOptions * channel, listChannels) {
        wButtonStyled * bChannel = new wButtonStyled(ui->widgetScrollAreaChannels);
        bChannel->index = i+1;
        QString nameChannel = channel->GetChannelName().size() ? (" | " + channel->GetChannelName()) : " ";
        bChannel->setText("КАНАЛ " + QString::number(bChannel->index) + nameChannel);
        bChannel->setMinimumSize(QSize(0, 70));
        bChannel->setColorText(QColor(0xff,0xff,0xff));
        bChannel->setColorBg(ColorButtonNormal);
        bChannel->setAlignLeft();
        connect(bChannel, SIGNAL(clicked(int)), this, SLOT(slotOpenChannel(int)));
        ui->verticalLayoutChannels->addWidget(bChannel);
        i++;
    }
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayoutChannels->addItem(verticalSpacer);
}

void dMenu::addWidgetDigitOutputs()
{
    clearLayout(ui->verticalLayoutDigitalOutputs);

    int i = 0;
    foreach (cRelay * relay, listRelais) {
        wButtonStyled * bOutput = new wButtonStyled(ui->scrollAreaWidgetDigitalOutputs);
        bOutput->index = i+1;
        bOutput->setText("ВЫХОД " + QString::number(bOutput->index));
        bOutput->setMinimumSize(QSize(0, 70));
        bOutput->setColorText(QColor(0xff,0xff,0xff));
        bOutput->setColorBg(ColorButtonNormal);
        bOutput->setAlignLeft();
        connect(bOutput, SIGNAL(clicked(int)), this, SLOT(slotOpenDigitOutput(int)));
        ui->verticalLayoutDigitalOutputs->addWidget(bOutput);
        i++;
    }
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayoutDigitalOutputs->addItem(verticalSpacer);
}

void dMenu::addWidgetMeasures()
{
    clearLayout(ui->verticalLayoutMeasures);
    listLabelDiagnostic.clear();
    // генерация виджетов
    int i = 0;
    foreach (ChannelOptions * channel, listChannels) {
        QFont font2;
        font2.setFamily(QStringLiteral("Open Sans"));
        font2.setPointSize(20);


        QFrame * frameMeasure1 = new QFrame(ui->widgetScrollAreaMeasures);
        frameMeasure1->setFrameShape(QFrame::NoFrame);
        frameMeasure1->setFrameShadow(QFrame::Raised);

        QHBoxLayout * horizontalLayout_3 = new QHBoxLayout(frameMeasure1);

        QLabel * labelMeasure1 = new QLabel(frameMeasure1);
        labelMeasure1->setFont(font2);
        labelMeasure1->setText("КАНАЛ " + QString::number(channel->getNum()));
        horizontalLayout_3->addWidget(labelMeasure1);

        QLabel * labelNameMeasure1 = new QLabel(frameMeasure1);
        labelNameMeasure1->setFont(font2);
        labelNameMeasure1->setAlignment(Qt::AlignCenter);
        labelNameMeasure1->setText(channel->GetChannelName());
        horizontalLayout_3->addWidget(labelNameMeasure1);
        ui->verticalLayoutMeasures->addWidget(frameMeasure1);

        QLabel * volMeasure1 = new QLabel(frameMeasure1);
        volMeasure1->setMinimumSize(QSize(131, 31));
        volMeasure1->setMaximumSize(QSize(185, 45));
        volMeasure1->setFont(font2);
        volMeasure1->setStyleSheet(QLatin1String("	background-color: rgb(21, 159, 133);\n"
                                                 "	color: rgb(255, 255, 255);\n"
                                                 "	border-radius: 0px;"));
        volMeasure1->setAlignment(Qt::AlignCenter);
        volMeasure1->setText(QString::number(channel->GetCurrentChannelValue()));
        listLabelDiagnostic.append(volMeasure1);
        horizontalLayout_3->addWidget(volMeasure1);

        QLabel * labelMesMeasure1 = new QLabel(frameMeasure1);
        labelMesMeasure1->setFont(font2);
        labelMesMeasure1->setAlignment(Qt::AlignCenter);
        labelMesMeasure1->setText(channel->GetUnitsName());
        horizontalLayout_3->addWidget(labelMesMeasure1);

        i++;
    }

        QSpacerItem * verticalSpacer = new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->verticalLayoutMeasures->addItem(verticalSpacer);
}



void dMenu::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                delete widget;
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}



void dMenu::setBrightness(int l)
{
#ifndef Q_OS_WIN32
    QFile fileBright("/sys/class/backlight/pwm-backlight/brightness");
    fileBright.open(QIODevice::WriteOnly);
    QTextStream out(&fileBright);
    out << l;
    fileBright.close();
#endif
}

void dMenu::slotUpdateEthernetStatus(bool online, bool enable)
{
//    if(enable)
//    {
//       ui->bToConnect->setText("ПОДКЛЮЧИТЬСЯ");
//    }
//    else
//    {
//        ui->bToConnect->setText("ОТКЛЮЧИТЬСЯ");
//    }
}

void dMenu::slotIpErr()
{
    ui->bToConnect->setText("ПОДКЛЮЧИТЬСЯ");
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
    addWidgetChannels();
}

void dMenu::openSettingsChannel(int num, int page)
{

    if(page == 0)
    {
        //проверка на наличие такого номера канала
        if((num <= 0) || (num > listChannels.size())) return;

        dialogSetingsChannel = new dSettings(listChannels, num, page);
        dialogSetingsChannel->exec();
        dialogSetingsChannel->deleteLater();
        addWidgetChannels();
        //sendConfigChannelsToSlave();
    }
    else if(page == 4)
    {
        //проверка на наличие такого номера входной группы
        if((num <= 0) || (num > listSteel.size())) return;
        dialogSetingsChannel = new dSettings(listChannels, num, page);
        dialogSetingsChannel->exec();
        dialogSetingsChannel->deleteLater();
    }
    else if(page == 3)
    {
        if((num <= 0) || (num > listUstavok.size())) return;
        dialogSetingsChannel = new dSettings(listChannels, num, page);
        dialogSetingsChannel->exec();
        dialogSetingsChannel->deleteLater();
        addWidgetUstavki();
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
    addWidgetUstavki();
}

void dMenu::on_bBackUstavki_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->nameSubMenu->setText("ПРИЛОЖЕНИЯ");
}


void dMenu::on_bDiagnost_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->nameSubMenu->setText("ДИАГНОСТИКА");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bBackDiagnostika_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
//        ui->nameSubMenu->setText("");
    ui->frameNameSubMenu->setHidden(true);
}

void dMenu::addChannels(QList<ChannelOptions *> channels)
{
    foreach (ChannelOptions * ch, channels) {
        listChannels.append(ch);
    }

    //генерация кнопок уставок
    addWidgetUstavki();

}

void dMenu::slotOpenSettings(int num)
{
    openSettingsChannel(num, 3);
}

void dMenu::slotOpenGroup(int num)
{
    curGroupEdit = num - 1;
    cGroupChannels * group = listGroup.at(curGroupEdit);
    ui->comboGroupState->setCurrentIndex(group->enabled);
    ui->nameGroup->setText(group->groupName);

    //определяем существующие каналы и добавляем в комбобоксы
    QStringList listComboChannels;
    listComboChannels.append("ОТКЛЮЧЕН");
    for(int i = 0; i < listChannels.size(); i++)
    {
        listComboChannels.append("АНАЛОГОВЫЙ ВХОД " + QString::number(i+1));
    }
//    foreach (ChannelOptions * ch, listChannels) {
//        listComboChannels.append(ch->GetChannelName());
//    }
    /*
     * добавить математические каналы и дискретные
    foreach (var, container) {

    }*/

    QList<QComboBox*> listCombo;
    listCombo.append(ui->comboGroupChannel1);
    listCombo.append(ui->comboGroupChannel2);
    listCombo.append(ui->comboGroupChannel3);
    listCombo.append(ui->comboGroupChannel4);
    foreach (QComboBox * combo, listCombo) {
        combo->clear();
        combo->addItems(listComboChannels);
    }

    for(int i = 0; i < listChannels.size(); i++)
    {
        for(int k = 0; k < listCombo.size(); k++)
        {
            if((group->typeInput[k] == 1)
                    && (group->channel[k] == listChannels.at(i)))
            {
                listCombo.at(k)->setCurrentIndex(i+1); ;
            }
        }
    }

    ui->stackedWidget->setCurrentIndex(23);
    ui->nameSubMenu->setText("ГРУППА " + QString::number(num));
}

void dMenu::slotOpenChannel(int num)
{
    openSettingsChannel(num, 0);
}

void dMenu::slotOpenDigitOutput(int num)
{
    curRelay = num - 1;
    ui->comboDigitOutputType->setCurrentIndex(listRelais.at(curRelay)->type & 1);
    ui->stackedWidget->setCurrentIndex(19);
    ui->nameSubMenu->setText("ВЫХОД " + QString::number(num));
}

void dMenu::selectPageWork()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->nameSubMenu->setText("РАБОТА");
    ui->frameNameSubMenu->setHidden(false);
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
    keyboard kb(this);\
    keyboard::olderprop = "";\
    kb.setModal(true);\
    kb.setWarning("Введите пароль режима ЭКСПЕРТ", true);\
    kb.exec();\
    cExpertAccess::accessRequest(keyboard::newString);
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

void dMenu::on_bExtMemory_clicked()
{
    ui->stackedWidget->setCurrentIndex(15);
    ui->nameSubMenu->setText("НАКОПИТЕЛИ");
    updateDriversWidgets();
}

void dMenu::on_bBackExternalDrive_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->nameSubMenu->setText("СИСТЕМА");
}

void dMenu::on_bSteel_clicked()
{
    ui->stackedWidget->setCurrentIndex(14);
    ui->nameSubMenu->setText("СТАЛЬ");
}

void dMenu::on_bBackDigitInputSettings_clicked()
{
    ui->stackedWidget->setCurrentIndex(16);
    ui->nameSubMenu->setText("ЦИФР. ВХОДА");
}

void dMenu::on_bBackDigitInputs_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->nameSubMenu->setText("ВХОДЫ");
}

void dMenu::on_bAddDigitInput_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitals_clicked()
{
    ui->stackedWidget->setCurrentIndex(16);
    ui->nameSubMenu->setText("ЦИФР. ВХОДА");
}

void dMenu::on_bDigitInput1_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput2_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput3_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput4_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput5_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput6_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput7_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bDigitInput8_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    ui->nameSubMenu->setText("НАСТР. ВХОДА");
}

void dMenu::on_bOutputs_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->nameSubMenu->setText("ВЫХОДЫ");
}

void dMenu::on_bBackDigitOutputs_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->nameSubMenu->setText("ВЫХОДЫ");
}

void dMenu::on_bBackDigitOutputSettings_clicked()
{
    addWidgetDigitOutputs();
    ui->stackedWidget->setCurrentIndex(18);
    ui->nameSubMenu->setText("ДИСКР. ВЫХОДЫ");
}

void dMenu::on_bMath_clicked()
{
    ui->stackedWidget->setCurrentIndex(20);
    ui->nameSubMenu->setText("МАТЕМАТИКА");
}

void dMenu::on_bBackMath_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->nameSubMenu->setText("ПРИЛОЖЕНИЯ");
}

void dMenu::on_bGroups_clicked()
{
    ui->stackedWidget->setCurrentIndex(22);
    ui->nameSubMenu->setText("ГРУППЫ");
}

void dMenu::on_bBackGroup_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->nameSubMenu->setText("ПРИЛОЖЕНИЯ");
}

// Vag: удалить, когда будет дейлизовано динамическое добавление
void dMenu::on_bAddGroup_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(23);
    ui->nameSubMenu->setText("ГРУППА 1");
}

void dMenu::on_bBackGroupSetting_clicked()
{
    ui->stackedWidget->setCurrentIndex(22);
    ui->nameSubMenu->setText("ГРУППЫ");
}

void dMenu::on_bBackDevices_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->nameSubMenu->setText("ДИАГНОСТИКА");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bDevices_clicked()
{
    ui->stackedWidget->setCurrentIndex(24);
    ui->nameSubMenu->setText("О ПРИБОРЕ");
}

void dMenu::on_bBackDevice_clicked()
{
    ui->stackedWidget->setCurrentIndex(24);
    ui->nameSubMenu->setText("О ПРИБОРЕ");
}

void dMenu::on_bDevice1_clicked()
{
    if(!listDevice.at(0)->getOnline()) return;
    ui->stackedWidget->setCurrentIndex(25);
    ui->nameSubMenu->setText("МОДУЛЬ 1");
    curDiagnostDevice = 1;
}

void dMenu::on_bDevice2_clicked()
{
    if(!listDevice.at(1)->getOnline()) return;
    ui->stackedWidget->setCurrentIndex(25);
    ui->nameSubMenu->setText("МОДУЛЬ 2");
    curDiagnostDevice = 2;
}

void dMenu::on_bDevice3_clicked()
{
    if(!listDevice.at(2)->getOnline()) return;
    ui->stackedWidget->setCurrentIndex(25);
    ui->nameSubMenu->setText("МОДУЛЬ 3");
    curDiagnostDevice = 3;
}

void dMenu::on_bDevice4_clicked()
{
    if(!listDevice.at(3)->getOnline()) return;
    ui->stackedWidget->setCurrentIndex(25);
    ui->nameSubMenu->setText("МОДУЛЬ 4");
    curDiagnostDevice = 4;
}

void dMenu::on_bDevice5_clicked()
{
    if(!listDevice.at(4)->getOnline()) return;
    ui->stackedWidget->setCurrentIndex(25);
    ui->nameSubMenu->setText("МОДУЛЬ 5");
    curDiagnostDevice = 5;
}

void dMenu::on_bDevice6_clicked()
{
    if(!listDevice.at(5)->getOnline()) return;
    ui->stackedWidget->setCurrentIndex(25);
    ui->nameSubMenu->setText("МОДУЛЬ 6");
    curDiagnostDevice = 6;
}

void dMenu::on_bBackLight_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->nameSubMenu->setText("РАБОТА");
}

void dMenu::on_bSetLight_clicked()
{
    ui->stackedWidget->setCurrentIndex(26);
    ui->nameSubMenu->setText("ЯРКОСТЬ");
}

void dMenu::on_bListDiagnostics_clicked()
{
    ui->stackedWidget->setCurrentIndex(27);
    ui->nameSubMenu->setText("ОШИБКИ");
    updateDiagnosticMess();
}

void dMenu::on_bBackListDiagnostics_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->nameSubMenu->setText("ДИАГНОСТИКА");
}

void dMenu::on_bBackMeasure_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->nameSubMenu->setText("ДИАГНОСТИКА");
}

void dMenu::on_bMeasuredValue_clicked()
{
    ui->stackedWidget->setCurrentIndex(28);
    ui->nameSubMenu->setText("ИЗМЕРЕНИЯ");
    addWidgetMeasures();
}

void dMenu::on_bModeling_clicked()
{
    ui->stackedWidget->setCurrentIndex(29);
    ui->nameSubMenu->setText("МОДЕЛИР.");
    addWidgetModeling();
}

void dMenu::on_bBackModeling_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->nameSubMenu->setText("ДИАГНОСТИКА");
}

void dMenu::on_bProtect_clicked()
{

}

void dMenu::on_bTypeConnect_clicked()
{
    ui->stackedWidget->setCurrentIndex(30);
    ui->nameSubMenu->setText("ТИП СВЯЗИ");
}

void dMenu::on_bEthernet_clicked()
{
//    if(ethernet->getOnline())
//    {
//        if(ethernet->getDhcpEn())
//        {
            ethernet->updateParamEternet();
            ui->ipAddr->setText(ethernet->getIpAddr());
            ui->netMask->setText(ethernet->getNetMask());
            ui->gateWay->setText(ethernet->getGateWay());
//        }
//        ui->bToConnect->setText("ОТКЛЮЧИТЬСЯ");
//    }
//    else
//    {
//        ui->ipAddr->setText("10.12.13.5");
//        ui->netMask->setText("255.255.255.0");
//        ui->gateWay->setText("10.12.13.255");
//        ui->bToConnect->setText("ПОДКЛЮЧИТЬСЯ");
//    }
    ui->stackedWidget->setCurrentIndex(31);
    ui->nameSubMenu->setText("ETHERNET");
}

void dMenu::on_bBackTypeConnect_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
}

void dMenu::on_bBackEthernet_clicked()
{
    ui->stackedWidget->setCurrentIndex(30);
    ui->nameSubMenu->setText("ТИП СВЯЗИ");
}

void dMenu::on_bBackFromOutputs_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->nameSubMenu->setText("НАСТРОЙКИ");
    ui->frameNameSubMenu->setHidden(false);
}

void dMenu::on_bDigitalOutputs_clicked()
{
    addWidgetDigitOutputs();
    ui->stackedWidget->setCurrentIndex(18);
    ui->nameSubMenu->setText("ДИСКР. ВЫХОДЫ");
}

void dMenu::updateDriversWidgets()
{
    listDrives.clear();
    int size = flash->getNumDisk();
    for(int i = 0; i < size; i++)
    {
        QString name = flash->getNameDisk(i);
#ifdef Q_OS_WIN
        name.resize(name.size()-1);
#endif
        listDrives.append(name);
    }
    ui->comboDrives->clear();
    ui->comboDrives->addItems(listDrives);
    ui->comboDrives->setCurrentIndex(ui->comboDrives->count() - 1);
}

void dMenu::on_bEditDataTime_clicked()
{
    if(cExpertAccess::getMode() == ACCESS_USER) return;
    dateTime = QDateTime::currentDateTime();
    QDateTime local = dateTime;
    ui->timeEdit_h->setTime(local.time());
    ui->timeEdit_h->setDisplayFormat("hh");
    ui->timeEdit_m->setTime(local.time());
    ui->timeEdit_m->setDisplayFormat("mm");
    ui->timeEdit_s->setTime(local.time());
    ui->timeEdit_s->setDisplayFormat("ss");
    ui->dateEdit_d->setDate(local.date());
    ui->dateEdit_d->setDisplayFormat("dd");
    ui->dateEdit_m->setDate(local.date());
    ui->dateEdit_m->setDisplayFormat("MM");
    ui->dateEdit_y->setDate(local.date());
    ui->dateEdit_y->setDisplayFormat("yyyy");
    ui->DateFormat->clear();
    ui->DateFormat->addItems(datestrings);
    ui->DateFormat->setCurrentIndex(dateindex);
    ui->timeformat->clear();
    ui->timeformat->addItems(timestrings);
    ui->timeformat->setCurrentIndex(timeindex);
    ui->stackedWidget->setCurrentIndex(9);
    ui->nameSubMenu->setText("ДАТА/ВРЕМЯ");
}

void dMenu::on_bDateTimeSet_clicked()
{

#ifndef WIN32
    QProcess process;
//    QDateTime newuidate = ui->dateEdit->dateTime();
//    QTime newuitime = ui->timeEdit->time();

    QString newdate = QString::number(dateTime.date().year()) + "-" + QString::number(dateTime.date().month()) + "-" + QString::number(dateTime.date().day()) ;
    QString newtime = dateTime.time().toString();

    process.startDetached("date --set " + newdate);
    process.startDetached("date --set " + newtime); // max freq on
    process.startDetached("hwclock -w");

#endif
    dateindex = ui->DateFormat->currentIndex();
    timeindex = ui->timeformat->currentIndex();
//    ui->timeEdit->setDisplayFormat(timestrings.at(timeindex));
//    ui->dateEdit->setDisplayFormat(datestrings.at(dateindex));

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
    int countChannels = listChannels.size();
    cFileManager::readChannelsSettings(pathtooptions);
    int newCountChannels = listChannels.size();
    for(int i = countChannels; i < newCountChannels; i++)
    {
        connect(listChannels.at(i), SIGNAL(updateSignal(int)), this, SLOT(updateChannelSlot(int)));
    }
    updateSystemOptions();
    log->addMess("Reset to default", cLogger::USER);
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

void dMenu::updateDiagnosticMess()
{
    QJsonArray messagesarray;
    cLogger log(pathtomessages);
    messagesarray = log.MessRead();

    for (int var = 0; var < messagesarray.count() ; ++var) {
        QJsonObject mes = messagesarray.at(var).toObject();
        if((mes.value("C") != cLogger::SERVICE) && \
                (mes.value("C") == cLogger::ERR))
        {
//            if((mes.value("S") == cLogger::CHANNEL) || \
//                    (mes.value("S") == cLogger::DEVICE) || \
//                    (mes.value("S") == cLogger::MODBUS))
//            {
                QString num = QString("%1").arg(var+1, 4, 10, QChar('0'));
                ui->listWidget->addItem(num + " | " + mes.value("D").toString() \
                                        + " " +  mes.value("T").toString() \
                                        + " | "+ mes.value("M").toString());
//            }
        }
    }
    ui->listWidget->scrollToBottom();
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

void dMenu::on_dateEdit_d_up_clicked()
{
    dateTime = dateTime.addDays(1);
    updUiTimeDate(dateTime);
}

void dMenu::on_dateEdit_m_up_clicked()
{
    dateTime = dateTime.addMonths(1);
    updUiTimeDate(dateTime);
}

void dMenu::on_dateEdit_y_up_clicked()
{
    dateTime = dateTime.addYears(1);
    updUiTimeDate(dateTime);
}

void dMenu::on_dateEdit_d_down_clicked()
{
    dateTime = dateTime.addDays(-1);
    updUiTimeDate(dateTime);
}

void dMenu::on_dateEdit_m_down_clicked()
{
    dateTime = dateTime.addMonths(-1);
    updUiTimeDate(dateTime);
}

void dMenu::on_dateEdit_y_down_clicked()
{
    dateTime = dateTime.addYears(-1);
    updUiTimeDate(dateTime);
}

void dMenu::on_timeEdit_h_up_clicked()
{
    dateTime = dateTime.addSecs(3600);
    updUiTimeDate(dateTime);
}

void dMenu::on_timeEdit_m_up_clicked()
{
    dateTime = dateTime.addSecs(60);
    updUiTimeDate(dateTime);
}

void dMenu::on_timeEdit_s_up_clicked()
{
    dateTime = dateTime.addSecs(1);
    updUiTimeDate(dateTime);
}

void dMenu::on_timeEdit_h_down_clicked()
{
    dateTime = dateTime.addSecs(-3600);
    updUiTimeDate(dateTime);
}

void dMenu::on_timeEdit_m_down_clicked()
{
    dateTime = dateTime.addSecs(-60);
    updUiTimeDate(dateTime);
}

void dMenu::on_timeEdit_s_down_clicked()
{
    dateTime = dateTime.addSecs(-1);
    updUiTimeDate(dateTime);
}



void dMenu::updUiTimeDate(QDateTime td)
{
    QDateTime local(td);
    ui->timeEdit_h->setTime(local.time());
    ui->timeEdit_h->setDisplayFormat("hh");
    ui->timeEdit_m->setTime(local.time());
    ui->timeEdit_m->setDisplayFormat("mm");
    ui->timeEdit_s->setTime(local.time());
    ui->timeEdit_s->setDisplayFormat("ss");
    ui->dateEdit_d->setDate(local.date());
    ui->dateEdit_d->setDisplayFormat("dd");
    ui->dateEdit_m->setDate(local.date());
    ui->dateEdit_m->setDisplayFormat("MM");
    ui->dateEdit_y->setDate(local.date());
    ui->dateEdit_y->setDisplayFormat("yyyy");
}




void dMenu::on_bReadSysFromDrive_clicked()
{
//    QProcess process;
    QString src, dest, path;
    path = ui->comboDrives->currentText() + "/" + ui->nameDirOnDrive->text() + "/";
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    src = QString(path + "systemoptions.txt");
    if(QFile::exists(src))
    {
        updateSystemOptions(src);
        log->addMess("Read system setting from media", cLogger::STATISTIC);
        qDebug() << "System settings successfully read from the media";
        QString mess = QString("Системные настройки успешно прочитаны с указаного носителя\r\n");
        mess == QString("Нажмите СОХРАНИТЬ, чтобы применить новые настройки");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
    else
    {
        log->addMess("Error read system setting from media", cLogger::ERR);
        qDebug() << "Error reading the system settings file on the media" << strerror(errno);
        QString mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя и файлов настроек\r\n");
        mess += QString("Убедитесь, что формат файловой системы носителя соответствует FAT32");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
}

void dMenu::on_bSaveSysToDrive_clicked()
{
    QString src, dest, path;
    path = QString(ui->comboDrives->currentText() + "/" + ui->nameDirOnDrive->text() + "/");
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    src = pathtosystemoptions;
    dest = QString(path + "systemoptions.txt");
    QDir dir;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    QFile::remove(dest + QString(".backup"));
    QFile::rename(dest, dest + QString(".backup"));
    if(QFile::copy(src, dest))
    {
        log->addMess("Save system setting to media", cLogger::STATISTIC);
        qDebug() << "The system settings were recorded successfully";
        qDebug() << "Path" << dest.toStdString().c_str();
        path = QString("/" + ui->nameDirOnDrive->text() + "/" + "systemoptions.txt");
        QString mess = QString("Системные настройки успешно сохранены на указаном носителе\r\n");
        mess += QString("Путь к файлу: %1").arg(path);
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
    else
    {
        log->addMess("Error save system setting to media", cLogger::ERR);
        qDebug() << "Error writing system settings" << strerror(errno);
        QString mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
}

void dMenu::on_bSaveMesToDrive_clicked()
{
    QString src, dest, path;
    path = QString(ui->comboDrives->currentText() + "/" + ui->nameDirOnDrive->text() + "/");
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    src = pathtomessages;
    dest = QString(path + "Log.txt");
    QDir dir;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    QFile::remove(dest + QString(".backup"));
    QFile::rename(dest, dest + QString(".backup"));
    if(QFile::copy(src, dest))
    {
        log->addMess("Save log to media", cLogger::STATISTIC);
        qDebug() << "The event log was successfully saved on the media";
        qDebug() << "Path" << dest.toStdString().c_str();
        path = QString("/" + ui->nameDirOnDrive->text() + "/" + "Log.txt");
        QString mess = QString("Журнал событий успешно сохранён на указаном носителе\r\n");
        mess += QString("Путь к файлу: %1").arg(path);
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
    else
    {
        log->addMess("Error save log to media", cLogger::ERR);
        qDebug() << "Error writing event log" << strerror(errno);
        QString mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
}

void dMenu::on_bSaveChanToDrive_clicked()
{
    QString src, src2, dest, dest2, path;
    path = QString(ui->comboDrives->currentText() + "/" + ui->nameDirOnDrive->text() + "/");
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    src = pathtooptions;
    src2 = pathtosteeloptions;
    dest = QString(path + "options.txt");
    dest2 = QString(path + "steeloptions.txt");
    QDir dir;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    QFile::remove(dest + QString(".backup"));
    QFile::rename(dest, dest + QString(".backup"));
    QFile::remove(dest2 + QString(".backup"));
    QFile::rename(dest2, dest2 + QString(".backup"));
    if(QFile::copy(src, dest) && QFile::copy(src2, dest2))
    {
        log->addMess("Save settings to media", cLogger::STATISTIC);
        qDebug() << "The settings for the inputs and outputs were successfully saved on the media";
        qDebug() << "Path" << dest.toStdString().c_str();
        qDebug() << "Path" << dest2.toStdString().c_str();
        path = QString("/" + ui->nameDirOnDrive->text() + "/");
        QString mess = QString("Настройки входов и выходов успешно сохранены на носителе\r\n");
        mess += QString("Директория файлов: %1").arg(path);
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
    else
    {
        log->addMess("Error save settings to media", cLogger::ERR);
        qDebug() << "Error writing settings" << strerror(errno);
        QString mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
}

void dMenu::on_bReadChanFromDrive_clicked()
{
    QString src, src2, dest, dest2, path;
    path = ui->comboDrives->currentText() + "/" + ui->nameDirOnDrive->text() + "/";
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    src = QString(path + "options.txt");
    src2 = QString(path + "steeloptions.txt");
    if(QFile::exists(src) && QFile::exists(src2))
    {
        updateSystemOptions(src);
        log->addMess("Read settings from media", cLogger::STATISTIC);
        qDebug() << "I / O settings were successfully read from the specified media";
        QString mess = QString("Настройки входов и выходов успешно прочитаны с указаного носителя\r\n");
        mess == QString("Нажмите СОХРАНИТЬ, чтобы применить новые настройки");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
    else
    {
        log->addMess("Error read settings from media", cLogger::ERR);
        qDebug() << "Error reading input and output settings" << strerror(errno);
        QString mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя и файлов настроек\r\n");
        mess += QString("Убедитесь, что формат файловой системы носителя соответствует FAT32");
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
}

void dMenu::on_bSaveArchiveToDrive_clicked()
{
    ui->progressBarLoadFiles->setValue(0);
    ui->progressBarLoadFiles->setHidden(false);
    QString src, dest, path;
    QDateTime currTime = QDateTime::currentDateTime();
    int daysArray[4] = {1, 7, 30, 365};
    int days = daysArray[ui->periodArchiveToDrive->currentIndex()];
    QDateTime firstTime = currTime.addDays(-days);
    QString pathArch = QString(pathtoarchivedata);
    QStringList sl = pathArch.split(".");
    int n = sl.size();
    QStringList strlist = pathArch.split("/");
    path = QString(ui->comboDrives->currentText() + "/" \
                   + ui->nameDirOnDrive->text() + "/");
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    QDir dir;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    countArchFiles = 0;
    QTimer * copyTimer = new QTimer;
    connect(copyTimer, SIGNAL(timeout()), this, SLOT(copyArchiveFile()));
    connect(this, SIGNAL(finishCopyArchive()), this, SLOT(copyLastArchFile()));
    connect(this, SIGNAL(finishCopyArchive()), copyTimer, SLOT(deleteLater()));
    copyTimer->start(100);
//    for(int i = 0; i < days; i++)
//    {
//        QString strDay = firstTime.addDays(i).toString("yyMMdd");
//        src = sl.at(n-2) + "_sek_" + strDay + "." + sl.at(n-1);
//        dest = QString(path + "archive_sek_" + strDay + ".dat");    //Vag: изменить
//        QFile::remove(dest);    //удалить с носителя (устарел)
//        QFile arch_sek(src);
//        if(arch_sek.exists())   //проверка наличия такого файла
//        {
//            QFile::copy(src, dest);
//        }
//        int progress = (i*100)/days;
//        ui->progressBarLoadFiles->setValue(progress);
//    }
//    src = sl.at(n-2) + "_sek." + sl.at(n-1);;
//    dest = QString(path + "archive_sek.dat");
//    QFile::remove(dest);    //удалить с носителя (устарел)
//    QFile arch_sek(src);
//    if(arch_sek.exists())   //проверка наличия такого файла
//    {
//        ui->progressBarLoadFiles->setValue(100);
//        QString mess;
//        if(QFile::copy(src, dest))
//        {
//            qDebug() << "The measurement archive was copied to the specified medium";
//            qDebug() << "Path" << dest.toStdString().c_str();
//            mess = QString("Архив измерений "\
//                                   + ui->periodArchiveToDrive->currentText() \
//                                   + " успешно скопирован на указанный носитель\r\n");
//        }
//        else
//        {
//            qDebug() << "Error writing measurement archive" << strerror(errno);
//            mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя");
//        }
//        mesDialog.showInfo(mess, "Сообщение");
//        mesDialog.setWindowModality(Qt::WindowModal);
//        mesDialog.show();
//    }
//    ui->progressBarLoadFiles->setHidden(true);
}

void dMenu::copyArchiveFile()
{
    QString src, dest, path;
    QDateTime currTime = QDateTime::currentDateTime();
    int daysArray[4] = {1, 7, 30, 365};
    int days = daysArray[ui->periodArchiveToDrive->currentIndex()];
    if(countArchFiles == days)
        emit finishCopyArchive();
    else
    {
        QDateTime firstTime = currTime.addDays(-days);
        QString pathArch = QString(pathtoarchivedata);
        QStringList sl = pathArch.split(".");
        int n = sl.size();
        QStringList strlist = pathArch.split("/");
        path = QString(ui->comboDrives->currentText() + "/" \
                       + ui->nameDirOnDrive->text() + "/");
#ifdef Q_OS_LINUX
        path = QString("/media/" + path);
#endif
        QString strDay = firstTime.addDays(countArchFiles).toString("yyMMdd");
        src = sl.at(n-2) + "_sek_" + strDay + "." + sl.at(n-1);
        dest = QString(path + "archive_sek_" + strDay + ".dat");    //Vag: изменить
        QFile::remove(dest);    //удалить с носителя (устарел)
        QFile arch_sek(src);
        if(arch_sek.exists())   //проверка наличия такого файла
        {
            QFile::copy(src, dest);
        }
        //        int progress = (countArchFiles*100)/days;
        ui->progressBarLoadFiles->setValue((countArchFiles*100)/days);
        countArchFiles++;
    }
}

void dMenu::copyLastArchFile()
{
    QString src, dest, path;
    path = QString(ui->comboDrives->currentText() + "/" \
                   + ui->nameDirOnDrive->text() + "/");
#ifdef Q_OS_LINUX
    path = QString("/media/" + path);
#endif
    QString pathArch = QString(pathtoarchivedata);
    QStringList sl = pathArch.split(".");
    int n = sl.size();
    src = sl.at(n-2) + "_sek." + sl.at(n-1);;
    dest = QString(path + "archive_sek.dat");
    QFile::remove(dest);    //удалить с носителя (устарел)
    QFile arch_sek(src);
    if(arch_sek.exists())   //проверка наличия такого файла
    {
        ui->progressBarLoadFiles->setValue(100);
        QString mess;
        if(QFile::copy(src, dest))
        {
            QString periods[4] = {"day", "week", "month", "year"};
            QString period = periods[ui->periodArchiveToDrive->currentIndex()];
            log->addMess("Save archive(" + period + ") to media", cLogger::STATISTIC);
            qDebug() << "The measurement archive was copied to the specified medium";
            qDebug() << "Path" << dest.toStdString().c_str();
            mess = QString("Архив измерений "\
                                   + ui->periodArchiveToDrive->currentText() \
                                   + " успешно скопирован на указанный носитель\r\n");
        }
        else
        {
            log->addMess("Error save archive to media", cLogger::ERR);
            qDebug() << "Error writing measurement archive" << strerror(errno);
            mess = QString("Ошибка! Проверьте, пожалуйста, доступность носителя");
        }
        mesDialog.showInfo(mess, "Сообщение");
        mesDialog.setWindowModality(Qt::WindowModal);
        mesDialog.show();
    }
    ui->progressBarLoadFiles->setHidden(true);
}


void dMenu::on_comboDigitInputsFunc_currentIndexChanged(int index)
{
    ui->stackedWidgetDIFunc->setCurrentIndex(index);
}

void dMenu::on_digitInoutToOutput_currentIndexChanged(int index)
{
    ui->stackedObjectsMenage->setCurrentIndex(index);
}


void dMenu::on_bAddUstavka_clicked()
{
    Ustavka *ust = new Ustavka(this);
    int i = listUstavok.size();
    ust->setIdentifikator("Limit " + QString::number(i+1));
    listUstavok.append(ust);
    emit newUstavka(i);

    //регенерация кнопок уставок
    addWidgetUstavki();
}



void dMenu::on_bAddGroup_clicked()
{
    cGroupChannels *group = new cGroupChannels();
    int size = listGroup.size();
    if(size >= MAX_NUM_GROUP) return;
    group->enabled = true;
    group->groupName = "Group " + QString::number(size+1);
    listGroup.append(group);

    //регенерация кнопок уставок
    addWidgetGroup();
}

void dMenu::on_bDelGroup_clicked()
{
    if(listGroup.size() > 1)
    {
        listGroup.removeAt(curGroupEdit);
    }
    ui->stackedWidget->setCurrentIndex(22);
    ui->nameSubMenu->setText("ГРУППЫ");
    addWidgetGroup();
}

void dMenu::on_bApplayGroup_clicked()
{
    cGroupChannels * g = listGroup.at(curGroupEdit);
    g->enabled = ui->comboGroupState->currentIndex();
    g->groupName = ui->nameGroup->text();
    QList<QComboBox*> listCombo;
    listCombo.append(ui->comboGroupChannel1);
    listCombo.append(ui->comboGroupChannel2);
    listCombo.append(ui->comboGroupChannel3);
    listCombo.append(ui->comboGroupChannel4);
    for(int i = 0; i < listCombo.size(); i++)
    {

        int indexCh = listCombo.at(i)->currentIndex();
        if(indexCh == 0)
        {
            g->typeInput[i] = 0;
        }
        else if(indexCh <= listChannels.size())
        {
            g->typeInput[i] = 1;
            g->channel[i] = listChannels.at(indexCh-1);
        }
//    else if(index <= listMath.size())
//    {

//    }
    }

    ui->stackedWidget->setCurrentIndex(22);
    ui->nameSubMenu->setText("ГРУППЫ");
    addWidgetGroup();
}



void dMenu::on_lightUp_clicked()
{
    if(light < 91) light += 10;
    ui->volLight->setText(QString::number(light));
    ui->progressLight->setValue(light);
    setBrightness(light);
}

void dMenu::on_lightDown_clicked()
{
    if(light > 10) light -= 10;
    ui->volLight->setText(QString::number(light));
    ui->progressLight->setValue(light);
    setBrightness(light);
}

void dMenu::on_bLogEvents_clicked()
{
    dialogSetingsChannel = new dSettings(listChannels, 1, 1);
    dialogSetingsChannel->exec();
    dialogSetingsChannel->deleteLater();
}


void dMenu::updateLabelDiagnostic()
{
    int i = 0;
    foreach(QLabel * volLabel, listLabelDiagnostic)
    {
        if(i < listChannels.size())
        {
            volLabel->setText(QString::number(listChannels.at(i)->GetCurrentChannelValue()));
        }
        i++;
    }
}

void dMenu::updateLabelModeling()
{
    int i = 0;
    foreach(QLabel * volLabel, listLabelModeling)
    {
        QList<QColor> colors;
                  //ON        //OFF            //ERR
        colors << ColorCh1 << ColorCh4Light << ColorCh3;

        QStringList strStyle;
        strStyle << "background-color: rgb(" + \
                    QString::number(colors.at(0).red()) + ", " + \
                    QString::number(colors.at(0).green()) + ", " + \
                    QString::number(colors.at(0).blue()) + ");\n"
                    "color: rgb(255, 255, 255);\n"
                    "border-radius: 0px;";
        strStyle << "background-color: rgb(" + \
                    QString::number(colors.at(1).red()) + ", " + \
                    QString::number(colors.at(1).green()) + ", " + \
                    QString::number(colors.at(1).blue()) + ");\n"
                    "color: rgb(255, 255, 255);"
                    "border-radius: 0px;";
        strStyle << "background-color: rgb(" + \
                    QString::number(colors.at(2).red()) + ", " + \
                    QString::number(colors.at(2).green()) + ", " + \
                    QString::number(colors.at(2).blue()) + ");\n"
                    "color: rgb(255, 255, 255);"
                    "border-radius: 0px;";

        if(i < listRelais.size())
        {
            cRelay * r = listRelais.at(i);
            volLabel->setText(r->getCurState() ? "ON" : "OFF");
//            if(r->confirmedState)
//            {
                volLabel->setStyleSheet(r->getCurState() ? strStyle.at(0) : strStyle.at(1));
//            }
//            else
//            {
//                volLabel->setStyleSheet(strStyle.at(2));
//            }
        }
        i++;
    }
}



void dMenu::on_modelingOn_clicked()
{
    listRelais.at(0)->setState(true);
}

void dMenu::on_modelingOff_clicked()
{
    listRelais.at(0)->setState(false);
}

void dMenu::addWidgetModeling()
{

    clearLayout(ui->verticalLayouModeling);
    listLabelModeling.clear();

    QFont font6;
    font6.setFamily(QStringLiteral("Open Sans"));
    font6.setPointSize(20);

    int i = 0;
    foreach(cRelay * relay, listRelais)
    {

        QFrame * frameModeling = new QFrame(ui->widgetScrollAreaModeling);
        frameModeling->setFrameShape(QFrame::NoFrame);
        frameModeling->setFrameShadow(QFrame::Raised);

        QHBoxLayout * horizontalLayout_8 = new QHBoxLayout(frameModeling);

        QLabel * labelModeling = new QLabel(frameModeling);
        labelModeling->setFont(font6);
        horizontalLayout_8->addWidget(labelModeling);

        QSpacerItem * horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        horizontalLayout_8->addItem(horizontalSpacer_5);

        QPushButton * modelingOn = new QPushButton(frameModeling);
        modelingOn->setObjectName(QStringLiteral("bModeling_On_") + QString::number(i));
        modelingOn->setMinimumSize(QSize(100, 41));
        modelingOn->setMaximumSize(QSize(100, 41));
        modelingOn->setFont(font6);
        modelingOn->setStyleSheet(QLatin1String("background-color: rgb(230, 230, 230);\n"
                                                "color: rgb(0, 0, 0);\n"
                                                "border-radius: 0px;"));
        modelingOn->installEventFilter(this);
        horizontalLayout_8->addWidget(modelingOn);
//        listButtonModeling.append(modelingOn);


        QLabel * modelingVol = new QLabel(frameModeling);
        modelingVol->setMinimumSize(QSize(131, 31));
        modelingVol->setMaximumSize(QSize(185, 45));
        modelingVol->setFont(font6);
        modelingVol->setStyleSheet(QLatin1String("	background-color: rgb(21, 159, 133);\n"
                                                 "	color: rgb(255, 255, 255);\n"
                                                 "	border-radius: 0px;"));
        modelingVol->setAlignment(Qt::AlignCenter);
        horizontalLayout_8->addWidget(modelingVol);
        listLabelModeling.append(modelingVol);

        QPushButton * modelingOff = new QPushButton(frameModeling);
        modelingOff->setObjectName(QStringLiteral("bModeling_Off_") + QString::number(i));
        modelingOff->setMinimumSize(QSize(100, 41));
        modelingOff->setMaximumSize(QSize(100, 41));
        modelingOff->setFont(font6);
        modelingOff->setStyleSheet(QLatin1String("background-color: rgb(230, 230, 230);\n"
                                                 "color: rgb(0, 0, 0);\n"
                                                 "border-radius: 0px;"));
        modelingOff->installEventFilter(this);
        horizontalLayout_8->addWidget(modelingOff);
        ui->verticalLayouModeling->addWidget(frameModeling);
//        listButtonModeling.append(modelingOff);

        QString strOut = "ВЫХОД " + QString::number(i+1);
        labelModeling->setText(strOut);
        modelingOn->setText(QApplication::translate("dMenu", "ВКЛ", Q_NULLPTR));
        modelingVol->setText(relay->getCurState() ? "ON" : "OFF");
        modelingOff->setText(QApplication::translate("dMenu", "ОТКЛ", Q_NULLPTR));

        i++;
    }

    QSpacerItem * verticalSpacer_36 = new QSpacerItem(20, 165, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayouModeling->addItem(verticalSpacer_36);
}

void dMenu::updateDevicesUI()
{
    QList<wButtonStyled*> listButtonDevices;
    listButtonDevices << ui->bDevice1 << ui->bDevice2 << ui->bDevice3 \
                      << ui->bDevice4 << ui->bDevice5 << ui->bDevice6;
    assert(listDevice.size() == listButtonDevices.size());
    QStringList strType;
    strType << "" << "4AI" << "8RP" << "STEEL";
    int i = 0;
    foreach(wButtonStyled * bDev, listButtonDevices)
    {
        cDevice * device = listDevice.at(i);
        QString str = "МОДУЛЬ РАСШИРЕНИЯ " + QString::number(i+1) + " | ";
        QStringList strOnline;
        strOnline << "ОТКЛЮЧЕН" << "ВКЛЮЧЕН";
        str += strOnline.at(device->getOnline());

        if(device->getOnline())
        {
            if(strType.at((int)(device->deviceType) % strType.size()).size())
            {
                str +=  " | " + strType.at(device->deviceType % strType.size());
            }
        }

        bDev->setText(str);
        i++;
    }
    if(curDiagnostDevice != 0)
    {
        cDevice * curDev = listDevice.at(curDiagnostDevice - 1);
        ui->deviceType->setText(strType.at(curDev->deviceType));
        ui->deviceState->setText(QString::number(curDev->deviceState));
        QStringList strStatus;
        strStatus << "NOINIT" << "CONFIG" << "EXECUTE" << "IDLE" << "ERROR";
        ui->deviceStatus->setText(strStatus.at(curDev->deviceStatus));
        int numErr = 0;
        QStringList devErrors;
        devErrors << "MODEL" << "SERIAL" << "FACTORY" << "CRC32" << "MODE" << "ADDRESS"\
                  << "SPEED" << "MODEL_CHECK" << "REZERV" << "RESERVE" << "RESERVE"
                  << "RESERVE" << "RESERVE" << "RESERVE" << "RESERVE" << "RESERVE";
        QStringList activeErrors;
        for(int i = 0; i < 16; i++)
        {
//            numErr += ((curDev->devErrors >> i) & 1);
            if((curDev->devErrors >> i) & 1)
            {
                numErr++;
                activeErrors << devErrors.at(i);
            }
        }
        ui->devErrors->setText(QString::number(numErr));
        ui->listDeviceErrors->clear();
        ui->listDeviceErrors->addItems(activeErrors);
        ui->protocolVersion->setText(QString::number(curDev->protocolVersion));
        ui->hardwareVersion->setText(QString::number(curDev->hardwareVersion));
        ui->softwareVersion->setText(QString::number(curDev->softwareVersion));
        ui->serialNumber->setText(QString::number(curDev->serialNumber));
        ui->uptime->setText(QString::number(curDev->uptime));
        int year = (curDev->factoryDate >> 24) & 0xFF;
        int month = (curDev->factoryDate >> 16) & 0xFF;
        int day = (curDev->factoryDate >> 8) & 0xFF;
        QString strDate = QString::number(day) + "."\
                + QString::number(month) + "."\
                + QString::number(year);
        ui->dataOrder->setText(strDate);
    }
}

void dMenu::updateDeviceInfo(uint8_t index)
{
    assert(listDevice.size() >= index);
    cDevice * device = listDevice.at(index);
    if(!device->getOnline()){
        ui->frameDeviceInfo1->hide();
        ui->frameDeviceInfo2->hide();
    }
    else
    {
        QStringList strType;
        strType << "" << "4AI" << "8RP" << "STEEL";
        ui->deviceType->setText(strType.at(device->deviceType % strType.size()));
        ui->deviceState->setText(QString::number(device->deviceState));
        ui->deviceStatus->setText(QString::number(device->deviceStatus));
//        int countErr = 0;
//        QList<int> listErrors;
//        for(int i = 0; i < 8; i++)
//        {
//            if(device->)
//        }
//        ui->devErrors->setText();
        ui->frameDeviceInfo1->show();
        ui->frameDeviceInfo2->show();
    }
}


//void dMenu::on_comboDhcpEn_currentIndexChanged(int index)
//{
//    ui->ipAddr->setEnabled(index == 0);
//    ui->netMask->setEnabled(index == 0);
//    ui->gateWay->setEnabled(index == 0);
//}

void dMenu::on_bToConnect_clicked()
{
    ethernet->setConfig(ui->ipAddr->text(), \
                            ui->netMask->text(), ui->gateWay->text());
}



void dMenu::on_bDigitOutputSettingsApply_clicked()
{
    listRelais.at(curRelay)->type = ui->comboDigitOutputType->currentIndex();
}
