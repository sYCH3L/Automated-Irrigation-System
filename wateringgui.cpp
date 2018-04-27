#include "wateringgui.h"
#include "ui_wateringgui.h"

WateringGUI::WateringGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WateringGUI)
{
    ui->setupUi(this);
    cntrlThread = new QThread();
    cntrl->moveToThread(cntrlThread);
    cntrl = new Control();

    ui->sFrame->setEnabled(false);
    ui->cFrame->setEnabled(false);

    connect(cntrl,SIGNAL(devConnected(QString)),this,SLOT(addDev(QString)));
    connect(cntrl,SIGNAL(devDisconnected(QString)),this,SLOT(removeDev(QString)));

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
    ui->activeDev->setText(QString(cntrl->getActiveDev()));
    ui->totalDev->setText(QString(cntrl->getTotalDev()));

    ui->cDevList->removeItem(ui->cDevList->findText(id,Qt::MatchExactly));
    ui->sDevList->removeItem(ui->sDevList->findText(id,Qt::MatchExactly));
    QListWidgetItem *tmp;
    tmp->setText(id);
    ui->mDevList->removeItemWidget(tmp);


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
    ui->errorView->addItem(msg);
}

void WateringGUI::on_cDevList_currentIndexChanged(const QString &arg1)
{
    ui->cFrame->setEnabled(true);
    emit devSelect_c(arg1);
}
void WateringGUI::loadcFrame(QString id)
{
    ui->devID->setText(id);
    ui->lastUpdate->setText(cntrl->dataHndlr->getData(id).getLatestTimeStamp());
    ui->lastHum->setText(cntrl->dataHndlr->getData(id).getLatestValue());
    ui->reqHum->setText(QString::number(cntrl->getSettings(id)->getReqHum()));
}
void WateringGUI::loadsFrame(QString id)
{

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
