#ifndef TemplateList_H
#define TemplateList_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
       explicit TemplateList(Project *project, XmlStackedHandlerReader *xmlReader,
                            QObject *parent = NULL);
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

       /**
        *
        * @author 2017-11-01 Christopher Combs
        * @internal
        *   @history 2017-11-01 Christopher Combs - Maintains a list of Templates so that templates
        *     can easily be copied from one Project to another, saved to disk, or deleted from disk.
        *     Adapted from ControlList.
        */
       class XmlHandler : public XmlStackedHandler {

         public:
           XmlHandler(TemplateList *templateList, Project *project);

           virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                     const QString &qName, const QXmlAttributes &atts);
           virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                   const QString &qName);

         private:
           Q_DISABLE_COPY(XmlHandler);

           TemplateList *m_templateList; //!< TemplateList to be read or written
           Project *m_project; //!< Project that contains the template list
       };

   };
 }

#endif
