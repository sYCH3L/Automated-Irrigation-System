#include "bluetooth.h"

bluetooth::bluetooth(QObject *parent) : QObject(parent)
{
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    re = new QTimer();

    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

    connect(re,SIGNAL(timeout()),this,SLOT(scanStart()));

    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

}
bluetooth::~bluetooth()
{
    delete discoveryAgent;
    delete re;
    qDeleteAll(deviceList);

}
void bluetooth::addDevice(const QBluetoothDeviceInfo &info)
{
    if(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        if(info.name().contains("Kastmine"))
        {
            QBluetoothDeviceInfo inf = info;
            Device *dev = new Device(inf);
            connect(dev,SIGNAL(devCon(QString)),this,SLOT(devConnected(QString)));
            connect(dev,SIGNAL(devDis(QString)),this,SLOT(devDisconnected(QString)));
            connect(dev,SIGNAL(Packet(QString,QByteArray)),this,SLOT(newPacket(QString,QByteArray)));
            connect(dev,SIGNAL(errorMsg(QString)),this,SLOT(errorMsg_(QString)));
            deviceList.append(dev);
        }
    }
}
void bluetooth::remDevice(const QBluetoothDeviceInfo &info)
{
    for(auto &a : deviceList)
    {
        if(a->getAddress() == info.address())
        {
            deviceList.removeOne(a);
        }
    }
}
void bluetooth::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error er)
{
    emit errorMsg(QString("Error from Bluetooth discovery: %1").arg(discoveryAgent->errorString()));
}
void bluetooth::scanFinished()
{
    //discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    re->start(300000);
}
void bluetooth::scanStart()
{
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}
void bluetooth::devConnected(QString id)
{
    emit dConnect(id);
}
void bluetooth::devDisconnected(QString id)
{
    emit dDisconnect(id);
}
QString bluetooth::read(QString id)
{
    for(auto a : deviceList)
    {
        if(a->getName() == id)
        {
            QString tmp = a->read();
            return tmp;
        }
    }    
    return QString("No Data");
}
void bluetooth::write(QString id, QString data)
{
    for(auto a : deviceList)
    {
        if(a->getName() == id)
        {
            a->write(data);
        }
    }
}
void bluetooth::disconnectDevice(QString id)
{
    for(auto a : deviceList)
    {
        if(a->getName() == id)
        {
            a->disconnectIt();
        }
    }
}
void bluetooth::newPacket(QString id, QByteArray data)
{
    emit newData(id,data);
}
void bluetooth::errorMsg_(QString error)
{
    emit errorMsg(error);
}
