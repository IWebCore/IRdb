#include "IRdbPostgresDialect.h"
#include "rdb/IRdbAbort.h"
#include "rdb/entity/IRdbTableInfo.h"

QString IRdbPostgresDialect::databaseType() const
{
    return "Postgres";
}

QString IRdbPostgresDialect::getSqlType(const IRdbTableInfo &info, int index) const
{
    static const QMap<int, QString> map = {
        {QMetaType::Char, "smallint"},
        {QMetaType::UChar, "smallint"},
        {QMetaType::Short, "smallint"},
        {QMetaType::UShort, "integer"},
        {QMetaType::Int, "integer"},
        {QMetaType::UInt, "integer"},
        {QMetaType::Long, "bigint"},
        {QMetaType::ULong, "bigint"},
        {QMetaType::LongLong, "bigint"},
        {QMetaType::ULongLong, "bigint"},
        {QMetaType::Double, "double precision"},
        {QMetaType::Float, "real"},
        {QVariant::Bool, "boolean"},
        {QVariant::Date, "date"},
        {QVariant::Time, "time"},
        {QVariant::DateTime, "timestamp"},
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

QString IRdbPostgresDialect::createSqlCommonKeyClause(const IRdbTableInfo &info, int index) const
{
    const auto& name = info.fieldNames[index];

    QString piece = name;
    if(info.autoIncrement == index){
        QList<QMetaType::Type> types = {
            QMetaType::Long, QMetaType::LongLong, QMetaType::ULong, QMetaType::ULongLong
        };
        if(types.contains(info.fields[index].typeId)){
            piece = piece + " BIGSERIAL";
        }else{
            piece = piece + " SERIAL";
        }
    }else{
        auto sqlType = getSqlType(info, index);
        piece = piece + " " + sqlType;
    }

    if(info.notNullKeys.contains(index)){
        piece.append(" NOT NULL");
    }
    if(info.uniqueKeys.contains(index)){
        piece.append(" UNIQUE");
    }
    if(info.primaryKey == index){
        piece.append(" PRIMARY KEY");
    }
    if(info.constraints.contains(index)){
        piece.append(" ").append(info.constraints[index]);
    }
    return piece;
}
