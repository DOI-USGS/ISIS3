#include "Isis.h"

#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QString>

#include "Application.h"
#include "Cube.h"
#include "ExportDescription.h"
#include "FileName.h"
#include "Process.h"
#include "ProcessExportPds.h"
#include "ProcessExportPds4.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlToXmlTranslationManager.h"

using namespace std;
using namespace Isis;

Pvl toPvl(PvlGroup &container) { 
  Pvl newPvl;
  std::stringstream buffer;
  buffer << container << std::endl; 
  buffer >> newPvl;
  return newPvl; 
}

Pvl toPvl(PvlObject &container) { 
  Pvl newPvl;
  std::stringstream buffer;
  buffer << container << std::endl; 
  buffer >> newPvl;
  return newPvl; 
}

QDomDocument emptyDoc() { 
  QDomDocument doc;
  QDomElement root = doc.createElement("Product_Observational");
  doc.appendChild(root);
  return doc; 
}

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  
  QString translationFile = "$ISISROOT/appdata/translations/Hayabusa2OncPds4Export.trn";

  // Setup the process and set the input cube
  ProcessExportPds4 process;
  Cube *inputCube = process.SetInputCube("FROM");
  Pvl *inputLabel = inputCube->label();
  
  process.setImageType(ProcessExportPds4::BinSetSpectrum);

  QDomDocument &pdsLabel = process.StandardPds4Label();
  ProcessExportPds4::translateUnits(pdsLabel);
  
  QString logicalId = ui.GetString("PDS4LOGICALIDENTIFIER");
  process.setLogicalId(logicalId); 

  QStringList xmlPath = {"Product_Observational", "File_Area_Observational"};
  
  for (int i = 0; i < inputLabel->objects(); i++) {
    PvlObject obj = inputLabel->object(i);
    std::cout << obj.name() << std::endl; 
    if (obj.name() == "Table") {
      QDomDocument doc = emptyDoc();
      
      Pvl pvlObj = toPvl(obj);     
      PvlToXmlTranslationManager tableXlator(pvlObj,"$ISISROOT/appdata/translations/pds4ExportSpiceTable.trn");
      tableXlator.Auto(doc);
      QDomElement recordBinary = doc.createElement("Record_Binary");
      QDomElement fields = doc.createElement("fields");
      QDomElement groups = doc.createElement("groups");
      QDomElement tableBinary = process.getElement({"Product_Observational", "File_Area_Observational", "Table_Binary"}, doc.documentElement()); 
      
      PvlToXmlTranslationManager::setElementValue(fields, QString::number(obj.groups()));
      PvlToXmlTranslationManager::setElementValue(groups, QString::number(0)); 
      tableBinary.appendChild(recordBinary).appendChild(fields); 
      recordBinary.appendChild(groups); 

      // Translate Field Groups 
      for (int j = 0; j < obj.groups(); j++) {
        PvlGroup grp = obj.group(j);
        QDomElement field = doc.createElement("Field_Binary");

        QDomElement name = doc.createElement("name");
        PvlToXmlTranslationManager::setElementValue(name, grp["Name"]);
        
        QDomElement fieldNumber = doc.createElement("field_number");
        PvlToXmlTranslationManager::setElementValue(fieldNumber, QString::number(j+1));
        
        QDomElement fieldLocation = doc.createElement("field_location");
        PvlToXmlTranslationManager::setElementValue(fieldLocation, QString::number((j*8)+1));
        fieldLocation.setAttribute("unit", "byte");

        QDomElement dataType = doc.createElement("data_type");
        PvlToXmlTranslationManager::setElementValue(dataType, "IEEE754MSBDouble");
           
        QDomElement fieldLength = doc.createElement("field_length");
        PvlToXmlTranslationManager::setElementValue(fieldLength, QString::number(8));
        fieldLength.setAttribute("unit", "byte"); 

        QDomElement description = doc.createElement("description");
        PvlToXmlTranslationManager::setElementValue(description, "Spice Table Field");
        
        field.appendChild(name);
        field.appendChild(fieldNumber);
        field.appendChild(fieldLocation);
        field.appendChild(dataType);
        field.appendChild(fieldLength);
        field.appendChild(description);
        recordBinary.appendChild(field);

      }    
      
      // trasnlation files do not support adding attrs to sublings so we have to do it manually
      QDomNodeList fieldList = doc.elementsByTagName("field_length");
      for (int j = 0; j < fieldList.size(); j++) {
        fieldList.at(j).toElement().setAttribute("unit", "byte");
      }

      QDomElement tableElement = process.getElement(xmlPath, doc.documentElement()).firstChildElement("Table_Binary"); 
      QDomElement base = process.getElement(xmlPath, pdsLabel.documentElement());  
      base.appendChild(tableElement);
    }
  }
 
  PvlToXmlTranslationManager xlator(*(inputLabel), translationFile);
  xlator.Auto(pdsLabel);
  
  QDomElement base = process.getElement(xmlPath, pdsLabel.documentElement()); 

  QDomDocument tempDoc = emptyDoc(); 
  PvlToXmlTranslationManager originalLabelXlator(*(inputLabel), "$ISISROOT/appdata/translations/pds4ExportOriginalLabel.trn");
  originalLabelXlator.Auto(tempDoc);
  QDomElement ogLabelElem = process.getElement(xmlPath, tempDoc.documentElement()).firstChildElement("Header");
  base.appendChild(ogLabelElem);   
  
  tempDoc = emptyDoc();
  PvlToXmlTranslationManager histXlator(*(inputLabel), "$ISISROOT/appdata/translations/pds4ExportHistory.trn");
  histXlator.Auto(tempDoc); 
  QDomElement histElem = process.getElement(xmlPath, tempDoc.documentElement()).firstChildElement("Header");
  base.appendChild(histElem);  


  QString outFile = ui.GetFileName("TO");
  process.WritePds4(outFile);

  return;
}

