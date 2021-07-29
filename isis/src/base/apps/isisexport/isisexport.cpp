#include <fstream>
#include <iostream>
#include <time.h>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include <QDomDocument>
#include <QFile>

#include "cubeatt.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "md5wrapper.h"
#include "OriginalLabel.h"
#include "OriginalXmlLabel.h"
#include "Process.h"
#include "PvlToJSON.h"
#include "XmlToJson.h"

#include "isisexport.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;

namespace Isis {
  QString PDS4PixelType(Isis::PixelType ipixelType, Isis::ByteOrder ibyteOrder);

  void isisexport(UserInterface &ui, Pvl *log) {
    Cube *icube = new Cube();
    icube->open(ui.GetFileName("FROM"));
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube->setVirtualBands(inAtt.bands());
    }
    isisexport(icube, ui);
  }

  void isisexport(Cube *icube, UserInterface &ui, Pvl *log) {

    Process p;
    p.SetInputCube(icube);

    // Setup the output file so that we can use it in callbacks
    QString outputFile = ui.GetFileName("TO");

    // Name for output image
    FileName outputFileName(outputFile);
    FileName genDefaultTemplate = ("$ISISROOT/appdata/export/pvl2template.tpl");
    QString path(outputFileName.originalPath());
    QString name(outputFileName.baseName());
    QString outputCubePath = path + "/" + name + ".cub";

    CubeAttributeOutput outputAttributes("+bsq");
    cubeatt(icube, outputCubePath, outputAttributes);

    json dataSource;

    Environment env;

    Pvl &cubeLabel = *icube->label();

    // Add the input cube PVL label to template engine data
    dataSource["MainLabel"].update(pvlToJSON(cubeLabel));


    // Handle TEMPLATE argument
    FileName templateFn;
    if(ui.WasEntered("TEMPLATE")) {
      templateFn = ui.GetFileName("TEMPLATE");
    }
    else {
         std::string templateFnStd = env.render_file(genDefaultTemplate.expanded().toStdString(), dataSource);
         templateFn = FileName(QString::fromStdString(templateFnStd));
     
    }

    if(!templateFn.fileExists()) {
      QString msg = "File does not exist: " + templateFn.expanded();

      if(!ui.WasEntered("TEMPLATE")) {
        msg += ". Unsupported Spacecraft/Instrument for PDS4 export.";
      }
      throw IException(IException::User, msg, _FILEINFO_);
    }



    // Add the original label (from an ingestion app) to the template engine data
    // Wrap it in an OriginalLabel so existing elements don't get overwritten
    if (cubeLabel.hasObject("OriginalLabel")) {
      OriginalLabel origLabel = icube->readOriginalLabel();
      Pvl pvlOrigLabel;
      pvlOrigLabel = origLabel.ReturnLabels();
      dataSource["OriginalLabel"].update(pvlToJSON(pvlOrigLabel));
    }
    else if (cubeLabel.hasObject("OriginalXmlLabel")) {
      OriginalXmlLabel origXmlBlob = icube->readOriginalXmlLabel();
      QDomDocument doc = origXmlBlob.ReturnLabels();
      dataSource["OriginalLabel"].update(xmlToJson(doc));
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
                              + "Previous value [" + QString::fromStdString(dataSource["ExtraPvl"][element.key()].dump())
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
                              + "Previous value [" + QString::fromStdString(dataSource["ExtraXml"][element.key()].dump())
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
                              + "Previous value [" + QString::fromStdString(dataSource["ExtraJson"][element.key()].dump())
                              + "] will be overwritten.";
              duplicateWarnings += PvlKeyword("Duplicate", message);
              log->addGroup(duplicateWarnings);
            }
          }
        }
        dataSource["ExtraJson"].update(extraJson);
      }
    }

    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);

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
     * Renders to the filename of the output image file
     */
    env.add_callback("imageFileName", 0, [outputCubePath](Arguments& args) {
      return outputCubePath.split("/").back().toStdString();
    });

    /**
     * Renders to the final file size in bytes of the output image file
     */
    env.add_callback("outputFileSize", 0, [outputCubePath](Arguments& args) {
      FileName cubeFileName = outputCubePath;
      return QFile(cubeFileName.expanded()).size();
    });

    /**
     * Renders to the MD5 hash for the output image file
     */
    env.add_callback("md5Hash", 0, [outputCubePath](Arguments& args) {
      md5wrapper md5;
      return md5.getHashFromFile(outputCubePath).toStdString();
    });

    /***
     * Renders the pixel type of the input cube as a PDS4 compliant type
     */
    env.add_callback("pixelType", 0, [icube](Arguments& args) {
      return PDS4PixelType(icube->pixelType(), icube->byteOrder()).toStdString();
    });

    std::string result;
    try {
      result = env.render_file(templateFn.expanded().toStdString(), dataSource);
    }
    catch (const std::exception &ex) {
      throw IException(IException::ErrorType::Unknown, ex.what(), _FILEINFO_);
    }  
    std::ofstream outFile(ui.GetFileName("TO").toStdString());
    outFile << result;
    outFile.close();

    if (ui.WasEntered("DATA")) {
      std::ofstream jsonDataFile(FileName(ui.GetFileName("DATA")).expanded().toStdString());
      jsonDataFile << dataSource.dump(4);
      jsonDataFile.close();
    }
  }

  QString PDS4PixelType(Isis::PixelType ipixelType, Isis::ByteOrder ibyteOrder) {
    QString pds4Type("UNK");
    if(ipixelType == Isis::UnsignedByte) {
      pds4Type = "UnsignedByte";
    }
    else if((ipixelType == Isis::UnsignedWord) && (ibyteOrder == Isis::Msb)) {
      pds4Type = "UnsignedMSB2";
    }
    else if((ipixelType == Isis::UnsignedWord) && (ibyteOrder == Isis::Lsb)) {
      pds4Type = "UnsignedLSB2";
    }
    else if((ipixelType == Isis::SignedWord) && (ibyteOrder == Isis::Msb)) {
      pds4Type = "SignedMSB2";
    }
    else if((ipixelType == Isis::SignedWord) && (ibyteOrder == Isis::Lsb)) {
      pds4Type = "SignedLSB2";
    }
    else if((ipixelType == Isis::Real) && (ibyteOrder == Isis::Msb)) {
      pds4Type = "IEEE754MSBSingle";
    }
    else if((ipixelType == Isis::Real) && (ibyteOrder == Isis::Lsb)) {
      pds4Type = "IEEE754LSBSingle";
    }
    else {
      QString msg = "Unsupported PDS pixel type or sample size";
      throw Isis::IException(Isis::IException::User, msg, _FILEINFO_);
    }
    return pds4Type;
  }
}
