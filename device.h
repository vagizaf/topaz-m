#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDebug>

struct deviceparametrs
{
    QString name;
    uint16_t Offset;
    uint8_t StorageType;
    uint8_t ParamType;
    uint8_t WorkLevelAccess;
};

struct devicestruct
{
    QString name;
    uint16_t ID;
    uint16_t DeviceType;
    uint16_t ModbusNetworkAddress;
    uint16_t AIcount;
    uint16_t MeasureType;
    uint16_t SignalType;
    uint16_t FilterType;
    uint16_t TransferFunction;
    uint16_t Status;
    uint16_t SupportedSignals;

    float TransferSignalLowLimit;
    float TransferSignalHiLimit;
    float TransferScaleHiLimit;
    float TransferScaleLoLimit;
    float DataMeasureGenerate;
    float UserCalibration1Gain;
    float UserCalibration1Offset;
    uint32_t UserCalibration1Date;

    float UserCalibration2Gain;
    float UserCalibration2Offset;
    uint32_t UserCalibration2Date;
    float UserCalibration3Gain;
    float UserCalibration3Offset;
    uint32_t UserCalibration3Date;

    float UserCalibration4Gain;
    float UserCalibration4Offset;
    uint32_t UserCalibration4Date;

    float FactoryCalibration1Gain;
    float FactoryCalibration1Offset;
    uint32_t FactoryCalibration1Date;

    float FactoryCalibration2Gain;
    float FactoryCalibration2Offset;
    uint32_t FactoryCalibration2Date;

    float FactoryCalibration3Gain;
    float FactoryCalibration3Offset;
    uint32_t FactoryCalibration3Date;

    float FactoryCalibration4Gain;
    float FactoryCalibration4Offset;
    uint32_t FactoryCalibration4Date;
    uint16_t DataFlags;
    uint16_t AdditionalCustomParameter;
};

class Device
{
public:
    Device()
    {
        // constructor blt

        DataChan0.Offset = 0;
        DataChan0.StorageType = Device::D;
        DataChan0.ParamType = Device::F32;
        DataChan0.WorkLevelAccess = Device::R;
        DataChan0.name = "DataChan0";

        DeviceParametrsList.append(DataChan0);

        modbusAddress.Offset = 16389;
        modbusAddress.StorageType = Device::D;
        modbusAddress.ParamType = Device::U16;
        modbusAddress.WorkLevelAccess = Device::R;
        modbusAddress.name = "modbusAddress";

        DeviceParametrsList.append(modbusAddress);

        deviceStatus.Offset = 16406;
        deviceStatus.StorageType = Device::D;
        deviceStatus.ParamType = Device::U16;
        deviceStatus.WorkLevelAccess = Device::R;
        deviceStatus.name = "deviceStatus";

        DeviceParametrsList.append(deviceStatus);

        chan0Data.Offset = 0 ; // 32768
        chan0Data.StorageType = Device::D;
        chan0Data.ParamType = Device::F32;
        chan0Data.WorkLevelAccess = Device::R;
        chan0Data.name = "chan0Data";

        DeviceParametrsList.append(chan0Data);

        chan0Status.Offset = 32771;
        chan0Status.StorageType = Device::D;
        chan0Status.ParamType = Device::U16;
        chan0Status.WorkLevelAccess = Device::R;
        chan0Status.name = "chan0Status";

        DeviceParametrsList.append(chan0Status);

        chan0SupportedSignals.Offset = 32780;
        chan0SupportedSignals.StorageType = Device::D;
        chan0SupportedSignals.ParamType = Device::U16;
        chan0SupportedSignals.WorkLevelAccess = Device::R;
        chan0SupportedSignals.name = "chan0SupportedSignals";

        DeviceParametrsList.append(chan0SupportedSignals);

        chan0SignalType.Offset = 32781;
        chan0SignalType.StorageType = Device::NV;
        chan0SignalType.ParamType = Device::U16;
        chan0SignalType.WorkLevelAccess = Device::RW;
        chan0SignalType.name = "chan0SignalType";

        DeviceParametrsList.append(chan0SignalType);

        chan0AdditionalParameter1.Offset = 32782;
        chan0AdditionalParameter1.StorageType = Device::NV;
        chan0AdditionalParameter1.ParamType = Device::A12;
        chan0AdditionalParameter1.WorkLevelAccess = Device::RW;
        chan0AdditionalParameter1.name = "chan0AdditionalParameter1";

        DeviceParametrsList.append(chan0AdditionalParameter1);

        chan0AdditionalParameter2.Offset = 32788;
        chan0AdditionalParameter2.StorageType = Device::NV;
        chan0AdditionalParameter2.ParamType = Device::A12;
        chan0AdditionalParameter2.WorkLevelAccess = Device::RW;
        chan0AdditionalParameter2.name = "chan0AdditionalParameter2";

        DeviceParametrsList.append(chan0AdditionalParameter2);

        chan1Data.Offset = 4 ; //32768
        chan1Data.StorageType = Device::D;
        chan1Data.ParamType = Device::F32;
        chan1Data.WorkLevelAccess = Device::R;
        chan1Data.name = "chan1Data";

        elmetroAB1.Offset = 0;
        elmetroAB1.StorageType = Device::D;
        elmetroAB1.ParamType = Device::F32;
        elmetroAB1.WorkLevelAccess = Device::R;
        elmetroAB1.name = "elmetroAB1";

        foreach (deviceparametrs dp, DeviceParametrsList) {
            qDebug() << "Device: " << dp.name;
        }
    }


    QList<deviceparametrs> DeviceParametrsList;

    deviceparametrs DataChan0;
    deviceparametrs protocolVersion;
    deviceparametrs hardwareVersion;
    deviceparametrs softwareVersion;
    deviceparametrs modbusAddress;
    deviceparametrs deviceStatus;
    deviceparametrs chan0Data;
    deviceparametrs chan0Status;
    deviceparametrs chan0SupportedSignals;
    deviceparametrs chan0SignalType;
    deviceparametrs chan0AdditionalParameter1;
    deviceparametrs chan0AdditionalParameter2;
    deviceparametrs chan1Data;
    deviceparametrs chan1Status;
    deviceparametrs chan1SupportedSignals;
    deviceparametrs chan1SignalType;
    deviceparametrs chan1AdditionalParameter1;
    deviceparametrs chan1AdditionalParameter2;

    deviceparametrs elmetroAB1;


public:
    enum StorageType { D  = 0,
                       S  = 1,
                       NV = 2,
                     };

    enum ParamType { U16 = 0,
                     U32 = 1,
                     F32 = 2,
                     A12 = 3
                   };

    enum WorkLevelAccess{ R  = 0,
                          W  = 1,
                          RW = 2,
                        };
};

#endif // DEVICE_H
