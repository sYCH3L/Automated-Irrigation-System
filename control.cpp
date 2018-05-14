#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    dataHndlr = new DataHandler();
    blHndlr = new bluetooth();

    connect(blHndlr,SIGNAL(newData(QString,QByteArray)),this,SLOT(handleData(QString,QByteArray)));
    connect(blHndlr,SIGNAL(dConnect(QString)),this,SLOT(connectDevice(QString)));
    connect(blHndlr,SIGNAL(dDisconnect(QString)),this,SLOT(deviceDisconnected(QString)));
   connect(blHndlr,SIGNAL(errorMsg(QString)),this,SLOT(errorMsg_(QString)));

}
Control::~Control()
{
    delete dataHndlr;
    delete blHndlr;

    qDeleteAll(deviceList);
}
int Control::getActiveDev()
{
    int i = 0;
    for(auto &a : deviceList)
    {
        if(a->getStatus() == true)
        {
            i++;
        }
    }
    return i;
}
void Control::registerNewDevice(QString id)
{

    Devices *nDevice = new Devices(id);
    deviceList.push_back(nDevice);

    if(deviceExists(id))
    {
        //connect(nDevice,SIGNAL(Timeout(QString)),this,SLOT(sendHumidty(QString)));
        emit devConnected(id);
    }

}
void Control::sendHumidty(QString id)
{
    blHndlr->write(id,QString("getHumidity"));
}

void Control::deviceDisconnected(QString id)
{
    if(deviceExists(id))
    {
        auto dv = getSettings(id);
        if(dv->getId() != NULL)
        {
            dv->setStatus(false);
        }
        emit devDisconnected(id);
        return;
    }
}
void Control::connectDevice(QString id)
{
    if(!deviceExists(id) && !isWaitingToBeRegistered(id))
    {
        blHndlr->write(id,"register");
        devicesToRegister.append(id);
    }
    else if(!isWaitingToBeRegistered(id))
    {
        sendHumidty(id);
    }
}
bool Control::deviceExists(QString id)
{
    for(auto &d : deviceList)
    {
        if(d->getId() == id)
        {
            return true;
        }
    }
    return false;
}
Devices *Control::getSettings(QString id)
{
    for(auto d : deviceList)
    {
        if(d->getId() == id)
        {
            return d;
        }
    }
    return NULL;
}
void Control::chooseAction(QString id, double hum)
{
    auto device = getSettings(id);
    if(device == NULL)
    {
        emit errorMsg(QString("Error: Could not find the device with id %1 when choosing action").arg(id));
    }
    else
    {
        if(device->getReqHum() > hum)
        {
            if(dataHndlr->WriteData(id, HumidtyData(QDateTime::currentDateTime().toString(),hum)))
            {
                auto tmz = getSettings(id);
                if(tmz != NULL)
                {
                    getSettings(id)->setLastHum(hum);
                    getSettings(id)->setLastUpdt(QDateTime::currentDateTime().toString());
                }
                initIrrigation(device->getId());
            }
            return;
        }
        initTimeout(device->getId());
    }
}
void Control::initIrrigation(QString id)
{
    blHndlr->write(id,QString("IRRIGATE"));
}
void Control::initTimeout(QString id)
{
    auto dev = getSettings(id);
    if(dev != NULL)
    {
        //blHndlr->write(id,QString("TIMEOUT"));
    }
    else
    {
        emit errorMsg("Error: Could not retrive device with id % when trying to initiate a timeout for device!");
    }


}
void Control::handleData(QString id, QByteArray data)
{
    QString cmd(data.toStdString().c_str());
    if(deviceExists(id) || isWaitingToBeRegistered(id))
    {
        if(isWaitingToBeRegistered(id))
        {
            if(cmd.contains("GER"))
            {
                registerNewDevice(id);
            }
        }
        else
        {
            blHndlr->disconnectDevice(id);
        }
        if(cmd.contains("HUM"))
        {
            auto humidty = cmd.midRef(2).toDouble();
            chooseAction(id,humidty);


        }

    }
    else
    {
        blHndlr->disconnectDevice(id);
    }
}
bool Control::isWaitingToBeRegistered(QString id)
{
    for(auto &a : devicesToRegister)
    {
        if(a == id)
        {
            return true;
        }
    }
    return false;
}
void Control::errorMsg_(QString msg)
{
    emit errorMsg(msg);
}
