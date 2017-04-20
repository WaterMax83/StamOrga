#ifndef SEASONTICKET_H
#define SEASONTICKET_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

class SeasonTicketItem: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString place READ place WRITE setPlace NOTIFY placeChanged)
    Q_PROPERTY(quint8 discount READ discount WRITE setDiscount NOTIFY discountChanged)
public:
    explicit SeasonTicketItem(QObject *parent = 0);

    QString name() { return this->m_name; }
    void setName(const QString &name)
    {
        if (this->m_name != name) {
            this->m_name = name;
            emit nameChanged();
        }
    }

    QString place() { return this->m_place; }
    void setPlace(const QString &place)
    {
        if (this->m_place != place) {
            this->m_place = place;
            emit placeChanged();
        }
    }

    quint8 discount() { return this->m_discount; }
    void setDiscount(const quint8 discount)
    {
        if (this->m_discount != discount) {
            this->m_discount = discount;
            emit this->discountChanged();
        }
    }

    Q_INVOKABLE bool isTicketYourOwn() { return this->m_ownTicket; }
    void setTicketOwn(bool value)
    {
        this->m_ownTicket = value;
    }



signals:
    void nameChanged();
    void placeChanged();
    void discountChanged();

public slots:

private:
        QString m_name;
        QString m_place;
        quint8 m_discount;
        bool m_ownTicket;
};

#endif // SEASONTICKET_H
