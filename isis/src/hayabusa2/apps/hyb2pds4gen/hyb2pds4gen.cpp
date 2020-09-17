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

#include "hyb2pds4gen.h"

using namespace std;

namespace Isis {
    static QMap<QString, QString> descMap({
            {"J2000Q0",  "element q0 of quaternion representing a rotation"},
            {"J2000Q1",  "element q1 of quaternion representing a rotation"},
            {"J2000Q2",  "element q2 of quaternion representing a rotation"},
            {"J2000Q3",  "element q3 of quaternion representing a rotation"},
            {"AV1",      "Angular velocity vector"},
            {"AV2",      "Angular velocity vector"},
            {"AV3",      "Angular velocity vector"},
            {"ET",       "Ephemeris time"},
            {"J2000X",   "J2000 position x"},
            {"J2000Y",   "J2000 position y"},
            {"J2000Z",   "J2000 position z"},
            {"J2000XV",  "J2000 velocity xv"},
            {"J2000YV",  "J2000 velocity yv"},
            {"J2000ZV",  "J2000 velocity zv"}
        }); 

    Pvl toPvl(PvlObject &container);
    QDomDocument emptyDoc(); 

    extern void hyb2pds4gen(UserInterface &ui) {
        Cube icube;
        icube.open(ui.GetFileName("FROM"));
        hyb2pds4gen(&icube, ui);
    }

    void hyb2pds4gen(Cube *inputCube, UserInterface &ui) {
    QString translationFile = "$ISISROOT/appdata/translations/Hayabusa2OncPds4Export.trn";

    // Setup the process and set the input cube
    ProcessExportPds4 process;
    process.SetInputCube(inputCube);
    Pvl *inputLabel = inputCube->label();
    
    process.setImageType(ProcessExportPds4::BinSetSpectrum);

    QDomDocument &pdsLabel = process.StandardPds4Label();
    ProcessExportPds4::translateUnits(pdsLabel);
    
    QString logicalId = ui.GetString("PDS4LOGICALIDENTIFIER");
    process.setLogicalId(logicalId); 

    QStringList xmlPath = {"Product_Observational", "File_Area_Observational"};
    
    for (int i = 0; i < inputLabel->objects(); i++) {
        PvlObject obj = inputLabel->object(i);
        if (obj.name() == "Table") {
        QDomDocument doc = emptyDoc();
        
        Pvl pvlObj = toPvl(obj);     
        PvlToXmlTranslationManager tableXlator(pvlObj,"$ISISROOT/appdata/translations/pds4ExportSpiceTable.trn");
        tableXlator.Auto(doc);
        QDomElement recordBinary = doc.createElement("Record_Binary");
        QDomElement fields = doc.createElement("fields");
        QDomElement groups = doc.createElement("groups");
        QDomElement record_len = doc.createElement("record_length");
        
        QDomElement tableBinary = process.getElement({"Product_Observational", "File_Area_Observational", "Table_Binary"}, doc.documentElement()); 
        
        PvlToXmlTranslationManager::setElementValue(fields, QString::number(obj.groups()));
        PvlToXmlTranslationManager::setElementValue(groups, QString::number(0)); 
        PvlToXmlTranslationManager::setElementValue(record_len, obj["Bytes"]); 
        record_len.setAttribute("unit", "byte"); 
        
        tableBinary.appendChild(recordBinary).appendChild(fields); 
        recordBinary.appendChild(groups); 
        recordBinary.appendChild(record_len);

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
            PvlToXmlTranslationManager::setElementValue(description, descMap[grp["Name"]]);
            
            field.appendChild(name);
            field.appendChild(fieldNumber);
            field.appendChild(fieldLocation);
            field.appendChild(dataType);
            field.appendChild(fieldLength);
            field.appendChild(description);
            recordBinary.appendChild(field);

        } 
        
        // translation files do not support adding attrs to siblings so we have to do it manually
        QDomNodeList fieldList = doc.elementsByTagName("field_length");
        for (int j = 0; j < fieldList.size(); j++) {
            fieldList.at(j).toElement().setAttribute("unit", "byte");
        }

        QDomElement tableElement = process.getElement(xmlPath, doc.documentElement()).firstChildElement("Table_Binary"); 
        QDomElement base = process.getElement(xmlPath, pdsLabel.documentElement());  
        base.appendChild(tableElement);
        }
    }
    
