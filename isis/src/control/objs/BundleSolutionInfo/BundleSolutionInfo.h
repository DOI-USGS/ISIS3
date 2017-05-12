#ifndef BundleSolutionInfo_h
#define BundleSolutionInfo_h

/**
 * @file
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
   *   @history 2016-12-01 Ian Humphrey - Modified an sprintf() call in outputImagesCSV() to
   *                           prevent a -Wformat-security warning from occurring.
   *   @history 2016-12-08 Ian Humphrey - Modified outputImagesCSVHeader() to treat TWIST the same
   *                           as the other angles when determining how many headers to create.
   *                           Fixes #4557.
   *   @history 2017-04-24 Ian Humphrey - Removed pvlObject(). Fixes #4797.
   *   @history 2017-05-01 Makayla Shepherd - Added imageList() to track and return the images used
   *                           in the bundle adjustment. These images will be displayed on the
   *                           project tree under results/bundle/<runtime> and will keep the same
   *                           structure as the input on the project tree. Fixes #4818.
   *   @history 2017-05-02 J Bonn - Fixed XML serialzation and code cleanup.  Fixes #4835.
   *   @history 2017-05-02 Tracie Sucharski - Moved XMLHandler code to bottom of file for
   *                           consistency;  all other classes have the XmlHandler at end of file.
   *                           Fixes #4822.
   *   @history 2017-05-04 Ian Humphrey & Makayla Shepherd - Modified save() to write the bundle
   *                           solution info images to the correct directory in the project on disk.
   *                           Fixes #4804, #4837.
   */
  class BundleSolutionInfo : public QObject {
    Q_OBJECT
    public:
      BundleSolutionInfo(BundleSettingsQsp inputSettings,
                    FileName controlNetworkFileName,
                    BundleResults outputStatistics,
                    QList<ImageList *> imgList,
                    QObject *parent = 0);
      BundleSolutionInfo(Project *project,
                    XmlStackedHandlerReader *xmlReader,
                    QObject *parent = 0);  //TODO does xml stuff need project???
      BundleSolutionInfo(const BundleSolutionInfo &src);
      ~BundleSolutionInfo();
      BundleSolutionInfo &operator=(const BundleSolutionInfo &src);

      QString savedImagesFilename();
      QString savedPointsFilename();
      QString savedResidualsFilename();

      void setOutputStatistics(BundleResults statisticsResults);
      void setRunTime(QString runTime);

      QString id() const;
      QString controlNetworkFileName() const;
      BundleSettingsQsp bundleSettings();
      BundleResults bundleResults();
      QList<ImageList *> imageList();
      QString runTime() const;


      bool outputImagesCSVHeader(std::ofstream &fpOut);
      bool outputHeader(std::ofstream &fpOut);
      bool outputText();
      bool outputImagesCSV();
      bool outputPointsCSV();
      bool outputResiduals();

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

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

      // In theory the path in the BundlesSettings can change while running.  So we save the
      // filenames actually used when the most recent save of the file was done.
      QString m_csvSavedImagesFilename;
      QString m_csvSavedPointsFilename;
      QString m_csvSavedResidualsFilename;

  }; // end BundleSolutionInfo class


  void setStringAttribute(int locationId, QString locationName,
                          QString attributeName, QString attributeValue);
  QString getStringAttribute(int locationId, QString locationName, QString attributeName);
}; // end namespace Isis

Q_DECLARE_METATYPE(Isis::BundleSolutionInfo *);

#endif // BundleSolutionInfo_h
