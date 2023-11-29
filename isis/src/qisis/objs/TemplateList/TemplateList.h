#ifndef TemplateList_H
#define TemplateList_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>
#include <QList>
#include <QMetaType>
#include <QObject>

#include "Project.h"
#include "Template.h"


 namespace Isis {

   /**
    *
    * @author 2017-11-01 Christopher Combs
    *
    * @internal
    *   @history 2017-11-01 Christopher Combs - Maintains a list of Templates so that templates
    *                           can easily be copied from one Project to another, saved to disk, or
    *                           deleted from disk. Adapted from ControlList.
    *   @history 2018-07-07 Summer Stapleton - Fixed a few errors in how the xmlhandling was
    *                           occuring and added additional handling of separating map templates
    *                           from registration templates to reflect chagnes in Project.cpp.
    */
   class TemplateList : public QObject, public QList<Template *> {
     Q_OBJECT

     public:
       TemplateList(QString name, QString type, QString path, QObject *parent = NULL);
       explicit TemplateList(QObject *parent = NULL);
       TemplateList(const TemplateList &);
       ~TemplateList();

       QString name() const;
       QString type() const;
       QString path() const;

       void setName(QString newName);
       void setType(QString newType);
       void setPath(QString newPath);

       void deleteFromDisk(Project *project);
       void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

     private:
       QString m_path;
       QString m_name;
       QString m_type;

   };
 }

#endif
