#include "ISqlQuery.h"
#include "core/config/IProfileImport.h"
#include "rdb/dialect/IRdbDialectWare.h"
#include "rdb/exception/IRdbException.h"
#include "rdb/database/IRdbConnection.h"
#include "rdb/database/IRdbDatabaseWare.h"

$PackageWebCoreBegin

ISqlQuery::ISqlQuery(IRdbConnection* connection, const IRdbDialectWare & dialect)
    :QSqlQuery(connection->m_database),  m_dialect(dialect), m_connection(connection)
{
}

ISqlQuery::~ISqlQuery()
{
    if(m_connection){
        m_connection->releaseConnection();
        m_connection = nullptr;
    }
}

ISqlQuery::ISqlQuery(ISqlQuery &&query)
    : QSqlQuery(static_cast<QSqlQuery&&>(query)), m_dialect(query.m_dialect), m_parameters(std::move(query.m_parameters))
{
    m_connection = query.m_connection;
    query.m_connection = nullptr;
}

ISqlQuery::ISqlQuery(const ISqlQuery &query)
    : QSqlQuery(static_cast<const QSqlQuery& >(query)), m_dialect(query.m_dialect), m_parameters(std::move(query.m_parameters))
{
    m_connection = query.m_connection;
    query.m_connection = nullptr;
}

bool ISqlQuery::exec(const QString &sql)
{
    static $Bool showSql{"/rdb/showSql", true};

    bindExecParameters();
    auto ret = QSqlQuery::exec(sql);
    if(!ret && (lastError().type() != QSqlError::NoError)){
        qDebug().noquote() << m_dialect.databaseType() << lastQuery();
        qDebug().noquote() << lastError().text();
        throw IRdbException(lastError());
    }
    if(*showSql){
        qDebug().noquote() << m_dialect.databaseType() << lastQuery();
    }
    return ret;
}

bool ISqlQuery::exec()
{
    bindExecParameters();
    auto ret = QSqlQuery::exec();
    if(!ret && (lastError().type() != QSqlError::NoError)){
        qDebug().noquote() << lastQuery();
        qDebug().noquote() << lastError().text();
        throw IRdbException(lastError());
    }
    qDebug().noquote() << m_dialect.databaseType() << lastQuery();
    return ret;
}

void ISqlQuery::bindExecParameters()
{
    if(m_parameters.isEmpty()){
        return;
    }

    auto keys = m_parameters.keys();
    for(const auto& key : keys){
        m_dialect.bindParameter(*this, key, m_parameters[key]);
    }
}

void ISqlQuery::bindParameter(const QString &key, const QVariant &value)
{
    m_parameters[key] = value;
}

void ISqlQuery::bindParameters(const QVariantMap &map)
{
    for(auto it = map.begin(); it != map.end(); it++){
        m_parameters.insert(it.key(), it.value());
    }
}

$PackageWebCoreEnd
