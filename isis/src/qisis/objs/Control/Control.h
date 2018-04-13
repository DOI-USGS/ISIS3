#ifndef Control_H
#define Control_H
/**
 * @file
 * $Revision: 1.0 $
 * $Date: 2012/06/12 15:55:00 $
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
#include <QObject> // parent
#include <QString>

#include "FileName.h"
#include "XmlStackedHandler.h"

class QMutex;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  class ControlDisplayProperties;
  class ControlNet;
  class FileName;
  class Project;
  class PvlObject;

  /**
   * This represents an ISIS control net in a project-based GUI interface.
   * This encapsulates ideas about a control net such as it's filename and display properties.
   *
   * @author 2012-06-12 Ken Edmundson and Tracie Sucharski
   *
   * @internal
   *   @history 2012-08-02 Kimberly Oyama - Added comments to some of the methods and
   *                           member variables.
   *   @history 2012-09-11 Tracie Sucharski - Added new constructor that takes a ControlNet *.
   *   @history 2015-10-14 Jeffrey Covington - Declared Control * as a Qt
   *                           metatype for use with QVariant. References #3949
   *   @history 2016-06-02 Jeannie Backer - Updated documentation. Fixes #3949
   *   @history 2017-08-02 Cole Neuabuer - Added new constructor. This constructor takes a Project
   *                           and a filename of a controlnet. This was added because the Control
   *                           object needs to assign a mutex when the control net is opened and by
   *                           doing this internally we can Close control nets and not have to
   *                           track whether this step has happened Fixes #5026
   *   @history 2017-08-11 Cole Neuabuer - Added try catch throw to make it so importing an invalid
   *                           control net throws some type of error/warning Fixes #5064
   *   @history 2017-11-09 Tyler Wilson - Modified the copyToNewProjectRoot function so that the
   *                           control net is copied to it's new location like a binary file,
   *                           instead of being recreated from scratch by calling it's write method.
   *                           Fixes #5212.
   *   @history 2017-12-20 Tracie Sucharski - In ::copyToNewProjectRoot use string comparison
   *                           to compare project roots. References #4804, #4849.
   *   @history 2018-01-19 Tracie Sucharski - Do not copy control unless the project root has
   *                           changed. References #5212.
   *   @history 2018-03-30 Tracie Sucharski - Added setModified and is Modified methods to keep
   *                           track of the modification state of the control net. Add write method
   *                           to write the control net to disk.  This write method should be called
   *                           by ipce classes instead of calling the ControlNet::Write directly so
   *                           that control knows the state of the control net. If a project
   *                           is performing a "Save As", and there is a modified active control,the
   *                           cnet is written out to the new location, it is not save in the old
   *                           project location.
   */
  class Control : public QObject {
    Q_OBJECT
    public:
      ControlNet *m_controlNet; /**< A pointer to the ControlNet object associated with this
                                                    Control object.*/
      explicit Control(QString cnetFileName, QObject *parent = 0);
      explicit Control(Project *project, QString cnetFileName, QObject *parent = 0);
      explicit Control(ControlNet *controlNet, QString cnetFileName, QObject *parent = 0);
      Control(FileName cnetFolder, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);
      ~Control();

      ControlNet *controlNet();
      void openControlNet();
      ControlDisplayProperties *displayProperties();
      const ControlDisplayProperties *displayProperties() const;
      QString fileName() const;

      QString id() const;

      bool isModified();
      void setModified(bool modified = true);
      bool write();  

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;
      void copyToNewProjectRoot(const Project *project, FileName newProjectRoot);
      void deleteFromDisk();

    public slots:
      void updateFileName(Project *);
      void closeControlNet();

    private:
      /**
       * Nested class used to write the Control object information to an XML file for the
       * purpose of saving and restoring the state of the project.
       *
       * @author 2012-??-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(Control *control, FileName cnetFolder);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);

        private:
          Q_DISABLE_COPY(XmlHandler);

          Control *m_xmlHandlerControl;        /**< A pointer to the Control object to be read or
                                                    written.*/
          FileName m_xmlHandlerCnetFolderName; /**< The name of the folder for the control xml.*/
      };

    private:
      Control(const Control &other);
      Control &operator=(const Control &rhs);

      bool m_modified;

      ControlDisplayProperties *m_displayProperties; /**< Contains the display properties for this
                                                          Control object.*/
      Project *m_project; //! Project associated with this control
      QString m_fileName; /**< File name of the control net associated with this control.*/

      /**
       * A unique ID for this Control.
       * (useful for others to reference this Control when saving to disk).
       */
      QUuid *m_id;
  };
}

Q_DECLARE_METATYPE(Isis::Control *);

#endif // Control_H
