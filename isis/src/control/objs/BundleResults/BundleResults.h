#ifndef BundleResults_h
#define BundleResults_h

/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2009/10/15 01:35:17 $
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

#include <QList>
#include <QObject>
#include <QString>

#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  class BundleSettings;
  class BundleStatistics;
  class FileName;
  class ImageList;
  class Project;  // TODO: does xml stuff need project???
  class PvlObject;
  class XmlStackedHandlerReader;

  /**
   * @brief Container class for BundleAdjustment results. 
   * This class includes the settings used to run the bundle adjustment, the resulting statistics 
   * values, and the name of the control network used.
   *  
   * @ingroup ControlNetworks
   *
   * @author 2014-07-08 Jeannie Backer
   *
   * @internal
   *   @history 2014-07-08 Jeannie Backer - Original version.
   *   @history 2014-07-23 Jeannie Backer - Added implementation for the QDataStream << and >>
   *                           operators and the read/write methods.
   *  
   */
  class BundleResults : public QObject {
    Q_OBJECT
    public:
      BundleResults(BundleSettings inputSettings, FileName controlNetworkFileName, QObject *parent = 0);
      BundleResults(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent = 0);  // TODO: does xml stuff need project???
      BundleResults(const BundleResults &other);
      ~BundleResults();
      BundleResults &operator=(const BundleResults &other);

      void setOutputStatistics(BundleStatistics statisticsResults);
      void setRunTime(QString runTime);

      QString id() const;
      QString controlNetworkFileName() const;
      BundleSettings *bundleSettings();     // TODO: change return value to const reference or copy... unsafe to return pointers to member data???
      BundleStatistics *bundleStatistics(); // TODO: change return value to const reference or copy... unsafe to return pointers to member data???
      // ??? const BundleSettings *bundleSettings() const;
      // ??? const BundleStatistics *bundleStatistics() const;
      QString runTime() const;

      PvlObject pvlObject(QString resultsName = "BundleResults",
                          QString settingsName = "InputSettings",
                          QString statisticsName = "StatisticsResults");

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;  // TODO: does xml stuff need project and newRoot???
      void save(QXmlStreamWriter &stream, const Project *project) const;  // TODO: does xml stuff need project???

      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);

    private:
      /**
       *
       * @author 2014-07-21 Ken Edmundson
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(BundleResults *bundleResults, Project *project);  // TODO: does xml stuff need project???
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          BundleResults *m_xmlHandlerBundleResults;
          Project *m_xmlHandlerProject;  // TODO: does xml stuff need project???
          QString m_xmlHandlerCharacters;
          QList<ImageList *> *m_xmlHandlerImages;
          BundleSettings *m_xmlHandlerBundleSettings;
          BundleStatistics *m_xmlHandlerBundleStatistics;
      };

    private:
      BundleResults();

      /**
       * A unique ID for this BundleResults object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid              *m_id;
      QString             m_runTime;
      FileName           *m_controlNetworkFileName;
      BundleSettings     *m_settings;
      BundleStatistics   *m_statisticsResults;
      QList<ImageList *> *m_images;
  };
  // operators to read/write BundleResults to/from binary data
  QDataStream &operator<<(QDataStream &stream, const BundleResults &bundleResults);
  QDataStream &operator>>(QDataStream &stream, BundleResults &bundleResults);
};
#endif // BundleResults_h
