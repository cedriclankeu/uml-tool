/*****************************************************************************
**
** Copyright (C) 2016 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 24/03/2016.
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
#include "RelationFactory.h"

#include <QGraphicsScene>
#include <QDebug>

#include <DB/Database.h>
#include <DB/ProjectDatabase.h>

#include <Entity/Type.h>

#include <Project/Project.h>

#include <GUI/graphics/GraphicsRelation.h>
#include <GUI/graphics/Entity.h>

#include <Utility/helpfunctions.h>

#include "Relation.h"
#include "multiplyassociation.h"
#include "association.h"
#include "dependency.h"
#include "realization.h"
#include "QtHelpers.h"

namespace Relationship {

    namespace {
        using MakerR  = std::function<SharedRelation(const Common::ID &, const Common::ID &,
                                                     const DB::WeakTypeSearchers &)>;
        using MakersR = const QMap<RelationType, MakerR>;

        MakersR relationMaker = {
            {AggregationRelation,    [](auto &&tail, auto &&head, auto &&searchers){
                 auto rel =
                     std::make_shared<Relationship::MultiplyAssociation>(tail, head, searchers);
                 rel->setRelationType(AggregationRelation);
                 return rel;
             }},
            {CompositionRelation,    [](auto &&tail, auto &&head, auto &&searchers){
                 auto rel =
                     std::make_shared<Relationship::MultiplyAssociation>(tail, head, searchers);
                 rel->setRelationType(CompositionRelation);
                 return rel;
             }},
            {MultiRelation,          [](auto &&tail, auto &&head, auto &&searchers){
                 return std::make_shared<MultiplyAssociation>(tail, head, searchers); }},
            {AssociationRelation,    [](auto &&tail, auto &&head, auto &&searchers){
                 return std::make_shared<Association>(tail, head, searchers);         }},
            {DependencyRelation ,    [](auto &&tail, auto &&head, auto &&searchers){
                 return std::make_shared<Dependency>(tail, head, searchers);          }},
            {GeneralizationRelation, [](auto &&tail, auto &&head, auto &&searchers){
                 return std::make_shared<Generalization>(tail, head, searchers);      }},
            {RealizationRelation,    [](auto &&tail, auto &&head, auto &&searchers){
                 return std::make_shared<Realization>(tail, head, searchers);         }},
            {SimpleRelation,         [](auto &&tail, auto &&head, auto &&searchers){
                 return std::make_shared<Relation>(tail, head, searchers);            }}
        };

        void addRelation(const SharedRelation &relation, const DB::SharedProjectDatabase &projectDB,
                         const QPointer<QGraphicsScene> &graphicsScene,
                         RelationFactory::CreationOptions options)
        {
            if (!G_ASSERT(relation) || !G_ASSERT(projectDB) || !G_ASSERT(graphicsScene))
                return;

            if (options.testFlag(RelationFactory::AddToDatabase))
                projectDB->addRelation(relation);

            if (options.testFlag(RelationFactory::AddToScene)) {
                auto from = G_ASSERT(projectDB->graphicsEntity(G_ASSERT(relation->tailType())->id()));
                auto to   = G_ASSERT(projectDB->graphicsEntity(G_ASSERT(relation->headType())->id()));

                auto graphicsRelation = new Graphics::Relation(relation, from, to);
                projectDB->registerGraphicsRelation(graphicsRelation);
                graphicsScene->addItem(graphicsRelation);
            }

            if (options.testFlag(RelationFactory::AddToTreeModel)) {
                // TODO: implement when will be added ability to display relations
                //       in the project tree
            }
        }
    }

    /**
     * @brief RelationFactory::instance
     * @return
     */
    const RelationFactory &RelationFactory::instance()
    {
        static RelationFactory instance;
        return instance;
    }

    /**
     * @brief RelationFactory::make
     * @param relType
     * @param tail
     * @param head
     * @param addToScene
     * @return
     */
    SharedRelation RelationFactory::make(RelationType relType, const Common::ID &tail,
                                         const Common::ID &head, CreationOptions options) const
    {
        if (auto pr = G_ASSERT(project())) {
            if (G_ASSERT(pr->database() && project()->globalDatabase())) {
                if (auto maker = G_ASSERT(relationMaker[relType])) {
                    if (auto relation = maker(tail, head, {pr->database(), pr->globalDatabase()})) {
                        addRelation(relation, pr->database(), scene(), options);
                        return relation;
                    }
                }
            }
        }

        return nullptr;
    }

    /**
     * @brief RelationFactory::make
     * @param src
     * @param addToScene
     * @return
     */
    SharedRelation RelationFactory::make(const QJsonObject &src, ErrorList &errors,
                                         CreationOptions options) const
    {
        if (src.contains(Relationship::Relation::typeMarker())) {
            auto relType = RelationType(src[Relationship::Relation::typeMarker()].toInt());
            if (auto maker = G_ASSERT(relationMaker[relType])) {
                DB::WeakTypeSearchers ts {G_ASSERT(project())->database(), G_ASSERT(project())->globalDatabase()};
                if (auto relation = maker(Common::ID::nullID(), Common::ID::nullID(), ts)) {
                    relation->fromJson(src, errors);

                    if (errors.isEmpty()) {
                        addRelation(relation, G_ASSERT(project())->database(), scene(), options);
                        return relation;
                    } else {
                        qWarning() << "Relation was loaded with errors.";
                    }
                }
            }
        }

        return nullptr;
    }

    /**
     * @brief RelationFactory::RelationFactory
     * @param parent
     */
    RelationFactory::RelationFactory(QObject *parent)
        : Common::ElementsFactory(parent)
    {
    }

} // namespace relationship
