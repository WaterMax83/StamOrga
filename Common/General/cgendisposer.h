#ifndef CCGHDISPOSER_H
#define CCGHDISPOSER_H

#include <QObject>

class cGenDisposer : public QObject
{
    Q_OBJECT
public:
    explicit cGenDisposer(QObject* parent = nullptr);

    virtual qint32 initialize() { return 1; }

signals:

public slots:

protected:
    bool m_initialized;
};

#endif // CCGHDISPOSER_H
