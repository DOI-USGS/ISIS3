#include "spiceinit.h"

#include <iomanip>
#include <queue>

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "Kernel.h"
#include "KernelDb.h"
#include "Longitude.h"
#include "Process.h"
#include "PvlToPvlTranslationManager.h"
#include "SpiceClient.h"
#include "SpiceClientStarter.h"
#include "Table.h"

using namespace std;
using namespace Isis;

namespace Isis {

  spiceinitOptions getSpiceinitOptions(UserInterface &ui);

  void getUserEnteredKernel(Kernel &kernel,
                            const std::vector<QString> &userKernels);
  bool tryKernels(Cube *icube, Process &p,
                  const spiceinitOptions &options,
                  Pvl *log,
                  Kernel lk, Kernel pck,
                  Kernel targetSpk, Kernel ck,
                  Kernel fk, Kernel ik,
                  Kernel sclk, Kernel spk,
                  Kernel iak, Kernel dem,
                  Kernel exk);

  void requestSpice(Cube *icube,
                    Pvl &labels,
                    QString missionName,
                    const spiceinitOptions &options,
                    Pvl *log);


  /**
   * Spiceinit a cube in an Application
   *
   * @param ui The Application UI
   * @param(out) log The Pvl that attempted kernel sets will be logged to
   */
  void spiceinit(UserInterface &ui, Pvl *log) {
    // Open the input cube
    Process p;
    CubeAttributeInput cai;
    Cube *icube = p.SetInputCube(ui.GetFileName("FROM"), cai, ReadWrite);

    spiceinitOptions options = getSpiceinitOptions(ui);

    spiceinit(icube, options, log);

    p.EndProcess();
  }


