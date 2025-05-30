#include "IRdbConnection.h"
#include "rdb/database/IRdbDatabaseWare.h"
#include "core/application/IApplication.h"

$PackageWebCoreBegin

IRdbConnection::IRdbConnection(IRdbDatabaseWare &ware, QSqlDatabase db)
    :m_databaseWare(ware), m_database(db)
{
    m_createTime = IApplication::time();
    m_lastUseTime = IApplication::time();
}

IRdbConnection::~IRdbConnection()
{
    m_database.close();
}

void IRdbConnection::releaseConnection()
{
    m_lastUseTime = IApplication::time();
    m_useCount ++;
    m_databaseWare.unlockConnection(this);
}

bool IRdbConnection::isExpired(int second) const
{
    auto time = IApplication::time();
    return (time - m_lastUseTime) > (int64_t)second * 1000000000;
}

$PackageWebCoreEnd
