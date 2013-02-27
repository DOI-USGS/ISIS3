#include "Isis.h"

#include <iomanip>
#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QString>
#include <QStringList>

#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "IString.h"
#include "Kernel.h"
#include "KernelDb.h"
#include "Longitude.h"
#include "Process.h"
#include "Pvl.h"
#include "PvlTranslationManager.h"
#include "Table.h"
#include "TextFile.h"

using namespace std;
using namespace Isis;

bool g_ckSmithed = false;
bool g_ckRecon = false;
bool g_ckPredicted = false;
bool g_ckNadir = false;
bool g_spkSmithed = false;
bool g_spkRecon = false;
bool g_spkPredicted = false;
double g_startPad = 0.0;
double g_endPad = 0.0;
QString g_shapeKernelStr;

bool tryKernels(Pvl &labels, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik,
                Kernel sclk, Kernel spk,
                Kernel iak, Kernel dem,
                Kernel exk);

//! Combines all the temp files into one final output file
void packageKernels(QString toFile);

//! Read the spiceinit parameters
void parseParameters(QDomElement parametersElement);

//! Convert a table into an xml tag
QString tableToXml(QString tableName, QString file);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  try {
    Process p;

    g_ckSmithed = false;
    g_ckRecon = false;
    g_ckPredicted = false;
    g_ckNadir = false;
    g_spkSmithed = false;
    g_spkRecon = false;
    g_spkPredicted = false;
    g_shapeKernelStr = "";
    g_startPad = 0.0;
    g_endPad = 0.0;

    // Get the single line of encoded XML from the input file that the client,
    //   spiceinit, sent us.
    TextFile inFile(ui.GetFileName("FROM"));
    QString hexCode;

    // GetLine returns false if it was the last line... so we can't check for
    //   problems really
    inFile.GetLine(hexCode);

    Pvl label;
    QString otherVersion;

    if (!hexCode.isEmpty()) {
      // Convert HEX to XML
      QString xml(QByteArray::fromHex(hexCode.toAscii()).constData());

      // Parse the XML with Qt's XML parser... kindof convoluted, I'm sorry
      QDomDocument document;
      QString error;
      int errorLine, errorCol;
      if (document.setContent(QString(xml), &error, &errorLine, &errorCol)) {
        QDomElement rootElement = document.firstChild().toElement();

        for (QDomNode node = rootElement.firstChild();
            !node .isNull();
            node = node.nextSibling()) {
          QDomElement element = node.toElement();

          // Store off the other isis version
          if (element.tagName() == "isis_version") {
            QString encoded = element.firstChild().toText().data();
            otherVersion =
                QString(QByteArray::fromHex(encoded.toAscii()).constData());
          }
          else if (element.tagName() == "parameters") {
            // Read the spiceinit parameters
            parseParameters(element);
          }
          else if (element.tagName() == "label") {
            // Get the cube label
            QString encoded = element.firstChild().toText().data();
            stringstream labStream;
            labStream <<
                QString(QByteArray::fromHex(encoded.toAscii()).constData());
            labStream >> label;
          }
        }
      }
      else {
        QString err = "Unable to read XML. The reason given was [";
        err += error;
        err += "] on line [" + toString(errorLine) + "] column [";
        err += toString(errorCol) + "]";
        throw IException(IException::Io, err, _FILEINFO_);
      }
    }
    else {
      QString msg = "Unable to read input file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (otherVersion != Application::Version()) {
      QString msg = "The SPICE server only supports the latest Isis version [" +
                    Application::Version() + "], version [" + otherVersion +
                    "] is not compatible";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // This next section looks a lot like spiceinit, its semi-duplicated because
    //   I did not want users to be able to spiceinit a label without cube
    //   data.

    // Set up for getting the mission name
    // Get the directory where the system missions translation table is.
    QString transFile = p.MissionData("base",
                                      "translations/MissionName2DataDir.trn");

    // Get the mission translation manager ready
    PvlTranslationManager missionXlater(label, transFile);

    // Get the mission name so we can search the correct DB's for kernels
    QString mission = missionXlater.Translate("MissionName");

    // Get system base kernels
    unsigned int allowed = 0;
    unsigned int allowedCK = 0;
    unsigned int allowedSPK = 0;
    if (g_ckPredicted)  allowedCK |= Kernel::typeEnum("PREDICTED");
    if (g_ckRecon)      allowedCK |= Kernel::typeEnum("RECONSTRUCTED");
    if (g_ckSmithed)    allowedCK |= Kernel::typeEnum("SMITHED");
    if (g_ckNadir)      allowedCK |= Kernel::typeEnum("NADIR");
    if (g_spkPredicted) allowedSPK |= Kernel::typeEnum("PREDICTED");
    if (g_spkRecon)     allowedSPK |= Kernel::typeEnum("RECONSTRUCTED");
    if (g_spkSmithed)   allowedSPK |= Kernel::typeEnum("SMITHED");

    KernelDb baseKernels(allowed);
    KernelDb ckKernels(allowedCK);
    KernelDb spkKernels(allowedSPK);

    baseKernels.loadSystemDb(mission, label);
    ckKernels.loadSystemDb(mission, label);
    spkKernels.loadSystemDb(mission, label);

    Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
    QList< priority_queue<Kernel> > ck;
    lk        = baseKernels.leapSecond(label);
    pck       = baseKernels.targetAttitudeShape(label);
    targetSpk = baseKernels.targetPosition(label);
    ik        = baseKernels.instrument(label);
    sclk      = baseKernels.spacecraftClock(label);
    iak       = baseKernels.instrumentAddendum(label);
    fk        = ckKernels.frame(label);
    ck        = ckKernels.spacecraftPointing(label);
    spk       = spkKernels.spacecraftPosition(label);

    if (g_ckNadir) {
      // Only add nadir if no spacecraft pointing found
      QStringList nadirCk;
      nadirCk.push_back("Nadir");
      // if a priority queue already exists, add Nadir with low priority of 0
      if (ck.size() > 0) {
        ck[0].push(Kernel((Kernel::Type)0, nadirCk));
      }
      // if no queue exists, create a nadir queue
      else {
        priority_queue<Kernel> nadirQueue;
        nadirQueue.push(Kernel((Kernel::Type)0, nadirCk));
        ck.push_back(nadirQueue);
      }
    }

    // Get shape kernel
    if (g_shapeKernelStr == "system") {
      dem = baseKernels.dem(label);
    }
    else if (g_shapeKernelStr != "ellipsoid") {
      stringstream demPvlKeyStream;
      demPvlKeyStream << "ShapeModel = " + g_shapeKernelStr;
      PvlKeyword key;
      demPvlKeyStream >> key;

      for (int value = 0; value < key.Size(); value++) {
        dem.push_back(key[value]);
      }
    }

    bool kernelSuccess = false;

    if (ck.size() == 0 || ck.at(0).size() == 0) {
      throw IException(IException::Unknown,
                       "No Camera Kernel found for the image [" +
                        ui.GetFileName("FROM") + "]",
                       _FILEINFO_);
    }

    while (ck.at(0).size() != 0 && !kernelSuccess) {
      // create an empty kernel 
      Kernel realCkKernel;
      QStringList ckKernelList;

      // if multiple priority queues exist, then add the top priority ck
      // from each queue to the list of kernels to be loaded.
      if (ck.size() > 1) {
        for (unsigned int i = ck.size() - 1; i >= 0; i--) {
          if (ck.at(i).size() != 0) {
            Kernel topPriority = ck.at(i).top();
            ckKernelList.append(topPriority.kernels());
            // set the type to equal the type of the to priority of the first
            //queue 
            realCkKernel.setType(topPriority.type()); 
          }
        }
      }
      // pop the top priority ck off only the first queue so that the next 
      // iteration will test the next highest priority of the first queue with
      // the top priority of each of the other queues.
      ck[0].pop();

      // Merge SpacecraftPointing and Frame into ck
      for (int i = 0; i < fk.size(); i++) {
        realCkKernel.push_back(fk[i]);
      }

      kernelSuccess = tryKernels(label, p, lk, pck, targetSpk,
                                 realCkKernel, fk, ik, sclk, spk,
                                 iak, dem, exk);
    }

    if (!kernelSuccess) {
      throw IException(IException::Unknown,
                       "Unable to initialize camera model",
                       _FILEINFO_);
    }
    else {
      packageKernels(ui.GetFileName("TO"));
    }

    p.EndProcess();
  }
  catch (...) {
    // We failed at something, delete the temp files...
    QString outFile = ui.GetFileName("TO");
    QFile pointingFile(outFile + ".pointing");
    if (pointingFile.exists()) pointingFile.remove();

    QFile positionFile(outFile + ".position");
    if (positionFile.exists()) positionFile.remove();

    QFile bodyRotFile(outFile + ".bodyrot");
    if (bodyRotFile.exists()) bodyRotFile.remove();

    QFile sunFile(outFile + ".sun");
    if (sunFile.exists()) sunFile.remove();

    throw;
  }
}

