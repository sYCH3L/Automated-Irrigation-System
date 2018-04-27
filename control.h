#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <datahandler.h>
#include <bluetooth.h>
#include <QThread>

class Devices : public QObject {
    Q_OBJECT
public:
    Devices(QString ID, QObject *parent = nullptr) : m_id(ID) {
        m_connected = true;
        m_lastUpdate = QDateTime::currentDateTime().toString();
        m_reqHum = 50.0;
        m_updateTimeout = 300000;
        m_lastHumidty = 0;

        updateTimer = new QTimer();
        connect(updateTimer,SIGNAL(timeout()),this,SLOT(sendTimeout()));
    }
    ~Devices()
    {
        delete updateTimer;
    }

    /* Timer */
    void startTimer() { updateTimer->start(m_updateTimeout); }
    /* Getters */
    QString getId() { return m_id;}
    double getReqHum() { return m_reqHum; }
    double getTimeout() { return m_updateTimeout;}
    QString getLastUpdt() { return m_lastUpdate; }
    bool getStatus() { return m_connected; }
    double getLastHum() { return m_lastHumidty; }
    /* Setters */
    bool setReqHum(double hum) { return (m_reqHum = hum); }
    bool setLastUpdt(QString date){
        m_lastUpdate.clear();
        m_lastUpdate.append(date);
        return true;
    }
    bool setLastHum(double hum) { return (m_lastHumidty = hum); }
    bool setTimeout(double tim) { return (m_updateTimeout = tim); }
    bool setStatus(bool status) { return (m_connected = status); }

signals:
    void Timeout(QString id);
public slots:
    void sendTimeout() { emit Timeout(m_id); }

private:
       QTimer *updateTimer;
       QString m_id;
       bool m_connected;
       QString m_lastUpdate;
       double m_reqHum;
       double m_lastHumidty;
       int m_updateTimeout; // in mseconds
};

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    ~Control();
    void registerNewDevice(QString id);
    void setSettings(double reqHum, int timeout);
    Devices* getSettings(QString id);
    void chooseAction(QString id, double hum);
    bool deviceExists(QString id);
    bool isWaitingToBeRegistered(QString id);
    void initIrrigation(QString id);
    void initTimeout(QString id);

    int getActiveDev();
    int getTotalDev() { return deviceList.size(); }

    DataHandler *dataHndlr;
    bluetooth *blHndlr;
    QThread *dataHndlrThread;
    QThread *blHndlrThread;
    QList<QString> devicesToRegister;

protected:
    QList<Devices*> deviceList;
signals:
    void devConnected(QString id);
    void devDisconnected(QString id);
    void errorMsg(QString msg);

public slots:
    void handleData(QString id, QByteArray data);
    void connectDevice(QString id);
    void deviceDisconnected(QString id);
    void sendHumidty(QString id);
    void errorMsg_(QString msg);
};

#endif // CONTROL_H
