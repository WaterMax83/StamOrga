#ifndef PUSHNOTIFICATION_H
#define PUSHNOTIFICATION_H

#include <QObject>
#include <QQmlEngine>

class PushNotificationInformationHandler : public QObject
{
    Q_OBJECT
public:
    PushNotificationInformationHandler(QObject* parent = 0);
    ~PushNotificationInformationHandler();

    void setNewRegistrationToken(QString token);
signals:
    void fcmRegistrationTokenChanged(QString token);

private:
    QString m_fcmToken;
};


#endif // PUSHNOTIFICATION_H