bool tryKernels(Pvl &label, Process &p,
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
  spkQuality.AddValue(Kernel::typeEnum(spk.type()));
  currentKernels.AddKeyword(spkQuality, Pvl::Replace);

  PvlKeyword ckQuality("InstrumentPointingQuality");
  ckQuality.AddValue(Kernel::typeEnum(ck.type()));
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
  if (g_startPad > DBL_EPSILON)
    currentKernels.AddKeyword(PvlKeyword("StartPadding", toString(g_startPad), "seconds"));

  if (g_endPad > DBL_EPSILON)
    currentKernels.AddKeyword(PvlKeyword("EndPadding", toString(g_endPad), "seconds"));

  currentKernels.AddKeyword(
      PvlKeyword("CameraVersion", toString(CameraFactory::CameraVersion(lab))),
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
      applicationLog.Write(ui.GetFileName("TO") + ".print");
    }
    catch (IException &e) {
      Pvl errPvl = e.toPvl();

      if (errPvl.Groups() > 0)
        currentKernels += PvlKeyword("Error",
            errPvl.Group(errPvl.Groups() - 1)["Message"][0]);

      Application::Log(currentKernels);
      throw e;
    }

    Table ckTable = cam->instrumentRotation()->Cache("InstrumentPointing");
    ckTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    ckTable.Label() += PvlKeyword("Kernels");

    for (int i = 0; i < ckKeyword.Size(); i++)
      ckTable.Label()["Kernels"].AddValue(ckKeyword[i]);

    ckTable.Write(ui.GetFileName("TO") + ".pointing");

    Table spkTable = cam->instrumentPosition()->Cache("InstrumentPosition");
    spkTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    spkTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < spkKeyword.Size(); i++)
      spkTable.Label()["Kernels"].AddValue(spkKeyword[i]);

    spkTable.Write(ui.GetFileName("TO") + ".position");

    Table bodyTable = cam->bodyRotation()->Cache("BodyRotation");
    bodyTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    bodyTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < targetSpkKeyword.Size(); i++)
      bodyTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

    for (int i = 0; i < pckKeyword.Size(); i++)
      bodyTable.Label()["Kernels"].AddValue(pckKeyword[i]);

    bodyTable.Label() += PvlKeyword("SolarLongitude",
                                    toString(cam->solarLongitude().degrees()));
    bodyTable.Write(ui.GetFileName("TO") + ".bodyrot");

    Table sunTable = cam->sunPosition()->Cache("SunPosition");
    sunTable.Label() += PvlKeyword("Description", "Created by spiceinit");
    sunTable.Label() += PvlKeyword("Kernels");
    for (int i = 0; i < targetSpkKeyword.Size(); i++)
      sunTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

    sunTable.Write(ui.GetFileName("TO") + ".sun");

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
    kernelsLabels.Write(ui.GetFileName("TO") + ".lab");
  }
  catch (IException &) {
    return false;
  }

  return true;
}


