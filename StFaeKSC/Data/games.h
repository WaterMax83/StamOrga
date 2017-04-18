#ifndef GAMES_H
#define GAMES_H


#include <QtCore/QSettings>
#include <QtCore/QList>
#include <QtCore/QMutex>

struct GamesPlay {
    QString home;
    QString away;
    qint64 datetime;
    quint8 saisonIndex;
    quint8 competition;
    QString score;
    quint32 index;

    static bool compareDateTimeFunction(GamesPlay p1, GamesPlay p2)
    {
        if (p1.datetime > p2.datetime)
            return false;
        return true;
    }
};

#define GAMES_GROUP             "GAMES"
#define PLAY_ARRAY              "play"
#define PLAY_HOME               "home"
#define PLAY_AWAY               "away"
#define PLAY_DATETIME           "datetime"
#define PLAY_SAISON_INDEX       "sIndex"
#define PLAY_INDEX              "index"
#define PLAY_SCORE              "score"
#define PLAY_COMPETITION        "competition"

//#define DEFAULT_LOGIN_PROPS     0x0

//#define MIN_SIZE_USERNAME       5

class Games
{
public:
    Games();
    ~Games();

    int addNewGame(QString home, QString away, qint64 datetime, quint8 sIndex, QString score, quint8 comp);
//    int removeUser(const QString &name);
    int showAllGames();

    quint16 getNumberOfGames() { return this->m_lGamesPlay.size(); }

    GamesPlay *gameExists(quint8 sIndex, quint8 comp, qint64 datetime);
    bool gameExists(quint32 index);

    quint16 startRequestGetGamesPlay()
    {
        this->m_mGamesListMutex.lock();
        return this->getNumberOfGames();
    }

    GamesPlay *getRequestGamesPlay(int index)
    {
        if (index < this->m_lGamesPlay.size())
            return &this->m_lGamesPlay[index];
        return NULL;
    }

    void stopRequestGetGamesPlay()
    {
        this->m_mGamesListMutex.unlock();
    }

    void sortGamesListByTime();
//    bool userCheckPassword(QString name, QString passw);
//    bool userChangePassword(QString name, QString passw);
//    bool userChangeProperties(QString name, quint32 props);
//    quint32 getUserProperties(QString name);

private:
    QSettings           *m_pGamesSettings = NULL;
    QList<GamesPlay>    m_lGamesPlay;
    QMutex              m_mGamesIniMutex;
    QMutex              m_mGamesListMutex;

    QList<GamesPlay>    m_lAddGamesPlayProblems;

    void saveActualGamesList();

    bool addNewGamesPlay(QString home, QString away, qint64 datetime, quint8 sIndex, QString score, quint8 comp, quint32 index, bool checkGame = true);
    void addNewGamesPlay(QString home, QString away, qint64 datetime, quint8 sIndex, QString score, quint8 comp, quint32 index, QList<GamesPlay> *pList);
//    quint32 getUserLoginIndex(const QString &name);
    quint32 getNextGameIndex();

    bool updateGamesPlayValue(GamesPlay *pGame, QString key, QVariant value);

};

#endif // GAMES_H
