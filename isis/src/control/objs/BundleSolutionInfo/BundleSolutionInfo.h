#ifndef BundleSolutionInfo_h
#define BundleSolutionInfo_h

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

#include "BundleSettings.h"

#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  class BundleResults;
  class FileName;
  class ImageList;
  class Project;  //TODO does xml stuff need project???
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
   *   @history 2014-12-04 Jeannie Backer - Renamed from BundleResults to BundleSolutionInfo.
   *   @history 2015-09-03 Jeannie Backer - Added preliminary hdf5 read/write capabilities.
   *   @history 2015-10-14 Jeffrey Covington - Declared BundleSolutionInfo * as 
   *                           a Qt metatype for use with QVariant.
   *   @history 2016-06-13 Makayla Shepherd - Added updateFileName() and updated documentation.
   *                           Fixes #2298.
   *   @history 2016-08-15 Jesse Mapel - added output, outputHeader, outputText, outputPointsCSV,
   *                           and outputResiduals from BundleAdjust.  Fixes #4159.
   *   @history 2016-08-18 Jeannie Backer - Removed all references to deprecated
   *                           BundleSettings::solveMethod. References #4162.
   *   @history 2016-08-23 Jesse Mapel - Removed output() method.  Individual output
   *                           file methods must be called.  Fixes #4279.
   *   @history 2016-09-02 Jesse Mapel - Added camera point and position input parameters to output
   *                           files when using only one set of solve settings.  Fixes #4316.
   *   @history 2016-10-06  Tyler Wilson - Added methods outputImagesCSV()
   *                           and outputImagesCSVHeader which enables jigsaw users to output
   *                           the bundleout_images.csv file.  Fixes #4314.
   *   @history 2016-10-17 Jesse Mapel - Removed multiple solve settings output in accordance with
   *                           USEPVL being removed from jigsaw.  References #4316.
   *   @history 2016-10-28 Tyler Wilson - Modified outputText() to check and output if the solution
   *                           is solving for the radius.  References #4317.
   *   @history 2016-11-14 Ken Edmundson Modified the following...
   *                           -Changed column headers in images.csv to match row headers in bundleout.txt
   *                            (instead of at2, bt, c to indicate coefficients, now using t2, t1, t0, etc)
   *                           -Added output of CKDEGREE, CKSOLVEDEGREE, SPKDEGREE, SPKSOLVEDEGREE to
   *                            bundleout.txt header when CAMSOLVE=ALL and/or SPSOLVE=ALL
   *                           -Fixed typo under SPACECRAFT OPTIONS; what should have said
   *                            "SPSOLVE: All POLYNOMIAL COEFFICIENTS" was
   *                            "CAMSOLVE: All POLYNOMIAL COEFFICIENTS"
   *                           -modified output of image EO in bundleout.txt for images solved with
   *                            observation mode; previously one entry per observation was written,
   *                            now all images in the observation are written separately. 
   */
  class BundleSolutionInfo : public QObject {
    Q_OBJECT
    public:
      BundleSolutionInfo(BundleSettingsQsp inputSettings,
                    FileName controlNetworkFileName, 
                    BundleResults outputStatistics, 
                    QObject *parent = 0);
      BundleSolutionInfo(Project *project, 
                    XmlStackedHandlerReader *xmlReader, 
                    QObject *parent = 0);  //TODO does xml stuff need project???
      BundleSolutionInfo(FileName bundleSolutionInfoFile);
      BundleSolutionInfo(const BundleSolutionInfo &src);
      ~BundleSolutionInfo();
      BundleSolutionInfo &operator=(const BundleSolutionInfo &src);

      void setOutputStatistics(BundleResults statisticsResults);
      void setRunTime(QString runTime);

      QString id() const;
      QString controlNetworkFileName() const;
      BundleSettingsQsp bundleSettings();
      BundleResults bundleResults();
      QString runTime() const;


      bool outputImagesCSVHeader(std::ofstream &fpOut);     
      bool outputHeader(std::ofstream &fpOut);
      bool outputText();
      bool outputImagesCSV();
      bool outputPointsCSV();
      bool outputResiduals();

      PvlObject pvlObject(QString resultsName = "BundleSolutionInfo",
                          QString settingsName = "InputSettings",
                          QString statisticsName = "StatisticsResults");
       
      //TODO does xml stuff need project and newRoot???
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;  
      
      //TODO does xml stuff need project???
      void save(QXmlStreamWriter &stream, const Project *project) const;  

      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);

      void writeH5File(FileName outputFileName) const;
      void readH5File(FileName outputFileName) const;

      void createH5File(FileName outputFileName) const;
      void openH5File(FileName outputFileName);
//      BundleSolutionInfo(FileName bundleSolutionInfoFile);
      
      public slots:
      void updateFileName(Project *);

    private:
      /**
       * This class is used to read an images.xml file into an image list
       * 
       * @see QXmlDefaultHandler documentation
       * @author 2014-07-21 Ken Edmundson
       *
       * @internal
       *   @history 2016-06-13 Makayla Shepherd - Added updateFileName() and updated documentation.
       *                           Fixes #2298.
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          //TODO does xml stuff need project???
          XmlHandler(BundleSolutionInfo *bundleSolutionInfo, Project *project);  
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          BundleSolutionInfo *m_xmlHandlerBundleSolutionInfo; //!< The bundleSolutionInfo object
          Project *m_xmlHandlerProject;  //TODO does xml stuff need project???
          QString m_xmlHandlerCharacters; //!< List of characters that have been handled
          QList<ImageList *> *m_xmlHandlerImages; //!< List of pointers to images
          BundleSettingsQsp m_xmlHandlerBundleSettings; //!< Settings used to run the bundle adjust
          BundleResults *m_xmlHandlerBundleResults; //!< Results from the bundle adjust
      };

    private:
      BundleSolutionInfo();

      //! A unique ID for this BundleSolutionInfo object (useful for others to reference this
      //! object when saving to disk).
      QUuid              *m_id;
      QString             m_runTime; //!< The run time of the bundle adjust
      FileName           *m_controlNetworkFileName; //!< The name of the control network
      BundleSettingsQsp   m_settings; //!< The settings from the bundle adjust
      BundleResults      *m_statisticsResults; //!< The results of the bundle adjust
      QList<ImageList *> *m_images; //!< The list of images that were adjusted
  }; // end BundleSolutionInfo class

  // operators to read/write BundleSolutionInfo to/from binary data
  QDataStream &operator<<(QDataStream &stream, const BundleSolutionInfo &bundleSolutionInfo);
  QDataStream &operator>>(QDataStream &stream, BundleSolutionInfo &bundleSolutionInfo);

  void setStringAttribute(int locationId, QString locationName, 
                          QString attributeName, QString attributeValue);
  QString getStringAttribute(int locationId, QString locationName, QString attributeName);
}; // end namespace Isis

Q_DECLARE_METATYPE(Isis::BundleSolutionInfo *);

#endif // BundleSolutionInfo_h
