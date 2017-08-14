#ifndef PUSHNOTIFICATION_H
#define PUSHNOTIFICATION_H

#include <QObject>
#include <QQmlEngine>

class PushNotificationRegistrationTokenHandler : public QObject
{
    Q_OBJECT
public:
    PushNotificationRegistrationTokenHandler(QObject* parent = 0);
    ~PushNotificationRegistrationTokenHandler();
signals:
    void fcmRegistrationTokenChanged(QString token);

private:
    //    QString m_fcmToken;
};


#endif // PUSHNOTIFICATION_H
