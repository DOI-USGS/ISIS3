#include "ControlNet.h"
#include "SpecialPixel.h"
#include "iException.h"
#include "CameraFactory.h"
#include "iTime.h"

namespace Isis {
  //!Creates an empty ControlNet object
  ControlNet::ControlNet () {
    p_invalid = false;
  }


 /**
  * Creates a ControlNet object with the given list of control points and cubes
  *
  * @param ptfile Name of file containing a Pvl list of control points 
  * @param progress A pointer to the progress of reading in the control points 
  * @param forceBuild Forces invalid Control Points to be added to this Control 
  *                   Network
  */
  ControlNet::ControlNet(const std::string &ptfile, Progress *progress, bool forceBuild) {
    p_invalid = false;
    ReadControl(ptfile, progress, forceBuild);
  }


 /**
  * Adds a ControlPoint to the ControlNet 
  *  
  * @param point Control point to be added 
  * @param forceBuild Forces invalid Control Points to be added to this Control 
  *                   Network
  *  
  * @throws Isis::iException::Programmer - "ControlPoint must 
  *             have unique Id"
  */
  void ControlNet::Add (const ControlPoint &point, bool forceBuild) {
    if(p_pointsHash.contains(QString::fromStdString(point.Id()))) {
      std::string msg = "ControlPoint must have unique Id";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    } else {
      p_pointsHash.insert(QString::fromStdString(point.Id()), point);
      p_pointIds.push_back(QString::fromStdString(point.Id()));
    }
  }


