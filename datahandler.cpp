#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) : QObject(parent)
{

}
DataHandler::~DataHandler()
{

}

bool DataHandler::WriteData(QString id, HumidtyData dat)
{
    Data tmp(id);
    tmp.appData(dat);
    dataHolder.push_back(tmp);
    if(getData(id).getID() == id)
    {
        return true;
    }
    return false;
}

Data DataHandler::getData(QString id)
{
    for(auto b : dataHolder)
    {
        if(b.getID() == id)
        {
            return b;
        }
    }
    return Data(NULL);
}
