#include "IRdbConnection.h"
#include "rdb/database/IRdbDatabaseWare.h"
#include "core/application/IApplicationManage.h"

$PackageWebCoreBegin

IRdbConnection::IRdbConnection(IRdbDatabaseWare &ware, QSqlDatabase db)
    :m_databaseWare(ware), m_database(db)
{
    m_createTime = IApplicationManage::instance().time();
    m_lastUseTime = IApplicationManage::instance().time();
}

IRdbConnection::~IRdbConnection()
{
    m_database.close();
}

void IRdbConnection::releaseConnection()
{
    m_lastUseTime = IApplicationManage::instance().time();
    m_useCount ++;
    m_databaseWare.unlockConnection(this);
}

bool IRdbConnection::isExpired(int second) const
{
    auto time = IApplicationManage::instance().time();
    return (time - m_lastUseTime) > (int64_t)second * 1000000000;
}

$PackageWebCoreEnd
