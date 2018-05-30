#include "device.h"

Device::Device(QBluetoothDeviceInfo &info,QObject *parent) : QObject(parent), m_info(info)
{
    m_control = new QLowEnergyController(info.address());
    m_name = info.name();
    connect(m_control,SIGNAL(connected()),this,SLOT(deviceConnected()));
    connect(m_control,SIGNAL(disconnected()),this,SLOT(deviceDisconnected()));
    connect(m_control,SIGNAL(serviceDiscovered(QBluetoothUuid)),this,SLOT(addService(QBluetoothUuid)));
    connect(m_control,SIGNAL(discoveryFinished()),this,SLOT(selectService()));
    connect(m_control,SIGNAL(error(QLowEnergyController::Error)),this,SLOT(error(QLowEnergyController::Error)));
    m_control->connectToDevice();


}
Device::~Device()
{
    if(m_service != NULL) {delete m_service;}
    m_control->disconnectFromDevice();
    delete m_control;
}
void Device::deviceConnected()
{
    m_control->discoverServices();

}
void Device::deviceDisconnected()
{
    emit devDis(m_info.name());
}
void Device::addService(QBluetoothUuid serv)
{
    services.append(serv);
}
void Device::write(QString data)
{
    QByteArray tmp;
    tmp.append(data);
    tmp.append("\r\n");
    if(!m_chars.isEmpty())
    {
        m_service->writeCharacteristic(m_chars.first(),tmp);
    }
}
void Device::selectService()
{
    m_service = m_control->createServiceObject(services.last());
    connect(m_service,SIGNAL(stateChanged(QLowEnergyService::ServiceState)),this,SLOT(selectChar(QLowEnergyService::ServiceState)));
    connect(m_service,SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),this,SLOT(characteristicChanged(QLowEnergyCharacteristic,QByteArray)));
    connect(m_service,SIGNAL(error(QLowEnergyService::ServiceError)),this,SLOT(error(QLowEnergyService::ServiceError)));
    m_service->discoverDetails();
}
void Device::selectChar(QLowEnergyService::ServiceState a)
{
    m_chars = m_service->characteristics();
    if(a == QLowEnergyService::ServiceDiscovered)
    {
        emit devCon(m_info.name());
    }
}
QString Device::read()
{
    return m_chars.first().value();
}
void Device::characteristicChanged(QLowEnergyCharacteristic b, QByteArray a)
{
    emit Packet(this->m_info.name(),a);
}
void Device::error(QLowEnergyController::Error er)
{
    switch(er)
    {
    case QLowEnergyController::UnknownRemoteDeviceError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): The remote BLE device with the address is not found or no local bluetooth device").arg(m_info.name()));
        break;
    case QLowEnergyController::NetworkError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): the attempt to read from or write to the device failed").arg(m_info.name()));
        break;
    case QLowEnergyController::InvalidBluetoothAdapterError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): bluetooth adapter cannot be found or no local device").arg(m_info.name()));
        break;
    case QLowEnergyController::ConnectionError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): attempt to connect to this device failed").arg(m_info.name()));
        break;
    case QLowEnergyController::AdvertisingError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): attempt to start advertising failed").arg(m_info.name()));
        break;
    case QLowEnergyController::RemoteHostClosedError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): device closed connection").arg(m_info.name()));
        break;
    case QLowEnergyController::UnknownError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): Unknown error").arg(m_info.name()));
        break;
    }
}
void Device::error(QLowEnergyService::ServiceError er)
{
    switch(er)
    {
    case QLowEnergyService::OperationError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): An operation was attempted while the service was not ready.").arg(m_info.name()));
        break;
    case QLowEnergyService::CharacteristicReadError:
        emit errorMsg(QString("Error from Bluetooth Device(%1):An attempt to read a characteristic value failed.").arg(m_info.name()));
        break;
    case QLowEnergyService::CharacteristicWriteError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): An attempt to write a new value to a characteristic failed.").arg(m_info.name()));
        break;
    case QLowEnergyService::DescriptorReadError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): An attempt to write a new value to a descriptor failed. ").arg(m_info.name()));
        break;
    case QLowEnergyService::DescriptorWriteError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): attempt to start advertising failed").arg(m_info.name()));
        break;
    case QLowEnergyService::UnknownError:
        emit errorMsg(QString("Error from Bluetooth Device(%1): An unknown error occurred when interacting with the service. ").arg(m_info.name()));
        break;
    }
}