    // remove elements not wanted in hyb2
    QDomElement del = process.getElement({"Product_Observational", "Observation_Area", "Discipline_Area"}, pdsLabel.documentElement());
    del.removeChild(del.firstChildElement("img:Imaging"));
    del.removeChild(del.firstChildElement("sp:Spectral_Characteristics"));  

    del = process.getElement({"Product_Observational", "File_Area_Observational", "Array_3D_Spectrum"}, pdsLabel.documentElement()); 
    del.removeChild(del.firstChildElement("Special_Constants")); 
    
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
    
    tempDoc = emptyDoc();
    PvlToXmlTranslationManager labXlator(*(inputLabel), "$ISISROOT/appdata/translations/pds4ExportLabelObject.trn");
    labXlator.Auto(tempDoc); 
    QDomElement labElem = process.getElement(xmlPath, tempDoc.documentElement()).firstChildElement("Header");
    QDomElement fileElem = process.getElement({"Product_Observational", "File_Area_Observational", "Array_3D_Spectrum"}, pdsLabel.documentElement());
    base.insertBefore(labElem, fileElem);   

    PvlGroup instGroup = inputLabel->findObject("IsisCube").findGroup("Instrument");
    
    QStringList subFramePath = {"Product_Observational",
                                "Observation_Area",
                                "Discipline_Area",
                                "img:Imaging",
                                "img:Image_Product_Information",
                                "img:Subframe_Parameters"};

    if (instGroup.hasKeyword("FirstLine") && instGroup.hasKeyword("LastLine")) {

        int lines = (int) instGroup["LastLine"] - (int) instGroup["FirstLine"];
        QDomElement baseElement = pdsLabel.documentElement();
        QDomElement subframeParametersElement = process.getElement(subFramePath, baseElement);
        
        QDomElement linesElement = pdsLabel.createElement("img:lines");
        PvlToXmlTranslationManager::setElementValue(linesElement, toString(lines));
        subframeParametersElement.appendChild(linesElement);

    }

    if (instGroup.hasKeyword("FirstSample") && instGroup.hasKeyword("LastSample")) {
        int samples = (int) instGroup["LastSample"] - (int) instGroup["FirstSample"];
        QDomElement baseElement = pdsLabel.documentElement();
        QDomElement subframeParametersElement = process.getElement(subFramePath, baseElement);

        QDomElement samplesElement = pdsLabel.createElement("img:samples");
        PvlToXmlTranslationManager::setElementValue(samplesElement, toString(samples));
        subframeParametersElement.appendChild(samplesElement);
    }

    QString outFile = ui.GetFileName("TO");
    process.WritePds4(outFile);

    return;
    }


    /**
    * Converts a PvlObject instance to a PVL instance. 
    *  
    * Something about the relationship between the Pvl class and PvlObject class 
    * makes it impossible to simply cast without a segfault, so we have to do a 
    * bit more massaging. 
    *
    */
    Pvl toPvl(PvlObject &container) { 
    Pvl newPvl;
    std::stringstream buffer;
    buffer << container << std::endl; 
    buffer >> newPvl;
    return newPvl; 
    }


    /**
    * Returns a minimal QDomDocument for running it through 
    * PvlToXmlTranslationManager
    */ 
    QDomDocument emptyDoc() { 
    QDomDocument doc;
    QDomElement root = doc.createElement("Product_Observational");
    doc.appendChild(root);
    return doc; 
    }


};