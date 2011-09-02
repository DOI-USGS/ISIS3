#include "Isis.h"

#include <iomanip>
#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "iString.h"
#include "KernelDb.h"
#include "Longitude.h"
#include "Process.h"
#include "Pvl.h"
#include "PvlTranslationManager.h"
#include "Table.h"
#include "TextFile.h"

using namespace std;
using namespace Isis;

bool ckSmithed = false;
bool ckRecon = false;
bool ckPredicted = false;
bool ckNadir = false;
bool spkSmithed = false;
bool spkRecon = false;
bool spkPredicted = false;
double startPad = 0.0;
double endPad = 0.0;
iString shapeKernelStr;

bool TryKernels(Pvl &labels, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik,
                Kernel sclk, Kernel spk,
                Kernel iak, Kernel dem,
                Kernel exk);

//! Combines all the temp files into one final output file
void PackageKernels(iString toFile);

//! Read the spiceinit parameters
void ParseParameters(QDomElement parametersElement);

//! Convert a table into an xml tag
iString TableToXml(iString tableName, iString file);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  try {
    Process p;

    ckSmithed = false;
    ckRecon = false;
    ckPredicted = false;
    ckNadir = false;
    spkSmithed = false;
    spkRecon = false;
    spkPredicted = false;
    shapeKernelStr = "";
    startPad = 0.0;
    endPad = 0.0;

    // Get the single line of encoded XML from the input file that the client,
    //   spiceinit, sent us.
    TextFile inFile(ui.GetFilename("FROM"));
    iString hexCode;

    // GetLine returns false if it was the last line... so we can't check for
    //   problems really
    inFile.GetLine(hexCode);

    Pvl label;
    iString otherVersion;

    if (!hexCode.empty()) {
      // Convert HEX to XML
      iString xml(QByteArray::fromHex(QByteArray(hexCode.c_str())).constData());

      // Parse the XML with Qt's XML parser... kindof convoluted, I'm sorry
      QDomDocument document;
      QString error;
      int errorLine, errorCol;
      if (document.setContent(QString(xml.c_str()), &error,
                              &errorLine, &errorCol)) {
        QDomElement rootElement = document.firstChild().toElement();

        for (QDomNode node = rootElement.firstChild();
            !node .isNull();
            node = node.nextSibling()) {
          QDomElement element = node.toElement();

          // Store off the other isis version
          if (element.tagName() == "isis_version") {
            QString encoded = element.firstChild().toText().data();
            otherVersion =
                iString(QByteArray::fromHex(encoded.toAscii()).constData());
          }
          else if (element.tagName() == "parameters") {
            // Read the spiceinit parameters
            ParseParameters(element);
          }
          else if (element.tagName() == "label") {
            // Get the cube label
            QString encoded = element.firstChild().toText().data();
            stringstream labStream;
            labStream <<
                iString(QByteArray::fromHex(encoded.toAscii()).constData());
            labStream >> label;
          }
        }
      }
      else {
        iString err = "Unable to read XML. The reason given was [";
        err += error.toStdString();
        err += "] on line [" + iString(errorLine) + "] column [";
        err += iString(errorCol) + "]";
        throw iException::Message(iException::Io, err, _FILEINFO_);
      }
    }
    else {
      iString msg = "Unable to read input file";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    if (otherVersion != Application::Version()) {
      iString msg = "The SPICE server only supports the latest Isis version [" +
                    Application::Version() + "], version [" + otherVersion +
                    "] is not compatible";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // This next section looks a lot like spiceinit, its semi-duplicated because
    //   I did not want users to be able to spiceinit a label without cube
    //   data.

    // Set up for getting the mission name
    // Get the directory where the system missions translation table is.
    string transFile = p.MissionData("base",
                                     "translations/MissionName2DataDir.trn");

    // Get the mission translation manager ready
    PvlTranslationManager missionXlater(label, transFile);

    // Get the mission name so we can search the correct DB's for kernels
    string mission = missionXlater.Translate("MissionName");

    // Get system base kernels
    unsigned int allowed = 0;
    unsigned int allowedCK = 0;
    unsigned int allowedSPK = 0;
    if (ckPredicted)  allowedCK |= spiceInit::kernelTypeEnum("PREDICTED");
    if (ckRecon)      allowedCK |= spiceInit::kernelTypeEnum("RECONSTRUCTED");
    if (ckSmithed)    allowedCK |= spiceInit::kernelTypeEnum("SMITHED");
    if (ckNadir)      allowedCK |= spiceInit::kernelTypeEnum("NADIR");
    if (spkPredicted) allowedSPK |= spiceInit::kernelTypeEnum("PREDICTED");
    if (spkRecon)     allowedSPK |= spiceInit::kernelTypeEnum("RECONSTRUCTED");
    if (spkSmithed)   allowedSPK |= spiceInit::kernelTypeEnum("SMITHED");

    KernelDb baseKernels(allowed);
    KernelDb ckKernels(allowedCK);
    KernelDb spkKernels(allowedSPK);

    baseKernels.LoadSystemDb(mission);
    ckKernels.LoadSystemDb(mission);
    spkKernels.LoadSystemDb(mission);

    Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
    std::priority_queue< Kernel > ck;
    lk        = baseKernels.LeapSecond(label);
    pck       = baseKernels.TargetAttitudeShape(label);
    targetSpk = baseKernels.TargetPosition(label);
    ik        = baseKernels.Instrument(label);
    sclk      = baseKernels.SpacecraftClock(label);
    iak       = baseKernels.InstrumentAddendum(label);
    fk        = ckKernels.Frame(label);
    ck        = ckKernels.SpacecraftPointing(label);
    spk       = spkKernels.SpacecraftPosition(label);

    if (ckNadir) {
      // Only add nadir if no spacecraft pointing found
      std::vector<std::string> kernels;
      kernels.push_back("Nadir");
      ck.push(Kernel((spiceInit::kernelTypes)0, kernels));
    }

    // Get shape kernel
    if (shapeKernelStr == "system") {
      dem = baseKernels.Dem(label);
    }
    else if (shapeKernelStr != "ellipsoid") {
      stringstream demPvlKeyStream;
      demPvlKeyStream << "ShapeModel = " + shapeKernelStr;
      PvlKeyword key;
      demPvlKeyStream >> key;

      for (int value = 0; value < key.Size(); value++) {
        dem.push_back(key[value]);
      }
    }

    bool kernelSuccess = false;

    if (ck.size() == 0) {
      throw iException::Message(iException::Camera,
                                "No Camera Kernel found for the image [" +
                                  ui.GetFilename("FROM") + "]",
                                _FILEINFO_);
    }

    while (ck.size() != 0 && !kernelSuccess) {
      Kernel realCkKernel = ck.top();
      ck.pop();

      // Merge SpacecraftPointing and Frame into ck
      for (int i = 0; i < fk.size(); i++) {
        realCkKernel.push_back(fk[i]);
      }

      kernelSuccess = TryKernels(label, p, lk, pck, targetSpk,
                                 realCkKernel, fk, ik, sclk, spk,
                                 iak, dem, exk);
    }

    if (!kernelSuccess) {
      throw iException::Message(iException::Camera,
                                "Unable to initialize camera model",
                                _FILEINFO_);
    }
    else {
      PackageKernels(ui.GetFilename("TO"));
    }

    p.EndProcess();
  }
  catch (...) {
    // We failed at something, delete the temp files...
    iString outFile = ui.GetFilename("TO");
    QFile pointingFile(QString::fromStdString(outFile + ".pointing"));
    if (pointingFile.exists()) pointingFile.remove();

    QFile positionFile(QString::fromStdString(outFile + ".position"));
    if (positionFile.exists()) positionFile.remove();

    QFile bodyRotFile(QString::fromStdString(outFile + ".bodyrot"));
    if (bodyRotFile.exists()) bodyRotFile.remove();

    QFile sunFile(QString::fromStdString(outFile + ".sun"));
    if (sunFile.exists()) sunFile.remove();

    throw;
  }
}

bool TryKernels(Pvl &label, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik, Kernel sclk,
                Kernel spk, Kernel iak,
                Kernel dem, Kernel exk) {
  UserInterface &ui = Application::GetUserInterface();

  // copy the label
  Pvl lab = label;

  // Add the new kernel files to the existing kernels group
  PvlKeyword lkKeyword("LeapSecond");
  PvlKeyword pckKeyword("TargetAttitudeShape");
  PvlKeyword targetSpkKeyword("TargetPosition");
  PvlKeyword ckKeyword("InstrumentPointing");
  PvlKeyword ikKeyword("Instrument");
  PvlKeyword sclkKeyword("SpacecraftClock");
  PvlKeyword spkKeyword("InstrumentPosition");
  PvlKeyword iakKeyword("InstrumentAddendum");
  PvlKeyword emptyDemKeyword("ShapeModel");
  PvlKeyword demKeyword("ShapeModel");
  PvlKeyword exkKeyword("Extra");

  for (int i = 0; i < lk.size(); i++) {
    lkKeyword.AddValue(lk[i]);
  }
  for (int i = 0; i < pck.size(); i++) {
    pckKeyword.AddValue(pck[i]);
  }
  for (int i = 0; i < targetSpk.size(); i++) {
    targetSpkKeyword.AddValue(targetSpk[i]);
  }
  for (int i = 0; i < ck.size(); i++) {
    ckKeyword.AddValue(ck[i]);
  }
  for (int i = 0; i < ik.size(); i++) {
    ikKeyword.AddValue(ik[i]);
  }
  for (int i = 0; i < sclk.size(); i++) {
    sclkKeyword.AddValue(sclk[i]);
  }
  for (int i = 0; i < spk.size(); i++) {
    spkKeyword.AddValue(spk[i]);
  }
  for (int i = 0; i < iak.size(); i++) {
    iakKeyword.AddValue(iak[i]);
  }
  for (int i = 0; i < dem.size(); i++) {
    demKeyword.AddValue(dem[i]);
  }
  for (int i = 0; i < exk.size(); i++) {
    exkKeyword.AddValue(exk[i]);
  }

  PvlGroup originalKernels = lab.FindGroup("Kernels", Pvl::Traverse);
  PvlGroup &currentKernels = lab.FindGroup("Kernels", Pvl::Traverse);
  currentKernels.AddKeyword(lkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(pckKeyword, Pvl::Replace);
  currentKernels.AddKeyword(targetSpkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(ckKeyword, Pvl::Replace);
  currentKernels.AddKeyword(ikKeyword, Pvl::Replace);
  currentKernels.AddKeyword(sclkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(spkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(iakKeyword, Pvl::Replace);
  currentKernels.AddKeyword(emptyDemKeyword, Pvl::Replace);

  // report qualities
  PvlKeyword spkQuality("InstrumentPositionQuality");
  spkQuality.AddValue(spiceInit::kernelTypeEnum(spk.kernelType));
  currentKernels.AddKeyword(spkQuality, Pvl::Replace);

  PvlKeyword ckQuality("InstrumentPointingQuality");
  ckQuality.AddValue(spiceInit::kernelTypeEnum(ck.kernelType));
  currentKernels.AddKeyword(ckQuality, Pvl::Replace);

  if (!exkKeyword.IsNull())
    currentKernels.AddKeyword(exkKeyword, Pvl::Replace);
  else if (currentKernels.HasKeyword("EXTRA"))
    currentKernels.DeleteKeyword("EXTRA");

  // Get rid of old keywords from previously inited cubes
  if (currentKernels.HasKeyword("SpacecraftPointing"))
    currentKernels.DeleteKeyword("SpacecraftPointing");

  if (currentKernels.HasKeyword("SpacecraftPosition"))
    currentKernels.DeleteKeyword("SpacecraftPosition");

  if (currentKernels.HasKeyword("ElevationModel"))
    currentKernels.DeleteKeyword("ElevationModel");

  if (currentKernels.HasKeyword("Frame"))
    currentKernels.DeleteKeyword("Frame");

  if (currentKernels.HasKeyword("StartPadding"))
    currentKernels.DeleteKeyword("StartPadding");

  if (currentKernels.HasKeyword("EndPadding"))
    currentKernels.DeleteKeyword("EndPadding");

  // Add any time padding the user specified to the spice group
  if (startPad > DBL_EPSILON)
    currentKernels.AddKeyword(PvlKeyword("StartPadding", startPad, "seconds"));

  if (endPad > DBL_EPSILON)
    currentKernels.AddKeyword(PvlKeyword("EndPadding", endPad, "seconds"));

  currentKernels.AddKeyword(
      PvlKeyword("CameraVersion", CameraFactory::CameraVersion(lab)),
      Pvl::Replace);

  // Create the camera so we can get blobs if necessary
  try {
    Camera *cam = NULL;
    try {
      cam = CameraFactory::Create(lab);

      // If success then pretend we had the shape model keyword in there...
      //   this doesn't actually effect the blobs that we care about
      currentKernels.AddKeyword(demKeyword, Pvl::Replace);
      Pvl applicationLog;
      applicationLog += lab.FindGroup("Kernels", Pvl::Traverse);
      applicationLog.Write(ui.GetFilename("TO") + ".print");
    }
    catch (iException &e) {
      Pvl errPvl = e.PvlErrors();

      if (errPvl.Groups() > 0)
        currentKernels += PvlKeyword("Error",
            errPvl.Group(errPvl.Groups() - 1)["Message"][0]);

      Application::Log(currentKernels);
      throw e;
    }

    Table ckTable = cam->InstrumentRotation()->Cache("InstrumentPointing");
    ckTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    ckTable.Label() += PvlKeyword("Kernels");

    for (int i = 0; i < ckKeyword.Size(); i++)
      ckTable.Label()["Kernels"].AddValue(ckKeyword[i]);

    ckTable.Write(ui.GetFilename("TO") + ".pointing");

    Table spkTable = cam->InstrumentPosition()->Cache("InstrumentPosition");
    spkTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    spkTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < spkKeyword.Size(); i++)
      spkTable.Label()["Kernels"].AddValue(spkKeyword[i]);

    spkTable.Write(ui.GetFilename("TO") + ".position");

    Table bodyTable = cam->BodyRotation()->Cache("BodyRotation");
    bodyTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    bodyTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < targetSpkKeyword.Size(); i++)
      bodyTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

    for (int i = 0; i < pckKeyword.Size(); i++)
      bodyTable.Label()["Kernels"].AddValue(pckKeyword[i]);

    bodyTable.Label() += PvlKeyword("SolarLongitude",
                                    cam->SolarLongitude().GetDegrees());
    bodyTable.Write(ui.GetFilename("TO") + ".bodyrot");

    Table sunTable = cam->SunPosition()->Cache("SunPosition");
    sunTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    sunTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < targetSpkKeyword.Size(); i++)
      sunTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

    sunTable.Write(ui.GetFilename("TO") + ".sun");

    //  Save original kernels in keyword before changing to Table
    PvlKeyword origCk = currentKernels["InstrumentPointing"];
    PvlKeyword origSpk = currentKernels["InstrumentPosition"];
    PvlKeyword origTargPos = currentKernels["TargetPosition"];

    currentKernels["InstrumentPointing"] = "Table";
    for (int i = 0; i < origCk.Size(); i++)
      currentKernels["InstrumentPointing"].AddValue(origCk[i]);

    currentKernels["InstrumentPosition"] = "Table";
    for (int i = 0; i < origSpk.Size(); i++)
      currentKernels["InstrumentPosition"].AddValue(origSpk[i]);

    currentKernels["TargetPosition"] = "Table";
    for (int i = 0; i < origTargPos.Size(); i++)
      currentKernels["TargetPosition"].AddValue(origTargPos[i]);

    Pvl kernelsLabels;
    kernelsLabels += lab.FindGroup("Kernels", Pvl::Traverse);
    kernelsLabels += cam->getStoredNaifKeywords();
    kernelsLabels.Write(ui.GetFilename("TO") + ".lab");
  }
  catch (iException &e) {
    e.Clear();
    return false;
  }

  return true;
}


iString TableToXml(iString tableName, iString file) {
  iString xml;
  xml += "    <" + tableName + ">\n";

  QFile tableFile(file);
  if (!tableFile.open(QIODevice::ReadOnly)) {
    iString msg = "Unable to read temporary file [" + file + "]";
    throw iException::Message(iException::Io, msg, _FILEINFO_);
  }

  QByteArray data = tableFile.readAll();
  xml += iString(data.toHex().constData()) + "\n";
  tableFile.close();
  // we should now be completely done with this temp file
  tableFile.remove();

  xml += "    </" + tableName + ">\n";
  return xml;
}


void ParseParameters(QDomElement parametersElement) {
  for (QDomNode node = parametersElement.firstChild();
       !node .isNull();
       node = node.nextSibling()) {
    QDomElement element = node.toElement();

    if (element.tagName() == "cksmithed") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      ckSmithed = (attribute.value() == "yes");
    }
    else if (element.tagName() == "ckrecon") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      ckRecon = (attribute.value() == "yes");
    }
    else if (element.tagName() == "ckpredicted") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      ckPredicted = (attribute.value() == "yes");
    }
    else if (element.tagName() == "cknadir") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      ckNadir = (attribute.value() == "yes");
    }
    else if (element.tagName() == "spksmithed") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      spkSmithed = (attribute.value() == "yes");
    }
    else if (element.tagName() == "spkrecon") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      spkRecon = (attribute.value() == "yes");
    }
    else if (element.tagName() == "spkpredicted") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      spkPredicted = (attribute.value() == "yes");
    }
    else if (element.tagName() == "shape") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      shapeKernelStr = attribute.value();
    }
    else if (element.tagName() == "startpad") {
      QDomNode node = element.attributes().namedItem("time");
      QDomAttr attribute = *((QDomAttr *)&node);
      startPad = attribute.value().toDouble();
    }
    else if (element.tagName() == "endpad") {
      QDomNode node = element.attributes().namedItem("time");
      QDomAttr attribute = *((QDomAttr *)&node);
      endPad = attribute.value().toDouble();
    }
  }
}


