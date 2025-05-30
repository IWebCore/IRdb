#include "IRdbEntityInfo.h"

#include "core/util/IMetaUtil.h"
#include "rdb/IRdbAbort.h"
#include "rdb/entity/IRdbTableInfo.h"

$PackageWebCoreBegin

IRdbEntityInfo::IRdbEntityInfo(const QMetaObject &meta)
    : m_metaObject(meta)
{
    this->className = meta.className();
    this->entityName = IMetaUtil::getMetaClassInfoByName(meta, "sql_entityName", this->className);

    int count = meta.propertyCount();
    for(int i=0; i<count; i++){
        Field field;
        field.index = i;
        field.property = meta.property(i);
        field.typeName = field.property.typeName();
        field.name = field.property.name();
        fieldNames.append(field.name);
        field.typeId = QMetaType::Type(field.property.type());
        if(field.typeId == QMetaType::User){
            field.typeId = QMetaType::Type(QMetaType::type(field.typeName.toUtf8()));
            if(field.typeId == QMetaType::UnknownType){
                QString tip = QString("in your field declaration, your field type is not registered, please register it. Field:").append(field.name);
                IRdbAbort::abortTableDeclarationError(tip, $ISourceLocation);
            }
        }
        this->fields.append(field);
    }
}

$PackageWebCoreEnd
