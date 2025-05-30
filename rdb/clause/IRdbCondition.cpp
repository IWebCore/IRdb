#include "IRdbCondition.h"
#include "rdb/clause/impl/IRdbConditionImpl.h"
#include "rdb/ISqlQuery.h"

$PackageWebCoreBegin

IRdbCondition::IRdbCondition() : m_impl(new IRdbConditionImpl)
{
}

IRdbCondition::IRdbCondition(IRdbCondition && condition)
    : m_impl(condition.m_impl), m_whereIndex(condition.m_whereIndex.load())
{
    condition.m_impl = nullptr;
}

IRdbCondition::IRdbCondition(const IRdbCondition &condition)
    : m_impl(new IRdbConditionImpl(*condition.m_impl)), m_whereIndex(condition.m_whereIndex.load())
{
}

IRdbCondition &IRdbCondition::operator=(IRdbCondition && condition)
{
    std::swap(this->m_impl, condition.m_impl);
    this->m_whereIndex = condition.m_whereIndex.load();
    return *this;
}

IRdbCondition &IRdbCondition::operator=(const IRdbCondition &)
{

    return *this;
}

IRdbCondition::~IRdbCondition()
{
    delete m_impl;
}

IRdbCondition::IRdbCondition(IRdbWhereClause &&where) : m_impl(new IRdbConditionImpl)
{
    append(std::move(where));
}

IRdbCondition::IRdbCondition(IRdbOrderByClause&& orderBy) : m_impl(new IRdbConditionImpl)
{
    m_impl->m_orderBys.push_back(std::move(orderBy));
}

IRdbCondition::IRdbCondition(IRdbGroupByClause &&groupBy)
{
    m_impl->m_groupBys.push_back(std::move(groupBy));
}

IRdbCondition::IRdbCondition(IRdbLimitClause &&limit)
{
    m_impl->m_limit = std::move(limit);
}

IRdbCondition &IRdbCondition::where(const QString &sql, IRdb::Relation relation)
{
    return append(IRdb::where(sql, relation));
}

IRdbCondition &IRdbCondition::whereLike(const QString &field, const QString &argument, IRdb::Relation relation)
{
    return append(IRdb::whereLike(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereNotLike(const QString &field, const QString &argument, IRdb::Relation relation)
{
    return append(IRdb::whereNotLike(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereEndWith(const QString &field, const QString &argument, IRdb::Relation relation)
{
    return append(IRdb::whereEndWith(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereStartWith(const QString &field, const QString &argument, IRdb::Relation relation)
{
    return append(IRdb::whereStartWith(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereBetween(const QString &field, const QVariant &lower, const QVariant &upper, IRdb::Relation relation)
{
    return append(IRdb::whereBetween(field, lower, upper, relation));
}

IRdbCondition &IRdbCondition::whereIn(const QString &field, const QVariantList &variants, IRdb::Relation relation)
{
    return append(IRdb::whereIn(field, variants, relation));
}

IRdbCondition &IRdbCondition::whereEqual(const QString &field, const QVariant &argument, IRdb::Relation relation)
{
    return append(IRdb::whereEqual(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereNotEqual(const QString &field, const QVariant &argument, IRdb::Relation relation)
{
    return append(IRdb::whereNotEqual(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereGreaterThan(const QString &field, const QVariant &argument, IRdb::Relation relation)
{
    return append(IRdb::whereGreaterThan(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereGreaterEqual(const QString &field, const QVariant &argument, IRdb::Relation relation)
{
    return append(IRdb::whereGreaterEqual(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereLessThan(const QString &field, const QVariant &argument, IRdb::Relation relation)
{
    return append(IRdb::whereLessThan(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereLessEqual(const QString &field, const QVariant &argument, IRdb::Relation relation)
{
    return append(IRdb::whereLessEqual(field, argument, relation));
}

IRdbCondition &IRdbCondition::whereIsNull(const QString &field, IRdb::Relation relation)
{
    return append(IRdb::whereIsNull(field, relation));
}

IRdbCondition &IRdbCondition::whereIsNotNull(const QString &field, IRdb::Relation relation)
{
    return append(IRdb::whereIsNotNull(field, relation));
}

IRdbCondition &IRdbCondition::whereTrue(IRdb::Relation relation)
{
    return append(IRdb::whereTrue(relation));
}

IRdbCondition &IRdbCondition::whereFalse(IRdb::Relation relation)
{
    return append(IRdb::whereFalse(relation));
}

IRdbCondition &IRdbCondition::whereAnd(IRdbCondition condition)
{
    condition.m_impl->m_isAnd = true;
    condition.m_impl->m_isNot = false;
    append(std::move(condition));
    return *this;
}

IRdbCondition &IRdbCondition::whereAndNot(IRdbCondition condition)
{
    condition.m_impl->m_isAnd = true;
    condition.m_impl->m_isNot = true;
    append(std::move(condition));
    return *this;
}

IRdbCondition &IRdbCondition::whereOr(IRdbCondition condition)
{
    condition.m_impl->m_isAnd = false;
    condition.m_impl->m_isNot = false;
    append(std::move(condition));
    return *this;
}

IRdbCondition &IRdbCondition::whereOrNot(IRdbCondition condition)
{
    condition.m_impl->m_isAnd = false;
    condition.m_impl->m_isNot = true;
    append(std::move(condition));
    return *this;
}

IRdbCondition &IRdbCondition::groupBy(const QString &field)
{
    return append(IRdb::groupBy(field));
}

IRdbCondition &IRdbCondition::orderBy(const QString &field, bool m_isAscending)
{
   return append(IRdbOrderByClause(field, m_isAscending));
}

IRdbCondition &IRdbCondition::orderByAsc(const QString &field)
{
    return append(IRdbOrderByClause(field, true));
}

IRdbCondition &IRdbCondition::orderByDesc(const QString &field)
{
    return append(IRdbOrderByClause(field, false));
}

IRdbCondition &IRdbCondition::having(const QString &sql, IRdb::Relation relation)
{
    return append(IRdb::having(sql, relation));
}

IRdbCondition &IRdbCondition::limit(unsigned long long limit, unsigned long long offset)
{
    return append(IRdb::limit(limit, offset));
}

IRdbCondition &IRdbCondition::append(IRdbCondition &&condition)
{
    condition.m_impl->m_isChild = true;
    m_impl->m_conditions[m_whereIndex++] = std::move(condition);
    return *this;
}

IRdbCondition &IRdbCondition::append(IRdbWhereClause &&where)
{
    m_impl->m_wheres[m_whereIndex++] = std::move(where);
    return *this;
}

IRdbCondition &IRdbCondition::append(IRdbOrderByClause &&orderBy)
{
    m_impl->m_orderBys.push_back(std::move(orderBy));
    return *this;
}

IRdbCondition &IRdbCondition::append(IRdbGroupByClause &&group)
{
    m_impl->m_groupBys.push_back(std::move(group));
    return *this;
}

IRdbCondition &IRdbCondition::append(IRdbHavingClause &&having)
{
    m_impl->m_havings.push_back(std::move(having));
    return *this;
}

IRdbCondition &IRdbCondition::append(IRdbLimitClause &&limit)
{
    m_impl->m_limit = std::move(limit);
    return *this;
}

void IRdbCondition::bindParameters(ISqlQuery &query) const
{
    for(const auto& [index, where] : m_impl->m_wheres){
        query.bindParameters(where.m_boundValue);
    }
    for(const auto& orderBy : m_impl->m_orderBys){
        query.bindParameters(orderBy.m_boundValue);
    }

    // and so on;
}

$PackageWebCoreEnd
