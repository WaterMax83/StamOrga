#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>

#define DEFAULT_PADDING_SIZE    4

inline uint CalculatePaddingSize(uint uLength)
{
    int nRem = uLength % DEFAULT_PADDING_SIZE;
    if (nRem > 0)
        return DEFAULT_PADDING_SIZE - nRem;
    else
        return 0;
}

#endif // CONFIG_H
