#pragma once

#ifdef IWEBCORE_FLATTEN_CRTP

#include "core/util/IHeaderUtil.h"
#include "core/task/unit/ITaskWareUnit.h"
#include "rdb/IRdbAbort.h"
#include "rdb/IRdbCatagory.h"
#include "rdb/model/IRdbEntityModelWare.h"
#include "rdb/entity/IRdbViewInfo.h"

$PackageWebCoreBegin

template<typename T, typename View, typename Db, bool enabled = true>
class IRdbViewModelInterface : public ITaskWare
    // : public IRdbEntityModelWare<View, Db>, public ITaskWareUnit<T, IRdbCatagory, enabled>
{
    $AsTaskUnit(IRdbViewModelInterface)
    Q_DISABLE_COPY_MOVE(IRdbViewModelInterface)
public:
    IRdbViewModelInterface();

    // taskware
protected:
    virtual const std::string& $name() const final;
    virtual const std::string& $catagory() const final;

    // Entity model ware
public:
    std::size_t count();
    std::size_t count(const IRdbCondition&);

public:
    IResult<Table> findOne(const IRdbCondition&);
    Table findOneRaw(const IRdbCondition&);
    QList<Table> findAll();
    QList<Table> findAll(const IRdbCondition&);
    QVariantList findColumn(const QString& column);
    QVariantList findColumn(const QString& column, const IRdbCondition& condition);
    QList<QVariantMap> findColumns(const QStringList& columns);
    QList<QVariantMap> findColumns(const QStringList& columns, const IRdbCondition& condition);

public:
    bool exist(const IRdbCondition& condition);

public:
    ISqlQuery createQuery();
    ISqlQuery createQuery(const QString& sql);
    ISqlQuery createQuery(const QString& sql, const QVariantMap& values);

protected:
    const IRdbEntityInfo& entityInfo() const;

protected:
    IRdbDatabaseWare& m_database;
    const IRdbDialectWare& m_dialect;

    // view model interface
public:
    virtual QString createViewSql() const;

protected:
    virtual double $order() const override;
    virtual void $task() final;

protected:
    const IRdbViewInfo& viewInfo();
};

template<typename T, typename Table, typename Db, bool enabled>
const std::string& IRdbViewModelInterface<T, Table, Db, enabled>::$name() const
{
    static const std::string name = IMetaUtil::getBareTypeName<T>();
    return name;
}

template<typename T, typename Table, typename Db, bool enabled>
const std::string& IRdbViewModelInterface<T, Table, Db, enabled>::$catagory() const
{
    static const std::string name = IMetaUtil::getBareTypeName<IRdbCatagory>();
    return name;
}


template<typename T, typename View, typename Db, bool enabled>
IRdbViewModelInterface<T, View, Db, enabled>::IRdbViewModelInterface()
    : m_database(Db::instance()), m_dialect(Db::DialectType::instance())
{
}


template<typename T, typename Table, typename Db, bool enabled>
std::size_t IRdbViewModelInterface<T, Table, Db, enabled>::count()
{
    auto query = createQuery(m_dialect.countSql(entityInfo()));
    query.exec();
    bool ok = true;
    auto value = IRdbUtil::getLongLong(query, ok);
    return ok ? value : 0;
}

template<typename T, typename Table, typename Db, bool enabled>
std::size_t IRdbViewModelInterface<T, Table, Db, enabled>::count(const IRdbCondition & condition)
{
    auto query = createQuery(m_dialect.countSql(entityInfo(), condition));
    condition.bindParameters(query);
    query.exec();
    bool ok = true;
    auto value = IRdbUtil::getLongLong(query, ok);
    return ok ? value : 0;
}

template<typename T, typename Table, typename Db, bool enabled>
IResult<Table> IRdbViewModelInterface<T, Table, Db, enabled>::findOne(const IRdbCondition& condition)
{
    auto sql =m_dialect.findOneSql(entityInfo(), condition);
    auto query = createQuery(sql);
    condition.bindParameters(query);
    if(!query.exec()){
        if(query.lastError().type() != QSqlError::NoError){
            throw IRdbException(query.lastError());
        }
        return std::nullopt;
    }
    return IRdbUtil::getEntity<Table>(query);
}

template<typename T, typename Table, typename Db, bool enabled>
Table IRdbViewModelInterface<T, Table, Db, enabled>::findOneRaw(const IRdbCondition & condition)
{
    auto val = findOne(condition);
    if(val){
        return *val;
    }
    return {};
}

template<typename T, typename Table, typename Db, bool enabled>
QList<Table> IRdbViewModelInterface<T, Table, Db, enabled>::findAll()
{
    auto query = createQuery(m_dialect.findAllSql(entityInfo()));
    query.exec();
    return IRdbUtil::getEntities<Table>(query);
}

template<typename T, typename Table, typename Db, bool enabled>
QList<Table> IRdbViewModelInterface<T, Table, Db, enabled>::findAll(const IRdbCondition& condition)
{
    auto query = createQuery(m_dialect.findAllSql(entityInfo(), condition));
    condition.bindParameters(query);
    query.exec();
    return IRdbUtil::getEntities<Table>(query);
}

