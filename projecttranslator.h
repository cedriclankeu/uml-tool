#pragma once

#include "types.h"

namespace translator {

    class ProjectTranslator
    {
    public:
        ProjectTranslator();
        ProjectTranslator(const db::SharedDatabase &globalDb,
                          const db::SharedDatabase &projectDb);

        db::SharedDatabase globalDatabase() const;
        void setGlobalDatabase(const db::SharedDatabase &globalDatabase);

        db::SharedDatabase projectDatabase() const;
        void setProjectDatabase(const db::SharedDatabase &projectDatabase);

        QString generateCode(const entity::SharedType &type, bool withNamespace = true,
                             const db::SharedDatabase &localeDatabase = nullptr) const;
        QString generateCode(const entity::SharedExtendedType &extType,
                             bool alias = false, bool withNamespace = true,
                             const db::SharedDatabase &localeDatabase = nullptr) const;
        QString generateCode(const entity::SharedField &field, bool withNamespace = true,
                             const db::SharedDatabase &localeDatabase = nullptr) const;
        QString generateCode(const entity::SharedEnum &_enum,
                             bool generateNumbers = false) const;
        QString generateCode(const entity::SharedMethod &method) const; // TODO: add template case
        QString generateCode(const entity::SharedUnion &_union) const;
        QString generateCode(const entity::SharedClass &_class) const; // TODO: add template case

    private:
        void checkDb() const;
        QString generateCodeForExtTypeOrType(const QString &id, bool withNamespace = true,
                                             const db::SharedDatabase &localeDatabase = nullptr) const;
        void generateClassSection(const entity::SharedClass &_class,
                                  entity::Section section, QString &out) const;

        db::SharedDatabase m_GlobalDatabase;
        db::SharedDatabase m_ProjectDatabase;
    };

} // namespace translator
