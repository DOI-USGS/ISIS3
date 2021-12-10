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
#include "PvlToJSON.h"
#include "ProcessImport.h"
#include "TextFile.h"
#include "XmlToJson.h"

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
      catch(IException &e) {
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

    env.add_callback("capitalize", 1, [](Arguments& args) {
      std::string str = args.at(0)->get<string>();
      std::transform(str.begin(), str.end(),str.begin(), ::tolower);
      str[0] = toupper(str[0]);
      return str;
    });

    env.add_callback("CassiniIssBandInfo", 3, [](Arguments& args) {
      std::string instrumentID = args.at(0)->get<string>();
      std::string filter1 = args.at(1)->get<string>();
      std::string filter2 = args.at(2)->get<string>();
      QString filter = QString(filter1.c_str()) + "/" + QString(filter2.c_str());
      QString dir = "$ISISROOT/appdata/translations";
      QString cameraAngleDefs;
      if(instrumentID.at(3) == 'N') {
        cameraAngleDefs = dir + "/CassiniIssNarrowAngle.def";
      }
      else if(instrumentID.at(3) == 'W') {
        cameraAngleDefs = dir + "/CassiniIssWideAngle.def";
      }

      double center = 0;
      double width = 0;

      TextFile cameraAngle(cameraAngleDefs);
      int numLines = cameraAngle.LineCount();
      bool foundfilter = false;
      for(int i = 0; i < numLines; i++) {
        QString line;
        cameraAngle.GetLine(line, true);

        QStringList tokens = line.simplified().split(" ");
        if(tokens.count() > 2 && tokens.first() == filter) {
          center = toDouble(tokens[1]);
          width = toDouble(tokens[2]);
          foundfilter = true;
          break;
        }
      }
      vector<double> bandInfo = {center, width};
      return bandInfo;
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


    /**
     * Add ImageNumber to Archive Group based on StartTime and ProductId.
     *
     *   Last digit of the year (eg, 1997 => 7), followed by the
     *   Day of the year (Julian day) followed by the
     *   Last five digits of the ProductId
     */
     env.add_callback("SetImageNumber", 2, [](Arguments& args) {
       std::string yearDoy = args.at(0)->get<string>();
       std::string productId = args.at(1)->get<string>();

       // grab the last digit of the year
       std::string imageNumber = yearDoy.substr(3, 1);
       // grab the DOY
       imageNumber += yearDoy.substr(4, 3);
       // grab the last 5 digits of productId
       imageNumber += productId.substr(4);

       return imageNumber;
     });

     /**
      * Add ImageKeyId to Archive Group based on StartTime and ProductId
      */
      env.add_callback("SetImageKeyId", 2, [](Arguments& args) {
        std::string clockCount = args.at(0)->get<string>();
        std::string productId = args.at(1)->get<string>();

        std::string imageKeyId = clockCount.substr(0, 5) + productId.substr(4);

        return imageKeyId;
      });

     // end of inja callbacks


    ProcessImport importer;
    if (inputFileName.removeExtension().addExtension("dat").fileExists()){
      importer.SetInputFile(inputFileName.removeExtension().addExtension("dat").expanded());
    }
    else if (inputFileName.removeExtension().addExtension("img").fileExists()) {
      importer.SetInputFile(inputFileName.removeExtension().addExtension("img").expanded());
    }
    else {
      importer.SetInputFile(inputFileName.expanded());
    }

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
      PvlKeyword dataFilePointer = translation["DataFilePointer"];

      int offset = 0;
      int recSize = 1;
      QString units = "BYTES";

      if (dataFilePointer.size() == 1) {
        offset = toInt(dataFilePointer) - 1;
        units = dataFilePointer.unit();
      }
      else if (dataFilePointer.size() == 2) {
        offset = toInt(dataFilePointer[1]) - 1;
        units = dataFilePointer.unit(1);
      }

      if (translation.hasKeyword("DataFileRecordBytes")) {
        recSize = toInt(translation["DataFileRecordBytes"]);
      }

      importer.SetFileHeaderBytes((offset) * recSize);
    }
    // Assume PDS4
    else {
      importer.SetFileHeaderBytes(0);
    }

    // Checks that are unique to mgsmoc
    if (translation.hasKeyword("compressed") && translation.hasKeyword("projected")) {
      if (toBool(translation["compressed"])) {
        QString msg = "[" + inputFileName.name() + "] may be compressed. Please run image through mocuncompress to uncompress.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      if (toBool(translation["projected"])) {
        QString msg = "[" + inputFileName.name() + "] appears to be an rdr file.";
        msg += " Use pds2isis.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    // Processing unique to mroctx
    if (translation.hasKeyword("DataPrefixBytes")) {
      importer.SetDataPrefixBytes(translation["DataPrefixBytes"]);
      importer.SaveDataPrefix();
    }

    if (translation.hasKeyword("DataSuffixBytes")) {
      importer.SetDataSuffixBytes(translation["DataSuffixBytes"]);
      importer.SaveDataSuffix();
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

    QString cubeAtts = "";
    if (translation.hasKeyword("CubeAtts")) {
      cubeAtts = QString(translation["CubeAtts"]);
    }
    CubeAttributeOutput att = CubeAttributeOutput(cubeAtts);
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
