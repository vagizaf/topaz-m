#ifndef CEXTMODBUS_H
#define CEXTMODBUS_H

#include <QObject>
//#include <QString>
#include <QMutex>

#include "src/modbus.h"
#include "modbus_lookup_tables.h"

#define SIZE_EXT_MODBUS_BUFFER 256

typedef struct {
    uint8_t data[SIZE_EXT_MODBUS_BUFFER];
} tModbusBuffer;

class cExtModbus : public QObject
{
    Q_OBJECT
public:
    explicit cExtModbus(QObject *parent = 0);
    ~cExtModbus();
    int init(int type);

    enum {
        MB_OFF,
        TCP,
        RTU
    };

signals:
    void signalUpdateParam(QString name, tModbusBuffer buffer);
    void signalActualizeParam(QString name);

public slots:
    void run();
    void updateData(QString name, tModbusBuffer buffer);
    void slotReinit();
private:
    int socket;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    int use_backend;
    int set_backend;
    int tcp_port;
    uint8_t *query;
    int header_length;
    int maxNbInputRegisters;
    int maxNbHoldingRegisters;
    int maxNbCoil;
    int maxNbDiscrete;
    QMutex mutex;
    int master_socket;
    fd_set refset;
    fd_set rdset;
    /* Maximum file descriptor number */
    int fdmax;
    bool needReinit;


    const tExtLookupRegisters *getLookupElementByOffsetAndFunc(uint16_t offset, uint8_t func);
    uint8_t updateParam(const void *param);
    void reply(int req_length);

    bool isAccess(uint8_t func, uint8_t access);
    void reinit(int type);
};


#endif // CEXTMODBUS_H




