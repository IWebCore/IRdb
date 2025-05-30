#include "IRdbSqlServerDialect.h"
#include "rdb/IRdbAbort.h"
#include "rdb/ISqlQuery.h"
#include "rdb/clause/IRdbLimitClause.h"
#include "rdb/entity/IRdbTableInfo.h"

$PackageWebCoreBegin

QString IRdbSqlServerDialect::databaseType() const
{
    return "SqlServer";
}

QString IRdbSqlServerDialect::createTableSql(const IRdbTableInfo &info) const
{
    QStringList fields;
    for (auto index = 0; index < info.fields.length(); index++) {
        fields.append(createSqlCommonKeyClause(info, index));
    }

    return QString("CREATE TABLE ")
            .append(info.entityName)
            .append(" (").append(fields.join(", ") + ')');
}

QString IRdbSqlServerDialect::dropTableSql(const IRdbTableInfo &info) const
{
    return "DROP TABLE IF EXISTS " + quoteName(info.entityName);
}

QString IRdbSqlServerDialect::getSqlType(const IRdbTableInfo &info, int index) const
{
    static const QMap<int, QString> map = {
        {QMetaType::Char, "smallint"},
        {QMetaType::UChar, "tinyint"},
        {QMetaType::Short, "smallint"},
        {QMetaType::UShort, "int"},
        {QMetaType::Int, "int"},
        {QMetaType::UInt, "bigint"},
        {QMetaType::Long, "bigint"},
        {QMetaType::ULong, "bigint"},
        {QMetaType::LongLong, "bigint"},
        {QMetaType::ULongLong, "bigint"},
        {QMetaType::Double, "float"},
        {QMetaType::Float, "real"},
        {QVariant::Bool, "bit"},
        {QVariant::Date, "date"},
        {QVariant::Time, "time"},
        {QVariant::DateTime, "datetime"},
        {QVariant::String, "varchar(255)"},
        {qMetaTypeId<std::string>(), "varchar(255)"},
        {qMetaTypeId<IString>(), "varchar(255)"},
    };
    if(info.sqlType.contains(index)){
        return info.sqlType[index];
    }

    auto typeId = info.fields[index].typeId;
    if(!map.contains(typeId)){
        QString tip = QString("Type is not supported. Type:").append(QVariant::typeToName(typeId));
        IRdbAbort::abortDialectError(tip, $ISourceLocation);
    }
    return map[typeId];
}

QString IRdbSqlServerDialect::createSqlCommonKeyClause(const IRdbTableInfo &info, int index) const
{
    const auto& name = info.fieldNames[index];
    auto sqlType = getSqlType(info, index);

    QString piece = name + " " + sqlType;
    if(info.notNullKeys.contains(index)){
        piece.append(" NOT NULL");
    }
    if(info.uniqueKeys.contains(index)){
        piece.append(" UNIQUE");
    }
    if(info.primaryKey == index){
        piece.append(" PRIMARY KEY");
    }
    if(info.autoIncrement == index){
        piece.append(" IDENTITY(1,1)");
    }
    if(info.constraints.contains(index)){
        piece.append(" ").append(info.constraints[index]);
    }
    return piece;
}

QString IRdbSqlServerDialect::fromLimitClause(const IRdbLimitClause &limit) const
{
    if(limit.m_limit < 0){
        return {};
    }

    QString sql;
    if(limit.m_offset >0){
        sql += " OFFSET " + QString::number(limit.m_offset) + " ROWS";
    }

    sql += " FETCH NEXT " + QString::number(limit.m_limit) + " ROWS ONLY";
    return sql;
}

void IRdbSqlServerDialect::bindParameter(QSqlQuery &query, const QString &field, const QVariant &value) const
{
    auto type = value.type();
    if(static_cast<int>(type) >= QMetaType::User && (value.typeName() == std::string("std::string") || value.typeName() == std::string("IString"))){
        return query.bindValue(field, value.value<QString>());
    }

    switch(type){
    case QMetaType::QDate:
    case QMetaType::QTime:
    case QMetaType::QDateTime:
        query.bindValue(field, value.toString());
        break;
    default:
        query.bindValue(field, value);
    }
}

QString IRdbSqlServerDialect::quoteName(const QString &name) const
{
    return "[" + name + "]";
}

$PackageWebCoreEnd
