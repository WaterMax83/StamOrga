#ifndef GAMES_H
#define GAMES_H


#include <QtCore/QList>


#include "configlist.h"

class GamesPlay : public ConfigItem
{
public:
    QString away;
    quint8 saisonIndex;
    quint8 competition;
    QString score;

    static bool compareTimeStampFunction(GamesPlay p1, GamesPlay p2)
    {
        if (p1.m_timestamp > p2.m_timestamp)
            return false;
        return true;
    }
};

#define PLAY_AWAY               "away"
#define PLAY_SAISON_INDEX       "sIndex"

#define PLAY_SCORE              "score"
#define PLAY_COMPETITION        "competition"

class Games : public ConfigList
{
public:
    Games();
    ~Games();

    int addNewGame(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp);
    int showAllGames();

    quint32 getNumberOfInternalList() { return this->m_lInteralList.size(); }

    GamesPlay *gameExists(quint8 sIndex, quint8 comp, qint64 timestamp);
    bool gameExists(quint32 index);

    GamesPlay *getRequestConfigItem(int index)
    {
        if (index < this->m_lInteralList.size())
            return &this->m_lInteralList[index];
        return NULL;
    }

    void sortGamesListByTime();


private:
    QList<GamesPlay>    m_lInteralList;
    QList<GamesPlay>    m_lAddItemProblems;

    void saveCurrentInteralList() override;

    bool addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp, quint32 index, bool checkGame = true);
    void addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp, quint32 index, QList<GamesPlay> *pList);

    bool updateGamesPlayValue(GamesPlay *pGame, QString key, QVariant value);

};

#endif // GAMES_H