QString tableToXml(QString tableName, QString file) {
  QString xml;
  xml += "    <" + tableName + ">\n";

  QFile tableFile(file);
  if (!tableFile.open(QIODevice::ReadOnly)) {
    QString msg = "Unable to read temporary file [" + file + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  QByteArray data = tableFile.readAll();
  xml += QString(data.toHex().constData()) + "\n";
  tableFile.close();
  // we should now be completely done with this temp file
  tableFile.remove();

  xml += "    </" + tableName + ">\n";
  return xml;
}


void parseParameters(QDomElement parametersElement) {
  for (QDomNode node = parametersElement.firstChild();
       !node .isNull();
       node = node.nextSibling()) {
    QDomElement element = node.toElement();

    if (element.tagName() == "cksmithed") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_ckSmithed = (attribute.value() == "yes");
    }
    else if (element.tagName() == "ckrecon") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_ckRecon = (attribute.value() == "yes");
    }
    else if (element.tagName() == "ckpredicted") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_ckPredicted = (attribute.value() == "yes");
    }
    else if (element.tagName() == "cknadir") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_ckNadir = (attribute.value() == "yes");
    }
    else if (element.tagName() == "spksmithed") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_spkSmithed = (attribute.value() == "yes");
    }
    else if (element.tagName() == "spkrecon") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_spkRecon = (attribute.value() == "yes");
    }
    else if (element.tagName() == "spkpredicted") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_spkPredicted = (attribute.value() == "yes");
    }
    else if (element.tagName() == "shape") {
      QDomNode node = element.attributes().namedItem("value");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_shapeKernelStr = attribute.value();
    }
    else if (element.tagName() == "startpad") {
      QDomNode node = element.attributes().namedItem("time");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_startPad = attribute.value().toDouble();
    }
    else if (element.tagName() == "endpad") {
      QDomNode node = element.attributes().namedItem("time");
      QDomAttr attribute = *((QDomAttr *)&node);
      g_endPad = attribute.value().toDouble();
    }
  }
}


