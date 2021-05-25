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

  static QString convertUniqueIdToObservationId(Pvl &outputLabel);

  void isisimport(UserInterface &ui, Pvl *log) {
    FileName xmlFileName = ui.GetFileName("FROM");

    // Convert xml file to json so inja can use it
    json pds4Data = xmlToJson(xmlFileName.toString());

    // std::cout << pds4Data.dump(4);
    std::string inputTemplate = ui.GetFileName("TEMPLATE").toStdString();

    Environment env;

    // Use inja to get number of lines, samples, and bands from the input PDS4 label
    std::string result = env.render_file(inputTemplate, pds4Data);

    // Turn this into a Pvl label
    Pvl newLabel;
    newLabel.fromString(result);

    // To read the DN data
    ProcessImport importer;
    if(xmlFileName.removeExtension().addExtension("dat").fileExists()){
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

    // Remove trailing "Z" from PDS4 .xml (on re-ingestion) and create YearDoy keyword in Archive group
    PvlKeyword *startTime = &outLabel.findGroup("Instrument", Pvl::Traverse)["StartTime"];
    QString startTimeString = startTime[0];
    if (startTimeString.endsWith("Z", Qt::CaseInsensitive)) {
      startTimeString.chop(1);
      startTime->setValue(startTimeString);
    }
    iTime stime(startTimeString);
    PvlGroup &archive = outLabel.findGroup("Archive", Pvl::Traverse);
    PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
    archive.addKeyword(yeardoy);

    if (!outputCube->group("Archive").hasKeyword("ObservationId")){
      convertUniqueIdToObservationId(outLabel);
    }

    importer.EndProcess();

    return;
  }

  QString convertUniqueIdToObservationId(Pvl &outputLabel) {
    if (outputLabel.findObject("IsisCube").hasGroup("Mosaic")) {
      return ""; // translation file should auto translate this case to Mosaic group.
                 // For any other product, this ID goes in the Archive group.
    }

    QString target = "";
    if (outputLabel.findObject("IsisCube").hasGroup("Instrument")) {
      target = outputLabel.findGroup("Instrument", Pvl::Traverse)
                          .findKeyword("TargetName")[0];
    }
    else {
      target = outputLabel.findGroup("Mapping", Pvl::Traverse)
                          .findKeyword("TargetName")[0];
    }

    PvlGroup &archiveGroup = outputLabel.findGroup("Archive", Pvl::Traverse);
    QString uniqueId = archiveGroup.findKeyword("UniqueIdentifier")[0];

    QString observationId = "";
    BigInt uniqueIdDecimalValue = uniqueId.toLongLong();
    BigInt operationPeriod = (uniqueIdDecimalValue & 1879048192);
    operationPeriod /= qPow(2,28);
    FileName transFile("$ISISROOT/appdata/translations/TgoCassisOperationPeriod.trn");
    PvlTranslationTable transTable(transFile);
    observationId = transTable.Translate("OperationPeriod", toString(operationPeriod));
    BigInt orbitNumber = (uniqueIdDecimalValue & 268433408);
    orbitNumber /= qPow(2,11);
    observationId += "_";
    observationId += QString("%1").arg(orbitNumber, 6, 10, QChar('0'));

    int orbitPhase = (uniqueIdDecimalValue & 2044);
    if (target.compare("mars", Qt::CaseInsensitive) == 0) {
      orbitPhase /= qPow(2,2);
    }
    else {
      orbitPhase = 900;
    }
    observationId += "_";
    observationId += QString("%1").arg(orbitPhase, 3, 10, QChar('0'));

    int imageType = (uniqueIdDecimalValue & 3);
    observationId += "_";
    observationId += toString(imageType);

    archiveGroup += PvlKeyword("ObservationId", observationId);

    return observationId;
  }
}
