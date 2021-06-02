#include <iostream>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <QString>
#include <QtMath>

#include "CubeAttribute.h"
#include "FileName.h"
#include "iTime.h"
#include "OriginalXmlLabel.h"
#include "XmlToJson.h"
#include "ProcessImport.h"

#include "isisimport.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;

namespace Isis {

  void isisimport(UserInterface &ui, Pvl *log) {
    FileName fileTemplate = ("$ISISROOT/appdata/import/fileTemplate.tpl");
    FileName xmlFileName = ui.GetFileName("FROM");

    // To read the DN data
    ProcessImport importer;
    if (xmlFileName.removeExtension().addExtension("dat").fileExists()){
      importer.SetInputFile(xmlFileName.removeExtension().addExtension("dat").expanded());
    }
    else if (xmlFileName.removeExtension().addExtension("img").fileExists()) {
      importer.SetInputFile(xmlFileName.removeExtension().addExtension("img").expanded());
    }
    else {
      QString msg = "Cannot find image file for [" + xmlFileName.name() + "]. Confirm that the "
        ".dat or .img file for this XML exists and is located in the same directory.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    Environment env;

    // Convert xml file to json so inja can use it
    json pds4Data = xmlToJson(xmlFileName.toString());

    FileName inputTemplate;
    if (ui.WasEntered("TEMPLATE")) {
      inputTemplate = ui.GetFileName("TEMPLATE");
    }
    else {
      try {
        std::string templateFile = env.render_file(fileTemplate.expanded().toStdString(), pds4Data);
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
      });

    // Use inja to get number of lines, samples, and bands from the input PDS4 label
    std::string result = env.render_file(inputTemplate.expanded().toStdString(), pds4Data);

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

    // TODO: how to handle this?
    importer.SetFileHeaderBytes(0);

    CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
    Cube *outputCube = importer.SetOutputCube(ui.GetFileName("TO"), att);

    OriginalXmlLabel xmlLabel;
    xmlLabel.readFromXmlFile(xmlFileName);

    importer.StartProcess();

    outputCube->write(xmlLabel);

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