void packageKernels(QString toFile) {
  QString xml;
  xml += "<spice_data>\n";

  xml += "  <application_log>\n";

  QString logFile(toFile + ".print");
  Pvl logMessage(logFile);
  remove(logFile.toAscii().data());
  stringstream logStream;
  logStream << logMessage;
  xml +=
      QString(QByteArray(logStream.str().c_str()).toHex().constData()) + "\n";
  xml += "  </application_log>\n";

  xml += "  <kernels_label>\n";

  QString kernLabelsFile(toFile + ".lab");
  Pvl kernLabels(kernLabelsFile);
  remove(kernLabelsFile.toAscii().data());
  stringstream labelStream;
  labelStream << kernLabels;

  xml +=
      QString(QByteArray(labelStream.str().c_str()).toHex().constData()) + "\n";

  xml += "  </kernels_label>\n";

  xml += "  <tables>\n";
  xml += tableToXml("instrument_pointing", toFile + ".pointing");
  xml += tableToXml("instrument_position", toFile + ".position");
  xml += tableToXml("body_rotation", toFile + ".bodyrot");
  xml += tableToXml("sun_position", toFile + ".sun");

  xml += "  </tables>\n";
  xml += "</spice_data>\n";
  QString encodedXml(xml.toAscii().toHex().constData());

  QFile finalOutput(toFile);
  finalOutput.open(QIODevice::WriteOnly);
  finalOutput.write(encodedXml.toAscii());
  finalOutput.close();
}
