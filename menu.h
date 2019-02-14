#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QWidget>
#include <QMovie>
#include <QTimer>
#include <settings.h>
#include <systemoptions.h>
#include <defines.h>
#include "kvitirovanie.h"
#include "log.h"

namespace Ui {
class dMenu;
}

class dMenu : public QDialog
{
    Q_OBJECT

public:
    explicit dMenu(QWidget *parent = 0);
    ~dMenu();
    QTimer timerLoad;
    void addChannels(QList<ChannelOptions *> channels);
    void addSteels(QList<cSteel*> st, typeSteelTech tech);
    void selectPageWork();
    bool eventFilter(QObject* watched, QEvent* event);

public slots:
    void openSettingsChannel(int num, int page = 0);
    void on_exitButton_clicked();
    void on_saveButton_clicked();
    void timeoutLoad();
    void on_bWork_clicked();
    void on_bBack_clicked();
    void on_bSettings_clicked();
    void on_bBackFromSettings_clicked();
    void on_bBackFromSystem_clicked();
    void on_bBackFromInputs_clicked();
    void on_bInputs_clicked();
    void on_bSystem_clicked();
    void DateUpdate();
    void on_bBackChannels_clicked();
    void on_bAnalog_clicked();
    void on_bChannel1_clicked();
    void on_bChannel2_clicked();
    void on_bChannel3_clicked();
    void on_bChannel4_clicked();
    void on_bApplication_clicked();
    void on_bBackApplications_clicked();
    void on_bUstavki_clicked();
    void on_bBackUstavki_clicked();
    void on_bDiagnost_clicked();
    void on_bBackDiagnostika_clicked();
    void on_bBackDateTime_clicked();
    void on_bEditDataTime_clicked();
    void on_bDateTimeSet_clicked();
    void slotOpenSettings(int num);
    void slotOpenGroup(int num);
    void slotOpenChannel(int num);
    void slotOpenDigitOutput(int num);

private:
    Ui::dMenu *ui;
    QMovie mo;
    QTimer tUpdateTime;
    QTimer tUpdateBar;
    QTimer tUpdateDiagnostic;
    QTimer tUpdateDeviceUI;
    dSettings * dialogSetingsChannel;
    QList<QLabel*> listLabelDiagnostic;
    QList<QLabel*> listLabelModeling;
//    QList<QPushButton*> listButtonModeling;
//    QList<ChannelOptions *> listChannels;
    void UpdateAnalyze();
    void updateDiagnosticMess();
    void updateSystemOptions(QString path = pathtosystemoptions);
    cSystemOptions sysOptions;
    QDateTime dateTime;
    QStringList listDrives;
    kvitirovanie mesDialog;
    int countArchFiles;
    cLogger * log;
    void addWidgetUstavki();
    void addWidgetGroup();
    void addWidgetChannels();
    void addWidgetDigitOutputs();
    void addWidgetMeasures();
    void addWidgetModeling();
    int curGroupEdit;       //текущая редактируемая группа каналов
    int light;
    int curDiagnostDevice;
    int curRelay;

//    QList<cSteel*> listSteels;
//    typeSteelTech steelTech[NUM_TECHNOLOGIES];

