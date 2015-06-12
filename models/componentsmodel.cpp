/*****************************************************************************
**
** Copyright (C) 2015 Fanaskov Vitaly (vt4a2h@gmail.com)
**
** Created 24/05/2015.
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
#include "componentsmodel.h"

#include <type_traits>

#include <entity/class.h>
#include <entity/classmethod.h>
#include <entity/enum.h>
#include <entity/field.h>

#include <gui/signaturemaker.h>

namespace models {

    namespace  {
        const int colCount = 2;

        int count(const entity::SharedComponents &components, DisplayPart part)
        {
            switch (part) {
                case DisplayPart::Fields:
                    return components->fields().count();

                case DisplayPart::Methods:
                    return components->methods().count();

                case DisplayPart::Elements:
                    return components->variables().count();

                case DisplayPart::Properties:
                    return 0;

                case DisplayPart::Invalid:
                    return 0;
            }

            return 0;
        }
    }

    /**
     * @brief ClassComponentsModel::ClassComponentsModel
     * @param parent
     */
    ComponentsModel::ComponentsModel(const entity::SharedComponents &components, QObject *parent)
        : QAbstractTableModel(parent)
        , m_Components(components)
    {
    }

    /**
     * @brief ClassComponentsModel::setSignatureMaker
     * @param maker
     */
    void ComponentsModel::setSignatureMaker(gui::UniqueSignatureMaker &&maker)
    {
        m_SignatureMaker = std::move(maker);
    }

    /**
     * @brief ClassComponentsModel::clear
     */
    void ComponentsModel::clear()
    {
        beginResetModel();
        m_Components.reset();
        endResetModel();
    }

    /**
     * @brief ComponentsModel::addMethod
     */
    entity::SharedMethod ComponentsModel::addMethod()
    {
        return add<entity::SharedMethod>([this](){ return m_Components->addNewMethod(); }, m_Components->methods().count());
    }

    /**
     * @brief ComponentsModel::addExistsMethod
     * @param method
     */
    void ComponentsModel::addExistsMethod(const entity::SharedMethod &method, int pos)
    {
        int count = m_Components->methods().count();
        addExists([this, &method](int pos){ m_Components->addExistsMethod(method, pos); }, count, pos);
    }

    /**
     * @brief ComponentsModel::removeMethod
     * @param method
     * @return
     */
    int ComponentsModel::removeMethod(const entity::SharedMethod &method)
    {
        return removeMethod(index(m_Components->methods().indexOf(method), 0));
    }

    /**
     * @brief ComponentsModel::removeMethod
     * @param index
     * @return
     */
    int ComponentsModel::removeMethod(const QModelIndex &index)
    {
        return remove([this](int pos){ return m_Components->removeMethod(m_Components->methods()[pos]); }, index);
    }

    /**
     * @brief ComponentsModel::addField
     */
    entity::SharedField ComponentsModel::addField()
    {
        return add<entity::SharedField>([this](){ return m_Components->addNewFiled(); }, m_Components->fields().count());
    }

    /**
     * @brief ComponentsModel::addExistsField
     * @param field
     * @param pos
     */
    void ComponentsModel::addExistsField(const entity::SharedField &field, int pos)
    {
        int count = m_Components->fields().count();
        addExists([this, &field](int pos){ m_Components->addExistsFiled(field, pos); }, count, pos);
    }

    /**
     * @brief ComponentsModel::removeField
     * @param field
     * @return
     */
    int ComponentsModel::removeField(const entity::SharedField &field)
    {
        return removeField(index(m_Components->fields().indexOf(field), 0));
    }

    /**
     * @brief ComponentsModel::removeField
     * @param index
     * @return
     */
    int ComponentsModel::removeField(const QModelIndex &index)
    {
        return remove([this](int pos){ return m_Components->removeField(m_Components->fields()[pos]); }, index);
    }

    /**
     * @brief ComponentsModel::addElement
     */
    void ComponentsModel::addElement()
    {
        // TODO: implemet
    }

    /**
     * @brief ComponentsModel::addProperty
     */
    void ComponentsModel::addProperty()
    {
        // TODO: implemet
    }

    /**
     * @brief ClassComponentsModel::~ClassComponentsModel
     */
    ComponentsModel::~ComponentsModel()
    {
    }

    /**
     * @brief ClassComponentsModel::rowCount
     * @param parent
     * @return
     */
    int ComponentsModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_Components ? count(m_Components, m_display) : 0;
    }

    /**
     * @brief ClassComponentsModel::columnCount
     * @param parent
     * @return
     */
    int ComponentsModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return colCount;
    }

    /**
     * @brief ClassComponentsModel::data
     * @param index
     * @param role
     * @return
     */
    QVariant ComponentsModel::data(const QModelIndex &index, int role) const
    {
        Q_ASSERT(m_SignatureMaker);
        // TODO: use maps of lambdas instead

        if (role == Qt::DisplayRole && m_Components) {
            if (index.column() == 0) {
                switch (m_display) {
                    case DisplayPart::Methods:
                        return m_SignatureMaker->signature(m_Components->methods()[index.row()]);

                    case DisplayPart::Fields:
                        return m_SignatureMaker->signature(m_Components->fields()[index.row()]);

                    case DisplayPart::Elements:
                        return m_Components->variables()[index.row()].first; // TODO: add variables to signature maker

                    case DisplayPart::Properties:
                        return "stub";

                    case DisplayPart::Invalid:
                        return tr("Invalid display value");
                }
            }
        }

        if (role == InternalData && m_Components) {
            switch (m_display) {
                case DisplayPart::Methods:
                    return QVariant::fromValue(m_Components->methods()[index.row()]);

                case DisplayPart::Fields:
                    return QVariant::fromValue(m_Components->fields()[index.row()]);

                case DisplayPart::Elements:
                    return QVariant::fromValue(m_Components->variables()[index.row()]);

                case DisplayPart::Properties:
                    return QVariant(); // TODO: implement

                case DisplayPart::Invalid:
                    return QVariant();
            }
        }

        return QVariant();
    }

    /**
     * @brief ClassComponentsModel::flags
     * @param index
     * @return
     */
    Qt::ItemFlags ComponentsModel::flags(const QModelIndex &index) const
    {
        Qt::ItemFlags flags = Qt::ItemIsEnabled;

        if (index.isValid() && index.column() == ShortSignature)
            flags |= Qt::ItemIsSelectable;

        return  flags;
    }

    /**
     * @brief ClassComponentsModel::components
     * @return
     */
    entity::SharedComponents ComponentsModel::components() const
    {
        return m_Components;
    }

    /**
     * @brief ClassComponentsModel::setComponents
     * @param components
     */
    void ComponentsModel::setComponents(const entity::SharedComponents &components)
    {
        beginResetModel();
        m_Components = components;
        endResetModel();

        for (int i = 0; i < count(m_Components, m_display); ++i)
            emit showButtonsForIndex(index(i, 1));
    }

    /**
     * @brief ClassComponentsModel::display
     * @return
     */
    DisplayPart ComponentsModel::display() const
    {
        return m_display;
    }

    /**
     * @brief ClassComponentsModel::setDisplay
     * @param display
     */
    void ComponentsModel::setDisplay(const DisplayPart &display)
    {
        if (display != m_display) {
            beginResetModel();
            m_display = display;
            endResetModel();

            for (int i = 0, max = count(m_Components, m_display); i < max; ++i)
                emit showButtonsForIndex(index(i, 1));
        }
    }

    /**
     * @brief ComponentsModel::addExists
     * @param inserter
     * @param count
     * @param pos
     */
    void ComponentsModel::addExists(const std::function<void (int)> &inserter, int count, int pos)
    {
        int innerIndex = pos == -1 ? count : pos;
        beginInsertRows(QModelIndex(), innerIndex, innerIndex);
        inserter(innerIndex);
        endInsertRows();

        showButtonsForIndex(index(innerIndex, 1));
    }

    /**
     * @brief ComponentsModel::remove
     * @param deleter
     * @param index
     * @return
     */
    int ComponentsModel::remove(const std::function<int(int)> &deleter, const QModelIndex &index)
    {
        if (!index.isValid())
            return -1;

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        int pos = deleter(index.row());
        endRemoveRows();

        return pos;
    }

} // namespace models