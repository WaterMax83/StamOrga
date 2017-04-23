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
    Q_PROPERTY(quint32 index READ index WRITE setIndex NOTIFY indexChanged)
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

    quint32 index() { return this->m_index; }
    void setIndex(const quint32 index)
    {
        if (this->m_index != index) {
            this->m_index = index;
            emit this->indexChanged();
        }
    }

    quint32 userIndex() { return this->m_userIndex; }
    void setUserIndex(const quint32 userIndex)
    {
        if (this->m_userIndex != userIndex) {
            this->m_userIndex = userIndex;
        }
    }

    Q_INVOKABLE bool isTicketYourOwn() { return this->m_ownTicket; }
    void checkTicketOwn(quint32 userIndex)
    {
        if (this->m_userIndex == userIndex)
            this->m_ownTicket = true;
        else
            this->m_ownTicket = false;
    }



signals:
    void nameChanged();
    void placeChanged();
    void discountChanged();
    void indexChanged();

public slots:

private:
        QString m_name;
        QString m_place;
        quint8 m_discount;
        quint32 m_index;
        quint32 m_userIndex;
        bool m_ownTicket;
};

#endif // SEASONTICKET_H