    void clearLayout(QLayout *layout, bool deleteWidgets = true);
    void changeVisibleWidgets();
    QList<QWidget *> listWidgetsExpert;
    QList<QWidget *> listWidgetsAdmin;

private slots:
    void updateDevicesUI();
    void updateDeviceInfo(uint8_t index);
    void updateLabelDiagnostic();
    void updateLabelModeling();
    void setBrightness(int l);
    void slotUpdateEthernetStatus(bool online, bool enable);
    void slotIpErr();
    void on_bResetToDefault_clicked();
    void on_bExpert_clicked();
    void on_bAnaliz_clicked();
    void on_bBackExpert_2_clicked();
    void on_bOptions_clicked();
    void on_bBackSystemOptions_clicked();
    void on_bModeDiagram_clicked();
    void on_bBackOtobrazhenie_clicked();
    void on_bBackSteel_clicked();
    void on_bSteel_clicked();
    void on_bSteel1_clicked();
    void on_bSteel2_clicked();
    void on_bSteel3_clicked();
    void on_bSteel4_clicked();
    void on_radioButSteelModes_clicked();
    void on_radioButAnalogModes_clicked();
    void on_dateEdit_d_up_clicked();
    void on_dateEdit_m_up_clicked();
    void on_dateEdit_y_up_clicked();
    void on_dateEdit_d_down_clicked();
    void on_dateEdit_m_down_clicked();
    void on_dateEdit_y_down_clicked();
    void on_timeEdit_h_up_clicked();
    void on_timeEdit_m_up_clicked();
    void on_timeEdit_s_up_clicked();
    void on_timeEdit_h_down_clicked();
    void on_timeEdit_m_down_clicked();
    void on_timeEdit_s_down_clicked();
    void updUiTimeDate(QDateTime td);
    void on_bExtMemory_clicked();
    void on_bBackExternalDrive_clicked();
    void updateDriversWidgets();
    void on_bReadSysFromDrive_clicked();
    void on_bSaveSysToDrive_clicked();
    void on_bSaveMesToDrive_clicked();
    void on_bSaveChanToDrive_clicked();
    void on_bReadChanFromDrive_clicked();
    void on_bSaveArchiveToDrive_clicked();
    void copyArchiveFile();
    void copyLastArchFile();
    void on_bBackDigitInputSettings_clicked();
    void on_bBackDigitInputs_clicked();
    void on_bAddDigitInput_clicked();
    void on_bDigitals_clicked();
    void on_comboDigitInputsFunc_currentIndexChanged(int index);
    void on_digitInoutToOutput_currentIndexChanged(int index);
    void on_bDigitInput1_clicked();
    void on_bDigitInput2_clicked();
    void on_bDigitInput3_clicked();
    void on_bDigitInput4_clicked();
    void on_bDigitInput5_clicked();
    void on_bDigitInput6_clicked();
    void on_bDigitInput7_clicked();
    void on_bDigitInput8_clicked();
    void on_bOutputs_clicked();
    void on_bBackDigitOutputs_clicked();
    void on_bBackDigitOutputSettings_clicked();
    void on_bAddUstavka_clicked();
    void on_bMath_clicked();
    void on_bBackMath_clicked();
    void on_bGroups_clicked();
    void on_bBackGroup_clicked();
    void on_bAddGroup_2_clicked();
    void on_bBackGroupSetting_clicked();
    void on_bAddGroup_clicked();
    void on_bDelGroup_clicked();
    void on_bApplayGroup_clicked();
    void on_bBackDevices_clicked();
    void on_bDevices_clicked();
    void on_bDevice1_clicked();
    void on_bBackDevice_clicked();
    void on_bDevice2_clicked();
    void on_bDevice3_clicked();
    void on_bDevice4_clicked();
    void on_bDevice5_clicked();
    void on_bDevice6_clicked();
    void on_lightUp_clicked();
    void on_lightDown_clicked();
    void on_bBackLight_clicked();
    void on_bSetLight_clicked();
    void on_bLogEvents_clicked();
    void on_bListDiagnostics_clicked();
    void on_bBackListDiagnostics_clicked();
    void on_bBackMeasure_clicked();
    void on_bMeasuredValue_clicked();
    void on_bModeling_clicked();
    void on_bBackModeling_clicked();
    void on_modelingOn_clicked();
    void on_modelingOff_clicked();
    void on_bProtect_clicked();

    void on_bTypeConnect_clicked();

    void on_bEthernet_clicked();

    void on_bBackTypeConnect_clicked();

    void on_bBackEthernet_clicked();

//    void on_comboDhcpEn_currentIndexChanged(int index);

    void on_bToConnect_clicked();

    void on_bBackFromOutputs_clicked();

    void on_bDigitalOutputs_clicked();

    void on_bDigitOutputSettingsApply_clicked();

signals:
    void saveButtonSignal();
    void finishCopyArchive();
    void newUstavka(int num);
};

#endif // MENU_H