 /**
  * Deletes the ControlPoint at the specified index in the ControlNet
  *
  * @param index The index of the ControlPoint to be deleted
  *
  * @throws Isis::iException::User - "There is no ControlPoint at 
  *             the given index number"
  */
  void ControlNet::Delete (int index) {
    if (index >= (int)p_pointsHash.size() || index < 0) {
      std::string msg = "There is no ControlPoint at the given index number";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    else {
      // See if removing this point qualifies for a re-check of validity
      bool check = false;
      if( p_invalid && p_pointsHash[p_pointIds[index]].Invalid()) check = true;

      p_pointsHash.remove(p_pointIds[index]);
      p_pointIds.remove(index);

      // Check validity if needed
      if( check ) {
        p_invalid = false;
        for (int i=0; i<Size() && !p_invalid; i++) {
          if(p_pointsHash.contains(QString::fromStdString(p_pointsHash[p_pointIds[i]].Id()))) {
              p_invalid = true;
          }
        }
      }

    }
  }


 /**
  * Deletes the ControlPoint with the given id in the ControlNet
  *
  * @param id The id of the ControlPoint to be deleted
  *
  * @throws Isis::iException::User - "A ControlPoint matching 
  *                                  the id was not found in the
  *                                  ControlNet"
  */
  void ControlNet::Delete (const std::string &id) {
    // If the QHash contains the point with this id, then
    // remove it from p_pointsHash.
    if(p_pointsHash.contains(QString::fromStdString(id))) {
      p_pointsHash.remove(QString::fromStdString(id));
    } else {
       // If a match was not found, throw an error
      std::string msg = "A ControlPoint matching the id [" + id
        + "] was not found in the ControlNet";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // If the QVector contains the point with this id, then
    // remove it from p_pointIds.
    if(p_pointIds.contains(QString::fromStdString(id))) {
      p_pointIds.remove(p_pointIds.indexOf(QString::fromStdString(id)));
    }else {
       // If a match was not found, throw an error
      std::string msg = "A ControlPoint matching the id [" + id
        + "] was not found in the ControlNet";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    
  }


 /**
  * Reads in the control points from the given file
  *
  * @param ptfile Name of file containing a Pvl list of control points 
  * @param progress A pointer to the progress of reading in the control points 
  * @param forceBuild Forces invalid Control Points to be added to this Control 
  *                   Network
  *
  * @throws Isis::iException::User - "Invalid Network Type"
  * @throws Isis::iException::User - "Invalid Control Point" 
  * @throws Isis::iException::User - "Invalid Format" 
  *  
  * @internal 
  * @history 2009-04-07 Tracie Sucharski - Keep track of ignored measures. 
  *  
  */
  void ControlNet::ReadControl(const std::string &ptfile, Progress *progress, bool forceBuild) {
    Pvl p(ptfile);
    try {
      PvlObject cn = p.FindObject("ControlNetwork");
      p_networkId = (std::string)cn["NetworkId"];
      if ((std::string)cn["NetworkType"] == "Singleton") {
        p_type = Singleton;
      }
      else if ((std::string)cn["NetworkType"] == "ImageToImage") {
        p_type = ImageToImage;
      }
      else if ((std::string)cn["NetworkType"] == "ImageToGround") {
        p_type = ImageToGround;
      }
      else {
        std::string msg = "Invalid Network Type.";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }

      p_targetName = (std::string)cn["TargetName"];
      p_userName = (std::string)cn["UserName"];
      p_created = (std::string)cn["Created"];
      p_modified = (std::string)cn["LastModified"];
      p_description = (std::string)cn["Description"];

      // Prep for reporting progress
      if (progress != NULL) {
        progress->SetText("Loading Control Points...");
        progress->SetMaximumSteps(cn.Objects());
        progress->CheckStatus();
      }
      for (int i=0; i<cn.Objects(); i++) {
        try {
          if (cn.Object(i).IsNamed("ControlPoint")) {
            ControlPoint cp;
            cp.Load(cn.Object(i),forceBuild);
            Add(cp,forceBuild);
          }
        }
        catch (iException &e) {
          std::string msg = "Invalid Control Point at position [" + iString(i)
             + "]";
          throw iException::Message(iException::User,msg,_FILEINFO_);
        }
        if (progress != NULL) progress->CheckStatus();
      }
    }
    catch (iException &e) {
      std::string msg = "Invalid Format in [" + ptfile + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }


 /**
  * Writes out the ControlPoints
  *
  * @param ptfile Name of file containing a Pvl list of control points 
  * @throws Isis::iException::Programmer - "Invalid Net 
  *             Enumeration"
  * @throws Isis::iException::Io - "Unable to write PVL
  *             infomation to file"
  */
  void ControlNet::Write(const std::string &ptfile) {
    Pvl p;
    PvlObject net("ControlNetwork");
    net += PvlKeyword("NetworkId", p_networkId);
    if (p_type == Singleton) {
      net += PvlKeyword("NetworkType", "Singleton");
    }
    else if (p_type == ImageToImage) {
      net += PvlKeyword("NetworkType", "ImageToImage");
    }
    else if (p_type == ImageToGround) {
      net += PvlKeyword("NetworkType", "ImageToGround");
    }
    else {
      std::string msg = "Invalid Net Enumeration, [" + iString(p_type) + "]";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    net += PvlKeyword("TargetName", p_targetName);
    net += PvlKeyword("UserName", p_userName);
    std::string mod = iString(p_modified).UpCase();
    if( mod == "NULL"  ||  mod == "" ) {
      SetModifiedDate( Isis::iTime::CurrentLocalTime() );
    }
    std::string cre = iString(p_created).UpCase();
    if( cre == "NULL"  ||  cre == "" ) {
      SetCreatedDate( p_modified );
    }
    net += PvlKeyword("Created", p_created);
    net += PvlKeyword("LastModified", p_modified);
    net += PvlKeyword("Description", p_description);

    for (int i=0; i<(int)p_pointsHash.size(); i++) {
      PvlObject cp = p_pointsHash[p_pointIds[i]].CreatePvlObject();
      net.AddObject(cp);
    }
    p.AddObject(net);

    try {
      p.Write(ptfile);
    }
    catch (iException e) {
      std::string message = "Unable to write PVL infomation to file [" +
                       ptfile + "]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }


 /**
  * Finds and returns a pointer to the ControlPoint with the specified id
  *
  * @param id The id of the ControlPoint to be deleted
  *
  * @return <B>ControlPoint*</B> Pointer to the ControlPoint with
  *         the given id
  *
  * @throws Isis::iException::User - "A ControlPoint matching the 
  *                                  id was not found in the
  *                                  ControlNet"
  */
  ControlPoint *ControlNet::Find(const std::string &id) {
    if(!p_pointIds.contains(QString::fromStdString(id))) {
      std::string msg = "A ControlPoint matching the id [" + id
        + "] was not found in the ControlNet";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    return &p_pointsHash.find(QString::fromStdString(id)).value();
  }


  /**
   * Returns true if the given ControlPoint has the same id as 
   * another ControlPoint in class 
   *  
   * @param point The ControlPoint whos id is being compared 
   *  
   * @return <B>bool</B> If the ControlPoint id was found 
   */
  bool ControlNet::Exists( ControlPoint &point ) {
    if(p_pointsHash.contains(QString::fromStdString(point.Id()))) {
      return true;
    }
    return false;
  }


 /**
  * Finds and returns a pointer to the closest ControlPoint to the
  * ControlMeasure with the given serial number and line sample location
  *
  * @param serialNumber The serial number of the the file the ControlMeasure is
  *                     on
  * @param sample The sample number of the ControlMeasure
  * @param line The line number of the ControlMeasure
  *
  * @return <B>ControlPoint*</B> Pointer to the ControlPoint 
  *         closest to the given line, sample position
  */
  ControlPoint *ControlNet::FindClosest(const std::string &serialNumber,
                                        double sample, double line) {

    ControlPoint *savePoint=NULL;
    double dist;
    double minDist=99999.;
    for (int i=0; i < (int)p_pointsHash.size(); i++) {
      for (int j=0; j < p_pointsHash[p_pointIds[i]].Size(); j++) {
        if (p_pointsHash[p_pointIds[i]][j].CubeSerialNumber() != serialNumber) continue;
        //Find closest line sample & return that controlpoint
        dist = fabs(sample - p_pointsHash[p_pointIds[i]][j].Sample()) +
               fabs(line - p_pointsHash[p_pointIds[i]][j].Line());
        if (dist < minDist) {
          minDist = dist;
          savePoint = &p_pointsHash[p_pointIds[i]];
        }
      }
    }
    return savePoint;
  }


  /**
   * Compute aprior values for each point in the network
   */
  void ControlNet::ComputeApriori() {
    // TODO:  Make sure the cameras have been initialized
    for (int i=0; i<(int)p_pointsHash.size(); i++) {
      p_pointsHash[p_pointIds[i]].ComputeApriori();
    }
  }


  /**
   * Compute error for each point in the network
   */
  void ControlNet::ComputeErrors() {
    // TODO:  Make sure the cameras have been initialized
    for (int i=0; i<(int)p_pointsHash.size(); i++) {
      p_pointsHash[p_pointIds[i]].ComputeErrors();
    }
  }


  /**
   * Determine the maximum error of all points in the network 
   * @return <B>double</B> Max error of points
   */
  double ControlNet::MaximumError() {
    // TODO:  Make sure the cameras have been initialized
    double maxError = 0.0;
    for (int i=0; i<(int)p_pointsHash.size(); i++) {
      double error = p_pointsHash[p_pointIds[i]].MaximumError();
      if (error > maxError) maxError = error;
    }
    return maxError;
  }


  /**
   * Compute the average error of all points in the network 
   * @return <B>double</B> Average error of points
   */
  double ControlNet::AverageError() {
    // TODO:  Make sure the cameras have been initialized
    double avgError = 0.0;
    int count = 0;
    for (int i=0; i<(int)p_pointsHash.size(); i++) {
      if (p_pointsHash[p_pointIds[i]].Ignore()) continue;
      avgError += p_pointsHash[p_pointIds[i]].AverageError();
      count++;
    }
    if (count == 0) return avgError;
    return avgError / count;
  }


  /**
   * Creates the ControlNet's image cameras based on an input file
   * 
   * @param imageListFile The list of images
   */
  void ControlNet::SetImages (const std::string &imageListFile) {
    SerialNumberList list(imageListFile);
    SetImages(list);
  }


  /**
   * Creates the ControlNet's image camera's based on the list of Serial Numbers
   * 
   * @param list The list of Serial Numbers
   * @param progress A pointer to the progress of creating the cameras 
   * @throws Isis::iException::System - "Unable to create camera 
   *        for cube file"
   * @throws Isis::iException::User - "Control point measure does 
   *        not have a cube with a matching serial number"
   * @internal 
   *   @history 2009-01-06 Jeannie Walldren - Fixed typo in
   *            exception output.
   */
  void ControlNet::SetImages (SerialNumberList &list, Progress *progress) {
    // Prep for reporting progress
    if (progress != NULL) {
      progress->SetText("Setting input images...");
      progress->SetMaximumSteps(list.Size());
      progress->CheckStatus();
    }
    // Open the camera for all the images in the serial number list
    for (int i=0; i<list.Size(); i++) {
      std::string serialNumber = list.SerialNumber(i);
      std::string filename = list.Filename(i);
      Pvl pvl(filename);

      try {
        Isis::Camera *cam = CameraFactory::Create(pvl);
        p_cameraMap[serialNumber] = cam;
        p_cameraList.push_back(cam);
      }
      catch (Isis::iException &e) {
        std::string msg = "Unable to create camera for cube file ";
        msg += filename;
        throw Isis::iException::Message(Isis::iException::System,msg,_FILEINFO_);
      }

      if (progress != NULL) progress->CheckStatus();
    }

    // Loop through all measures and set the camera
    for (int p=0; p<Size(); p++) {
      for (int m=0; m<p_pointsHash[p_pointIds[p]].Size(); m++) {
        if (p_pointsHash[p_pointIds[p]][m].Ignore()) continue;
        std::string serialNumber = p_pointsHash[p_pointIds[p]][m].CubeSerialNumber();
        if (list.HasSerialNumber(serialNumber)) {
          p_pointsHash[p_pointIds[p]][m].SetCamera(p_cameraMap[serialNumber]);
        }
        else {
          std::string msg = "Control point [" + p_pointsHash[p_pointIds[p]].Id() + "], ";
          msg += "measure [" + p_pointsHash[p_pointIds[p]][m].CubeSerialNumber() + "] ";
          msg += "does not have a cube with a matching serial number";
          throw Isis::iException::Message(iException::User,msg,_FILEINFO_);
          // TODO: DO we allow to continue or not?
        }
      }
    }
  }


  /** 
   * Returns the number of non-ignored control points 
   *  
   * @return Number of valid control points 
   */
  int ControlNet::NumValidPoints() {
    int size = 0;
    for(int cp = 0; cp < Size(); cp ++) {
      if(!p_pointsHash[p_pointIds[cp]].Ignore()) size ++;
    }
    return size;
  }

  /*
   * Return the total number of measures for all control points in the network
   *
   * @return Number of control measures
   */
  int ControlNet::NumMeasures() {
    int numMeasures = 0;
    for (int cp = 0; cp < Size(); cp++) {
      numMeasures += p_pointsHash[p_pointIds[cp]].Size();
    }
    return numMeasures;
  }

  /*
   * Return the number of valid (non-ignored) measures for all control points
   * in the network
   *
   * @return Number of valid measures
   */
  int ControlNet::NumValidMeasures() {
    int numValidMeasures = 0;
    for (int cp = 0; cp < Size(); cp++) {
      numValidMeasures += p_pointsHash[p_pointIds[cp]].NumValidMeasures();
    }
    return numValidMeasures;
  }

  /*
   * Return the total number of ignored measures for all control points in the
   * network
   *
   * @return Number of valid measures
   */
  int ControlNet::NumIgnoredMeasures() {
    int numIgnoredMeasures = 0;
    for (int cp = 0; cp < Size(); cp++) {
      ControlPoint &pt = p_pointsHash[p_pointIds[cp]];
      numIgnoredMeasures += pt.Size() - pt.NumValidMeasures();
    }
    return numIgnoredMeasures;
  }
}
