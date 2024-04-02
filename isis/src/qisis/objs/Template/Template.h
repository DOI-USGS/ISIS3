#ifndef Template_H
#define Template_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

 #include <QObject>
 #include <QString>
 #include <QXmlStreamWriter>
 #include <QXmlStreamReader>

 #include "FileName.h"
 #include "XmlStackedHandler.h"

 namespace Isis {
   class FileName;
   class Project;

   /**
    * @author 2017-11-01 Christopher Combs
    * @internal
    *   @history 2017-11-01 Christopher Combs -  This represents an ISIS template in a
    *                       project-based GUI interface.  This encapsulates ideas about a
    *                       template such as it's filename and import name.
    */
   class Template : public QObject {
     Q_OBJECT

   public:
     explicit Template(QString fileName, QString templateType, QString importName, QObject *parent = 0);
     Template(FileName templateFolder, QXmlStreamReader *xmlReader, QObject *parent = 0);
     ~Template();

     QString importName() const;
     QString fileName() const;
     QString templateType() const;

     void deleteFromDisk();
     void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

   public slots:
     void updateFileName(Project * project);

   private:
     /**
      * @author ????-??-?? Steven Lambright
      *
      * @internal
      *   @history ????-??-?? Steven Lambright -  Nested class used to write the Template object
      *                             information to an XML file for the purpose of saving and
      *                             restoring the state of the project.
      */
     class XmlHandler : public XmlStackedHandler {
       public:
         XmlHandler(Template *currentTemplate, FileName templateFolder);

         virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                   const QString &qName, const QXmlAttributes &atts);

       private:
         Q_DISABLE_COPY(XmlHandler);

         Template *m_xmlHandlerTemplate;        /**< A pointer to the Template object to be read or
                                                   written.*/
         FileName m_xmlHandlerTemplateFolderName; /**< The name of the folder for the template xml.*/
     };

   private:
     QString m_fileName; // File name of the template associated with this object
     QString m_templateType; // Type of template (maps/registrations)
     QString m_importName; // Name of TemplateList this was imported in

   };
 }

Q_DECLARE_METATYPE(Isis::Template *);

 #endif