template<typename T, typename Table, typename Db, bool enabled>
QVariantList IRdbViewModelInterface<T, Table, Db, enabled>::findColumn(const QString& column)
{
    auto query = createQuery(m_dialect.findColumnSql(entityInfo(), {column}));
    if(!query.exec()){
        return {};
    }
    return IRdbUtil::getVariantList(query);
}

template<typename T, typename Table, typename Db, bool enabled>
QVariantList IRdbViewModelInterface<T, Table, Db, enabled>::findColumn(const QString &column, const IRdbCondition &condition)
{
    auto query = createQuery(m_dialect.findColumnSql(entityInfo(), {column}, condition));
    condition.bindParameters(query);
    if(!query.exec()){
        return {};
    }
    return IRdbUtil::getVariantList(query);
}

template<typename T, typename Table, typename Db, bool enabled>
QList<QVariantMap> IRdbViewModelInterface<T, Table, Db, enabled>::findColumns(const QStringList& columns)
{
    auto query = createQuery(m_dialect.findColumnSql(entityInfo(), columns));
    if(!query.exec()){
        return {};
    }
    return IRdbUtil::getVariantMapList(query);
}

template<typename T, typename Table, typename Db, bool enabled>
QList<QVariantMap> IRdbViewModelInterface<T, Table, Db, enabled>::findColumns(const QStringList& columns, const IRdbCondition& condition)
{
    auto query = createQuery(m_dialect.findColumnSql(entityInfo(), columns, condition));
    condition.bindParameters(query);
    if(!query.exec()){
        return {};
    }
    return IRdbUtil::getVariantMapList(query);
}

template<typename T, typename Table, typename Db, bool enabled>
bool IRdbViewModelInterface<T, Table, Db, enabled>::exist(const IRdbCondition &condition)
{
    auto query = createQuery(m_dialect.existSql(entityInfo(), condition));
    condition.bindParameters(query);
    if(!query.exec()){
        return false;
    }
    auto value = IRdbUtil::getBool(query);
    if(value){
        return *value;
    }
    return false;
}

template<typename T, typename Table, typename Db, bool enabled>
ISqlQuery IRdbViewModelInterface<T, Table, Db, enabled>::createQuery()
{
    return m_database.createQuery();
}

template<typename T, typename Table, typename Db, bool enabled>
ISqlQuery IRdbViewModelInterface<T, Table, Db, enabled>::createQuery(const QString &sql)
{
    auto query = m_database.createQuery();
    query.prepare(sql);
    return query;
}

template<typename T, typename Table, typename Db, bool enabled>
ISqlQuery IRdbViewModelInterface<T, Table, Db, enabled>::createQuery(const QString& sql, const QVariantMap& values)
{
    auto query = m_database.createQuery();
    query.prepare(sql);
    query.bindParameters(values);
    return query;
}

template<typename T, typename Table, typename Db, bool enabled>
const IRdbEntityInfo &IRdbViewModelInterface<T, Table, Db, enabled>::entityInfo() const
{
    return Table::staticEntityInfo();
}


template<typename T, typename View, typename Db, bool enabled>
QString IRdbViewModelInterface<T, View, Db, enabled>::createViewSql() const
{
    return {};
}

template<typename T, typename View, typename Db, bool enabled>
double IRdbViewModelInterface<T, View, Db, enabled>::$order() const
{
    return 51;
}

template<typename T, typename View, typename Db, bool enabled>
void  IRdbViewModelInterface<T, View, Db, enabled>::$task()
{
    if constexpr (enabled){
        const auto& name = viewInfo().m_entityName;
        if(IRdbEntityModelWare<View, Db>::m_database.getRdbViews().contains(name)){
//            Db::instance().dropView(m_viewInfo);
            return;
        }

        auto sql = createViewSql();
        if(sql.isEmpty()){
            sql = viewInfo().m_createViewSql;
        }
        if(sql.isEmpty()){
            IRdbAbort::abortEntityCannotBeCreated("no create view sql clause exist, please add createViewSql", $ISourceLocation);
        }


        this->createQuery().exec(sql);
        if(IRdbEntityModelWare<View, Db>::m_database.getRdbViews().contains(name)){
            qDebug().noquote() << IMetaUtil::getTypeName<T>() << "CREATE VIEW: " << name;
        }else{
            qDebug().noquote() << IMetaUtil::getTypeName<T>() << "CREATE VIEW: " << name << "FAILED";
        }
    }
}

template<typename T, typename View, typename Db, bool enabled>
const IRdbViewInfo &IRdbViewModelInterface<T, View, Db, enabled>::viewInfo()
{
    return View::staticEntityInfo();
}


template<typename T, typename Table, typename Db, bool enabled>
typename IRdbViewModelInterface<T, Table, Db, enabled>::IRdbViewModelInterfaceInitPrivate
    IRdbViewModelInterface<T, Table, Db, enabled>::m_private;

template<typename T, typename Table, typename Db, bool enabled>
IRdbViewModelInterface<T, Table, Db, enabled>::IRdbViewModelInterfaceInitPrivate::IRdbViewModelInterfaceInitPrivate()
{
    if constexpr (enabled){
        static std::once_flag flag;
        std::call_once(flag, [](){
            ITaskManage::instance().addTaskWare(&ISolo<T>());
        });
    }
}


$PackageWebCoreEnd

#endif
