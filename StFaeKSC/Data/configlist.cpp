#include "configlist.h"

ConfigList::ConfigList()
{

}


quint32 ConfigList::getNextInternalIndex()
{
    quint32 savedIndex;

    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(ITEM_INDEX_GROUP);
    savedIndex = this->m_pConfigSettings->value(ITEM_MAX_INDEX, 0).toUInt();

    savedIndex++;

    this->m_pConfigSettings->setValue(ITEM_MAX_INDEX, savedIndex);
    this->m_pConfigSettings->endGroup();

    return savedIndex;
}
