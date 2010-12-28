#include "Isis.h"

#include "CameraFactory.h"
#include "ControlNet.h"
#include "ControlNetValidMeasure.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "CubeManager.h"
#include "FileList.h"
#include "Filename.h"
#include "iException.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "UserInterface.h"

#include <map>
#include <set>

using namespace Isis;

void SetControlPointLatLon(const std::string &incubes, const std::string &cnet);

std::map< std::string, std::pair<double, double> > p_pointLatLon;
std::map< int, std::set<std::string> > p_modifiedMeasures;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  FileList list2(ui.GetFilename("ADDLIST"));
  bool hasDuplicateSerialNumbers = false;

  bool log = false;
  Filename logFile;
  if(ui.WasEntered("LOG")) {
    log = true;
    logFile = ui.GetFilename("LOG");
  }
  Pvl results;
  results.SetName("cnetadd_Results");
  PvlKeyword added("FilesAdded");
  PvlKeyword omitted("FilesOmitted");
  PvlKeyword pointsModified("PointsModified");

  bool checkMeasureValidity = ui.WasEntered("DEFFILE");
  ControlNetValidMeasure validator;
  if(checkMeasureValidity) {
    Pvl deffile(ui.GetFilename("DEFFILE"));
    validator = ControlNetValidMeasure(deffile);
  }

  std::string retrievalOpt = ui.GetString("RETRIEVAL");
  PvlKeyword duplicates("DupSerialNumbers");
  if(retrievalOpt == "REFERENCE") {
    FileList list1(ui.GetFilename("FROMLIST"));
    SerialNumberList addSerials(ui.GetFilename("ADDLIST"));
    SerialNumberList fromSerials(ui.GetFilename("FROMLIST"));

    //Check for duplicate files in the lists by serial number
    for(int i = 0; i < addSerials.Size(); i++) {

      // Check for duplicate SNs accross the lists
      if(fromSerials.HasSerialNumber(addSerials.SerialNumber(i))) {
        duplicates.AddValue(addSerials.Filename(i));
      }

      // Check for duplicate SNs within the addlist
      for(int j = i + 1; j < addSerials.Size(); j++) {
        if(addSerials.SerialNumber(i) == addSerials.SerialNumber(j)) {
          std::string msg = "Add list files [" + addSerials.Filename(i) + "] and [";
          msg += addSerials.Filename(j) + "] share the same serial number.";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
      }
    }
    // If duplicates throw error
    if(duplicates.Size() > 0) {
      hasDuplicateSerialNumbers = true;
    }
    SetControlPointLatLon(ui.GetFilename("FROMLIST"), ui.GetFilename("INNET"));
  }

  Filename outNetFile(ui.GetFilename("OUTNET"));

  ControlNet inNet = ControlNet(ui.GetFilename("INNET"));
  inNet.SetUserName(Isis::Application::UserName());
  inNet.SetModifiedDate(Isis::iTime::CurrentLocalTime());     //This should be done in ControlNet's Write fn

  Progress progress;
  progress.SetText("Adding Images");
  progress.SetMaximumSteps(list2.size());
  progress.CheckStatus();

  // loop through all the images
  std::vector<int> modPoints;
  for(unsigned int img = 0; img < list2.size(); img++) {
    bool imageAdded = false;
    Cube cube;
    cube.Open(list2[img]);
    Pvl *cubepvl = cube.Label();
    Camera *cam = cube.Camera();

    //loop through all the control points
    for(int cp = 0; cp < inNet.Size(); cp++) {
      ControlPoint point(inNet[cp]);

      // If the point is locked and Apriori source is "AverageOfMeasures" 
      // then do not add the measures.
      if(point.IsEditLocked() && 
         point.GetAprioriSurfacePointSource() == ControlPoint::SurfacePointSource::AverageOfMeasures){
        continue;
      }
      // If there are duplicate serial numbers in the addlist, prevent double adding
      if(hasDuplicateSerialNumbers) {
        std::string sn = SerialNumber::Compose(*cubepvl);
        bool hasSerialNumber = false;

        for(int cm = 0; cm < point.Size() && !hasSerialNumber; cm ++) {
          if(sn == point[cm].GetCubeSerialNumber()) {
            hasSerialNumber = true;
          }
        }

        if(hasSerialNumber) {
          continue;
        }
      }
          
      double latitude;
      double longitude;
      if(retrievalOpt == "REFERENCE") {
        // Get the lat/long coords from the existing reference measure
        latitude = p_pointLatLon[point.GetId()].first;
        longitude = p_pointLatLon[point.GetId()].second;
      }
      else {
        // Get the lat/long coords from the current control point
        latitude = point.GetSurfacePoint().GetLatitude().GetDegrees();
        longitude = point.GetSurfacePoint().GetLongitude().GetDegrees();
        if(latitude == Isis::Null  ||  longitude == Isis::Null) {
          std::string msg = "Unable to retreive lat/lon from Control Point [";
          msg += point.GetId() + "]. RETREIVAL=POINT cannot be used unless all Control ";
          msg += "Points have Latitude/Longitude keywords.";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
      }

      if(cam->SetUniversalGround(latitude, longitude)) {

        // Make sure the samp & line are inside the image
        if(cam->InCube()) {
          std::string sn = SerialNumber::Compose(* cubepvl);

          // If there are duplicate serial numbers in the addlist, prevent double adding
          if(hasDuplicateSerialNumbers) {
            bool hasSerialNumber = false;

            for(int cm = 0; cm < point.Size() && !hasSerialNumber; cm ++) {
              if(sn == point[cm].GetCubeSerialNumber()) {
                hasSerialNumber = true;
              }
            }

            // if the current Control Point contains this image,
            // continue on to the next Control Point
            if(hasSerialNumber) {
              continue;
            }
          }

          ControlMeasure newCm;
          newCm.SetCoordinate(cam->Sample(), cam->Line(), ControlMeasure::Candidate);
          newCm.SetAprioriSample(cam->Sample());
          newCm.SetAprioriLine  (cam->Line());
          newCm.SetCubeSerialNumber(sn);
          newCm.SetDateTime();
          newCm.SetChooserName("Application cnetadd");

          // Check the measure for DEFFILE validity
          if(checkMeasureValidity) {
            if(!validator.ValidEmissionAngle(cam->EmissionAngle())) {
              //TODO: log that it was Emission Angle that failed the check
              newCm.SetIgnore(true);
            }
            else if(!validator.ValidIncidenceAngle(cam->IncidenceAngle())) {
              //TODO: log that it was Incidence Angle that failed the check
              newCm.SetIgnore(true);
            }
            else if(!validator.ValidResolution(cam->Resolution())) {
              //TODO: log that it was Resolution that failed the check
              newCm.SetIgnore(true);
            }
            else if(!validator.PixelsFromEdge((int)cam->Sample(), (int)cam->Line(), &cube)) {
              //TODO: log that it was Pixels from Edge that failed the check
              newCm.SetIgnore(true);
            }
            else {
              Isis::Portal portal(1, 1, cube.PixelType());
              portal.SetPosition(cam->Sample(), cam->Line(), 1);
              cube.Read(portal);
              if(!validator.ValidDnValue(portal[0])) {
                //TODO: log that it was DN that failed the check
                newCm.SetIgnore(true);
              }
            }
          }

          point.Add(newCm);

          // Record the modified Point and Measure
          p_modifiedMeasures[cp].insert(newCm.GetCubeSerialNumber());

          if(retrievalOpt == "POINT" && inNet[cp].Size() == 1) {
            point.SetIgnore(false);
          }

          if(log) {
            // If we can't find this control point in the list of control points
            // that have already been modified, then add it to the list
            bool doesntContainPoint = true;
            for(unsigned int i = 0; i < modPoints.size() && doesntContainPoint; i++) {
              if(modPoints[i] == cp) doesntContainPoint = false;
            }
            if(doesntContainPoint) {
              modPoints.push_back(cp);
            }

            imageAdded = true;
          }
        }
      }
      
      inNet.UpdatePoint(point);
    }

    cubepvl = NULL;
    cam = NULL;

    if(log) {
      if(imageAdded) added.AddValue(Isis::Filename(list2[img]).Basename());
      else omitted.AddValue(Isis::Filename(list2[img]).Basename());
    }

    progress.CheckStatus();
  }


  if(log) {

    // Shell sort the list of modified control points
    int increments[] = { 1391376, 463792, 198768, 86961, 33936, 13776, 4592, 1968,
                         861, 336, 112, 48, 21, 7, 3, 1
                       };
    for(unsigned int k = 0; k < 16; k++) {
      int inc = increments[k];
      for(unsigned int i = inc; i < modPoints.size(); i++) {
        int temp = modPoints[i];
        int j = i;
        while(j >= inc && modPoints[j - inc] > temp) {
          modPoints[j] = modPoints[j - inc];
          j -= inc;
        }
        modPoints[j] = temp;
      }
    }

    // Add the list of modified points to the output log file
    for(unsigned int i = 0; i < modPoints.size(); i++) {
      pointsModified += inNet[modPoints[i]].GetId();
    }

    results.AddKeyword(added);
    results.AddKeyword(omitted);
    results.AddKeyword(pointsModified);
    if(duplicates.Size() > 0) {
      results.AddKeyword(duplicates);
    }

    results.Write(logFile.Expanded());
  }

  // List the modified points
  if(ui.WasEntered("MODIFIEDPOINTS")) {
    Filename pointList(ui.GetFilename("MODIFIEDPOINTS"));

    // Set up the output file for writing
    std::ofstream out_stream;
    out_stream.open(pointList.Expanded().c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(std::map< int, std::set<std::string> >::iterator it = p_modifiedMeasures.begin();
        it != p_modifiedMeasures.end(); it ++) {
      out_stream << inNet[it->first].GetId() << std::endl;
    }

    out_stream.close();
  }

  // Modify the inNet to only have modified points/measures
  if(ui.GetString("EXTRACT") == "MODIFIED") {
    for(int cp = inNet.Size() - 1; cp >= 0; cp --) {
      std::map< int, std::set<std::string> >::iterator it = p_modifiedMeasures.find(cp);
      // If the point was not modified, delete
      if(it == p_modifiedMeasures.end()) {
        inNet.Delete(cp);
      }
      // Else, remove the unwanted measures from the modified point
      else {
        for(int cm = inNet[cp].Size() - 1; cm >= 0; cm --) {
          if(!inNet[cp][cm].GetType() ==  ControlMeasure::Reference &&
              it->second.find(inNet[cp][cm].GetCubeSerialNumber()) == it->second.end()) {
            inNet[cp].Delete(cm);
          }
        }
      }
    }
  }

  inNet.Write(outNetFile.Expanded());

}


/**
 * Calculates the lat/lon of the ControlNet.
 *
 * @param incubes The filename of the list of cubes in the ControlNet
 * @param cnet    The filename of the ControlNet
 */
void SetControlPointLatLon(const std::string &incubes, const std::string &cnet) {
  SerialNumberList snl(incubes);
  ControlNet net(cnet);

  CubeManager manager;
  manager.SetNumOpenCubes(50);   //Should keep memory usage to around 1GB

  Progress progress;
  progress.SetText("Calculating Lat/Lon");
  progress.SetMaximumSteps(net.Size());
  progress.CheckStatus();

  for(int cp = 0; cp < net.Size(); cp++) {
    ControlPoint point(net[cp]);
    ControlMeasure cm(point[ net[cp].GetReferenceIndex() ]);

    Cube *cube = manager.OpenCube(snl.Filename(cm.GetCubeSerialNumber()));
    try {
      cube->Camera()->SetImage(cm.GetSample(), cm.GetLine());
      p_pointLatLon[point.GetId()].first = cube->Camera()->UniversalLatitude();
      p_pointLatLon[point.GetId()].second = cube->Camera()->UniversalLongitude();
    }
    catch(Isis::iException &e) {
      std::string msg = "Unable to create camera for cube file [";
      msg += snl.Filename(cm.GetCubeSerialNumber()) + "]";
      throw Isis::iException::Message(Isis::iException::System, msg, _FILEINFO_);
    }
    cube = NULL; //Do not delete, manager still has ownership

    progress.CheckStatus();
  }

  manager.CleanCubes();
}
