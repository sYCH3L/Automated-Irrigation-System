#include "wateringgui.h"
#include "ui_wateringgui.h"

WateringGUI::WateringGUI(QString ver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WateringGUI)
{
    ui->setupUi(this);
    upTime.start();

    ui->label_13->setText(ver);
    ui->uptime->setText(convertToTime(upTime.elapsed()));

    cntrlThread = new QThread();
    cntrl = new Control();
    //cntrl->moveToThread(cntrlThread);

    ui->sFrame->setEnabled(false);
    ui->cFrame->setEnabled(false);

    connect(cntrl,SIGNAL(devConnected(QString)),this,SLOT(addDev(QString)));
    connect(cntrl,SIGNAL(devDisconnected(QString)),this,SLOT(removeDev(QString)));
    connect(cntrl,SIGNAL(errorMsg(QString)),this,SLOT(errorHandler(QString)));

    connect(this,SIGNAL(devSelect_c(QString)),this,SLOT(loadcFrame(QString)));
    connect(this,SIGNAL(devSelect_s(QString)),this,SLOT(loadsFrame(QString)));

}

WateringGUI::~WateringGUI()
{
    delete ui;
    delete cntrl;
    delete cntrlThread;
}
void WateringGUI::addDev(QString id)
{
    ui->activeDev->setText(QString(cntrl->getActiveDev()));
    ui->totalDev->setText(QString(cntrl->getTotalDev()));

    ui->cDevList->addItem(id);
    ui->sDevList->addItem(id);
    ui->mDevList->addItem(id);
}
void WateringGUI::removeDev(QString id)
{
    if(!id.isEmpty())
    {
        ui->activeDev->setText(QString(cntrl->getActiveDev()));
        ui->totalDev->setText(QString(cntrl->getTotalDev()));

        ui->cDevList->removeItem(ui->cDevList->findText(id,Qt::MatchExactly));
        ui->sDevList->removeItem(ui->sDevList->findText(id,Qt::MatchExactly));
        ui->mDevList->removeItemWidget(ui->mDevList->findItems(id,Qt::MatchExactly).first());
    }


}

void WateringGUI::on_cDevList_currentIndexChanged(int index)
{
    ui->cFrame->setEnabled(true);
}

void WateringGUI::on_sDevList_currentIndexChanged(int index)
{
    ui->sFrame->setEnabled(true);
}

void WateringGUI::on_sDevList_currentIndexChanged(const QString &arg1)
{
    ui->sFrame->setEnabled(true);
    emit devSelect_s(arg1);
}
void WateringGUI::errorHandler(QString msg)
{
    ui->errorTab->setCurrentIndex(3);
    ui->errorView->addItem(msg);
}

void WateringGUI::on_cDevList_currentIndexChanged(const QString &arg1)
{
    ui->cFrame->setEnabled(true);
    emit devSelect_c(arg1);
}
void WateringGUI::loadcFrame(QString id)
{
    if(!id.isEmpty())
    {
        ui->devID->setText(id);
        ui->lastUpdate->setText(cntrl->dataHndlr->getData(id).getLatestTimeStamp());
        ui->lastHum->setText(cntrl->dataHndlr->getData(id).getLatestValue());
        ui->reqHum->setText(QString::number(cntrl->getSettings(id)->getReqHum()));
        ui->timeInterval->setValue(cntrl->getSettings(id)->getTimeout());
    }
    else
    {
        ui->cFrame->setEnabled(false);
    }
}
void WateringGUI::loadsFrame(QString id)
{
    Data tmp = cntrl->dataHndlr->getData(id);
    if(tmp.hasData())
    {
        QLineSeries *series = new QLineSeries();
        QList<HumidtyData> list = tmp.getList();

        //Delete old widget
        //ui->groupBox_2->layout()->removeItem(ui->groupBox_2->layout()->itemAt(0));
//        if(!ui->groupBox_2->layout()->isEmpty())
//        {
//            ui->groupBox_2->layout()->removeItem(ui->groupBox_2->layout()->itemAt(0));
//        }
        ui->listWidget_2->clear();




        // create charts && add dates irrigated
        for(auto a : list)
        {
            QDateTime date = QDateTime::fromString(a.getTimeStamp());
            series->append(date.toMSecsSinceEpoch(),a.getValue());
            ui->listWidget_2->addItem(a.getTimeStamp());
        }

        QChart *chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series);
        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setTickCount(10);
        axisX->setFormat("MMM yyyy");
        axisX->setTitleText("Date");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis;
        axisY->setLabelFormat("%i");
        axisY->setTitleText("Humidity");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        //chart->setTitle("Humidity in 7 days");

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        QVBoxLayout *vbox = new QVBoxLayout();
        vbox->addWidget(chartView);

        ui->groupBox_2->setLayout(vbox);

    }
    else
    {
        ui->errorView->addItem(QString("No data available for %1 device").arg(id));
        //ui->sFrame->setEnabled(false);
    }


}

void WateringGUI::on_setHum_clicked()
{
    cntrl->getSettings(ui->cDevList->currentText())->setReqHum(ui->newReqHum->text().toDouble());
    emit devSelect_c(ui->cDevList->currentText());
}

void WateringGUI::on_sendCMD_clicked()
{
    cntrl->blHndlr->write(ui->cDevList->currentText(),ui->customCMD->text());
}

void WateringGUI::on_checkHumbtn_clicked()
{
    cntrl->blHndlr->write(ui->cDevList->currentText(),"getHumidity");
}

void WateringGUI::on_forceIrrbtn_clicked()
{
    cntrl->blHndlr->write(ui->cDevList->currentText(),"irrigate");
}

void WateringGUI::on_restartDvcbtn_clicked()
{
    cntrl->blHndlr->write(ui->cDevList->currentText(),"restart");
}
QString WateringGUI::convertToTime(qint64 time)
{
    if(time > 0)
    {
        auto seconds = (time / 1000) % 60;
        auto minutes = (time / 1000) / 60;
        auto hours = seconds / 3600;

        return QString("H:%1 M:%2 S:%3").arg(QString::number(hours),QString::number(minutes),QString::number(seconds));
    }
    return QString("0");
}
void WateringGUI::on_errorTab_tabBarClicked(int index)
{
    if(index == 0)
    {
        ui->uptime->setText(convertToTime(upTime.elapsed()));
    }
    else if(index == 1)
    {
        loadcFrame(ui->cDevList->currentText());
    }
    else if(index == 2)
    {
        loadsFrame(ui->sDevList->currentText());
    }

}

void WateringGUI::on_timeInterval_sliderReleased()
{
    if(!ui->cDevList->currentText().isEmpty())
    {
        auto conv = 60000 * ui->timeInterval->value();
        auto obj = cntrl->getSettings(ui->cDevList->currentText());
        if(obj != NULL)
        {
            obj->setTimeout(conv);
            QString tx("ARV:%1");
            tx.arg(QString::number(conv));
            cntrl->blHndlr->write(ui->cDevList->currentText(),tx);
        }
    }
}

void WateringGUI::on_falseData_clicked()
{
    cntrl->falseData(ui->cDevList->currentText());
}
