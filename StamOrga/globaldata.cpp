#include <QtGui/QGuiApplication>
#include <QtCore/QDebug>


#include "globaldata.h"

GlobalData::GlobalData(QObject *parent) : QObject(parent)
{
    QGuiApplication::setOrganizationName("WaterMax");
    QGuiApplication::setApplicationName("StamOrga");
    this->bIsConnected = false;
    this->uUserProperties = 0x0;

    int tmp = QHostInfo::lookupHost("watermax83.ddns.net", this, SLOT(callBackLookUpHost(QHostInfo)));
    qDebug().noquote() << QString("Called lookup host %1").arg(tmp);
}

void GlobalData::loadGlobalSettings()
{
    this->m_pMainUserSettings = new QSettings();

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->setUserName(this->m_pMainUserSettings->value("UserName", "").toString());
    this->setPassWord(this->m_pMainUserSettings->value("Password", "").toString());
    this->setIpAddr(this->m_pMainUserSettings->value("IPAddress", "140.80.61.57").toString());
    this->setConMasterPort(this->m_pMainUserSettings->value("ConMasterPort", 55000).toInt());

    this->m_pMainUserSettings->endGroup();
}

void GlobalData::saveGlobalUserSettings()
{
    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->m_pMainUserSettings->setValue("UserName", this->userName());
    this->m_pMainUserSettings->setValue("Password", this->passWord());
    this->m_pMainUserSettings->setValue("IPAddress", this->ipAddr());
    this->m_pMainUserSettings->setValue("ConMasterPort", this->conMasterPort());

    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->sync();

}

void GlobalData::callBackLookUpHost(const QHostInfo &host)
{
#ifdef Q_OS_ANDROID
    if (host.addresses().size() > 0)
        this->setIpAddr(host.addresses().value(0).toString());
#else
    if (host.addresses().size() > 0)
        qDebug().noquote() << QString("Getting host info ip: %1").arg(host.addresses().value(0).toString()) ;
#endif

}
