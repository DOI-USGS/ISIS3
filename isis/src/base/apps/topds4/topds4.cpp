#include <fstream>
#include <iostream>
#include <time.h>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <QFile>

#include "FileName.h"
#include "md5wrapper.h"
#include "OriginalLabel.h"
#include "PvlToJSON.h"
#include "XmlToJson.h"

#include "topds4.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;


namespace Isis {

  void topds4(UserInterface &ui, Pvl *log) {
    Cube *icube = new Cube();
    icube->open(ui.GetFileName("FROM"));
    topds4(icube, ui);
  }

  void topds4(Cube *icube, UserInterface &ui, Pvl *log) {

    Process p;
    p.SetInputCube(icube);

    // NEED TO WRITE AND CLOSE THE OUTPUT FILE BEFORE RENDERING SO THE FILE SIZE CALLBACK CAN GET THE FINAL FILE SIZE
    QString outputFile = ui.GetFileName("TO");

    json dataSource;

    Pvl &cubeLabel = *icube->label();

    // Add the input cube PVL label to template engine data
    dataSource["MainLabel"].update(pvlToJSON(cubeLabel));

    // Add the original label (from an ingestion app) to the template engine data
    // Wrap it in an OriginalLabel so existing elements don't get overwritten
    if (cubeLabel.hasObject("OriginalLabel")) {
      OriginalLabel origBlob;
      icube->read(origBlob);
      Pvl origLabel;
      origLabel = origBlob.ReturnLabels();
      dataSource["OriginalLabel"].update(pvlToJSON(origLabel));
    }
    else if (cubeLabel.hasObject("OriginalXmlLabel")) {
      // get the xml label and add it to the template data
    }

    // Add any extra files to the template engine data
    if (ui.WasEntered("EXTRAPVL")) {
      vector<QString> extraPvlFiles;
      ui.GetFileName("EXTRAPVL", extraPvlFiles);
      for (QString pvlFile : extraPvlFiles) {
        Pvl extraPvl(pvlFile);
        json extraJson = pvlToJSON(extraPvl);
        // Notify users of duplicate keys that will be overwritten
        if (log) {
          for (auto& element : extraJson.items()) {
            if (dataSource["ExtraPvl"].contains(element.key())) {
              PvlGroup duplicateWarnings("Warning");
              QString message = "Duplicate key [" + QString::fromStdString(element.key())
                              + "] in extra Pvl file [" + pvlFile + "]. "
                              + "Previous value [" + QString::fromStdString(element.value().dump())
                              + "] will be overwritten.";
              duplicateWarnings += PvlKeyword("Duplicate", message);
              log->addGroup(duplicateWarnings);
            }
          }
        }
        dataSource["ExtraPvl"].update(extraJson);
      }
    }

    if (ui.WasEntered("EXTRAXML")) {
      vector<QString> extraXmlFiles;
      ui.GetFileName("EXTRAXML", extraXmlFiles);
      for (QString xmlFile : extraXmlFiles) {
        // Notify users of duplicate keys that will be overwritten
        json extraJson = xmlToJson(xmlFile);
        if (log) {
          for (auto& element : extraJson.items()) {
            if (dataSource["ExtraXml"].contains(element.key())) {
              PvlGroup duplicateWarnings("Warning");
              QString message = "Duplicate element [" + QString::fromStdString(element.key())
                              + "] in extra xml file [" + xmlFile + "]. "
                              + "Previous value [" + QString::fromStdString(element.value().dump())
                              + "] will be overwritten.";
              duplicateWarnings += PvlKeyword("Duplicate", message);
              log->addGroup(duplicateWarnings);
            }
          }
        }
        dataSource["ExtraXml"].update(extraJson);
      }
    }

    if (ui.WasEntered("EXTRAJSON")) {
      vector<QString> extraJsonFiles;
      ui.GetFileName("EXTRAJSON", extraJsonFiles);
      for (QString jsonFile : extraJsonFiles) {
        ifstream extraJsonStream(jsonFile.toStdString());
        json extraJson;
        extraJsonStream >> extraJson;
        if (log) {
          for (auto& element : extraJson.items()) {
            if (dataSource["ExtraJson"].contains(element.key())) {
              PvlGroup duplicateWarnings("Warning");
              QString message = "Duplicate key [" + QString::fromStdString(element.key())
                              + "] in extra json file [" + jsonFile + "]. "
                              + "Previous value [" + QString::fromStdString(element.value().dump())
                              + "] will be overwritten.";
              duplicateWarnings += PvlKeyword("Duplicate", message);
              log->addGroup(duplicateWarnings);
            }
          }
        }
        dataSource["ExtraJson"].update(extraJson);
      }
    }

    Environment env;
    // Template engine call back functions
    /**
     * Renders to the current UTC time formatted as YYYY-MM-DDTHH:MM:SS
     */
    env.add_callback("currentTime", 0, [](Arguments& args) {
      time_t startTime = time(NULL);
      struct tm *tmbuf = gmtime(&startTime);
      char timestr[80];
      strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
      return string(timestr);
    });

    /**
     * Renders to the filename of the output img
     */
    env.add_callback("imageFileName", 0, [icube](Arguments& args) {
      QString cubeFilename = icube->fileName().split("/").back();
      return (cubeFilename.split(".")[0] + ".img").toStdString();
    });

    /**
     * Renders to the final file size in bytes of the output cube or img
     */
    env.add_callback("outputFileSize", 0, [outputFile](Arguments& args) {
      FileName cubeFilename = outputFile;
        return QFile(outputFile).size();
    });

    /**
     * Renders to the MD5 hash for the input cube
     */
    env.add_callback("md5Hash", 0, [icube](Arguments& args) {
      md5wrapper md5;
      return md5.getHashFromFile(icube->fileName()).toStdString();
    });

    std::string inputTemplate = ui.GetFileName("TEMPLATE").toStdString();
    std::string result = env.render_file(inputTemplate, dataSource);

    std::ofstream outFile(ui.GetFileName("TO").toStdString());
    outFile << result;
    outFile.close();

    if (ui.WasEntered("DATA")) {
      std::ofstream jsonDataFile(FileName(ui.GetFileName("DATA")).expanded().toStdString());
      jsonDataFile << dataSource.dump(4);
      jsonDataFile.close();
    }

    std::cout << "============Result===============" << std::endl;
    std::cout << result << std::endl;
    std::cout << "=================================" << std::endl;
  }
}
