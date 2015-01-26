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

#include "application.h"

#include <QFileInfo>
#include <QDir>
#include <QJsonObject>
#include <QDebug>

#include <db/database.h>
#include <db/projectdatabase.h>
#include <entity/scope.h>
#include <project/project.h>
#include <gui/mainwindow.h>
#include <models/projecttreemodel.h>

#include "enums.h"
#include "constants.cpp"

namespace application {

//    /**
//     * @brief Application::init
//     */
//    void Application::init()
//    {
//        readConfig();
//        readDatabase();
//    }

//    /**
//     * @brief Application::configuredGui
//     */
//    void Application::configuredGui()
//    {
//    }

//    /**
//     * @brief Application::readDatabase
//     */
//    void Application::readDatabase()
//    {
//        QString filePath(QDir::toNativeSeparators(QDir::currentPath() + "/" + APPLICATION_DATABASE_FULL_NAME));

//        if (QFileInfo(filePath).exists()) {
//            m_GlobalDatabase->setName(APPLICATION_DATABASEL_NAME);
//            m_GlobalDatabase->setPath(QDir::currentPath());
//            m_GlobalDatabase->load(*m_ErrorList);

//            if (m_ErrorList->isEmpty()){
//            }
//        } else {
//            *m_ErrorList << tr("Database file is not found.");
//        }
//    }

