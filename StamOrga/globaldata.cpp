#include <QtGui/QGuiApplication>
#include <QtCore/QDebug>

#include "globaldata.h"

GlobalData::GlobalData(QObject *parent) : QObject(parent)
{
    QGuiApplication::setOrganizationName("WaterMax");
    QGuiApplication::setApplicationName("StamOrga");

    this->m_pMainUserSettings = new QSettings();

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->setUserName(this->m_pMainUserSettings->value("UserName", "").toString());
    this->setIpAddr(this->m_pMainUserSettings->value("IPAddress", "140.80.61.57").toString());
    this->setConMasterPort(this->m_pMainUserSettings->value("ConPort", 55000).toInt());

    this->m_pMainUserSettings->endGroup();
}

void GlobalData::saveGlobalUserSettings()
{
    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->m_pMainUserSettings->setValue("UserName", this->userName());
    this->m_pMainUserSettings->setValue("IPAddress", this->ipAddr());
    this->m_pMainUserSettings->setValue("ConPort", this->conMasterPort());

    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->sync();

}
