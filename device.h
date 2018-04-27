#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QString>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QBluetoothDeviceInfo &info,QObject *parent = nullptr);
    ~Device();

    QString m_name;

    QString getName() { return m_name; }
    void setName(QString name) { m_name = name;}
    QBluetoothAddress getAddress() { return m_info.address(); }
    void write(QString data);
    QString read();

    void disconnectIt() { m_control->disconnectFromDevice(); }


private:
    QLowEnergyController* m_control;
    QLowEnergyService *m_service;
    QList<QLowEnergyCharacteristic> m_chars;
    QBluetoothDeviceInfo m_info;
    QList<QBluetoothUuid> services;

signals:
    void devCon(QString ID);
    void devDis(QString ID);
    void Packet(QString ID, QByteArray data);
    void errorMsg(QString msg);
public slots:
    void deviceConnected();
    void deviceDisconnected();
    void addService(QBluetoothUuid serv);
    void selectService();
    void selectChar(QLowEnergyService::ServiceState a);
    void characteristicChanged(QLowEnergyCharacteristic b ,QByteArray a);
    void error(QLowEnergyController::Error er);
    void error(QLowEnergyService::ServiceError er);

};

#endif // DEVICE_H
