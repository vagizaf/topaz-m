#ifndef CSYSTEMOPTIONS_H
#define CSYSTEMOPTIONS_H

#include <QObject>
#include <QtCore>

class cSystemOptions : public QObject
{
    Q_OBJECT
public:
    explicit cSystemOptions(QObject *parent = 0);
    int display;
    bool arrows;
    bool autoscale;
    int brightness;
    int mode;   // режим прибора
    int dateindex;
    int timeindex;

    enum DisplayParametrEnum {
        Bars = 0x00,
        TrendsBars = 0x01,
        PolarBars = 0x02,
        Cyfra = 0x04,
        TrendsCyfra = 0x05,
        PolarCyfra = 0x06,
        Steel = 0x08,
        //исключить
        Trends = 0xF1,
        Polar = 0xF2,
        BarsCyfra = 0xF7,
        TrendsCyfraBars = 0xF8
    };
    enum TypeMultigraphEnum {
        Multigraph = 0,
        Multigraph_Steel = 1
    };
    TypeMultigraphEnum typeMultigraph;
    enum TypeExtModbusInterface {
        ExtModbus_None = 0,
        ExtModbus_TCP = 1,
        ExtModbus_RTU = 2,
    };
//    TypeExtModbusInterface typeExtModbus;
    enum TypeExtModbusParity {
        ExtModbus_ParityNone = 0,
        ExtModbus_ParityOdd = 1,
        ExtModbus_ParityEven = 2,
    };
//    TypeExtModbusEven extModbasEven;
    typedef struct {
        TypeExtModbusInterface type;
        uint8_t adress;
        int baud;
        uint8_t dataBits;
        float stopBits;
        int parity;
        uint16_t port;
    } typeExtModbus;
    typeExtModbus extModbus;
    QList<int> listBauds;

signals:

public slots:
};

#endif // CSYSTEMOPTIONS_H
