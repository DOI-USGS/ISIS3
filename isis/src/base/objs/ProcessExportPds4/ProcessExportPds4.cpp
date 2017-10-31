/**
 *   Unless noted otherwise, the portions of Isis written by the
 *   USGS are public domain. See individual third-party library
 *   and package descriptions for intellectual property
 *   information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "ProcessExportPds4.h"

#include <cmath>
#include <iostream>
#include <sstream>

#include <QDomDocument>
#include <QString>

#include "FileName.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "PvlToXmlTranslationManager.h"

using namespace std;

namespace Isis {


  /**
   * Default Constructor - Set to default the data members
   *
   */
  ProcessExportPds4::ProcessExportPds4() {
    qSetGlobalQHashSeed(1031); // hash seed to force consistent output

    m_domDoc = new QDomDocument("");

    QString xmlModel;
    xmlModel += "href=\"http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1800.sch\" ";
    xmlModel += "schemetypens=\"http://purl.oclc.org/dsdl/schematron\"";
    QDomProcessingInstruction header =
        m_domDoc->createProcessingInstruction("xml-model", xmlModel);
    m_domDoc->appendChild(header);
  }


  /**
   * Destructor
   *
   */
  ProcessExportPds4::~ProcessExportPds4() {
    delete m_domDoc;
    m_domDoc = NULL;
  }


  /**
   * Create a standard PDS4 image label from the input cube.
   * 
   * @return @b QDomDocument The output PDS4 label.
   */
  QDomDocument &ProcessExportPds4::StandardPds4Label() {
    if (InputCubes.size() == 0) {
      QString msg("Must set an input cube before creating a PDS4 label.");
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    else {
      if (m_domDoc->documentElement().isNull()) {
        QDomElement root = m_domDoc->createElement("Product_Observational");
        root.setAttribute("xmlns", "http://pds.nasa.gov/pds4/pds/v1");
        root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        root.setAttribute("xsi:schemaLocation",
                          "http://pds.nasa.gov/pds4/pds/v1 http://pds.nasa.gov/pds4/pds/v1");
        m_domDoc->appendChild(root);
      }

      CreateImageLabel();
      return *m_domDoc;
    }
  }


  /**
   * Create a standard PDS label for type IMAGE. The image label will be
   * stored internally in the class.
   */
  void ProcessExportPds4::CreateImageLabel() {

    //TODO Figure out what needs to be in FixedImageRoot and add a try catch around it if needed
    FixedImageRoot();

    try {
      StandardImageImage();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export standard image information.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
    try {
      StandardAllMapping();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export mapping group.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This method corrects the Image Root information in a PDS4 output label.
   * 
   * @note This method is not implemented currently and does nothing.
   */
  void ProcessExportPds4::FixedImageRoot() {
    //Don't know what needs to go in here yet....
    /**
     * ProcessExportPds has Pds version, record type, record bytes, file records, label records,
     * and md5 checksum
     *
     * Where does this information need to go in a Pds4 xml label?
     */
  }


  /**
   * Create and internalize a standard image output label from the input image.
   */
  void ProcessExportPds4::StandardImageImage() {

      try {
          Pvl *inputLabel = InputCubes[0]->label(); 
        FileName transfile;
        transfile = "$base/translations/pds4Export.trn";
        PvlToXmlTranslationManager xlator(*inputLabel, transfile.expanded());
        xlator.Auto(*m_domDoc);

        QDomElement rootElement = m_domDoc->documentElement();
        QDomElement fileAreaObservationalElement =
                        rootElement.firstChildElement("File_Area_Observational");

        if (!fileAreaObservationalElement.isNull()) {

          QDomElement array2DImageElement =
                          fileAreaObservationalElement.firstChildElement("Array_2D_Image");

          if (!array2DImageElement.isNull()) {
            QDomElement elementArrayElement = m_domDoc->createElement("Element_Array");
            array2DImageElement.appendChild(elementArrayElement);

            //The next three values are assuming that the cube is Real
            QDomElement dataTypeElement = m_domDoc->createElement("data_type");
            PvlToXmlTranslationManager::setElementValue(dataTypeElement, "IEEE754LSBSingle");
            elementArrayElement.appendChild(dataTypeElement);

            QDomElement scalingFactorElement = m_domDoc->createElement("scaling_factor");
            PvlToXmlTranslationManager::setElementValue(scalingFactorElement, "1.00");
            elementArrayElement.appendChild(scalingFactorElement);

            QDomElement offsetElement = m_domDoc->createElement("offset");
            PvlToXmlTranslationManager::setElementValue(offsetElement, "0.00");
            elementArrayElement.appendChild(offsetElement);
          }
        }

        
    //    QDomElement observationAreaElement = rootElement.firstChildElement("Observation_Area");
    //    if (observationAreaElement.isNull()) {
    //        observationAreaElement = m_domDoc->createElement("Observation_Area"); 
    //        rootElement.appendChild(observationAreaElement);
    //    }
    //
    //    QDomElement disciplineAreaElement = observationAreaElement.firstChildElement("Discipline_Area");
    //    if (disciplineAreaElement.isNull()) {
    //        disciplineAreaElement = m_domDoc->createElement("Discipline_Area"); 
    //        observationAreaElement.appendChild(disciplineAreaElement);
    //    }
    //
    //    QDomElement displaySettingsElement = m_domDoc->createElement("disp:Display_Settings");
    //    disciplineAreaElement.appendChild(displaySettingsElement);
    //
    //    QDomElement displayDirectionElement    = m_domDoc->createElement("disp:Display_Direction");
    //    displaySettingsElement.appendChild(displayDirectionElement);
    //
    //    QDomElement horizontalAxisElement = m_domDoc->createElement("disp:horizontal_display_axis");
    //    PvlToXmlTranslationManager::setElementValue(horizontalAxisElement, "Sample");
    //    displayDirectionElement.appendChild(horizontalAxisElement);
    //
    //    QDomElement horizontalDirectionElement = m_domDoc->createElement("disp:horizontal_display_direction");
    //    PvlToXmlTranslationManager::setElementValue(horizontalDirectionElement, "Left To Right");
    //    displayDirectionElement.appendChild(horizontalDirectionElement);
    //
    //    QDomElement verticalAxisElement = m_domDoc->createElement("disp:vertical_display_axis");
    //    PvlToXmlTranslationManager::setElementValue(verticalAxisElement, "Line");
    //    displayDirectionElement.appendChild(verticalAxisElement);
    //
    //    QDomElement verticalDirectionElement = m_domDoc->createElement("disp:vertical_display_direction");
    //    PvlToXmlTranslationManager::setElementValue(verticalDirectionElement, "Top To Bottom");
    //    displayDirectionElement.appendChild(verticalDirectionElement);
    //
      } 
      catch (IException &e) {
          throw IException(e, IException::Programmer, "Unable to translate image and display labels.", _FILEINFO_);
      }
  }


  /**
   * Write the XML label to the supplied stream.
   *
   * @param[out] os file stream to which the XML label will be written.
   */
  void ProcessExportPds4::OutputLabel(std::ofstream &os) {
    os << m_domDoc->toString() << endl;
  }


  /**
   * This method fills the image data of the PDS4 file using the parent class
   * ProcessExport::StartProcess.
   *
   * @param[out] fout Output file stream to be filled with the PDS4 data.
   *
   */
  void ProcessExportPds4::StartProcess(std::ofstream &fout) {
    ProcessExport::StartProcess(fout);
  }


  /**
   * Return the internalized PDS4 label. If no label is internalized yet, an
   * empty label will be returned.
   * 
   * @return @b QDomDocument The PDS4 Xml label
   */
  QDomDocument &ProcessExportPds4::GetLabel() {
    if (m_domDoc->documentElement().isNull()) {
      QDomElement root = m_domDoc->createElement("Product_Observational");
      root.setAttribute("xmlns", "http://pds.nasa.gov/pds4/pds/v1");
      root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
      root.setAttribute("xsi:schemaLocation",
                        "http://pds.nasa.gov/pds4/pds/v1 http://pds.nasa.gov/pds4/pds/v1");
      m_domDoc->appendChild(root);
    }
    return *m_domDoc;
  }
  
  
  /**
   * This method write out the labels and image data to the specified output file.
   * Creates an IMG and XML file.
   *
   * @param outFile QString of the name of the output file. Will create an XML 
   *        and an IMG file with the output file name.
   *
   */
  void ProcessExportPds4::WritePds4(QString outFile) {
    
    FileName outputFile(outFile);

    QString path(FileName(outFile).originalPath());
    QString name(FileName(outFile).baseName());
    QString labelName = path + "/" + name + ".xml";
    ofstream oLabel(labelName.toLatin1().data());
    
    OutputLabel(oLabel);
    oLabel.close();
    
    ofstream oCube(outputFile.expanded().toLatin1().data());

    StartProcess(oCube);
    oCube.close();
    EndProcess();
  }


  /**
   * Create the standard keywords for the IMAGE_MAP_PROJECTION group in a PDS
   * label
   *
   *
   * @throws IException::User "Unable to export projection [" + projName + "] to PDS4 product. " + 
                              "This projection is not supported in ISIS3."
   */
  void ProcessExportPds4::StandardAllMapping() {
    // Get the input Isis cube label and find the Mapping group if it has one
    Pvl *inputLabel = InputCubes[0]->label();
    if(inputLabel->hasObject("IsisCube") &&
        !(inputLabel->findObject("IsisCube").hasGroup("Mapping"))) return;
    PvlGroup &inputMapping = inputLabel->findGroup("Mapping", Pvl::Traverse);

    // Add carto schema processing instructions and xmlns

    // Add xml-model
    QString xmlModel;
    xmlModel += "href=\"https://pds.nasa.gov/pds4/cart/v1/PDS4_CART_1700.sch\" ";
    xmlModel += "schemetypens=\"http://purl.oclc.org/dsdl/schematron\"";
    QDomProcessingInstruction cartHeader =
        m_domDoc->createProcessingInstruction("xml-model", xmlModel);
    m_domDoc->insertAfter(cartHeader, m_domDoc->firstChild());

    // Translate the projection specific keywords for a PDS IMAGE_MAP_PROJECTION
    Projection *proj = ProjectionFactory::Create(*inputLabel); 
    QString projName = proj->Name();
    try {
      PvlToXmlTranslationManager xlatorSpecProj(*inputLabel, 
                                                "$base/translations/pds4Export" + projName + ".trn");
      xlatorSpecProj.Auto(*m_domDoc);
    } 
    catch (IException &e) {
      QString msg = "Unable to export projection [" + projName + "] to PDS4 product. " + 
                     "This projection is not supported in ISIS3.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }

    // Translate the target name
    PvlToXmlTranslationManager xlatorTarget(*inputLabel,
                                            "$base/translations/pdsExportTarget.trn");
    xlatorTarget.Auto(*m_domDoc);

    // convert units.
    QStringList xmlPath;
    xmlPath << "Product_Observational" 
            << "Observation_Area" 
            << "Discipline_Area" 
            << "cart:Cartography" 
            << "cart:Map_Projection" 
            << "cart:Spatial_Reference_Information" 
            << "cart:Horizontal_Coordinate_System_Definition"
            << "cart:Geodetic_Model";

    QDomElement baseElement = m_domDoc->documentElement();
    QDomElement geodeticModelElement = getElement(xmlPath, baseElement);
    QDomElement semiMajorRadElement = geodeticModelElement.firstChildElement("cart:semi_major_radius");
    if (!semiMajorRadElement.isNull()) {

      QString units = semiMajorRadElement.attribute("unit");
      if( units.compare("km", Qt::CaseInsensitive) != 0 && units.compare("kilometers", Qt::CaseInsensitive) != 0) { 

        //if no units, assume in meters
        double dValue = toDouble(semiMajorRadElement.text());
        dValue /= 1000.0;
        PvlToXmlTranslationManager::resetElementValue(semiMajorRadElement, toString(dValue), "km");
      }
    }

    QDomElement semiMinorRadElement = geodeticModelElement.firstChildElement("cart:semi_minor_radius");
    if (!semiMinorRadElement.isNull()) {

      QString units = semiMinorRadElement.attribute("unit");
      if( units.compare("km", Qt::CaseInsensitive) != 0 && units.compare("kilometers", Qt::CaseInsensitive) != 0) { 
        // If no units, assume in meters
        double dValue = toDouble(semiMinorRadElement.text());
        dValue /= 1000.0;
        PvlToXmlTranslationManager::resetElementValue(semiMinorRadElement, toString(dValue), "km");
      }
    }

    QDomElement polarRadElement = geodeticModelElement.firstChildElement("cart:polar_radius");
    if (!polarRadElement.isNull()) {
      QString units = polarRadElement.attribute("unit");
      if( units.compare("km", Qt::CaseInsensitive) != 0 && units.compare("kilometers", Qt::CaseInsensitive) != 0) { 
        // If no units, assume in meters
        double dValue = toDouble(polarRadElement.text());
        dValue /= 1000.0;
        PvlToXmlTranslationManager::resetElementValue(polarRadElement, toString(dValue), "km");
      }
    }
#if 0
    xmlPath[7] = "cart:Planar";
    xmlPath << "cart:Planar_Coordinate_Information" << "cart:Coordinate_Representation";

    QDomElement coordRepElement = getElement(xmlPath, baseElement);
    QDomElement pixelScaleXElement = coordRepElement.firstChildElement("cart:pixel_scale_x");
    if (!pixelScaleXElement.isNull()) {
      QString units = pixelScaleXElement.attribute("unit");


      //if no units, assume in meters/pixel
      if( (unit.toUpper() == "METERS/PIX") || (unit.toUpper() == "METERS/PIXEL") || (unit == "") ) {
        if(m_exportResolution == Kilometer) {
          double dValue = (double)mapScale;
          dValue /= 1000.0;
          mapScale.setValue(toString(dValue), "KM/PIXEL");
        }
        else {
          mapScale.setValue(toString((double)mapScale), "METERS/PIXEL");
        }
      }





      if( units.compare("km/pixel", Qt::CaseInsensitive) != 0 
          && units.compare("km/p", Qt::CaseInsensitive) != 0
          && units.compare("kilometers/pixel", Qt::CaseInsensitive) != 0 
          && units.compare("kpp", Qt::CaseInsensitive) != 0) { 
        //if no units, assume in meters
        double dValue = toDouble(polarRadElement.text());
        dValue /= 1000.0;
        PvlToXmlTranslationManager::resetElementValue(pixelScaleXElement, toString(dValue), "km");
      }
    }
    QDomElement pixelScaleYElement = coordRepElement.firstChildElement("cart:pixel_scale_y");
    if (!pixelScaleYElement.isNull()) {}
    QDomElement pixelResXElement   = coordRepElement.firstChildElement("cart:pixel_resolution_x");
    if (!pixelResXElement.isNull()) {}
    QDomElement pixelResYElement   = coordRepElement.firstChildElement("cart:pixel_resolution_y");
    if (!pixelResYElement.isNull()) {}
#endif

    // Add the EASTERNMOST AND WESTERNMOST LONGITUDE keywords
    PvlKeyword &isisLonDir = inputMapping.findKeyword("LongitudeDirection");
    QString lonDir = isisLonDir[0];
    lonDir = lonDir.toUpper();
    if (inputMapping.hasKeyword("MaximumLongitude") && inputMapping.hasKeyword("MinimumLongitude")) {
      double maxLon = inputMapping.findKeyword("MaximumLongitude");
      double minLon = inputMapping.findKeyword("MinimumLongitude");
      xmlPath.clear();
      xmlPath << "Product_Observational" 
              << "Observation_Area" 
              << "Discipline_Area" 
              << "cart:Cartography" 
              << "cart:Map_Projection" 
              << "cart:Spatial_Domain"
              << "cart:Bounding_Coordinates";
      QDomElement boundingCoordElement = getElement(xmlPath, baseElement);
      QDomElement eastElement = boundingCoordElement.firstChildElement("cart:east_bounding_coordinate");
      QDomElement westElement = boundingCoordElement.firstChildElement("cart:west_bounding_coordinate");

      if(lonDir == "POSITIVEEAST") {
        // west min, east max
        PvlToXmlTranslationManager::resetElementValue(eastElement, toString(maxLon), "deg");
        PvlToXmlTranslationManager::resetElementValue(westElement, toString(minLon), "deg");
      }
      else {
        // Keep east=min and west=max (as should be in translation file), but add units
        PvlToXmlTranslationManager::resetElementValue(eastElement, toString(minLon), "deg");
        PvlToXmlTranslationManager::resetElementValue(westElement, toString(maxLon), "deg");
      }
    }


  }


 /**
  * Convenience method to get an element given a path and its parent. 
  * 
  * @param xmlPath xml path to the element to retrieve
  * @param parent parent QDomElement
  * 
  * @return QDomElement 
  */
  QDomElement ProcessExportPds4::getElement(QStringList xmlPath, QDomElement parent) {
    QDomElement baseElement = parent;
    if (baseElement.isNull()) {
      baseElement = m_domDoc->documentElement();
    }
    if (baseElement.isNull()) { // should we error
      // return;
    }
    QString parentName = xmlPath[0];
    //if (parentName != baseElement) {
    //  // throw error - improper use of this method
    //}
    for (int i = 1; i < xmlPath.size(); i++) {
      QString elementName = xmlPath[i];
      QDomElement nextElement = baseElement.firstChildElement(elementName);
      if (nextElement.isNull()) {
         //return // throw error
      }
      baseElement = nextElement;
    }
    return baseElement;
  }


} // End of Isis namespace
