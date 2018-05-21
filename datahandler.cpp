#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) : QObject(parent)
{

}
DataHandler::~DataHandler()
{
    qDeleteAll(dataHolder);
}
bool DataHandler::isDataObject(QString id)
{
    return ((getData(id) != NULL) ? true : false);
}


bool DataHandler::WriteData(QString id, HumidtyData dat)
{

    if(isDataObject(id))
    {
        auto obj = getData(id);
        obj->appData(dat);
        return true;
    }
    else
    {
        Data* tmp = new Data(id);
        tmp->appData(dat);
        dataHolder.push_back(tmp);
        if(getData(id)->getID() == id)
        {
            return true;
        }
        return false;
    }
}

Data *DataHandler::getData(QString id)
{
    for(auto b : dataHolder)
    {
        if(b->getID() == id)
        {
            return b;
        }
    }
    return NULL;
}
