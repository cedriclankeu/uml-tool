#include <utility>

#include <utility>

/*****************************************************************************
**
** Copyright (C) 2014 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 03/11/2014.
**
** This file is part of Q-UML (UML tool for Qt).
**
** Q-UML is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Q-UML is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.

** You should have received a copy of the GNU Lesser General Public License
** along with Q-UML.  If not, see <http://www.gnu.org/licenses/>.
**
*****************************************************************************/
#include "Project.h"

#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QDebug>

#include <Common/BasicElement.h>

#include <Entity/Type.h>
#include <Entity/Scope.h>
#include <Entity/EntityFactory.h>

#include <DB/ProjectDatabase.h>

#include <GUI/graphics/Entity.h>

#include <Utility/helpfunctions.h>

#include <Relationship/RelationFactory.h>

#include <Helpers/GeneratorID.h>

#include "Constants.h"

namespace Projects {

    namespace {

        Entity::SharedScope makeProjectScope()
        {
            auto scope(std::make_shared<Entity::Scope>());
            scope->setId(Common::ID::projectScopeID());
            return scope;
        }

    } // namespace

    /**
     * @brief Project::Project
     */
    Project::Project()
        : Project(DEFAULT_PROJECT_NAME, DEFAULT_PROJECT_PATH)
    {}

    /**
     * @brief Project::Project
     * @param name
     * @param path
     */
    Project::Project(QString name, QString path)
        : m_Name(std::move(name))
        , m_Path(std::move(path))
        , m_nextUniqueID(Common::ID::firstFreeID().value())
        , m_Modified(false)
        , m_Database(std::make_shared<DB::ProjectDatabase>())
    {
        m_Database->addExistsScope(makeProjectScope());
        G_CONNECT(m_Database.get(), &DB::ProjectDatabase::scopeAdded,
                  [this](auto &&scope){ emit scopeAdded(this->name(), scope); });
        G_CONNECT(m_Database.get(), &DB::ProjectDatabase::scopeRemoved,
                  [this](auto &&scope){ emit scopeRemoved(this->name(), scope); });
    }

    /**
     * @brief Project::Project
     * @param src
     */
    Project::Project(Project &&src) noexcept
        : m_Name(std::move(src.m_Name))
        , m_Path(std::move(src.m_Path))
        , m_nextUniqueID(std::move(src.m_nextUniqueID))
        , m_Modified(src.m_Modified)
        , m_Database(std::move(src.m_Database))
        , m_CommandsStack(std::move(src.m_CommandsStack))
    {
    }

    /**
     * @brief Project::~Project
     */
    Project::~Project() = default;

    /**
     * @brief Project::operator =
     * @param lhs
     * @return
     */
    Project &Project::operator =(Project &&lhs) noexcept
    {
        if (this != &lhs) {
            m_Name = std::move(lhs.m_Name);
            m_Path = std::move(lhs.m_Path);
            m_nextUniqueID = std::move(lhs.m_nextUniqueID);
            m_Modified = lhs.m_Modified;
            m_Database = std::move(lhs.m_Database);
            m_CommandsStack = std::move(lhs.m_CommandsStack);
        }

        return *this;
    }

    /**
     * @brief operator ==
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator ==(const Project &lhs, const Project &rhs)
    {
        return lhs.m_Name == rhs.m_Name &&
               lhs.m_Path == rhs.m_Path &&
               lhs.m_nextUniqueID == rhs.m_nextUniqueID &&
               lhs.m_Modified   == rhs.m_Modified   &&
               lhs.m_Errors       == rhs.m_Errors       &&
               *lhs.m_Database    == *rhs.m_Database;
    }

    /**
     * @brief Project::load
     * @param path
     */
    void Project::load(const QString &path)
    {
        Q_ASSERT(!!m_Database);

        m_Errors.clear();

        if (!Util::readFromFile(*this, path))
            m_Errors << tr("Cannot read project file.");

        if (m_Path != path)
            m_Path = QFileInfo(path).absolutePath();

        // We need to temporary change current project of factories to this one in order to load
        // types and relations
        ScopedProjectSetter ps(safeShared()); Q_UNUSED(ps);

        m_Database->setPath(m_Path);
        m_Database->setName(databaseFileName());
        m_Database->load(m_Errors);

        setModified(!m_Errors.isEmpty());

        // Fixup if needed
        if (!m_Database->scope(Common::ID::projectScopeID()))
            m_Database->addExistsScope(makeProjectScope());

        if (!m_Errors.isEmpty())
            errors(tr("Project load error%1").arg(m_Errors.count() <= 1 ? "" : "s"), m_Errors);
    }

    /**
     * @brief Project::save
     */
    void Project::save()
    {
        Q_ASSERT(!!m_Database);

        m_Errors.clear();

        if (!m_Path.isEmpty()) {
            QDir dir(m_Path);
            if (!dir.exists())
                if (!dir.mkpath(m_Path)) {
                    m_Errors << tr("Cannot create project directory.");
                    emit errors(tr("Make path error%1").arg(m_Errors.count() <= 1 ? "" : "s"), m_Errors);
                    return;
                }

            if (!Util::writeToFile(*this, projectPath(m_Path)))
                m_Errors << tr("Cannot save project to file.");

            m_Database->setName(databaseFileName());
            m_Database->setPath(m_Path);
            if (!m_Database->save())
                m_Errors << tr("Cannot save database to file.");
        } else {
            m_Errors << "Project path is empty.";
        }

        setModified(!m_Errors.isEmpty());

        if (!m_Errors.isEmpty())
            emit errors(tr("Project save error%1").arg(m_Errors.count() <= 1 ? "" : "s"), m_Errors);
    }

