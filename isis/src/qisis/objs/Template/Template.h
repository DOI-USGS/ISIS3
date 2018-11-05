#ifndef Template_H
#define Template_H
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

 #include <QObject>
 #include <QString>
 #include <QXmlStreamWriter>

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
     Template(FileName templateFolder, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);
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
