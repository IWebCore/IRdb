#pragma once

#include "core/util/IHeaderUtil.h"

$PackageWebCoreBegin

class IRdbEntityInfo
{
public:
    struct Field
    {
        int index{};
        QString name;
        QString typeName;
        QMetaType::Type typeId;
        QMetaProperty property;
    };

public:
    explicit IRdbEntityInfo(const QMetaObject& meta);

public:
    const QMetaObject& m_metaObject;

public:
    QString className;
    QString entityName;

    QList<Field> fields;
    QStringList fieldNames;
};

$PackageWebCoreEnd
