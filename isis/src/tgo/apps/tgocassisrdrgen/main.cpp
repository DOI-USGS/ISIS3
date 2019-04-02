#include "Isis.h"

#include <QRegularExpression>
#include <QString>
#include <QDomDocument>

#include "Application.h"
#include "Cube.h"
#include "ExportDescription.h"
#include "FileName.h"
#include "Process.h"
#include "ProcessExportPds4.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlToXmlTranslationManager.h"

using namespace Isis;
using namespace std;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Check if input file is indeed, a cube
  if (ui.GetFileName("FROM").right(3) != "cub") {
    QString msg = "Input file [" + ui.GetFileName("FROM") +
                "] does not appear to be a cube";
    throw  IException(IException::User, msg, _FILEINFO_);
  }

  // Setup the process and set the input cube
  ProcessExportPds4 process;
  Cube *icube = process.SetInputCube("FROM");

  PvlObject *label= icube->label();

  PvlGroup targetGroup;
  QString logicalId = "urn:esa:psa:em16_tgo_frd:";

  if ( label->findObject("IsisCube").hasGroup("Instrument") ) {
    targetGroup = label->findObject("IsisCube").findGroup("Instrument");
    if (label->findObject("IsisCube").hasGroup("Mapping")) {
      logicalId += "data_projected:";
    }
    else {
      logicalId += "data_raw:";
    }
  }
  else if ( label->findObject("IsisCube").hasGroup("Mosaic") ) {
    targetGroup = label->findObject("IsisCube").findGroup("Mosaic");
    logicalId += "data_mosaic:";
  }

  // Check if the cube is able to be translated into a CaSSIS xml file
  // This could very well be unnecessary
  if (!targetGroup.findKeyword("InstrumentId").isEquivalent("CaSSIS")) {
    QString msg = "Input file [" + ui.GetFileName("FROM") +
                "] does not appear to be a CaSSIS RDR product. The image" +
                "instrument is not the CaSSIS instrument";
    throw  IException(IException::User, msg, _FILEINFO_);
  }

  // Add the ProductId keyword for translation. If a product id is not specified
  // by the user, set it to the Observation Id.
  // This is added before the translation instead of adding it to the exported xml
  // because of the ease of editing pvl vs xml.
  PvlKeyword productId("ProductId");
  if ( ui.WasEntered("PRODUCTID") ) {
    productId.setValue( ui.GetString("PRODUCTID") );
  }
  else {
    // Get the observationId from the Archive Group, or the Mosaic group, if the input is a mosaic
    QString observationId;

    if (label->findObject("IsisCube").hasGroup("Mosaic")) {
      PvlGroup mosaicGroup = label->findObject("IsisCube").findGroup("Mosaic");
      observationId = mosaicGroup.findKeyword("ObservationId")[0];
    }
    else if(label->findObject("IsisCube").hasGroup("Archive")){
      PvlGroup archiveGroup = label->findObject("IsisCube").findGroup("Archive");
      observationId = archiveGroup.findKeyword("ObservationId")[0];
    }
    productId.setValue(observationId);
  }

  targetGroup.addKeyword(productId);  
  logicalId += productId[0];
  process.setLogicalId(logicalId);

  // Set Title
  if ( ui.WasEntered("TITLE") ) {
    process.setTitle( ui.GetString("TITLE") );
  }

  // Set Version ID
  if ( ui.WasEntered("VERSIONID") ) {
    process.setVersionId( ui.GetString("VERSIONID") );
  }

  // std PDS4 label
  process.StandardPds4Label();

/*  process.addSchema("PDS4_PSA_1000.sch", 
                    "PDS4_PSA_1000.xsd",
                    "xmlns:psa", 
                    "http://psa.esa.int/psa/v1");

  process.addSchema("PDS4_PSA_EM16_CAS_1000.sch", 
                    "PDS4_PSA_EM16_CAS_1000.xsd",
                    "xmlns",
                    "http://psa.esa.int/psa/em16/cas/v1");*/

  process.addSchema("CASSIS_1010.sch", 
                    "CASSIS_1010.xsd",
                    "xmlns:cassis",
                    "local");
  // Add geometry schema for mosaics
  if (label->findObject("IsisCube").hasGroup("Mosaic")) {
    process.addSchema("PDS4_GEOM_1B00_1610.sch", 
                      "PDS4_GEOM_1B00_1610.xsd",
                      "xmlns:geom",
                      "https://pds.jpl.nasa.gov/datastandards/schema/released/geom/v1");
  }

 /*
  * Add additional pds label data here
  */
  QDomDocument &pdsLabel = process.GetLabel();


  // The default translation for for non-mosaicked output
  QString exportTranslationFile = "$tgo/translations/tgoCassisExport.trn"; 

  if (label->findObject("IsisCube").hasGroup("Mosaic")) {
    exportTranslationFile = "$tgo/translations/tgoCassisExportMosaic.trn";
  }

  PvlToXmlTranslationManager cubeLab(*(icube->label()), exportTranslationFile);
  cubeLab.Auto(pdsLabel);

  ProcessExportPds4::translateUnits(pdsLabel);
  process.reorder(); 

  QString outFile = ui.GetFileName("TO");

  process.WritePds4(outFile);
}
