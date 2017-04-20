#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

class GamePlay : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString home READ home WRITE setHome NOTIFY homeChanged)
    Q_PROPERTY(QString away READ away WRITE setAway NOTIFY awayChanged)
    Q_PROPERTY(QString score READ score WRITE setScore NOTIFY scoreChanged)
    Q_PROPERTY(QString timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString competition READ competition NOTIFY competitionChanged)
    Q_PROPERTY(quint8 index READ index WRITE setIndex NOTIFY indexChanged)
public:
    explicit GamePlay(QObject *parent = 0);

    QString home() { return this->m_home; }
    void setHome(const QString &home)
    {
        if (this->m_home != home) {
            this->m_home = home;
            emit homeChanged();
        }
    }

    QString away() { return this->m_away; }
    void setAway(const QString &away)
    {
        if (this->m_away != away) {
            this->m_away = away;
            emit awayChanged();
        }
    }

    QString score() { return this->m_score; }
    void setScore(const QString &score)
    {
        if (this->m_score != score) {
            this->m_score = score;
            emit scoreChanged();
        }
    }

    QString timestamp()
    {
        return QDateTime::fromMSecsSinceEpoch(this->m_timestamp).toString("ddd, dd.MM.yy hh:mm");
    }
    qint64 timestamp64Bit()
    {
        return this->m_timestamp;
    }
    void setTimeStamp(qint64 ts)
    {
        if (this->m_timestamp != ts) {
            this->m_timestamp = ts;
            emit this->timestampChanged();
        }
    }

    QString competition()
    {
        switch(this->m_comp) {
        case 1:
            return "1 Bundesliga";
        case 2:
            return "2 Bundesliga";
        default:
            return "Unknown";
        }
    }
    quint8 compValue()
    {
        return this->m_comp;
    }

    void setCompetition(quint8 co)
    {
        if (this->m_comp != co){
            this->m_comp = co;
            emit this->competitionChanged();
        }
    }

    quint8 index() { return this->m_index; }
    void setIndex(const quint8 index)
    {
        if (this->m_index != index) {
            this->m_index = index;
            emit this->indexChanged();
        }
    }
    Q_INVOKABLE QString getCompetitionIndex()
    {
        if (this->m_comp == 1 || this->m_comp == 2) {
            return QString("%1. Spieltag").arg(this->m_index);
        }
        return "not implemented";
    }

    Q_INVOKABLE bool isGameInPast()
    {
        QDateTime now = QDateTime::currentDateTime();
        if (now.toMSecsSinceEpoch() > this->m_timestamp)
            return true;
        return false;
    }


signals:
    void homeChanged();
    void awayChanged();
    void scoreChanged();
    void timestampChanged();
    void competitionChanged();
    void indexChanged();

public slots:

private:
        QString m_home;
        QString m_away;
        QString m_score;
        quint8 m_comp;
        quint8 m_index;
        qint64 m_timestamp;
};

#endif // GAMEPLAY_H
