#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth>
#include <QList>
#include <QTimer>
#include "device.h"


class bluetooth : public QObject
{
    Q_OBJECT
public:
    explicit bluetooth(QObject *parent = nullptr);
    ~bluetooth();
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QList<Device*> deviceList;
    QTimer *re;

    QString read(QString id);
    void write(QString id, QString data);
    void disconnectDevice(QString id);

signals:
    void dConnect(QString id);
    void dDisconnect(QString id);
    void errorMsg(QString error);
    void newData(QString id, QByteArray data);

public slots:
    void addDevice(const QBluetoothDeviceInfo &info);
    void remDevice(const QBluetoothDeviceInfo &info);
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error er);
    void scanFinished();
    void scanStart();
    void devConnected(QString id);
    void devDisconnected(QString id);
    void newPacket(QString id, QByteArray data);
    void errorMsg_(QString error);


};

#endif // BLUETOOTH_H
