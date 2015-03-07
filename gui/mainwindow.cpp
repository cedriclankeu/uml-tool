/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 06/01/2015.
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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QSplitter>
#include <QTreeView>
#include <QLineEdit>
#include <QGraphicsView>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <models/applicationmodel.h>
#include <models/projecttreemodel.h>

#include <project/project.h>

#include "about.h"
#include "newproject.h"
#include "addscope.h"
#include "constants.cpp"

namespace {
    const int treeViewIndent = 20;
    const QSize iconSize(20, 20);

    const double treeSizeFactor = 0.3;
    const double canvasSizeFactor = 0.7;
    const double consoleSizeFactor = 0.3;

    const QString titleTemplate = "%1Q-UML";
}

namespace gui {

    /**
     * @brief MainWindow::MainWindow
     * @param parent
     */
    MainWindow::MainWindow(const models::SharedApplicationModal &applicationModel, QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
        , m_MainVerticalSplitter(new QSplitter(this))
        , m_CanvasConsoleSplitter(new QSplitter(this))
        , m_ProjectTreeMenu(new QMenu(this))
        , m_ProjectTreeView(new QTreeView(this))
        , m_MainView(new QGraphicsView(this))
        , m_ConsoleOutput(new QTextEdit(this))
        , m_AboutWidget(new About(this))
        , m_NewProject(new NewProject(this))
        , m_AddScope(new AddScope(this))
        , m_ApplicationModel(applicationModel)
    {
        ui->setupUi(this);

        setUpWidgets();
        makeConnections();

        update();
    }

    /**
     * @brief MainWindow::~MainWindow
     */
    MainWindow::~MainWindow()
    {
        delete ui;
    }

    /**
     * @brief MainWindow::update
     */
    void MainWindow::update()
    {
        makeTitle();
        updateWindowState();
    }

    /**
     * @brief MainWindow::onExit
     */
    void MainWindow::onExit()
    {
        if (auto pr = m_ApplicationModel->currentProject()) {
            if (!pr->isSaved()) {
                int result = QMessageBox::question(this,
                                                   tr("Confirmation"),
                                                   tr("Project \"%1\" is not saved. Save?").arg(pr->name()),
                                                   QMessageBox::Yes | QMessageBox::No);
                if (result == QMessageBox::Yes)
                    pr->save();
            }
        }

        close();
    }

    /**
     * @brief MainWindow::onAbout
     */
    void MainWindow::onAbout()
    {
        m_AboutWidget->show();
    }

    /**
     * @brief MainWindow::onNewProject
     */
    void MainWindow::onNewProject()
    {
        m_NewProject->show();
    }

    /**
     * @brief MainWindow::onOpenProject
     */
    void MainWindow::onOpenProject()
    {
        QString caption(tr("Selet project file"));
        QString dir(QApplication::applicationDirPath()); // temporary
        QString filter(tr("Q-UML Project files (*.%1)").arg(PROJECT_FILE_EXTENTION));

        QString path = QFileDialog::getOpenFileName(this, caption, dir, filter);
        if (path.isEmpty())
            return ;

        auto newProject = std::make_shared<project::Project>();
        newProject->load(path);

        if (newProject->hasErrors()) {
            QMessageBox::critical
                ( this
                , tr("Open project error%1").arg(newProject->lastErrors().size() > 1 ? "s" : "")
                , newProject->lastErrors().join("\n")
                , QMessageBox::Ok
                );
        } else {
            if (m_ApplicationModel->addProject(newProject))
            {
                setCurrentProject(newProject->id());
                newProject->save();
                update();
            } else {
                QMessageBox::information
                    ( this
                    , tr("Q-UML - Information")
                    , tr("Project \"%1\" already exists.").arg(newProject->name())
                    , QMessageBox::Ok
                    );
            }
        }
    }

    /**
     * @brief MainWindow::onSaveProject
     */
    void MainWindow::onSaveProject()
    {
        if (auto currentPtoject = m_ApplicationModel->currentProject()) {
            if (!currentPtoject->isSaved())
                currentPtoject->save();
        } else {
            QMessageBox::information(this, tr("Information"), tr("Nothing to save."), QMessageBox::Ok);
        }
    }

    /**
     * @brief MainWindow::onCreateScope
     */
    void MainWindow::onCreateScope()
    {
        if (m_ApplicationModel->currentProject()) {
            m_AddScope->setProjectName(m_ApplicationModel->currentProject()->name());
            if (m_AddScope->exec())  {
                QString scopeName = m_AddScope->scopeName();
                if (!scopeName.isEmpty())
                    m_ApplicationModel->makeScope(scopeName);
            }
        } else
            QMessageBox::information(this, tr("Information"), tr("No current project."), QMessageBox::Ok);
    }

    /**
     * @brief MainWindow::onAddAlias
     */
    void MainWindow::onAddAlias()
    {

    }

    /**
     * @brief MainWindow::onAddUnion
     */
    void MainWindow::onAddUnion()
    {

    }

    /**
     * @brief MainWindow::onAddStruct
     */
    void MainWindow::onAddStruct()
    {

    }

    /**
     * @brief MainWindow::onAddClass
     */
    void MainWindow::onAddClass()
    {

    }

    /**
     * @brief MainWindow::onAddTemplate
     */
    void MainWindow::onAddTemplate()
    {

    }

    /**
     * @brief MainWindow::onMakeRelation
     */
    void MainWindow::onMakeRelation()
    {

    }

    /**
     * @brief MainWindow::createNewProject
     * @param name
     * @param path
     */
    void MainWindow::createNewProject(const QString &name, const QString &path)
    {
        if (project::SharedProject currentProject = m_ApplicationModel->currentProject()) {
            if (!currentProject->isSaved()) {
                int result =
                    QMessageBox::question
                        ( this
                        , tr("Project is not saved")
                        , tr("Would you like to save a project %1?").arg(currentProject->name())
                        , QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
                        );

                switch (result) {
                    case QMessageBox::Yes:
                        currentProject->save();
                        break;

                    case QMessageBox::No:
                        break;

                    case QMessageBox::Cancel:
                        return ;

                    default: ;
                }
            }
        }

        auto newProject = m_ApplicationModel->makeProject( name, path );
        setCurrentProject(newProject->id());
        newProject->save();
    }

    /**
     * @brief MainWindow::createScope
     * @param name
     */
    void MainWindow::createScope(const QString &name)
    {
        if (!m_ApplicationModel->makeScope(name)) {
            QMessageBox::information(this, tr("Information"), tr("Cannot create scope."), QMessageBox::Ok);
        }
    }

    /**
     * @brief MainWindow::makeElemnts
     */
    void MainWindow::setUpWidgets()
    {
        m_ProjectTreeView->setHeaderHidden(true);
        m_ProjectTreeView->setIndentation(treeViewIndent);
        m_ProjectTreeView->setIconSize(iconSize());
        m_ProjectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
        m_ProjectTreeView->setModel(m_ApplicationModel->treeModel().get());
        m_MainVerticalSplitter->addWidget(m_ProjectTreeView);

        m_CanvasConsoleSplitter->setOrientation(Qt::Vertical);
        m_CanvasConsoleSplitter->addWidget(m_MainView);
        m_CanvasConsoleSplitter->addWidget(m_ConsoleOutput);
        m_CanvasConsoleSplitter->setStretchFactor(0, 9);
        m_MainVerticalSplitter->addWidget(m_CanvasConsoleSplitter);

        m_MainVerticalSplitter->setStretchFactor(1, 9);

        m_MainLayout = std::make_unique<QHBoxLayout>();
        m_MainLayout->addWidget(m_MainVerticalSplitter);
        ui->centralwidget->setLayout(m_MainLayout.get());

        int treeSize(std::round(this->width() * treeSizeFactor));
        int canvasConsoleSize(std::round(this->width() * canvasSizeFactor));
        m_MainVerticalSplitter->setSizes({treeSize, canvasConsoleSize});

        int canvasSize(std::round(this->height() * canvasSizeFactor));
        int consoleSize(std::round(this->height() * consoleSizeFactor));
        m_CanvasConsoleSplitter->setSizes({canvasSize, consoleSize});

        auto a = m_ProjectTreeMenu->addAction("Make active");
        connect(a, &QAction::triggered, this, &MainWindow::setCurrentProjectViaMenu);
    }

    /**
     * @brief MainWindow::makeConnections
     */
    void MainWindow::makeConnections()
    {
        connect(m_ProjectTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onProjectTreeMenu);

        connect(m_NewProject, &NewProject::newProject, this, &MainWindow::createNewProject);
    }

    /**
     * @brief MainWindow::makeTitle
     */
    void MainWindow::makeTitle()
    {
        auto pr = m_ApplicationModel->currentProject();
        QString projectName = pr ? pr->name().append(pr->isSaved() ? "" : "*") : "";
        setWindowTitle(titleTemplate.arg(!projectName.isEmpty() ? projectName.append(" - ") : ""));
    }

    /**
     * @brief MainWindow::onTreeViewMenu
     * @param p
     */
    void MainWindow::onProjectTreeMenu(const QPoint &p)
    {
        QModelIndex index = m_ProjectTreeView->indexAt(p);
        QVariant data = index.data(models::ProjectTreeModel::SharedData);
        if (index.isValid() && data.canConvert<project::SharedProject>()) // NOTE: temporary
            m_ProjectTreeMenu->exec(m_ProjectTreeView->mapToGlobal(p));
    }

    /**
     * @brief MainWindow::setCurrentProject
     */
    void MainWindow::setCurrentProjectViaMenu()
    {
        QModelIndex index = m_ProjectTreeView->selectionModel()->currentIndex();
        QVariant data = index.data(models::ProjectTreeModel::SharedData);
        if (index.isValid() && data.canConvert<project::SharedProject>()) {
           setCurrentProject(index.data(models::ProjectTreeModel::ID).toString());
        }
    }

    /**
     * @brief MainWindow::setWindowState
     */
    void MainWindow::updateWindowState()
    {
        bool state = !!m_ApplicationModel->currentProject();

        ui->actionAddAlias->setEnabled( state );
        ui->actionAddClass->setEnabled( state );
        ui->actionAddStruct->setEnabled( state );
        ui->actionAddTemplate->setEnabled( state );
        ui->actionAddUnion->setEnabled( state );
        ui->actionCreateScope->setEnabled( state );
        ui->actionMakeRelation->setEnabled( state );
        ui->actionSaveProject->setEnabled(state && !m_ApplicationModel->currentProject()->isSaved() );
    }

    /**
     * @brief MainWindow::setCurrentProject
     * @param id
     */
    void MainWindow::setCurrentProject(const QString &id)
    {
        if (m_ApplicationModel->currentProject())
        {
            disconnect(m_ApplicationModel->currentProject().get(), &project::Project::saved, this, &MainWindow::update);
            disconnect(m_ApplicationModel->currentProject().get(), &project::Project::modified, this, &MainWindow::update);
        }

        if (m_ApplicationModel->setCurrentProject(id)) {
            connect(m_ApplicationModel->currentProject().get(), &project::Project::saved, this, &MainWindow::update);
            connect(m_ApplicationModel->currentProject().get(), &project::Project::modified, this, &MainWindow::update);
        } else {
            qWarning() << QString("Current project with id %1 is not found.").arg(id);
        }
    }

} // namespace gui