#pragma once

#include "core/util/IHeaderUtil.h"

$PackageWebCoreBegin

class IRdbConnection;
class IRdbDialectWare;
class ISqlQuery : public QSqlQuery
{
public:
    explicit ISqlQuery(IRdbConnection *connection, const IRdbDialectWare& dialect);
    ~ISqlQuery();
    ISqlQuery(ISqlQuery&& query);
    ISqlQuery(const ISqlQuery& query);
    ISqlQuery& operator=(ISqlQuery&& query) = delete;
    ISqlQuery& operator=(const ISqlQuery& other) = delete; // forbid any type of copy

public:
    void bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType type = QSql::In) = delete;
    void bindValue(int pos, const QVariant& val, QSql::ParamType type = QSql::In) = delete;
    void addBindValue(const QVariant& val, QSql::ParamType type = QSql::In) = delete;

public:
    bool exec(const QString& sql);
    bool exec();

    void bindParameter(const QString& key, const QVariant& value);
    void bindParameters(const QVariantMap& map);

private:
    void bindExecParameters();

private:
    const IRdbDialectWare& m_dialect;
    mutable IRdbConnection* m_connection{nullptr};
    mutable QMap<QString, QVariant> m_parameters;
};

$PackageWebCoreEnd
