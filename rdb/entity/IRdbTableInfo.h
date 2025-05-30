#pragma once

#include "core/util/IHeaderUtil.h"
#include "core/unit/INoCopyMoveUnit.h"
#include "rdb/IRdbManage.h"
#include "rdb/entity/IRdbEntityInfo.h"

$PackageWebCoreBegin

class IRdbTableInfo : public IRdbEntityInfo, public INoCopyMoveUnit
{
public:
    enum ValueMakerType{
        ReadValue,
        InsertValue,
        GenerateValue,
    };
    struct ValueMaker{
        int index{};
        ValueMakerType type{ValueMakerType::ReadValue};
        QString insertValue;
        IRdbManage::GeneratorFunction generator;
    };

public:
    IRdbTableInfo() = delete;
    explicit IRdbTableInfo(const QMetaObject& meta);

public:
    int primaryKey {-1};
    int autoIncrement{-1};
    QList<int> notNullKeys;
    QList<int> uniqueKeys;
    QMap<int, QString> constraints;
    QMap<int, QString> sqlType;
    QVector<ValueMaker> valueMakers;
};

$PackageWebCoreEnd
