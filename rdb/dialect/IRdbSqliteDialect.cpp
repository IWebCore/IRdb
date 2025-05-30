#include "IRdbSqliteDialect.h"
#include "rdb/IRdbAbort.h"
#include "rdb/entity/IRdbTableInfo.h"

$PackageWebCoreBegin

QString IRdbSqliteDialect::databaseType() const
{
    return "Sqlite";
}

QString IRdbSqliteDialect::dropTableSql(const IRdbTableInfo &info) const
{
    return "DROP TABLE IF EXISTS " + quoteName(info.entityName);
}

QString IRdbSqliteDialect::getSqlType(const IRdbTableInfo& info, int index) const
{
    static const QMap<int, QString> map = {
        {QMetaType::Char, "INTEGER"},
        {QMetaType::UChar, "INTEGER"},
        {QMetaType::Short, "INTEGER"},
        {QMetaType::UShort, "INTEGER"},
        {QMetaType::Int, "INTEGER"},
        {QMetaType::UInt, "INTEGER"},
        {QMetaType::Long, "INTEGER"},
        {QMetaType::ULong, "INTEGER"},
        {QMetaType::LongLong, "INTEGER"},
        {QMetaType::ULongLong, "INTEGER"},
        {QMetaType::Double, "REAL"},
        {QMetaType::Float, "REAL"},
        {QVariant::Bool, "BOOLEAN"},
        {QVariant::Date, "DATE"},
        {QVariant::Time, "TIME"},
        {QVariant::DateTime, "DATETIME"},
        {QVariant::String, "VARCHAR"},
        {qMetaTypeId<std::string>(), "VARCHAR"},
        {qMetaTypeId<IString>(), "VARCHAR"},
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

$PackageWebCoreEnd
