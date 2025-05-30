#include "IRdbTableInfo.h"

#include "rdb/IRdbAbort.h"
#include "rdb/IRdbManage.h"
#include "rdb/IRdbUtil.h"

$PackageWebCoreBegin

namespace detail
{
    void processNotations(const QMap<QString, QString> clsInfo, IRdbTableInfo& tableInfo);
    bool setPrimaryKey(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setUniqueKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setNotNullKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setAutoIncrementKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setSqlTypeKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setConstraintKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setInsertValueKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);
    bool setGeneratValueKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo);

    void processChecks(const QMap<QString, QString>& clsInfo, const IRdbTableInfo& tableInfo);
    void checkFieldTypes(const QMap<QString, QString>& clsInfo, const IRdbTableInfo& tableInfo);
    void checkDuplicatedPrimaryKey(const QMap<QString, QString>& clsInfo, const IRdbTableInfo& tableInfo);
    void checkAutoGenerateInfo(const QMap<QString, QString>& clsInfo, const IRdbTableInfo& tableInfo);
}

IRdbTableInfo::IRdbTableInfo(const QMetaObject &meta) : IRdbEntityInfo(meta)
{
    auto clsInfo = IMetaUtil::getMetaClassInfoMap(meta);
    detail::processNotations(clsInfo, *this);
    detail::processChecks(clsInfo, *this);

    for(int i=0; i<fields.size(); i++){
        valueMakers.append(ValueMaker{});
    }
}

void detail::processNotations(const QMap<QString, QString> clsInfo, IRdbTableInfo& tableInfo)
{
    using NotationFun = std::function<bool(const QString&, const QString&, IRdbTableInfo&)>;
    static QList<NotationFun> notationFun = {
        detail::setPrimaryKey,
        detail::setUniqueKeys,
        detail::setNotNullKeys,
        detail::setAutoIncrementKeys,
        detail::setSqlTypeKeys,
        detail::setConstraintKeys,
        detail::setInsertValueKeys,
        detail::setGeneratValueKeys
    };

    for(const auto& name : clsInfo.keys()){
        for(auto fun : notationFun){
            if(fun(name, clsInfo[name], tableInfo)){
                break;
            }
        }
    }
}

bool detail::setPrimaryKey(const QString& key, const QString& value, IRdbTableInfo& tableInfo)
{
    static const char* const PREFIX = "sql_primaryKey__";
    if(key.startsWith(PREFIX)){
        auto index = tableInfo.fieldNames.indexOf(value);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("primary key incorrect");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        if(tableInfo.primaryKey != -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("primary key already exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.primaryKey = index;
        return true;
    }
    return false;
}

bool detail::setUniqueKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo)
{
    static const char* const PREFIX = "sql_unique_key__";
    if(key.startsWith(PREFIX)){
        auto index = tableInfo.fieldNames.indexOf(value);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("error unique field names");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.uniqueKeys.append(index);
        return true;
    }
    return false;
}

bool detail::setNotNullKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo)
{
    static const char* const PREFIX = "sql_not_null_key__";
    if(key.startsWith(PREFIX)){
        auto index = tableInfo.fieldNames.indexOf(value);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("not null field not exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.notNullKeys.append(index);
        return true;
    }
    return false;
}

bool detail::setAutoIncrementKeys(const QString &key, const QString &value, IRdbTableInfo &tableInfo)
{
    static const char* const PREFIX = "sql_auto_increment__";
    if(key.startsWith(PREFIX)){
        auto index = tableInfo.fieldNames.indexOf(value);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className).append(" Why:").append("not exist auto_increment field");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.autoIncrement = index;
        return true;
    }
    return false;
}

