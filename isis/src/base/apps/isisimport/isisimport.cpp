#include <iostream>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <QString>
#include <QtMath>

#include "CubeAttribute.h"
#include "FileName.h"
#include "iTime.h"
#include "OriginalLabel.h"
#include "OriginalXmlLabel.h"
#include "OriginalLabel.h"
#include "XmlToJson.h"
#include "PvlToJSON.h"
#include "ProcessImport.h"

#include "isisimport.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;

namespace Isis {



  void isisimport(UserInterface &ui, Pvl *log) {
    FileName fileTemplate = ("$ISISROOT/appdata/import/fileTemplate.tpl");
    json jsonData;
    bool isPDS4 = false;
    FileName inputFileName = ui.GetFileName("FROM");

    if (inputFileName.extension().toUpper() == "IMQ"){
      QString msg = "Input image may be compressed. Please run image through vdcomp to uncompress"
                    "or verify image has correct file extension.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    try {
      // try to convert xml file to json
      jsonData = xmlToJson(inputFileName.toString());
      isPDS4 = true;
    }
    catch(...) {
      try {
        // try to convert pvl to json
        jsonData = pvlToJSON(inputFileName.toString());
        // QString labelOffset = jsonData["^QUBE"]["Value"];
        if (jsonData.find("^QUBE") != jsonData.end()) {
          jsonData["imageOffset"] = jsonData["^QUBE"]["Value"];
        }
      }
      catch(...) {
        QString msg = "Unable to process import image. Please confirm image is in PDS3 or PDS4 format";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    Environment env;

    // Find associated template
    FileName inputTemplate;
    if (ui.WasEntered("TEMPLATE")) {
      inputTemplate = ui.GetFileName("TEMPLATE");
    }
    else {
      try {
        std::string templateFile = env.render_file(fileTemplate.expanded().toStdString(), jsonData);
        inputTemplate = FileName(QString::fromStdString(templateFile));
      }
      catch(...) {
         QString msg = "Cannot locate a template for input label. Please provide a template file to use.";
         throw IException(IException::User, msg, _FILEINFO_);
      }
    }


    // Template engine call back functions
    /**
     * Renders YearDoy using StartTime to format to YYYYDOY
     */
    env.add_callback("YearDoy", 1, [](Arguments& args) {
      std::string startTime = args.at(0)->get<string>();
      std::string yearString = startTime.substr(0, 4);

      int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

      int year = stoi(startTime.substr(0, 4));
      int month = stoi(startTime.substr(5, 7));
      int day = stoi(startTime.substr(8, 10));
      int doy = day;
      if (month > 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        doy++;
      }
      // Add the days in the previous months
      while (--month > 0) {
        doy = doy + daysInMonth[month-1];
      }
      return yearString.append(to_string(doy));
    });

    /**
     * Converts UniqueId To ObservationId
     * Logic from convertUniqueIdToObservationId in tgocassis2isis app
     */
    env.add_callback("UniqueIdtoObservId", 2, [](Arguments& args) {
      std::string uniqueId = args.at(0)->get<string>();
      std::string target = args.at(1)->get<string>();
      std::string observationId = "";

      long long operationPeriod = (stoll(uniqueId) & 1879048192);
      operationPeriod /= pow(2,28);

      // previously pulled from TgoCassisOperationPeriod.trn
      if ((int)operationPeriod == 0) observationId = "CRUS";
      else if ((int)operationPeriod == 1) observationId = "MY34";
      else if ((int)operationPeriod == 2) observationId = "MY35";
      else if ((int)operationPeriod == 3) observationId = "MY36";
      else if ((int)operationPeriod == 4) observationId = "MY37";
      else if ((int)operationPeriod == 5) observationId = "MY38";
      else if ((int)operationPeriod == 5) observationId = "MY38";
      else if ((int)operationPeriod == 6) observationId = "TBD";
      else if ((int)operationPeriod == 7) observationId = "TEST";
      else observationId = "UNK";

      long long orbitNumber = (stoll(uniqueId) & 268433408);
      orbitNumber /= pow(2,11);
      observationId += "_";
      std::string orbitString = to_string(orbitNumber);
      orbitString.insert(orbitString.begin(), 6 - orbitString.length(), '0');
      observationId += orbitString;

      int orbitPhase = (stoll(uniqueId) & 2044);
      transform(target.begin(), target.end(), target.begin(), ::tolower);
      if (target.compare("mars") == 0) {
        orbitPhase /= pow(2,2);
      }
      else {
        orbitPhase = 900;
      }
      observationId += "_";
      observationId += to_string(orbitPhase);

      int imageType = (stoll(uniqueId) & 3);
      observationId += "_";
      observationId += to_string(imageType);

      return observationId;
    });

   /**
    * Removes 'Z' that is added to StartTime when image has been reingested
    */
    env.add_callback("RemoveStartTimeZ", 1, [](Arguments& args) {
      std::string startTime = args.at(0)->get<string>();

      if(startTime.back() == 'Z') {
        startTime.pop_back();
      }

      return startTime;
    }); // end of inja callbacks


    ProcessImport importer;
    importer.SetInputFile(inputFileName.expanded());


    // Use inja to get number of lines, samples, and bands from the input label
    std::string result = env.render_file(inputTemplate.expanded().toStdString(), jsonData);

    // Turn this into a Pvl label
    Pvl newLabel;
    newLabel.fromString(result);

    // Set everything needed by ProcessImport
    PvlGroup dimensions = newLabel.findObject("IsisCube").findObject("Core").findGroup("Dimensions");
    int ns = toInt(dimensions["Samples"]);
    int nl = toInt(dimensions["Lines"]);
    int nb = toInt(dimensions["Bands"]);
    importer.SetDimensions(ns, nl, nb);

    PvlGroup pixels = newLabel.findObject("IsisCube").findObject("Core").findGroup("Pixels");
    QString pixelType = pixels["Type"];
    QString byteOrder = pixels["ByteOrder"];
    double base = pixels["Base"];
    double multiplier = pixels["Multiplier"];
    importer.SetPixelType(PixelTypeEnumeration(pixelType));
    importer.SetByteOrder(ByteOrderEnumeration(byteOrder));
    importer.SetBase(base);
    importer.SetMultiplier(multiplier);

    PvlObject translation = newLabel.findObject("Translation");

    // Check translation for potential PDS3 offset
    if (translation.hasKeyword("DataFilePointer")) {
      int offset = toInt(translation["DataFilePointer"]);

      if (translation.hasKeyword("DataFileRecordBytes")) {
        int recSize = toInt(translation["DataFileRecordBytes"]);

        importer.SetFileHeaderBytes((offset - 1) * recSize);
      }
    }
    // Assume PDS4
    else {
      importer.SetFileHeaderBytes(0);
    }

    if (translation.hasKeyword("CoreAxisNames")) {
      QString originalAxisOrder = QString(translation["CoreAxisNames"]);
      if (originalAxisOrder == "SAMPLELINEBAND") {
        importer.SetOrganization(ProcessImport::BSQ);
      }
      else if (originalAxisOrder == "BANDSAMPLELINE") {
        importer.SetOrganization(ProcessImport::BIP);
      }
      else if (originalAxisOrder == "SAMPLEBANDLINE") {
        importer.SetOrganization(ProcessImport::BIL);
      }
      else {
        stringstream pdsOrgStream;
        pdsOrgStream << originalAxisOrder;

        QString msg = "Unsupported axis order [" + QString(originalAxisOrder) + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    // Set any special pixel values
    double pdsNull = Isis::NULL8;
    if (translation.hasKeyword("CoreNull")) {
      pdsNull = toDouble(translation["CoreNull"]);
    }

    double pdsLrs = Isis::Lrs;
    if (translation.hasKeyword("CoreLRS")) {
      pdsLrs = toDouble(translation["CoreLRS"]);
    }

    double pdsLis = Isis::Lis;
    if (translation.hasKeyword("CoreLIS")) {
      pdsLis = toDouble(translation["CoreLIS"]);
    }

    double pdsHrs = Isis::Hrs;
    if (translation.hasKeyword("CoreHRS")) {
      pdsHrs = toDouble(translation["CoreHRS"]);
    }

    double pdsHis = Isis::His;
    if (translation.hasKeyword("CoreHIS")) {
      pdsHis = toDouble(translation["CoreHIS"]);
    }
    importer.SetSpecialValues(pdsNull, pdsLrs, pdsLis, pdsHrs, pdsHis);

    CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
    Cube *outputCube = importer.SetOutputCube(ui.GetFileName("TO"), att);

    if (isPDS4) {
      OriginalXmlLabel xmlLabel;
      xmlLabel.readFromXmlFile(inputFileName);
      outputCube->write(xmlLabel);
    }
    // Assume PDS3
    else {
      Pvl pdsLab(inputFileName.expanded());
      OriginalLabel pds3Label(pdsLab);
      outputCube->write(pds3Label);
    }
    importer.StartProcess();

    // Write the updated label
    Isis::PvlObject &newCubeLabel = newLabel.findObject("IsisCube");
    Isis::Pvl &outLabel(*outputCube->label());
    Isis::PvlObject &outCubeLabel = outLabel.findObject("IsisCube");

    for(int g = 0; g < newCubeLabel.groups(); g++) {
      outCubeLabel.addGroup(newCubeLabel.group(g));
    }
    importer.EndProcess();

    return;
  }
}
