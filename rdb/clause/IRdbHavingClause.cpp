#include "IRdbHavingClause.h"

$PackageWebCoreBegin

IRdbHavingClause::IRdbHavingClause()
{
}

IRdbHavingClause::IRdbHavingClause(const QString &sql)
    : m_havingSql(sql)
{
}

IRdbHavingClause IRdb::having(const QString &sql, IRdb::Relation relation)
{
    IRdbHavingClause clause(sql);
    clause.m_isAnd = !(relation & 0x01);
    clause.m_isNot = !(relation & 0x10);
    return clause;
}

$PackageWebCoreEnd