void PackageKernels(iString toFile) {
  iString xml;
  xml += "<spice_data>\n";

  xml += "  <application_log>\n";

  iString logFile(toFile + ".print");
  Pvl logMessage(logFile);
  remove(logFile.c_str());
  stringstream logStream;
  logStream << logMessage;
  xml +=
      iString(QByteArray(logStream.str().c_str()).toHex().constData()) + "\n";
  xml += "  </application_log>\n";

  xml += "  <kernels_label>\n";

  iString kernLabelsFile(toFile + ".lab");
  Pvl kernLabels(kernLabelsFile);
  remove(kernLabelsFile.c_str());
  stringstream labelStream;
  labelStream << kernLabels;

  xml +=
      iString(QByteArray(labelStream.str().c_str()).toHex().constData()) + "\n";

  xml += "  </kernels_label>\n";

  xml += "  <tables>\n";
  xml += TableToXml("instrument_pointing", toFile + ".pointing");
  xml += TableToXml("instrument_position", toFile + ".position");
  xml += TableToXml("body_rotation", toFile + ".bodyrot");
  xml += TableToXml("sun_position", toFile + ".sun");

  xml += "  </tables>\n";
  xml += "</spice_data>\n";
  iString encodedXml(QByteArray(xml.c_str()).toHex().constData());

  QFile finalOutput(toFile);
  finalOutput.open(QIODevice::WriteOnly);
  finalOutput.write(encodedXml.c_str());
  finalOutput.close();
}