bool detail::setSqlTypeKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo)
{
    static const char* const PREFIX = "sql_field_type__";
    if(key.startsWith(PREFIX)){
        auto fieldName = key.mid(QString(PREFIX).length());
        auto index = tableInfo.fieldNames.indexOf(fieldName);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("sql type not exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        if(tableInfo.sqlType.contains(index)){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("sql type already exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.sqlType[index] = value;
        return true;
    }
    return false;
}

bool detail::setConstraintKeys(const QString &key, const QString &value, IRdbTableInfo &tableInfo)
{
    static const char* const PREFIX = "sql_column_constraint__";
    if(key.startsWith(PREFIX)){
        auto fieldName = key.mid(QString(PREFIX).length());
        auto index = tableInfo.fieldNames.indexOf(fieldName);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("sql constrait field not exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        if(tableInfo.constraints.contains(index)){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("sql constrait in field already exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.constraints[index] = QString(value).replace("\"", "");
        return true;
    }
    return false;
}

bool detail::setInsertValueKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo)
{
    static const char* const PREFIX = "sql_insert_value__";
    if(key.startsWith(PREFIX)){
        auto fieldName = key.mid(QString(PREFIX).length());
        auto index = tableInfo.fieldNames.indexOf(fieldName);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("sql constrait field not exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.valueMakers[index].type = IRdbTableInfo::ValueMakerType::InsertValue;
        tableInfo.valueMakers[index].insertValue = value;
        return true;
    }
    return false;
}

bool detail::setGeneratValueKeys(const QString& key, const QString& value, IRdbTableInfo& tableInfo)
{
    static const char* const PREFIX = "sql_generate_value__";
    if(key.startsWith(PREFIX)){
        auto fieldName = key.mid(QString(PREFIX).length());
        auto index = tableInfo.fieldNames.indexOf(fieldName);
        if(index == -1){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("sql constrait field not exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        tableInfo.valueMakers[index].type = IRdbTableInfo::ValueMakerType::GenerateValue;
        tableInfo.valueMakers[index].generator = IRdbManage::instance().getAutoGenerateKeyFunction(value);
        if(tableInfo.valueMakers[index].generator == nullptr){
            QString tip = QString("Table: ").append(tableInfo.className)
                    .append(" Why:").append("no field generator exist");
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
        return true;
    }
    return false;
}

void detail::processChecks(const QMap<QString, QString> &clsInfo, const IRdbTableInfo &tableInfo)
{
    detail::checkFieldTypes(clsInfo, tableInfo);
    detail::checkDuplicatedPrimaryKey(clsInfo, tableInfo);
    detail::checkAutoGenerateInfo(clsInfo, tableInfo);
}

void detail::checkFieldTypes(const QMap<QString, QString> &clsInfo, const IRdbTableInfo& tableInfo)
{
    Q_UNUSED(clsInfo)
    static const QList<QMetaType::Type> allowTypes ={
        QMetaType::Bool,
        QMetaType::QString,
        QMetaType::Char, QMetaType::UChar,
        QMetaType::Short, QMetaType::UShort,
        QMetaType::Int, QMetaType::UInt,
        QMetaType::Long, QMetaType::ULong,
        QMetaType::LongLong, QMetaType::ULongLong,
        QMetaType::Double, QMetaType::Float,
        QMetaType::QDate, QMetaType::QTime, QMetaType::QDateTime,
        QMetaType::Type(qRegisterMetaType<IString>()),
        QMetaType::Type(qRegisterMetaType<std::string>())
    };

    for(const auto& field : tableInfo.fields){
        if(!allowTypes.contains(field.typeId)){
            QString info = QString("this kind of type is not supported in table, please change to the supported type. \n"
                                   "Type : ").append(QMetaType::typeName(field.typeId));
            QString tip = QString("Table: ").append(tableInfo.className).append(" Why:").append(info);
            IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
        }
    }
}

void detail::checkDuplicatedPrimaryKey(const QMap<QString, QString> &clsInfo, const IRdbTableInfo& tableInfo)
{
    Q_UNUSED(clsInfo)
    if(tableInfo.primaryKey == -1){
        QString tip = QString("Table: ").append(tableInfo.className).append(" Why:").append("table has no priamary key, please mark one");
        IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
    }

    auto typeId = tableInfo.fields[tableInfo.primaryKey].typeId;
    if(!IRdbUtil::isPrimaryKeyType(typeId)){
        QString info = tableInfo.className + " table has incorrect primary key annotated type. \n"
                                   "the only allowed type is int, long, longlong/int64, QString\n"
                                   "field:" + tableInfo.fields[tableInfo.primaryKey].name + " type:" + tableInfo.fields[tableInfo.primaryKey].typeName;
        QString tip = QString("Table: ").append(tableInfo.className).append(" Why:").append(info);
        IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
    }

    if(typeId != QMetaType::QString && (typeId != QMetaType::LongLong) ){
        QString info = QString("primary key that is number type recommended is qlonglong/qint64 type.\n"
                               "current is ").append(QMetaType::typeName(typeId)).append(" in class ").append(tableInfo.className);
        qWarning() << info;
    }
}

void detail::checkAutoGenerateInfo(const QMap<QString, QString> &clsInfo, const IRdbTableInfo& tableInfo)
{
    Q_UNUSED(clsInfo)
    static const char* const Sql_AutoGenerateKeyClause = "autoIncrementKey__";
    static QList<QMetaType::Type> allowTypes = {
        QMetaType::Int, QMetaType::LongLong, QMetaType::QString
    };

    int index = 0;
    for(auto key : clsInfo.keys()){
        if(key.startsWith(Sql_AutoGenerateKeyClause)){
            index ++;
        }
    }
    if(index >1){
        QString info = tableInfo.className + " table: you can not have more than one increment note, please check";
        QString tip = QString("Table: ").append(tableInfo.className).append(" Why:").append(info);
        IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
    }
}

$PackageWebCoreEnd
