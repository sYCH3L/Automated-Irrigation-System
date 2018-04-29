#ifndef DATAHANDLER_H
#define DATAHANDLER_H


#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

class HumidtyData
{
public:
    HumidtyData(QString tmStmp, double val) : m_timeStamp(tmStmp) {
        m_value = val;
    }
    QString getTimeStamp() { return m_timeStamp;}
    double getValue() { return m_value; }
private:
    QString m_timeStamp;
    double m_value;
};

class Data {
public:
    Data(QString ID) : m_deviceID(ID){}
    void appData(HumidtyData t) { humData.append(t); }
    QString getID() { return m_deviceID; }
    bool hasData() { return (humData.size() > 0) ? true : false; }
    QString getLatestTimeStamp() { if(hasData()) {return humData.last().getTimeStamp(); } return QString("No Data"); }
    QString getLatestValue() { if(hasData()) { return QString(QString::number(humData.last().getValue())); } return QString("No Data"); }
    QList<HumidtyData> getList() { return humData; }

private:
      QString m_deviceID;
      QList<HumidtyData> humData;

};


class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QObject *parent = nullptr);
    Data getData(QString id);
    bool WriteData(QString id, HumidtyData dat);
private:
    QList<Data> dataHolder;
signals:

public slots:
};

#endif // DATAHANDLER_H
