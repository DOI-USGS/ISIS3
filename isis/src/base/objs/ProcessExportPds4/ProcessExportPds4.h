#ifndef ProcessExportPds4_h
#define ProcessExportPds4_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ProcessExport.h"
#include <vector>
#include <QString>
#include <QDomDocument>

namespace Isis {

  /**
   * @brief Process class for exporting cubes to PDS4 standards
   *
   * This class extends the ProcessExport class to allow the user
   * to export cubes to PDS4 format.
   *
   * @author 2017-05-25 Marjorie Hahn and Makayla Shepherd
   * @internal
   *   @history 2017-05-31 Ian Humphrey - Added check in StandardPds4Label to thrown an
   *                           exception if there is no input cube set.
   *   @history 2017-06-01 Ian Humphrey - Added XML declaration and updated attributes for
   *                           Product_Observational tag.
   *   @history 2017-06-02 Marjorie Hahn - Added global hash seed to force a consistent
   *                           output (XML attribute order).
   *   @history 2017-06-02 Adam Paquette - Updated call to translation file to use a translation file
   *                           in the isis data area.
   *   @history 2017-06-02 Makayla Shepherd - Added CreateImageLabel, FixedImageRoot, and
   *                           StandardImageImage to add hardcoded values to the xml label.
   *   @history 2017-06-04 Adam Paquette - Added GetLabel function and updated StandardPds4Label.
   *   @history 2017-06-08 Marjorie Hahn - Added WritePds4 method to write out the 
   *                           .img and .xml Pds4 data.
   *   @history 2017-09-26 Jesse Mapel - Improved test coverage and documentation. Fixes #5167.
   *   @history 2017-10-18 Jeannie Backer & Makayla Shepherd - Added convenience method getElement
   *                           and StandardAllMapping method to translate mapping information.
   *                           See #5202.
   *   @history 2017-10-31 Jeannie Backer - Added standardInstrument() and displaySettings()
   *                           translations.
   *   @history 2017-11-06 Kristin Berry - Added standardBandBin() 
   *   @history 2017-11-07 Jeannie Backer - Added code to search for target in the Mapping group
   *                           if no instrument group is found. Added Identification Area
   *                           tranlations. Added sanity checks to getElement().
   *   @history 2017-11-07 Jeannie Backer - Added code to write data file info into label file.
   *                           Added code to translate time format and add nil tag if empty.
   *   @history 2017-11-15 Jesse Mapel - Added translateUnits method to convert units to PDS4
   *                           standard format.
   *   @history 2017-11-16 Kristin Berry - Updated WritePds4 to put the File information first
   *                           in the output File_Area_Observational.
   *   @history 2017-11-17 Jesse Mapel - Removed empty FixedImageRoot method.
   *   @history 2017-11-20 Jeannie Backer - Updated StandardImageImage() to re-order the
   *                           Array_3D_Image values properly.
   *   @history 2017-11-21 Kristin Berry - Updated the constructor to add the xml version and 
   *                           encoding to the beginning of the XML file.
   *   @history 2018-02-05 Kristin Berry - Updated WritePds4 to remove the .xml and add a .img
   *                           if the user inputs something of the form filename.xml as the image
   *                           output name. 
   *   @history 2018-05-16 Christopher Combs - Fixed typo in xml namespaces and changed History 
   *                           attributes to elements. Matches pds validate tool specifations.
   *   @history 2018-06-12 Kristin Berry - Added schema associated with the img class when it is
   *                           used.
   *   @history 2019-03-01 Kristin Berry - Added ability to set version_id and title, added
   *                           Special_Constants to define ISIS special pixel values, fixed east/west
   *                           bounding coordinates swap bug. Fixes git issue #2635.
   *   @history 2019-06-15 Kristin Berry - Added a new addSchema() function for cases in which a
   *                           schematron file is not available and added setPixelDescription to
   *                           set a pixel description for the output image. 
   */

  class ProcessExportPds4: public Isis::ProcessExport {
    public:

      ProcessExportPds4();
      ~ProcessExportPds4();

      enum ImageType {
        StandardImage,
        BinSetSpectrum,
        UniformlySampledSpectrum
      };

      QDomDocument &StandardPds4Label();
      QDomDocument &SpectralPds4Label();
      void StandardAllMapping();

      void CreateImageLabel();
      void StandardImageImage();

      void OutputLabel(std::ofstream &os);

      // Include this using declaration to indicate that ProcessExportPds4
      // objects that call a StartProcess() method that has not been overridden
      // here should use the corresponding base class definitions
      using ProcessExport::StartProcess;
      void StartProcess(std::ofstream &fout);
      QDomDocument &GetLabel();
      void WritePds4(QString outFile);
      QDomElement getElement(QStringList xmlPath, QDomElement parent=QDomElement());
      void addHistory(QString description, QString date = "tbd", QString version = "1.0");
      void setLogicalId(QString lid);
      void setVersionId(QString versionId);
      void setTitle(QString title);
      void setSchemaLocation(QString schema);
      void setImageType(ImageType imageType);
      void setPixelDescription(QString description); 
      static void translateUnits(QDomDocument &label,
                      QString transMapFile = "$ISISROOT/appdata/translations/pds4ExportUnits.pvl");
      void reorder();
      void addSchema(QString sch, QString xsd, QString xmlns, QString xmlnsURI);
      void addSchema(QString xsd, QString xmlns, QString xmlnsURI);

    protected:
      void identificationArea();
      void standardInstrument();
      void standardBandBin(); 
      void displaySettings();
      void fileAreaObservational();
      QString PDS4PixelType(PixelType pixelType, ByteOrder endianType);
      static QMap<QString, QString> createUnitMap(Pvl configPvl);
      static void translateChildUnits(QDomElement parent, QMap<QString, QString> transMap);
      void translateBandBinImage(Pvl &inputLabel);
      void translateBandBinSpectrumUniform(Pvl &inputLabel);
      void translateBandBinSpectrumBinSet(Pvl &inputLabel);

      QDomDocument *m_domDoc;               //!< XML label.
      QString m_schemaLocation;             //!< QString with all schema locations required.
      QString m_lid;                        //!< QString with specified logical identifier.
      QString m_versionId;                  //!< QString with specified version id.
      QString m_title;                      //!< QString with specified title. 
      ImageType m_imageType;                //!< Type of image data to be written.
      QString m_pixelDescription;           //!< Description of pixel values.

  };
}

#endif
