
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
#include <QObject>

#include <QString>

#include "FileName.h"
#include "XmlStackedHandler.h"

class QUuid;
class QMutex;
class QXmlStreamWriter;

namespace Isis {
  class ControlNet;
  class ControlDisplayProperties;
  class FileName;
  class Project;
  class PvlObject;

  /**
   * @brief 
   *
   * @author 2012-06-12 Ken Edmundson and Tracie Sucharski
   *
   * @internal
   *   @history 2012-08-02 Kimberly Oyama - Added comments to some of the methods and
   *                             member variables.
   *   @history 2012-09-11 Tracie Sucharski - Added new constructor that takes a ControlNet *. 
   */
  class Control : public QObject {
    Q_OBJECT
    public:
      explicit Control(QString cnetFileName, QObject *parent = 0);
      explicit Control(ControlNet *controlNet, QString cnetFileName, QObject *parent = 0);
      Control(FileName cnetFolder, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);
      ~Control();

      ControlNet *controlNet();
      void openControlNet();
      void closeControlNet();
      ControlDisplayProperties *displayProperties();
      const ControlDisplayProperties *displayProperties() const;
      QString fileName() const;

      QString id() const;

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;
      void copyToNewProjectRoot(const Project *project, FileName newProjectRoot);
      void deleteFromDisk();

    public slots:
      void updateFileName(Project *);

    private:
      /**
       * @author 2012-??-?? ???
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

          Control *m_control;
          FileName m_cnetFolder;
      };

    private:
      Control(const Control &other);
      Control &operator=(const Control &rhs);

      ControlNet *m_controlNet;
      ControlDisplayProperties *m_displayProperties;
      QString m_fileName; //! File name of the control net associated with this control.
      QUuid *m_id;
  };
}

#endif // Control_H
