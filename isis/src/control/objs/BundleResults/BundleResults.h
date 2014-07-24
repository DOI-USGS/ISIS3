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
#include <QObject>
#include <QString>

#include "XmlStackedHandler.h"

class QDataStream;
class QXmlStreamWriter;
class QUuid;

namespace Isis {
  class BundleSettings;
  class BundleStatistics;
  class FileName;
  class Project;
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
      BundleResults(const BundleResults &other);
      ~BundleResults();
      BundleResults &operator=(const BundleResults &other);
      void setOutputStatistics(BundleStatistics statisticsResults);
      PvlObject pvlObject(QString resultsName = "BundleResults",
                          QString settingsName = "InputSettings",
                          QString statisticsName = "StatisticsResults");
      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);
      QString id() const;

      void setRunTime(QString runTime);
      QString runTime() const;

      QString controlNetworkFileName();
      BundleSettings *bundleSettings();
      BundleStatistics *bundleStatistics();

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

    private:
      /**
       *
       * @author 2014-07-21 Ken Edmundson
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(BundleResults *bundleResults, Project *project);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          BundleResults *m_bundleResults;
          Project *m_project;
          QString m_characters;
      };

    private:
      BundleResults();
      FileName         *m_controlNetworkFileName;
      BundleSettings   *m_settings;
      BundleStatistics *m_statisticsResults;

      /**
       * A unique ID for this BundleResults object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid *m_id;

      QString m_runTime;
  };
  // operators to read/write BundleResults to/from binary data
  QDataStream &operator<<(QDataStream &stream, const BundleResults &bundleResults);
  QDataStream &operator>>(QDataStream &stream, BundleResults &bundleResults);
};
#endif // BundleResults_h
