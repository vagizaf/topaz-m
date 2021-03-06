#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QMovie>
#include <QWidget>
#include <QTimer>
#include <Channels/channelOptions.h>
#include <ustavka.h>
#include <archivator.h>
#include <steel.h>

namespace Ui {
class dSettings;
}

class dSettings : public QDialog
{
    Q_OBJECT

public:
    explicit dSettings(QList<ChannelOptions*> channels,
                       int num,
                       int page = 0,
                       cArchivator * ar = NULL,
                       QWidget *parent = 0);
    ~dSettings();
    QTimer timerLoad;
    void addArch(cArchivator * archiv) {arch = archiv;}
    void addSteel(cSteel * st, typeSteelTech * tech);

    void setColorBar(QColor color, QColor colotLihgt);
public slots:
    void resizeEvent(QResizeEvent * s);
    void updateBar();

private:
    Ui::dSettings *ui;
    QMovie mo;
    QMovie moArch;
    QTimer tUpdateTime;
    QTimer tUpdateBar;
    QStringList StringListCurrent, StringListNapryagenie, StringListRTD, StringListTC; // списки названий для каждого типа сигналов
    QStringList StringListPrecisions;
    void updateGraf(int period);
//    QList<ChannelOptions *> listChannels;
    void updateWidgets();
    int numChannel;
    ChannelOptions * channel;   //канал с которым сейчас работаем
    Ustavka * ustavka;          //уставка канала с которым работаем
    void addChannel(QList<ChannelOptions *> channels, int num);
    int getIndexSignalTypeTable(int st);
    void updateUiSignalTypeParam(int index);
    int sensorShemaFromUiShemaIndex(int index);
    int indexUiShemaFromSensorShema(int sh);
    QVector<double> X_Coordinates, Y_coordinates_Chanel_1, Y_coordinates_Chanel_2, Y_coordinates_Chanel_3, Y_coordinates_Chanel_4;
    cArchivator * arch;
    QVector<QString> Labels;
    int xPos;
    int yPos;
    int yPosArea;
    double sizePlot;
    double posPlot;
    double sizePlotX;
    double posPlotX;
    bool mouseOnScalede;
    bool mouseOnScaledeX;
    bool mouseOnMove;
    bool fNeadRescale;
    QTimer timerUpdateGraf;
    int yPosList;
    int curRow;
    bool mouseScroll;
    int archivePeriod;
    int periodShift;
    int multiplier;
    QDateTime firstTime;
    QString strLabel;
    void updateUIfromSteel();
    cSteel * curSteel;
    void UpdateSteelUI(typeSteelTech * tech);
    QVector<double> X_Steel, Y_SteelTemp, Y_SteelEds;
    void saveParam();
    void saveParamToFile();
    void loadArchFromFile();
    void reactOnMousePress();
    void reactOnMouseRelease();
    void ReactOnMouseSlide();
    void updateArchGraf();
    bool fInitComboChannles;
    bool fInitCompoCopyChannels;

    int getIndexVoltageTable(int d);
    int getIndexTableTC(int d);
    int getIndexTableRTD(int d);
    void updateVer();
    int getIndexTablePrecisions(int p);
private slots:
    void initComboChannels(void);
    void on_exitButton_clicked();
    void on_saveButton_clicked();
    void timeoutLoad();
//    void on_buttonUstavk_clicked();
    void DateUpdate();

    void on_verticalScrollBar_sliderMoved(int position);
    void on_period_currentIndexChanged(int index);
//    void on_buttonBackUstavki_clicked();
    bool eventFilter(QObject* watched, QEvent* event);
    void on_typeSignal_currentIndexChanged(int index);
    void plotPress(QMouseEvent * pe);
    void plotReleas(QMouseEvent * pe);
    void plotMove(QMouseEvent * pe);
    void replotGraf();
    void drowGraf();
    void on_groupTech_currentIndexChanged(int index);
    void on_buttonResetSteel_clicked();
    void on_timeSteel_currentIndexChanged(const QString &arg1);
    void on_steelRelayBreak_activated(int index);
    void on_steelRelayReady_activated(int index);
    void on_steelRelayMeasure_activated(int index);
    void on_steelRelayTimeOut_activated(int index);
    void on_bDeleteUstavka_clicked();
    void on_bUserPeriod_clicked();
    void on_combo1ChannelArch_currentIndexChanged(int index);
    void on_combo2ChannelArch_currentIndexChanged(int index);
    void on_combo3ChannelArch_currentIndexChanged(int index);
    void on_combo4ChannelArch_currentIndexChanged(int index);

    void on_srcChannel_currentIndexChanged(int index);

    void on_comboTypeValue_currentIndexChanged(int index);
    void on_sensorDiapazon_currentIndexChanged(int index);
    void on_comboUnit_currentIndexChanged(int index);
};

#endif // SETTINGS_H
