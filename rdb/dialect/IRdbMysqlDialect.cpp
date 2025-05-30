#include "IRdbMysqlDialect.h"
#include "rdb/IRdbAbort.h"
#include "rdb/ISqlQuery.h"
#include "rdb/entity/IRdbTableInfo.h"

$PackageWebCoreBegin

QString IRdbMysqlDialect::databaseType() const
{
    return "MySql";
}

QString IRdbMysqlDialect::getSqlType(const IRdbTableInfo &info, int index) const
{
    static const QMap<int, QString> map = {
        {QMetaType::Char, "TINYINT"},
        {QMetaType::UChar, "TINYINT UNSIGNED "},
        {QMetaType::Short, "SMALLINT"},
        {QMetaType::UShort, "SMALLINT UNSIGNED"},
        {QMetaType::Int, "INT"},
        {QMetaType::UInt, "INT UNSIGNED"},
        {QMetaType::Long, "BIGINT"},
        {QMetaType::ULong, "BIGINT UNSIGNED"},
        {QMetaType::LongLong, "BIGINT"},
        {QMetaType::ULongLong, "BIGINT UNSIGNED"},
        {QMetaType::Double, "DOUBLE"},
        {QMetaType::Float, "FLOAT"},
        {QVariant::Bool, "TINYINT(1)"},
        {QVariant::Date, "DATE"},
        {QVariant::Time, "TIME"},
        {QVariant::DateTime, "DATETIME"},
        {QVariant::String, "VARCHAR(255)"},
        {qMetaTypeId<std::string>(), "VARCHAR(255)"},
        {qMetaTypeId<IString>(), "VARCHAR(255)"},
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

QString IRdbMysqlDialect::createSqlCommonKeyClause(const IRdbTableInfo &info, int index) const
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
        piece.append(" AUTO_INCREMENT");
    }
    if(info.constraints.contains(index)){
        piece.append(" ").append(info.constraints[index]);
    }
    return piece;
}

QString IRdbMysqlDialect::quoteName(const QString &name) const
{
    return "`" + name + "`";
}

$PackageWebCoreEnd
