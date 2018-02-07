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
#include <QMap>
#include <QString>

#include "FileName.h"
#include "IException.h"
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

    // base xml file 
    // <?xml version="1.0" encoding="UTF-8"?>
    QString xmlVersion = "version=\"1.0\" encoding=\"utf-8\"";
    QDomProcessingInstruction xmlHeader =
        m_domDoc->createProcessingInstruction("xml", xmlVersion);
    m_domDoc->appendChild(xmlHeader);

    // base pds4 schema location
    m_schemaLocation = "http://pds.nasa.gov/pds4/pds/v1 http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1800.xsd"; 

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
      translateUnits(*m_domDoc);
      return *m_domDoc;
    }
  }


  /**
   * Create a standard PDS label for type IMAGE. The image label will be
   * stored internally in the class.
   */
  void ProcessExportPds4::CreateImageLabel() {

    try {
      identificationArea();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export identification information.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
    try {
      standardInstrument();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export instrument information.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
    try {
      displaySettings();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export display settings.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
// Temporarily removed spectral processing because it needs further work. 
// 
//    try {
//      standardBandBin();
//    } 
//    catch (IException &e) {
//      QString msg = "Unable to translate and export spectral information.";
//      throw IException(e, IException::Programmer, msg, _FILEINFO_);
//    }

    try { 
      // <Discipline_Area>
      // display settings, and cartography handled in this method:
      StandardAllMapping();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export mapping group.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
    try {
      // file observation area
      StandardImageImage();
    }
    catch (IException &e) {
      QString msg = "Unable to translate and export standard image information.";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This method translates the information from the ISIS 
   * Instrument group to the PDS4 labels. 
   */
  void ProcessExportPds4::standardInstrument() {
    Pvl *inputLabel = InputCubes[0]->label(); 
    FileName transfile;

    // Translate the Instrument group
    transfile = "$base/translations/pds4ExportInstrument.trn";
    PvlToXmlTranslationManager instXlator(*inputLabel, transfile.expanded());
    instXlator.Auto(*m_domDoc);

    // If instrument and spacecraft values were translated, create the combined name
    QDomElement obsAreaNode = m_domDoc->documentElement().firstChildElement("Observation_Area");

    if ( !obsAreaNode.isNull() ) {

      // fix start/stop times, if needed
      QDomElement timeNode = obsAreaNode.firstChildElement("Time_Coordinates");
      if (!timeNode.isNull()) {
        QDomElement startTime = timeNode.firstChildElement("start_date_time");
        if (startTime.text() == "") {
          startTime.setAttribute("xsi:nil", "true");
        }
        else {
          QString timeValue = startTime.text();
          PvlToXmlTranslationManager::resetElementValue(startTime, timeValue + "Z");
        }
        QDomElement stopTime  = timeNode.firstChildElement("stop_date_time"); 
        if (stopTime.text() == "") {
          stopTime.setAttribute("xsi:nil", "true");
        }
        else {
          QString timeValue = stopTime.text();
          PvlToXmlTranslationManager::resetElementValue(stopTime, timeValue + "Z");
        }

      }

      QDomElement obsSysNode = obsAreaNode.firstChildElement("Observing_System");
      if ( !obsSysNode.isNull() ) {
        QString instrumentName;
        QString spacecraftName;
        QDomElement obsSysCompNode = obsSysNode.firstChildElement("Observing_System_Component");
        while ( !obsSysCompNode.isNull()) {
          QDomElement compTypeNode = obsSysCompNode.firstChildElement("type");
          if ( compTypeNode.text().compare("Spacecraft") == 0 ) {
            QString componentName = obsSysCompNode.firstChildElement("name").text();
            if (QString::compare(componentName, "TBD", Qt::CaseInsensitive) != 0) {
              spacecraftName = componentName; 
            }
          }
          else if ( compTypeNode.text().compare("Instrument") == 0 ) {
            QString componentName = obsSysCompNode.firstChildElement("name").text();
            if (QString::compare(componentName, "TBD", Qt::CaseInsensitive) != 0) {
              instrumentName = componentName;
            }
          }
          obsSysCompNode = obsSysCompNode.nextSiblingElement("Observing_System_Component");
        }
        QDomElement combinedNode = m_domDoc->createElement("name");
        QString combinedValue = "TBD";
        if ( !instrumentName.isEmpty() && !spacecraftName.isEmpty() ) {
          combinedValue = spacecraftName + " " + instrumentName;
        }
        combinedNode.appendChild( m_domDoc->createTextNode(combinedValue) );
        obsSysNode.insertBefore( combinedNode, obsSysNode.firstChild() );
      }
    }

    // Translate the Target name
    try {
      transfile = "$base/translations/pds4ExportTargetFromInstrument.trn"; 
      PvlToXmlTranslationManager targXlator(*inputLabel, transfile.expanded());
      targXlator.Auto(*m_domDoc);
    } 
    catch (IException &e1) {
      try {
        transfile = "$base/translations/pds4ExportTargetFromMapping.trn"; 
        PvlToXmlTranslationManager targXlator(*inputLabel, transfile.expanded());
        targXlator.Auto(*m_domDoc);
      }
      catch (IException &e2) {
        IException finalError(IException::Unknown, "Unable to find a target in input cube.", _FILEINFO_);
        finalError.append(e1);
        finalError.append(e2);
        throw finalError;
      }
    }
  }


  /**
   * This method writes the identification information to the PDS4
   * labels. 
   */
  void ProcessExportPds4::identificationArea() {
    Pvl *inputLabel = InputCubes[0]->label(); 
    FileName transfile;
    transfile = "$base/translations/pds4ExportIdentificationArea.trn";
    PvlToXmlTranslationManager xlator(*inputLabel, transfile.expanded());
    xlator.Auto(*m_domDoc);
  }

  
  /**
   * This method writes the display direction information to 
   * the PDS4 labels. 
   */
  void ProcessExportPds4::displaySettings() {

    Pvl *inputLabel = InputCubes[0]->label(); 
    FileName transfile;
    transfile = "$base/translations/pds4ExportDisplaySettings.trn";
    PvlToXmlTranslationManager xlator(*inputLabel, transfile.expanded());
    xlator.Auto(*m_domDoc);

    // Add header info
    addSchema("PDS4_DISP_1700.sch", 
              "PDS4_DISP_1700.xsd",
              "xmlns:disp", 
              "http://pds.nasa.gov/pds4/disp/v1"); 
  }

  
 /**
  * Export bandbin group to sp:Spectral Characteristics 
  * 
  */
  void ProcessExportPds4::standardBandBin() {
    // Spectra
    // Get the input Isis cube label and find the BandBin group if it has one
    Pvl *inputLabel = InputCubes[0]->label();
    if(inputLabel->hasObject("IsisCube") &&
        !(inputLabel->findObject("IsisCube").hasGroup("BandBin"))) return;

    FileName transfile;
    transfile = "$base/translations/pds4ExportBandBin.trn";
    PvlToXmlTranslationManager xlator(*inputLabel, transfile.expanded());
    xlator.Auto(*m_domDoc);

    // Add header info
    addSchema("PDS4_SP_1100.sch", 
              "PDS4_SP_1100.xsd",
              "xmlns:sp", 
              "http://pds.nasa.gov/pds4/sp/v1"); 
  }


  /**
   * Create and internalize a standard image output label from the input image. 
   * @todo determine whether to treat single band as 2d array
   */
  void ProcessExportPds4::StandardImageImage() {
    Pvl *inputLabel = InputCubes[0]->label(); 
    FileName transfile;

    transfile = "$base/translations/pds4ExportArray3DImage.trn"; 
    PvlToXmlTranslationManager xlator(*inputLabel, transfile.expanded());
    xlator.Auto(*m_domDoc);

    QDomElement rootElement = m_domDoc->documentElement();
    QDomElement fileAreaObservationalElement =
                    rootElement.firstChildElement("File_Area_Observational");

    // Calculate the core base/mult for the output cube
    double base = 0.0;
    double multiplier = 1.0;
    double outputMin, outputMax;

    double inputMin = (p_inputMinimum.size()) ? p_inputMinimum[0] : 0.0;
    double inputMax = (p_inputMaximum.size()) ? p_inputMaximum[0] : 0.0;

    for(unsigned int i = 0; i < p_inputMinimum.size(); i ++) {
      inputMin = std::min(inputMin, p_inputMinimum[i]);
      inputMax = std::max(inputMax, p_inputMaximum[i]);
    }

    outputMin = p_outputMinimum;
    outputMax = p_outputMaximum;

    if(p_inputMinimum.size() && ( p_pixelType == Isis::UnsignedByte ||
                                  p_pixelType == Isis::SignedWord   ||
                                  p_pixelType == Isis::UnsignedWord ) ) {
      multiplier = (inputMax - inputMin) / (outputMax - outputMin);
      base = inputMin - multiplier * outputMin;
    }

    if (!fileAreaObservationalElement.isNull()) {
      QDomElement array3DImageElement =
                      fileAreaObservationalElement.firstChildElement("Array_3D_Image");

      if (!array3DImageElement.isNull()) {

        // reorder axis elements. 
        // Translation order:  elements, axis_name, sequence_number
        // Correct order:      axis_name, elements, sequence_number
        QDomElement axisArrayElement = array3DImageElement.firstChildElement("Axis_Array");
        while( !axisArrayElement.isNull() ) {
          QDomElement axisNameElement = axisArrayElement.firstChildElement("axis_name");
          axisArrayElement.insertBefore(axisNameElement, 
                                        axisArrayElement.firstChildElement("elements"));
          axisArrayElement = axisArrayElement.nextSiblingElement("Axis_Array");
        }

        QDomElement elementArrayElement = m_domDoc->createElement("Element_Array");
        array3DImageElement.insertBefore(elementArrayElement,
                                         array3DImageElement.firstChildElement("Axis_Array"));

        QDomElement dataTypeElement = m_domDoc->createElement("data_type");
        PvlToXmlTranslationManager::setElementValue(dataTypeElement,
                                                    PDS4PixelType(p_pixelType, p_endianType));
        elementArrayElement.appendChild(dataTypeElement);

        QDomElement scalingFactorElement = m_domDoc->createElement("scaling_factor");
        PvlToXmlTranslationManager::setElementValue(scalingFactorElement,
                                                    toString(multiplier));
        elementArrayElement.appendChild(scalingFactorElement);

        QDomElement offsetElement = m_domDoc->createElement("value_offset");
        PvlToXmlTranslationManager::setElementValue(offsetElement,
                                                    toString(base));
        elementArrayElement.appendChild(offsetElement);
      }
    }
  }


  /**
   * Adds necessary information to the xml header for a pds4 class. 
   * 
   * @param sch Schematron filename without path
   * @param xsd Schema filename without path
   * @param xmlns The xml namespace used 
   * @param xmlnsURI Full URL to the xml namespace URI. Also used as the location of the sch and xsd
   */
  void ProcessExportPds4::addSchema(QString sch, QString xsd, QString xmlns, QString xmlnsURI) {
    // Add xml-model
    QString xmlModel;
    xmlModel += "href=\"";
    xmlModel +=  xmlnsURI;
    xmlModel +=  "/";
    xmlModel +=  sch;
    xmlModel += "\" schemetypens=\"http://purl.oclc.org/dsdl/schematron\"";
    QDomProcessingInstruction header =
        m_domDoc->createProcessingInstruction("xml-model", xmlModel);
    m_domDoc->insertAfter(header, m_domDoc->firstChild());

    // Add xmlns
    QDomElement root = m_domDoc->documentElement();
    root.setAttribute(xmlns, xmlnsURI);

    // Add to xsi:schemaLocation
    m_schemaLocation += " "; 
    m_schemaLocation += xmlnsURI;
    m_schemaLocation += " ";
    m_schemaLocation += xmlnsURI;
    m_schemaLocation += "/";
    m_schemaLocation += xsd;
    root.setAttribute("xsi:schemaLocation", m_schemaLocation);
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

    QString path(outputFile.originalPath());
    QString name(outputFile.baseName());
    QString labelName = path + "/" + name + ".xml";

    QDomElement rootElement = m_domDoc->documentElement();
    QDomElement fileAreaObservationalElement =
                    rootElement.firstChildElement("File_Area_Observational");

    QDomElement fileElement = m_domDoc->createElement("File");
    fileAreaObservationalElement.insertBefore(fileElement, 
                                              fileAreaObservationalElement.firstChildElement());

    QDomElement fileNameElement = m_domDoc->createElement("file_name");
    PvlToXmlTranslationManager::setElementValue(fileNameElement, outputFile.name());
    fileElement.appendChild(fileNameElement);

//    QDomElement creationElement = m_domDoc->createElement("creation_date_time");
//    PvlToXmlTranslationManager::setElementValue(creationElement, );
//    fileElement.appendChild(creationElement);

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
    // Cartography
    // Get the input Isis cube label and find the Mapping group if it has one
    Pvl *inputLabel = InputCubes[0]->label();
    if(inputLabel->hasObject("IsisCube") &&
        !(inputLabel->findObject("IsisCube").hasGroup("Mapping"))) return;
    PvlGroup &inputMapping = inputLabel->findGroup("Mapping", Pvl::Traverse);

    addSchema("PDS4_CART_1700.sch", 
              "PDS4_CART_1700.xsd",
              "xmlns:cart", 
              "http://pds.nasa.gov/pds4/cart/v1"); 

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

      // translation files currently handles Positive West case where east = min, west = max
      // so if positive east, swap min/max
      if(QString::compare(lonDir, "Positive East", Qt::CaseInsensitive) == 0) {
        // west min, east max
        PvlToXmlTranslationManager::resetElementValue(eastElement, toString(maxLon), "deg");
        PvlToXmlTranslationManager::resetElementValue(westElement, toString(minLon), "deg");
      }
    }


  }


 /**
  * Convenience method to get an element given a path and its parent. 
  * 
  * @param xmlPath The XML path to the element to retrieve, 
  *                starting at the parent element. Note: The
  *                first element of this path must be the same as
  *                the parent element passed in, unless the
  *                parent element is NULL. If the parent element
  *                passed in is NULL, then we assume that the
  *                parent is the root and a full path has been
  *                given.
  * @param parent The parent QDomElement of the given path. Defaults to
  *               the root element of the document.
  * 
  * @return QDomElement 
  */
  QDomElement ProcessExportPds4::getElement(QStringList xmlPath, QDomElement parent) {
    QDomElement baseElement = parent;
    if (baseElement.isNull()) {
      baseElement = m_domDoc->documentElement();
    }
    if (baseElement.isNull()) { 
      QString msg = "Unable to get element from empty XML document."; 
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    QString parentName = xmlPath[0];
    if (parentName != baseElement.tagName()) {
      QString msg = "The tag name of the parent element passed in "
                    "must be the first value in the given XML path."; 
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    for (int i = 1; i < xmlPath.size(); i++) {
      QString elementName = xmlPath[i];
      QDomElement nextElement = baseElement.firstChildElement(elementName);
      baseElement = nextElement;
    }
    return baseElement;
  }


  /**
   * Helper function for converting ISIS pixel type and byte order to a PDS4 data_type value.
   * 
   * @param pixelType The ISIS pixel type of the data
   * @param endianType The byte order of the data
   * 
   * @return @b QString The PDS4 data_type value for the given pixel type and byte order.
   */
  QString ProcessExportPds4::PDS4PixelType(PixelType pixelType, ByteOrder endianType) {
    QString pds4Type("UNK");
    if(p_pixelType == Isis::UnsignedByte) {
      pds4Type = "UnsignedByte";
    }
    else if((p_pixelType == Isis::UnsignedWord) && (p_endianType == Isis::Msb)) {
      pds4Type = "UnsignedMSB2";
    }
    else if((p_pixelType == Isis::UnsignedWord) && (p_endianType == Isis::Lsb)) {
      pds4Type = "UnsignedLSB2";
    }
    else if((p_pixelType == Isis::SignedWord) && (p_endianType == Isis::Msb)) {
      pds4Type = "SignedMSB2";
    }
    else if((p_pixelType == Isis::SignedWord) && (p_endianType == Isis::Lsb)) {
      pds4Type = "SignedLSB2";
    }
    else if((p_pixelType == Isis::Real) && (p_endianType == Isis::Msb)) {
      pds4Type = "IEEE754MSBSingle";
    }
    else if((p_pixelType == Isis::Real) && (p_endianType == Isis::Lsb)) {
      pds4Type = "IEEE754LSBSingle";
    }
    else {
      QString msg = "Unsupported PDS pixel type or sample size";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    return pds4Type;
  }


  /**
   * Add a modification history instance by adding a Modification_Detail entry
   * to the Modification_History element. If there are no existing entries,
   * this will create a Modification_History element also.
   * 
   * @param description The description of the modification.
   * @param date The date of the modification. Expected format is "YYYY-MM-DD". Defaults to "tbd".
   * @param version The product version. Expected format is "m.n". Defaults to "tbd".
   */
  void ProcessExportPds4::addHistory(QString description, QString date, QString version) {
    // Check that at least the "Identification_Area" element exists.
    QDomElement identificationElement;
    QStringList identificationPath;
    identificationPath.append("Product_Observational");
    identificationPath.append("Identification_Area");
    try {
      identificationElement = getElement(identificationPath);
      if( identificationElement.isNull() ) {
        throw IException(IException::Unknown, "", _FILEINFO_);
      }
    }
    catch(IException &e) {
      QString msg = "Could not find Identification_Area element "
                    "to add modification history under.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Check if the "Modification_History" element exists yet.
    // If not, create one.
    QDomElement historyElement = identificationElement.firstChildElement("Modification_History");
    if ( historyElement.isNull() ) {
      historyElement = m_domDoc->createElement("Modification_History");
      identificationElement.insertAfter( historyElement,
                                         identificationElement.lastChildElement() );
    }

    // Create the "Modification_Detail" element and add it to the end of the
    // "Modification_History" element.
    QDomElement detailElement = m_domDoc->createElement("Modification_Detail");
    detailElement.setAttribute("description", description);
    detailElement.setAttribute("modification_date", date);
    detailElement.setAttribute("version_id", version);

    historyElement.insertAfter( detailElement,
                                historyElement.lastChildElement() );
  }


  /**
   * This function will go through an XML document and attempt to convert all
   * "units" attributes to the appropriate PDS4 units format.
   * 
   * This method uses a pvl config file to determine what the proper PDS4
   * format is and what potential input formats are. The file is converted to
   * a map which is then used to convert all of the input units. See
   * $base/translations/pds4ExportUnits.pvl for more information on this file.
   * 
   * This method is automatically called in StandardPds4Label(), but may need
   * to be called again if the label is changed afterwards.
   * 
   * @param[in,out] label A reference to the label that the units will be
   *                      translated in.
   * @param transMapFile The path to the config file that will be used to
   *                     determine unit translations.
   */
  void ProcessExportPds4::translateUnits(QDomDocument &label, QString transMapFile) {
    Pvl configPvl;
    try {
      configPvl.read(transMapFile);
    }
    catch(IException &e) {
      QString msg = "Failed to read unit translation config file [" + transMapFile + "].";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    QMap<QString, QString> transMap;
    try {
      transMap = createUnitMap(configPvl);
    }
    catch(IException &e) {
      QString msg = "Failed to load unit translation config file [" + transMapFile + "].";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }

    // Now that the map is filled, recursively search through the XML document
    // for units and translate them.
    try {
      translateChildUnits( label.documentElement(), transMap );
    }
    catch(IException &e) {
      QString msg = "Failed to translate units with config file [" + transMapFile + "].";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }


  /**
   * Helper function for creating the unit translation map from a PVL object.
   * 
   * @param configPvl The config PVL that defines the map.
   * 
   * @return @b QMap<QString,QString> The map that converts lower case ISIS
   *                                  units to PDS4 units.
   * 
   * @see ProcessExportPds4::translateUnits
   */
  QMap<QString, QString> ProcessExportPds4::createUnitMap(Pvl configPvl) {
    QMap<QString, QString> transMap;
    for (int i = 0; i < configPvl.objects(); i++) {
      PvlObject unitObject = configPvl.object(i);
      for (int j = 0; j < unitObject.groups(); j++) {
        PvlGroup unitGroup = unitObject.group(j);
        if (!unitGroup.hasKeyword("PDS4_Unit")) {
          QString msg = "No PDS4 standard specified for for [" + unitGroup.name() + "]";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
        PvlKeyword pds4Key = unitGroup["PDS4_Unit"];
        // Add the PDS4 format for when the format is already correct.
        // This also handles case issues such as KM instead of km.
        transMap.insert(pds4Key[0].toLower(), pds4Key[0]);

        // If there are ISIS versions with different formats then add those.
        if (unitGroup.hasKeyword("ISIS_Units")) {
          PvlKeyword isisKey = unitGroup["ISIS_Units"];
          for (int k = 0; k < isisKey.size() ; k++) {
            transMap.insert(isisKey[k].toLower(), pds4Key[0]);
          }
        }
      }
    }

    return transMap;
  }


  /**
   * Recursive method that will translate the "unit" attribute of any child
   * elements of a given element. Returns void if the given element has no
   * children.
   * 
   * @param parent The element whose children's units will be translated. This
   *               method will be recursively called on all child elements.
   * @param transMap The translation map with lowercase ISIS units as keys and
   *                 PDS4 units as values.
   * 
   * @see ProcessExportPds4::translateUnits
   */
  void ProcessExportPds4::translateChildUnits(QDomElement parent, QMap<QString, QString> transMap) {
    QDomElement childElement = parent.firstChildElement();

    while( !childElement.isNull() ) {
      if ( childElement.hasAttribute("unit") ) {
        QString originalUnit = childElement.attribute("unit");
        if ( transMap.contains( originalUnit.toLower() ) ) {
          childElement.setAttribute("unit", transMap.value( originalUnit.toLower() ) );
        }
        else {
          QString msg = "Could not translate unit [" + originalUnit + "] to PDS4 format.";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }
      translateChildUnits(childElement, transMap);
      childElement = childElement.nextSiblingElement();
    }

    // Base case: If there are no more children end return
    return;
  }


} // End of Isis namespace
