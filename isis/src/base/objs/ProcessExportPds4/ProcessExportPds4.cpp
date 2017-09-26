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

    FixedImageRoot();

    StandardImageImage();

    //StandardAllMapping(mainPvl);
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
        xlator.setElementValue(dataTypeElement, "IEEE754LSBSingle");
        elementArrayElement.appendChild(dataTypeElement);

        QDomElement scalingFactorElement = m_domDoc->createElement("scaling_factor");
        xlator.setElementValue(scalingFactorElement, "1.00");
        elementArrayElement.appendChild(scalingFactorElement);

        QDomElement offsetElement = m_domDoc->createElement("offset");

        xlator.setElementValue(offsetElement, "0.00");
        elementArrayElement.appendChild(offsetElement);
      }
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

} // End of Isis namespace
