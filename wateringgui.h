#ifndef WATERINGGUI_H
#define WATERINGGUI_H

#include <QMainWindow>
#include <control.h>
#include <QThread>
#include <QtCharts>

namespace Ui {
class WateringGUI;
}

class WateringGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit WateringGUI(QString ver,QWidget *parent = 0);
    ~WateringGUI();
    Control *cntrl;
    QThread *cntrlThread;

public slots:
    void removeDev(QString id);
    void addDev(QString id);
private slots:
    void on_cDevList_currentIndexChanged(int index);

    void on_sDevList_currentIndexChanged(int index);

    void on_sDevList_currentIndexChanged(const QString &arg1);

    void on_cDevList_currentIndexChanged(const QString &arg1);

    void loadsFrame(QString id);
    void loadcFrame(QString id);

    void on_setHum_clicked();

    void on_sendCMD_clicked();

    void errorHandler(QString msg);

    void on_checkHumbtn_clicked();

    void on_forceIrrbtn_clicked();

    void on_restartDvcbtn_clicked();

signals:
    void devSelect_c(QString id);
    void devSelect_s(QString id);

private:
    Ui::WateringGUI *ui;
};

#endif // WATERINGGUI_H