    /**
     * @brief Project::db
     * @return
     */
    DB::SharedProjectDatabase Project::database() const
    {
        return m_Database;
    }

    /**
     * @brief Project::setDatabase
     * @param db
     */
    void Project::setDatabase(const DB::SharedProjectDatabase &database)
    {
        m_Database = database;
    }

    /**
     * @brief Project::globalDatabase
     * @return
     */
    DB::SharedDatabase Project::globalDatabase() const
    {
        return m_Database ? m_Database->globalDatabase() : nullptr;
    }

    /**
     * @brief Project::setGloablDatabase
     * @param database
     * @return
     */
    bool Project::setGlobalDatabase(const DB::SharedDatabase &database)
    {
        bool result(false);

        if (m_Database)
            m_Database->setGlobalDatabase(database);

        return result;
    }

    /**
     * @brief Project::toJson
     * @return
     */
    QJsonObject Project::toJson() const
    {
        QJsonObject result;

        result.insert("Name", m_Name);
        result.insert("NextID", m_nextUniqueID.toJson());

        return result;
    }

    /**
     * @brief Project::fromJson
     * @param src
     * @param errorList
     */
    void Project::fromJson(const QJsonObject &src, QStringList &errorList)
    {
        Util::checkAndSet(src, "Name", errorList, [&src, this](){
            m_Name = src["Name"].toString();
        });
        Util::checkAndSet(src, "NextID", errorList, [&, this](){
            m_nextUniqueID.fromJson(src["NextID"], errorList);
        });
    }

    /**
     * @brief Project::hasErrors
     * @return
     */
    bool Project::hasErrors() const
    {
        return !m_Errors.isEmpty();
    }

    /**
     * @brief Project::lastErrors
     * @return
     */
    ErrorList Project::lastErrors() const
    {
        return m_Errors;
    }

    /**
     * @brief Project::genID
     * @return
     */
    Common::ID Project::genID()
    {
        return m_nextUniqueID++;
    }

    /**
     * @brief Project::isModified
     * @return
     */
    bool Project::isModified() const
    {
       return m_Modified;
    }

    /**
     * @brief Project::setSaveStatus
     * @param newStatus
     */
    void Project::setModified(bool modified)
    {
        if (modified != m_Modified) {
            m_Modified = modified;

            emit modifiedStatusUpdated(m_Modified);
        }
    }

    /**
     * @brief Project::name
     * @return
     */
    QString Project::name() const
    {
        return m_Name;
    }

    /**
     * @brief Project::path
     * @return
     */
    QString Project::path() const
    {
        return m_Path;
    }

    /**
     * @brief Project::fullPath
     * @return
     */
    QString Project::fullPath() const
    {
        return projectPath(path());
    }

    /**
     * @brief Project::setName
     * @param name
     */
    void Project::setName(const QString &name)
    {
        if (m_Name == name)
            return;

        m_Name = name;
        emit nameChanged(name);
    }

    /**
     * @brief Project::setPath
     * @param path
     */
    void Project::setPath(const QString &path)
    {
        if (m_Path == path)
            return;

        m_Path = path;
        emit pathChanged(path);
    }

    /**
     * @brief Project::projectFileName
     * @return
     */
    QString Project::projectFileName() const
    {
        return m_Name.toLower().replace(" ", "_");
    }

    /**
     * @brief Project::databaseFileName
     * @return
     */
    QString Project::databaseFileName() const
    {
        return m_Name.toLower().replace(" ", "_") + "_db";
    }

    /**
     * @brief Project::makeProjectPath
     * @param basePath
     * @return
     */
    QString Project::projectPath(const QString &basePath) const
    {
        return QDir::toNativeSeparators(
                   basePath + "/" + projectFileName() + "." + PROJECT_FILE_EXTENTION);
    }

    /**
     * @brief ScopedProjectSetter::ScopedProjectSetter
     * @param p
     */
    ScopedProjectSetter::ScopedProjectSetter(const SharedProject &p)
    {
        Q_ASSERT(p);
        Q_ASSERT(Entity::EntityFactory::instance().project() ==
                 Relationship::RelationFactory::instance().project());

        G_CONNECT(this, &ScopedProjectSetter::projectChanged,
                  &Entity::EntityFactory::instance(), &Entity::EntityFactory::onProjectChanged);
        G_CONNECT(this, &ScopedProjectSetter::projectChanged,
                  &Relationship::RelationFactory::instance(), &Relationship::RelationFactory::onProjectChanged);

        m_OldProject = Entity::EntityFactory::instance().project();
        emit projectChanged(nullptr, p);
    }

    /**
     * @brief ScopedProjectSetter::~ScopedProjectSetter
     */
    ScopedProjectSetter::~ScopedProjectSetter()
    {
        emit projectChanged(nullptr, m_OldProject);

        G_DISCONNECT(this, &ScopedProjectSetter::projectChanged,
                     &Entity::EntityFactory::instance(), &Entity::EntityFactory::onProjectChanged);
        G_DISCONNECT(this, &ScopedProjectSetter::projectChanged,
                     &Relationship::RelationFactory::instance(), &Relationship::RelationFactory::onProjectChanged);
    }

    uint qHash(const SharedProject &p)
    {
        return ::qHash(p.get());
    }

} // namespace project