  /**
   * Spiceinit a Cube
   *
   * @param cube The Cube to spiceinit
   * @param options The options for how the cube should be spiceinit'd
   * @param(out) log The Pvl that attempted kernel sets will be logged to
   */
  void spiceinit(Cube *icube, const spiceinitOptions &options, Pvl *log) {
    // Open the input cube
    Process p;
    p.SetInputCube(icube);

    // Make sure at least one CK & SPK quality was selected
    if (!(options.cksmithed || options.ckrecon || options.ckpredicted || options.cknadir)) {
      QString msg = "At least one CK quality must be selected";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    if (!(options.spksmithed || options.spkrecon || options.spkpredicted)) {
      QString msg = "At least one SPK quality must be selected";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure it is not projected
    Projection *proj = NULL;
    try {
      proj = icube->projection();
    }
    catch(IException &) {
      proj = NULL;
    }

    if (proj != NULL) {
      QString msg = "Can not initialize SPICE for a map projected cube";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    Pvl lab = *icube->label();

    // if cube has existing polygon delete it
    if (icube->label()->hasObject("Polygon")) {
      icube->label()->deleteObject("Polygon");
    }

    // Set up for getting the mission name
    // Get the directory where the system missions translation table is.
    QString transFile = p.MissionData("base",
                                      "translations/MissionName2DataDir.trn");

    // Get the mission translation manager ready
    PvlToPvlTranslationManager missionXlater(lab, transFile);

    // Get the mission name so we can search the correct DB's for kernels
    QString mission = missionXlater.Translate("MissionName");

    if (options.web) {
      requestSpice(icube, *icube->label(), mission, options, log);
    }
    else {
      // Get system base kernels
      unsigned int allowed = 0;
      unsigned int allowedCK = 0;
      unsigned int allowedSPK = 0;

      if (options.ckpredicted)
        allowedCK |= Kernel::typeEnum("PREDICTED");
      if (options.ckrecon)
        allowedCK |= Kernel::typeEnum("RECONSTRUCTED");
      if (options.cksmithed)
        allowedCK |= Kernel::typeEnum("SMITHED");
      if (options.cknadir)
        allowedCK |= Kernel::typeEnum("NADIR");
      if (options.spkpredicted)
        allowedSPK |= Kernel::typeEnum("PREDICTED");
      if (options.spkrecon)
        allowedSPK |= Kernel::typeEnum("RECONSTRUCTED");
      if (options.spksmithed)
        allowedSPK |= Kernel::typeEnum("SMITHED");

      KernelDb baseKernels(allowed);
      KernelDb ckKernels(allowedCK);
      KernelDb spkKernels(allowedSPK);

      baseKernels.loadSystemDb(mission, lab);
      ckKernels.loadSystemDb(mission, lab);
      spkKernels.loadSystemDb(mission, lab);

      Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
      QList< priority_queue<Kernel> > ck;
      lk        = baseKernels.leapSecond(lab);
      pck       = baseKernels.targetAttitudeShape(lab);
      targetSpk = baseKernels.targetPosition(lab);
      ik        = baseKernels.instrument(lab);
      sclk      = baseKernels.spacecraftClock(lab);
      iak       = baseKernels.instrumentAddendum(lab);
      fk        = ckKernels.frame(lab);
      ck        = ckKernels.spacecraftPointing(lab);
      spk       = spkKernels.spacecraftPosition(lab);

      if (options.cknadir) {
        // Only add nadir if no spacecraft pointing found, so we will set (priority) type to 0.
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

      // Get user defined kernels and override ones already found
      getUserEnteredKernel(lk, options.lsk);
      getUserEnteredKernel(pck, options.pck);
      getUserEnteredKernel(targetSpk, options.tspk);
      getUserEnteredKernel(fk, options.fk);
      getUserEnteredKernel(ik, options.ik);
      getUserEnteredKernel(sclk, options.sclk);
      getUserEnteredKernel(spk, options.spk);
      getUserEnteredKernel(iak, options.iak);
      getUserEnteredKernel(exk, options.extra);

      // Get shape kernel
      if (options.shape == spiceinitOptions::USER) {
        getUserEnteredKernel(dem, options.model);
      }
      else if (options.shape == spiceinitOptions::SYSTEM) {
        dem = baseKernels.dem(lab);
      }

      bool kernelSuccess = false;

      if ((ck.size() == 0 || ck.at(0).size() == 0) && options.ck.empty()) {
        // no ck was found in system and user did not enter ck, throw error
        throw IException(IException::Unknown,
                         "No Camera Kernels found for the image [" + icube->fileName()
                         + "]",
                         _FILEINFO_);
      }
      else if (!options.ck.empty()) {
        // if user entered ck
        // empty ck queue list found in system
        while (ck.size()) {
          ck.pop_back();
        }
        // create queue with empty kernel so ck[0].size() != 0,
        // this allows us to get into the coming while loop
        priority_queue< Kernel > emptyKernelQueue;
        emptyKernelQueue.push(Kernel());
        ck.push_back(emptyKernelQueue);
      }

      // while the first queue is not empty, loop through it until tryKernels() succeeds
      while (ck.at(0).size() != 0 && !kernelSuccess) {
        // create an empty kernel
        Kernel realCkKernel;
        QStringList ckKernelList;

        // if the user entered ck kernels, populate the ck kernel list with the
        // user entered files
        if (!options.ck.empty()) {
          // convert user entered std vector to QStringList and add to ckKernelList
          ckKernelList = QVector<QString>::fromStdVector(options.ck).toList();
        }
        else {// loop through cks found in the system

          // Add the list of cks from each Kernel object at the top of each
          // priority queue. If multiple priority queues exist, we will not
          // pop of the top priority from any of the queues except for the
          // first one.  So each time tryKernels() fails, the same files
          // will be loaded with the next priority from the first queue.
          for (int i = ck.size() - 1; i >= 0; i--) {
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
          ckKernelList.push_back(fk[i]);
        }

        realCkKernel.setKernels(ckKernelList);

        kernelSuccess = tryKernels(icube, p, options, log, lk, pck, targetSpk,
                                   realCkKernel, fk, ik, sclk, spk, iak, dem, exk);
      }

      if (!kernelSuccess)
        throw IException(IException::Unknown,
                         "Unable to initialize camera model",
                         _FILEINFO_);
    }
    p.EndProcess();
  }


  /**
   * Parse the User Interface into an options struct.
   *
   * @param ui The UI from an application
   *
   * @return An options object with the parameters from the UI.
   */
  spiceinitOptions getSpiceinitOptions(UserInterface &ui) {
    spiceinitOptions options;

    options.web = ui.GetBoolean("WEB");
    options.attach = ui.GetBoolean("ATTACH");
    options.cksmithed = ui.GetBoolean("CKSMITHED");
    options.ckrecon = ui.GetBoolean("CKRECON");
    options.ckpredicted = ui.GetBoolean("CKPREDICTED");
    options.cknadir = ui.GetBoolean("CKNADIR");
    options.spksmithed = ui.GetBoolean("SPKSMITHED");
    options.spkrecon = ui.GetBoolean("SPKRECON");
    options.spkpredicted = ui.GetBoolean("SPKPREDICTED");
    if (ui.WasEntered("LS")) {
      ui.GetAsString("LS", options.lsk);
    }
    if (ui.WasEntered("PCK")) {
      ui.GetAsString("PCK", options.pck);
    }
    if (ui.WasEntered("TSPK")) {
      ui.GetAsString("TSPK", options.tspk);
    }
    if (ui.WasEntered("IK")) {
      ui.GetAsString("IK", options.ik);
    }
    if (ui.WasEntered("SCLK")) {
      ui.GetAsString("SCLK", options.sclk);
    }
    if (ui.WasEntered("CK")) {
      ui.GetAsString("CK", options.ck);
    }
    if (ui.WasEntered("FK")) {
      ui.GetAsString("FK", options.fk);
    }
    if (ui.WasEntered("SPK")) {
      ui.GetAsString("SPK", options.spk);
    }
    if (ui.WasEntered("IAK")) {
      ui.GetAsString("IAK", options.iak);
    }
    if (ui.WasEntered("EXTRA")) {
      ui.GetAsString("EXTRA", options.extra);
    }
    if (ui.WasEntered("MODEL")) {
      ui.GetAsString("MODEL", options.model);
    }
    if (ui.GetString("SHAPE") == "ELLIPSOID") {
      options.shape = spiceinitOptions::ELLIPSOID;
    }
    else if (ui.GetString("SHAPE") == "RINGPLANE") {
      options.shape = spiceinitOptions::RINGPLANE;
    }
    else if (ui.GetString("SHAPE") == "SYSTEM") {
      options.shape = spiceinitOptions::SYSTEM;
    }
    else if (ui.GetString("SHAPE") == "USER") {
      options.shape = spiceinitOptions::USER;
    }
    else {
      throw IException(IException::Unknown,
                       "Unknown SHAPE option [" + ui.GetString("SHAPE") + "].",
                       _FILEINFO_);
    }
    options.startpad = ui.GetDouble("STARTPAD");
    options.endpad = ui.GetDouble("ENDPAD");
    options.url = ui.GetString("URL");
    options.port = ui.GetInteger("PORT");

    return options;
  }


  /**
   * Helper function to overwrite the system kernels with specified kernels.
   *
   * @param(in/out) kernel The Kernel object to overwrite
   * @param userKernels The vector of specified kernels
   */
  void getUserEnteredKernel(Kernel &kernel,
                            const std::vector<QString> &userKernels) {
    if (!userKernels.empty()) {
      kernel.setKernels(QVector<QString>::fromStdVector(userKernels).toList());
    }
  }


  /**
    * Attempt to create a camera model from a set of kernels.
    *
    * @param(in/out) icube The Cube to create the camera from. If attach is true
    *                      in the options, then the SPICE data will be written
    *                      to the Cube's file.
    * @param p The process object that the Cube belongs to
    * @param options The spiceinit options
    * @param(out) log The Application log
    * @param lk The leap second kernels
    * @param pck The planetary constant kernels
    * @param targetspk The target state kernels
    * @param ck The camera kernels
    * @param fk The frame kernels
    * @param ik The instrument kernels
    * @param sclk The spacecraft clock kernels
    * @param spk The spacecraft state kernels
    * @param iak The instrument addendum kernels
    * @param dem The digital elevation model
    * @param exk The extra kernels
    *
    * @return If a camera model was successfully created
   */
  bool tryKernels(Cube *icube, Process &p,
                  const spiceinitOptions &options,
                  Pvl *log,
                  Kernel lk, Kernel pck,
                  Kernel targetSpk, Kernel ck,
                  Kernel fk, Kernel ik, Kernel sclk,
                  Kernel spk, Kernel iak,
                  Kernel dem, Kernel exk) {
    // Add the new kernel files to the existing kernels group
    PvlKeyword lkKeyword("LeapSecond");
    PvlKeyword pckKeyword("TargetAttitudeShape");
    PvlKeyword targetSpkKeyword("TargetPosition");
    PvlKeyword ckKeyword("InstrumentPointing");
    PvlKeyword ikKeyword("Instrument");
    PvlKeyword sclkKeyword("SpacecraftClock");
    PvlKeyword spkKeyword("InstrumentPosition");
    PvlKeyword iakKeyword("InstrumentAddendum");
    PvlKeyword demKeyword("ShapeModel");
    PvlKeyword exkKeyword("Extra");

    for (int i = 0; i < lk.size(); i++) {
      lkKeyword.addValue(lk[i]);
    }
    for (int i = 0; i < pck.size(); i++) {
      pckKeyword.addValue(pck[i]);
    }
    for (int i = 0; i < targetSpk.size(); i++) {
      targetSpkKeyword.addValue(targetSpk[i]);
    }
    for (int i = 0; i < ck.size(); i++) {
      ckKeyword.addValue(ck[i]);
    }
    for (int i = 0; i < ik.size(); i++) {
      ikKeyword.addValue(ik[i]);
    }
    for (int i = 0; i < sclk.size(); i++) {
      sclkKeyword.addValue(sclk[i]);
    }
    for (int i = 0; i < spk.size(); i++) {
      spkKeyword.addValue(spk[i]);
    }
    for (int i = 0; i < iak.size(); i++) {
      iakKeyword.addValue(iak[i]);
    }

    if (options.shape == spiceinitOptions::RINGPLANE) {
      demKeyword.addValue("RingPlane");
    }
    else {
      for (int i = 0; i < dem.size(); i++) {
        demKeyword.addValue(dem[i]);
      }
    }
    for (int i = 0; i < exk.size(); i++) {
      exkKeyword.addValue(exk[i]);
    }

    PvlGroup originalKernels = icube->group("Kernels");
    PvlGroup currentKernels = originalKernels;
    currentKernels.addKeyword(lkKeyword, Pvl::Replace);
    currentKernels.addKeyword(pckKeyword, Pvl::Replace);
    currentKernels.addKeyword(targetSpkKeyword, Pvl::Replace);
    currentKernels.addKeyword(ckKeyword, Pvl::Replace);
    currentKernels.addKeyword(ikKeyword, Pvl::Replace);
    currentKernels.addKeyword(sclkKeyword, Pvl::Replace);
    currentKernels.addKeyword(spkKeyword, Pvl::Replace);
    currentKernels.addKeyword(iakKeyword, Pvl::Replace);
    currentKernels.addKeyword(demKeyword, Pvl::Replace);

    // report qualities
    PvlKeyword spkQuality("InstrumentPositionQuality");
    spkQuality.addValue(Kernel::typeEnum(spk.type()));
    currentKernels.addKeyword(spkQuality, Pvl::Replace);

    PvlKeyword ckQuality("InstrumentPointingQuality");
    ckQuality.addValue(Kernel::typeEnum(ck.type()));
    currentKernels.addKeyword(ckQuality, Pvl::Replace);

    if (!exkKeyword.isNull()) {
      currentKernels.addKeyword(exkKeyword, Pvl::Replace);
    }
    else if (currentKernels.hasKeyword("EXTRA")) {
      currentKernels.deleteKeyword("EXTRA");
    }

    // Get rid of old keywords from previously inited cubes
    if (currentKernels.hasKeyword("Source"))
      currentKernels.deleteKeyword("Source");

    if (currentKernels.hasKeyword("SpacecraftPointing"))
      currentKernels.deleteKeyword("SpacecraftPointing");

    if (currentKernels.hasKeyword("SpacecraftPosition"))
      currentKernels.deleteKeyword("SpacecraftPosition");

    if (currentKernels.hasKeyword("ElevationModel"))
      currentKernels.deleteKeyword("ElevationModel");

    if (currentKernels.hasKeyword("Frame"))
      currentKernels.deleteKeyword("Frame");

    if (currentKernels.hasKeyword("StartPadding"))
      currentKernels.deleteKeyword("StartPadding");

    if (currentKernels.hasKeyword("EndPadding"))
      currentKernels.deleteKeyword("EndPadding");

    if (currentKernels.hasKeyword("RayTraceEngine")) {
      currentKernels.deleteKeyword("RayTraceEngine");
    }

    if (currentKernels.hasKeyword("OnError")) {
      currentKernels.deleteKeyword("OnError");
    }

    if (currentKernels.hasKeyword("Tolerance")) {
      currentKernels.deleteKeyword("Tolerance");
    }

    // Add any time padding the user specified to the spice group
    if (options.startpad > DBL_EPSILON) {
      currentKernels.addKeyword(PvlKeyword("StartPadding",
                                           toString(options.startpad), "seconds"));
    }

    if (options.endpad > DBL_EPSILON) {
      currentKernels.addKeyword(PvlKeyword("EndPadding",
                                           toString(options.endpad), "seconds"));
    }

    currentKernels.addKeyword(
        PvlKeyword("CameraVersion", toString(CameraFactory::CameraVersion(*icube))),
        Pvl::Replace);

    // Add the modified Kernels group to the input cube labels
    icube->putGroup(currentKernels);

    // Create the camera so we can get blobs if necessary
    try {
      Camera *cam;
      try {
        cam = icube->camera();
        currentKernels = icube->group("Kernels");

        PvlKeyword source("Source");

        if (cam->isUsingAle()) {
          source.setValue("ale");
        }
        else {
          source.setValue("isis");
        }

        currentKernels += source;
        icube->putGroup(currentKernels);
        if (log) {
          log->addGroup(currentKernels);
        }
      }
      catch(IException &e) {
        Pvl errPvl = e.toPvl();

        if (errPvl.groups() > 0) {
          currentKernels += PvlKeyword("Error", errPvl.group(errPvl.groups() - 1)["Message"][0]);
        }

        if (log) {
          log->addGroup(currentKernels);
        }
        icube->putGroup(originalKernels);
        throw IException(e);
      }

      if (options.attach) {
        Table ckTable = cam->instrumentRotation()->Cache("InstrumentPointing");
        ckTable.Label() += PvlKeyword("Description", "Created by spiceinit");
        ckTable.Label() += PvlKeyword("Kernels");

        for (int i = 0; i < ckKeyword.size(); i++)
          ckTable.Label()["Kernels"].addValue(ckKeyword[i]);

        icube->write(ckTable);

        Table spkTable = cam->instrumentPosition()->Cache("InstrumentPosition");
        spkTable.Label() += PvlKeyword("Description", "Created by spiceinit");
        spkTable.Label() += PvlKeyword("Kernels");
        for (int i = 0; i < spkKeyword.size(); i++)
          spkTable.Label()["Kernels"].addValue(spkKeyword[i]);

        icube->write(spkTable);

        Table bodyTable = cam->bodyRotation()->Cache("BodyRotation");
        bodyTable.Label() += PvlKeyword("Description", "Created by spiceinit");
        bodyTable.Label() += PvlKeyword("Kernels");
        for (int i = 0; i < targetSpkKeyword.size(); i++)
          bodyTable.Label()["Kernels"].addValue(targetSpkKeyword[i]);

        for (int i = 0; i < pckKeyword.size(); i++)
          bodyTable.Label()["Kernels"].addValue(pckKeyword[i]);

        bodyTable.Label() += PvlKeyword("SolarLongitude",
            toString(cam->solarLongitude().degrees()));
        icube->write(bodyTable);

        Table sunTable = cam->sunPosition()->Cache("SunPosition");
        sunTable.Label() += PvlKeyword("Description", "Created by spiceinit");
        sunTable.Label() += PvlKeyword("Kernels");
        for (int i = 0; i < targetSpkKeyword.size(); i++)
          sunTable.Label()["Kernels"].addValue(targetSpkKeyword[i]);

        icube->write(sunTable);

        //  Save original kernels in keyword before changing to Table
        PvlKeyword origCk = currentKernels["InstrumentPointing"];
        PvlKeyword origSpk = currentKernels["InstrumentPosition"];
        PvlKeyword origTargPos = currentKernels["TargetPosition"];

        currentKernels["InstrumentPointing"] = "Table";
        for (int i = 0; i < origCk.size(); i++)
          currentKernels["InstrumentPointing"].addValue(origCk[i]);

        currentKernels["InstrumentPosition"] = "Table";
        for (int i = 0; i < origSpk.size(); i++)
          currentKernels["InstrumentPosition"].addValue(origSpk[i]);

        currentKernels["TargetPosition"] = "Table";
        for (int i = 0; i < origTargPos.size(); i++)
          currentKernels["TargetPosition"].addValue(origTargPos[i]);

        icube->putGroup(currentKernels);

        Pvl *label = icube->label();
        int i = 0;
        while (i < label->objects()) {
          PvlObject currObj = label->object(i);
          if (currObj.isNamed("NaifKeywords")) {
            label->deleteObject(i);
          }
          else {
            i ++;
          }
        }

        *icube->label() += cam->getStoredNaifKeywords();
      }
      //modify Kernels group only
      else {
        Pvl *label = icube->label();
        int i = 0;
        while (i < label->objects()) {
          PvlObject currObj = label->object(i);
          if (currObj.isNamed("Table")) {
            if (currObj["Name"][0] == QString("InstrumentPointing") ||
                currObj["Name"][0] == QString("InstrumentPosition") ||
                currObj["Name"][0] == QString("BodyRotation") ||
                currObj["Name"][0] == QString("SunPosition")) {
              label->deleteObject(i);
            }
            else {
              i++;
            }
          }
          else if (currObj.isNamed("NaifKeywords")) {
            label->deleteObject(i);
          }
          else {
            i++;
          }
        }
      }

      p.WriteHistory(*icube);
    }
    catch(IException &) {
      icube->putGroup(originalKernels);
      return false;
    }

    return true;
  }


  /**
   * spiceinit a Cube via the spice web service
   *
   * @param icube The Cube to spiceinit
   * @param labels The Cube label
   * @param missionName The NAIF name of the mission the Cube is from
   * @param options The spiceinit options
   * @param log The Application log
   */
  void requestSpice(Cube *icube,
                    Pvl &labels,
                    QString missionName,
                    const spiceinitOptions &options,
                    Pvl *log) {
    QString instrumentId =
        labels.findGroup("Instrument", Pvl::Traverse)["InstrumentId"][0];

    QString url = options.url + "?mission=" + missionName + "&instrument=" + instrumentId;
    QString shape;
    switch (options.shape) {
      case spiceinitOptions::ELLIPSOID:
        shape = "ELLIPSOID";
        break;
      case spiceinitOptions::RINGPLANE:
        shape = "RINGPLANE";
        break;
      case spiceinitOptions::SYSTEM:
        shape = "SYSTEM";
        break;
      case spiceinitOptions::USER:
        shape = "USER";
        break;
      default:
        throw IException(IException::User,
                         "Invalid shape option for spice server[" +
                         toString(static_cast<int>(options.shape)) + "].",
                         _FILEINFO_);
        break;
    }

    if (shape == "user") {
      if (options.model.size() != 1) {
        throw IException(IException::User,
                         "Exactly one shape model must be entered when shape is set to USER; [" +
                         toString(static_cast<int>(options.model.size())) +
                         "] shape models were entered.",
                         _FILEINFO_);
      }
      shape = options.model.front();

      // Test for valid labels with mapping group at least
      Pvl shapeTest(shape);
      shapeTest.findGroup("Mapping", Pvl::Traverse);
    }

    SpiceClient client(url, options.port, labels,
                       options.cksmithed, options.ckrecon,
                       options.ckpredicted, options.cknadir,
                       options.spksmithed, options.spkrecon, options.spkpredicted,
                       shape, options.startpad, options.endpad);

    Progress connectionProgress;
    connectionProgress.SetText("Requesting Spice Data");
    connectionProgress.SetMaximumSteps(1);
    connectionProgress.CheckStatus();
    SpiceClientStarter starter(client);
    starter.start();
    client.blockUntilComplete();
    connectionProgress.CheckStatus();

    PvlGroup kernelsGroup = client.kernelsGroup();
    PvlGroup logGrp = client.applicationLog();
    PvlObject naifKeywords = client.naifKeywordsObject();
    Table *pointingTable = client.pointingTable();
    Table *positionTable = client.positionTable();
    Table *bodyTable = client.bodyRotationTable();
    Table *sunPosTable = client.sunPositionTable();

    // Verify everything in the kernels group exists, if not then our kernels are
    //   out of date.
    for (int keywordIndex = 0;
        keywordIndex < kernelsGroup.keywords();
        keywordIndex++) {
      PvlKeyword &curKeyword = kernelsGroup[keywordIndex];

      if (curKeyword.name() == "NaifFrameCode" ||
          curKeyword.name() == "InstrumentPointingQuality" ||
          curKeyword.name() == "InstrumentPositionQuality" ||
          curKeyword.name() == "CameraVersion" ||
          curKeyword.name() == "TargetPosition" ||
          curKeyword.name() == "InstrumentPointing" ||
          curKeyword.name() == "InstrumentPosition" ||
          curKeyword.name() == "TargetAttitudeShape") {
        continue;
      }
    }

    if (log) {
      log->addGroup(logGrp);
    }

    icube->putGroup(kernelsGroup);
    icube->label()->addObject(naifKeywords);

    icube->write(*pointingTable);
    icube->write(*positionTable);
    icube->write(*bodyTable);
    icube->write(*sunPosTable);

    try {
      icube->camera();
    }
    catch (IException &e) {
      throw IException(e, IException::Unknown,
         "The SPICE server returned incompatible SPICE data",
          _FILEINFO_);
    }

    delete pointingTable;
    pointingTable = NULL;

    delete positionTable;
    positionTable = NULL;

    delete bodyTable;
    bodyTable = NULL;

    delete sunPosTable;
    sunPosTable = NULL;
  }

}
