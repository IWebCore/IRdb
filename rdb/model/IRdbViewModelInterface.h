﻿#pragma once

#include "core/util/IHeaderUtil.h"
#include "core/task/unit/ITaskWareUnit.h"
#include "rdb/IRdbAbort.h"
#include "rdb/IRdbCatagory.h"
#include "rdb/model/IRdbEntityModelWare.h"
#include "rdb/entity/IRdbViewInfo.h"

$PackageWebCoreBegin

template<typename T, typename View, typename Db, bool enabled = true>
class IRdbViewModelInterface
    : public IRdbEntityModelWare<View, Db>, public ITaskWareUnit<T, IRdbCatagory, enabled>
{
public:
    IRdbViewModelInterface();

public:
    virtual QString createViewSql() const;

protected:
    virtual double $order() const override;
    virtual void $task() final;

protected:
    const IRdbViewInfo& m_viewInfo;
};

template<typename T, typename View, typename Db, bool enabled>
IRdbViewModelInterface<T, View, Db, enabled>::IRdbViewModelInterface()
    : IRdbEntityModelWare<View, Db>(), m_viewInfo(View::staticEntityInfo())
{
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
    if /*constexpr*/ (enabled){
        const auto& name = m_viewInfo.m_entityName;
        if(IRdbEntityModelWare<View, Db>::m_database.getRdbViews().contains(name)){
            Db::instance().dropView(m_viewInfo);
        }

        auto sql = createViewSql();
        if(sql.isEmpty()){
            sql = m_viewInfo.m_createViewSql;
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

$PackageWebCoreEnd