    /**
     * @brief Application::run
     */
    void Application::run()
    {
//        init();

        m_MainWindow->showMaximized();

//        if (hasErrors()) {
//            emit errors(tr("Application init errors"), *m_ErrorList);
//            m_ErrorList->clear();
//        }
    }

//    /**
//     * @brief Application::hasErrors
//     * @return
//     */
//    bool Application::hasErrors() const
//    {
//        return !m_ErrorList->isEmpty();
//    }

//    /**
//     * @brief Application::errors
//     * @return
//     */
//    ErrorList Application::getErrors() const
//    {
//        return *m_ErrorList;
//    }

//    /**
//     * @brief Application::createProject
//     * @param name
//     * @param path
//     * @return
//     */
//    void Application::createProject(const QString &name, const QString &path)
//    {
//        auto newProject = std::make_shared<project::Project>(name, path, m_ErrorList);
//        newProject->setGloablDatabase(m_GlobalDatabase);

//        if (!newProject->hasErrors()) {
//            m_Projects.insert(newProject->id(), newProject);
//            setCurrentProject(newProject->id());
//            emit projectCreated(newProject->toJson());
//            newProject->save();

//            auto basicScope = newProject->database()->addScope();
//            setCurrentScopeID(basicScope->id());
//        } else {
//            emit errors(tr("Project creation errors"), *m_ErrorList);
//            m_ErrorList->clear();
//            newProject.reset();
//        }
//    }

//    /**
//     * @brief Application::openProject
//     * @param path
//     * @return
//     */
//    bool Application::openProject(const QString &path)
//    {
//        project::SharedProject pr(std::make_shared<project::Project>());
//        pr->setErrorsList(m_ErrorList);
//        pr->setGloablDatabase(m_GlobalDatabase);

//        pr->load(path);

//        if (!m_ErrorList->isEmpty()) {
//            emit errors(tr("Open project errors."), *m_ErrorList);
//            m_ErrorList->clear();
//            return false;
//        }

//        // now you can open only one project
//        // force save changes and close project, yet
//        if (m_CurrentProject)
//            m_CurrentProject->save();

//        // TODO: maybe handle case, when user reopenning current project
//        m_Projects.insert(pr->id(), pr);

//        if (!pr->database()->anyScopes()) {
//            auto basicScope = pr->database()->addScope();
//            setCurrentScopeID(basicScope->id());
//        } else {
//            // TODO: preserve last scope and try to restore it
//            auto firstScope = pr->database()->scopes().first();
//            setCurrentScopeID(firstScope->id());
//        }

//        setCurrentProject(pr->id());
//        emit projectOpened(pr->toJson());
//        return true;
//    }

//    /**
//     * @brief Application::setActiveProject
//     * @param id
//     * @return
//     */
//    bool Application::setCurrentProject(const QString &id)
//    {
//        if (m_Projects.contains(id)) {

//            auto oldProject = m_CurrentProject;

//            m_CurrentProject = m_Projects[id];
//            if (m_CurrentProject->globalDatabase() != m_GlobalDatabase)
//                m_CurrentProject->setGloablDatabase(m_GlobalDatabase);

//            emit projectChanged();

//            return setCurrentProjectDatabase();
//        }

//        return false;
//    }

//    /**
//     * @brief Application::setCurrentProjectDatabase
//     * @param id
//     * @return
//     */
//    bool Application::setCurrentProjectDatabase()
//    {
//        if (m_CurrentProject && m_CurrentProject->database()) {
//            m_CurrentProjectDatabase = m_CurrentProject->database();

//            return true;
//        }

//        return false;
//    }

//    /**
//     * @brief Application::setCurrentScopeID
//     * @param arg
//     */
//    void Application::setCurrentScopeID(const QString &id)
//    {
//        if (m_currentScopeID == id)
//            return;

//        m_currentScopeID = id;
//        emit currentScopeIDChanged(id);
//    }

//    /**
//     * @brief Application::findProjectById
//     * @param id
//     * @return
//     */
//    project::SharedProject Application::findProjectById(const QString &id) const
//    {
//        return m_Projects.value(id, nullptr);
//    }

//    /**
//     * @brief Application::findProjectByName
//     * @param name
//     * @return
//     */
//    project::SharedProject Application::findProjectByName(const QString &name) const
//    {
//        auto projects = m_Projects.values();
//        auto it = std::find_if(projects.begin(), projects.end(),
//                               [&](const project::SharedProject &p){ return p->name() == name; });
//        return it != projects.end() ? *it : nullptr;
//    }

//    /**
//     * @brief Application::projects
//     * @return
//     */
//    project::ProjectsList Application::projects() const
//    {
//        return m_Projects.values();
//    }

//    /**
//     * @brief Application::projectsCount
//     * @return
//     */
//    uint Application::projectsCount() const
//    {
//        return m_Projects.count();
//    }

//    /**
//     * @brief Application::removeProjectById
//     * @param id
//     */
//    void Application::removeProjectById(const QString &id)
//    {
//        m_Projects.remove(id);
//    }

//    /**
//     * @brief Application::removeProjectByName
//     * @param name
//     */
//    void Application::removeProjectByName(const QString &name)
//    {
//        auto project = findProjectByName(name);
//        if (project)
//            m_Projects.remove(project->id());
//    }

//    /**
//     * @brief Application::currentScopeID
//     * @return
//     */
//    QString Application::currentScopeID() const
//    {
//        return m_currentScopeID;
//    }

    /**
     * @brief Application::Application
     */
    Application::Application(QObject *parent)
        : QObject(parent)
//        , m_ErrorList(std::make_shared<ErrorList>())
//        , m_GlobalDatabase(std::make_shared<db::Database>())
        , m_TestModel(std::make_unique<models::ProjectTreeModel>())
        , m_MainWindow(std::make_unique<gui::MainWindow>(m_TestModel.get()))
    {
//        configuredGui();
    }

    /**
     * @brief Application::~Application
     */
    Application::~Application()
    {
    }

//    /**
//     * @brief Application::readConfig
//     */
//    void Application::readConfig()
//    {
//        QString filePath(QDir::toNativeSeparators(QDir::currentPath() + "/" + CFG_FILE_NAME));

//        if (QFileInfo(filePath).exists()) {
//            // read config
//        } else {
//            *m_ErrorList << tr("Configuration file is not found.");
//        }
//    }

} // namespace application
