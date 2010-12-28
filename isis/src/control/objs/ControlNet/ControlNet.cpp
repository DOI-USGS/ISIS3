#include "ControlNet.h"

#include <sstream>

#include <QtAlgorithms>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "Application.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iException.h"
#include "iTime.h"
#include "PBControlNetIO.pb.h"
#include "PBControlNetLogData.pb.h"
#include "Progress.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace boost::numeric::ublas;
using namespace google::protobuf::io;


namespace Isis {
  //!Creates an empty ControlNet object
  ControlNet::ControlNet () {
    p_invalid = false;
    p_numMeasures = 0;
    p_numIgnoredMeasures = 0;
    p_created = Application::DateTime();
    p_modified = Application::DateTime();
  }


 /**
  * Creates a ControlNet object with the given list of control points and cubes
  *
  * @param ptfile Name of file containing a Pvl list of control points 
  * @param progress A pointer to the progress of reading in the control points 
  * @param forceBuild Forces invalid Control Points to be added to this Control 
  *                   Network
  */
  ControlNet::ControlNet(const iString &ptfile, Progress *progress,
                         bool forceBuild) {
    p_invalid = false;
    p_numMeasures = 0;
    p_numIgnoredMeasures = 0;
    ReadControl(ptfile, progress, forceBuild);
  }


  ControlNet::~ControlNet () {
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
   * @history 2010-08-06 Tracie Sucharski, Updated for changes made after
   *                           additional working sessions for Control network
   *                           design.
   *
   */
  void ControlNet::ReadControl(const iString &ptfile, Progress *progress, bool forceBuild) {
    Pvl p(ptfile);
    //Test to see if this is a binary control net file.
    if (p.HasObject("ProtoBuffer")) {
      ReadPBControl(ptfile);
      return;
    }
    try {
      PvlObject &cn = p.FindObject("ControlNetwork");

      if (cn.HasKeyword("NetworkId")) p_networkId = cn["NetworkId"][0];
      p_targetName = (std::string)cn["TargetName"];
      p_userName = (std::string)cn["UserName"];
      p_created = (std::string)cn["Created"];
      p_modified = (std::string)cn["LastModified"];
      if (cn.HasKeyword("Description")) p_description = cn["Description"][0];

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
            p_numMeasures += cp.Size();
            if (cp.IsIgnored()) {
              p_numIgnoredMeasures += cp.Size();
            }
            else {
              for (int m=0; m<cp.Size(); m++) {
                if (cp[m].IsIgnored()) p_numIgnoredMeasures++;
              }
            }
            Add(cp,forceBuild);
          }
        }
        catch (iException &e) {
          iString msg = "Invalid Control Point at position [" + iString(i)
             + "]";
          throw iException::Message(iException::User,msg,_FILEINFO_);
        }
        if (progress != NULL) progress->CheckStatus();
      }
    }
    catch (iException &e) {
      iString msg = "Invalid Format in [" + ptfile + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }


  /**
   * Read control network from a google protocol binary file
   *
   * @param ptfile 
   *
   * @history 2010-08-06 Tracie Sucharski, Updated for changes made after
   *                           additional working sessions for Control network
   *                           design.
   * @history 2010-09-01 Tracie Sucharski, Use google::protobuf coded input 
   *                           stream so we can set the byte limits.
   * @history 2010-10-06 Tracie Sucharski, Changed long to BigInt. 
   * @history 2010-12-09 Tracie Sucharski, Added new measure type of Ground 
   */
  void ControlNet::ReadPBControl(const iString &ptfile) {
    // Create an input file stream with the input file.
    Pvl protoFile(ptfile);

    PvlObject &protoBufferInfo = protoFile.FindObject("ProtoBuffer");
    PvlObject &protoBufferCore = protoBufferInfo.FindObject("Core");
    BigInt coreStartPos = protoBufferCore["StartByte"];
    BigInt coreLength =  protoBufferCore["Bytes"];

    fstream input(ptfile.c_str(), ios::in | ios::binary);
    if (!input.is_open()) {
      string msg = "Failed to open PB file" + ptfile;
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    IstreamInputStream inStream(&input);
    CodedInputStream codedInStream(&inStream);
    codedInStream.Skip(coreStartPos);
    codedInStream.PushLimit(coreLength);
    // max 512MB, warn at 400MB
    codedInStream.SetTotalBytesLimit(1024 * 1024 * 512, 1024 * 1024 * 400); 

    PBControlNet pbnet;

    // Now stream the rest of the input into the google protocol buffer.
    try {
      if (!pbnet.ParsePartialFromCodedStream(&codedInStream)) {
        string msg = "Failed to read input PB file " + ptfile;
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (...) {
      string msg = "Cannot parse binary PB file";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }

    PBControlNetLogData logData;
    bool readLogData = protoBufferInfo.HasObject("LogData");
    if(readLogData) {
      PvlObject &logDataInfo = protoBufferInfo.FindObject("LogData");
      BigInt logStartPos = logDataInfo["StartByte"];
      BigInt logLength = logDataInfo["Bytes"];

      input.clear();
      input.seekg(0, ios::beg);
      IstreamInputStream logInStream(&input);
      CodedInputStream codedLogInStream(&inStream);
      codedLogInStream.Skip(logStartPos);
      codedLogInStream.PushLimit(logLength);
      // max 512MB, warn at 400MB
      codedLogInStream.SetTotalBytesLimit(1024 * 1024 * 512, 1024 * 1024 * 400); 

      // Now stream the rest of the input into the google protocol buffer.
      try {
        if (!logData.ParsePartialFromCodedStream(&codedLogInStream)) {
          string msg = "Failed to read log data in PB file " + ptfile;
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
      catch (...) {
        string msg = "Cannot parse binary PB file's log data";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
      
      if(logData.points_size() != pbnet.points_size() || logData.points_size() == 0) {
        readLogData = false;
      }
    }

    // Set the private variable to the read in values from the input file.
    p_networkId = pbnet.networkid();
    p_targetName = pbnet.targetname();
    p_created = pbnet.created();
    p_modified = pbnet.lastmodified();
    p_description = pbnet.description();
    p_userName = pbnet.username();

    // Create a PvlObject for each point and create an Isis::ControlPoint
    // and call the Load(PvlObject &p, bool forceBuild = false) command with the pvlobject.
    for ( int pnts = 0 ; pnts < pbnet.points_size(); pnts++ ) {
      if(!readLogData) {
        ControlPoint point(pbnet.points(pnts));
        Add(point, true);
      }
      else {
        ControlPoint point(pbnet.points(pnts), logData.points(pnts));
        Add(point, true);
      }
    }

    input.close();
  }


  /**
   * Writes out the control network
   *
   * @param ptfile Name of file containing a Pvl list of control points 
   * @param pvl    Boolean indicating whether to write in pvl format 
   *               (Default=false)
   *
   * @throws Isis::iException::Programmer - "Invalid Net 
   *             Enumeration"
   * @throws Isis::iException::Io - "Unable to write PVL
   *             infomation to file"
   *
   * @history 2010-10-05 Tracie Sucharski - Renamed old WRite method to WritePvl 
   *                     and created this new method to determine format to
   *                     be written.
   */
  void ControlNet::Write(const iString &ptfile, bool pvl) {
    if (pvl) {
      WritePvl(ptfile);
    }
    else {
      WritePB(ptfile);
    }
  }


  /**
   *
   *
   *
   * @param ptfile 
   */
  void ControlNet::WritePB(const iString &ptfile) {

    //  Clear message before writing new
    PBControlNet pbnet;
    PBControlNetLogData logData;

    //  Gotta assign the Pedigree explicitly even though they default, otherwise
    //  they do not make it to the output file and error out!  Yes, this is by
    //  design.
    pbnet.mutable_pedigree()->set_version(pbnet.pedigree().version());
    pbnet.mutable_pedigree()->set_date(pbnet.pedigree().date());

    pbnet.set_networkid(p_networkId);
    pbnet.set_targetname(p_targetName);
    pbnet.set_created(p_created);
    pbnet.set_lastmodified(p_modified);
    pbnet.set_description(p_description);
    pbnet.set_username(p_userName);

    //  Now create ControlPoints
    for (int pnts = 0 ; pnts < p_pointsHash.size() ; pnts++ ) {
      ControlPoint &point = p_pointsHash[p_pointIds[pnts]];
      *pbnet.add_points() = point.ToProtocolBuffer();
      *logData.add_points() = point.GetLogProtocolBuffer();
    }

    const int labelBytes = 65536;
    fstream output(ptfile.c_str(), ios::out | ios::trunc | ios::binary);

    char *blankLabel = new char[labelBytes];
    memset(blankLabel, 0, labelBytes);
    output.write(blankLabel, labelBytes);

    streampos startCorePos = output.tellp();

    if (!pbnet.SerializeToOstream(&output)) {
      string msg = "Failed to write output PB file [" + ptfile + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    streampos coreSize = output.tellp() - startCorePos;

    streampos startLogPos = output.tellp();
    if (!logData.SerializeToOstream(&output)) {
      string msg = "Failed to write output PB file [" + ptfile + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    streampos logSize = output.tellp() - startLogPos;

    Pvl p;
    PvlObject protoObj("ProtoBuffer");

    PvlObject protoCore("Core");
    protoCore.AddKeyword(PvlKeyword("StartByte", iString(startCorePos)));
    protoCore.AddKeyword(PvlKeyword("Bytes", iString(coreSize)));
    protoObj.AddObject(protoCore);

    PvlGroup netInfo("ControlNetworkInfo");
    netInfo.AddComment("This group is for informational purposes only");
    netInfo += Isis::PvlKeyword("NetworkId", p_networkId);
    netInfo += Isis::PvlKeyword("TargetName", p_targetName);
    netInfo += Isis::PvlKeyword("UserName", p_userName);
    netInfo += Isis::PvlKeyword("Created", p_created);
    netInfo += Isis::PvlKeyword("LastModified", p_modified);
    netInfo += Isis::PvlKeyword("Description", p_description);
    netInfo += Isis::PvlKeyword("NumberOfPoints", p_pointsHash.count());
    netInfo += Isis::PvlKeyword("Proto_Version", pbnet.pedigree().version());
    protoObj.AddGroup(netInfo);

    // Now write the log data section
    PvlObject logInfo("LogData");
    logInfo.AddKeyword(PvlKeyword("StartByte", iString(startLogPos)));
    logInfo.AddKeyword(PvlKeyword("Bytes", iString(logSize)));
    protoObj.AddObject(logInfo);

    p.AddObject(protoObj);

    output.seekp(0, ios::beg);
    output << p;
    output << '\n';
    output.close();
  }


  /**
   * Writes out the ControlPoints in Pvl format
   *
   * @param ptfile Name of file containing a Pvl list of control points 
   * @throws Isis::iException::Programmer - "Invalid Net 
   *             Enumeration"
   * @throws Isis::iException::Io - "Unable to write PVL
   *             infomation to file"
   */
  void ControlNet::WritePvl(const iString &ptfile) {

    Pvl p;
    PvlObject net("ControlNetwork");
    net += PvlKeyword("NetworkId", p_networkId);

    net += PvlKeyword("TargetName", p_targetName);
    net += PvlKeyword("UserName", p_userName);
    iString mod = iString(p_modified).UpCase();
    if( mod == "NULL"  ||  mod == "" ) {
      SetModifiedDate( Isis::iTime::CurrentLocalTime() );
    }
    net += PvlKeyword("Created", p_created);
    net += PvlKeyword("LastModified", p_modified);
    net += PvlKeyword("Description", p_description);

    for (int i=0; i<(int)p_pointsHash.count(); i++) {
      PvlObject cp = p_pointsHash[p_pointIds[i]].ToPvlObject();
      net.AddObject(cp);
    }
    p.AddObject(net);

    try {
      p.Write(ptfile);
    }
    catch (iException e) {
      iString message = "Unable to write PVL infomation to file [" +
                       ptfile + "]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
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
    if(p_pointsHash.contains(point.GetId())) {
      iString msg = "ControlPoint must have unique Id";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    else {
      p_pointsHash.insert(point.GetId(), point);
      p_pointIds.push_back(point.GetId());
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
    if (index >= (int)p_pointsHash.count() || index < 0) {
      iString msg = "There is no ControlPoint at the given index number";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    else {
      // See if removing this point qualifies for a re-check of validity
      bool check = false;
      if( p_invalid && p_pointsHash[p_pointIds[index]].IsInvalid()) check = true;

      p_pointsHash.remove(p_pointIds[index]);
      p_pointIds.removeAt(index);

      // Check validity if needed
      if( check ) {
        p_invalid = false;
        for (int i=0; i<Size() && !p_invalid; i++) {
          if(p_pointsHash.contains(p_pointsHash[p_pointIds[i]].GetId())) {
              p_invalid = true;
          }
        }
      }
    }
  }


  void ControlNet::UpdatePoint(const ControlPoint &point) {
    *Find(point) = point;
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
  void ControlNet::Delete (const iString &id) {
    // If the QHash contains the point with this id, then
    // remove it from p_pointsHash.
    if(p_pointsHash.contains(QString::fromStdString(id))) {
      p_pointsHash.remove(QString::fromStdString(id));
    }
    else {
       // If a match was not found, throw an error
      iString msg = "A ControlPoint matching the id [" + id
        + "] was not found in the ControlNet";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // If the QVector contains the point with this id, then
    // remove it from p_pointIds.
    if(p_pointIds.contains(QString::fromStdString(id))) {
      p_pointIds.removeAt(p_pointIds.indexOf(QString::fromStdString(id)));
    }
    else {
       // If a match was not found, throw an error
      iString msg = "A ControlPoint matching the id [" + id
        + "] was not found in the ControlNet";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }


  /**
   * Compute error for each point in the network 
   *
   * @history 2010-01-11  Tracie Sucharski, Renamed from ComputeErrors
   */
  void ControlNet::ComputeResiduals() {
    // TODO:  Make sure the cameras have been initialized
    for (int i=0; i<(int)p_pointsHash.count(); i++) {
      p_pointsHash[p_pointIds[i]].ComputeResiduals();
    }
  }


  /**
   * Compute aprior values for each point in the network
   */
  void ControlNet::ComputeApriori() {
    // TODO:  Make sure the cameras have been initialized
    for (int i=0; i<(int)p_pointsHash.count(); i++) {
      p_pointsHash[p_pointIds[i]].ComputeApriori();
    }
  }


  /**
   * Sort the entire Control Net by Point ID using Quick sort
   *
   * @author sprasad (8/26/2010)
   */
  void ControlNet::SortControlNet() {
    p_pointIds.sort();
  }


  /**
   * Compute the average error of all points in the network 
   * @return <B>double</B> Average error of points 
   *
   * @history 2010-01-12  Tracie Sucharski - Renamed from AverageError 
   */
  double ControlNet::AverageResidual() {
    // TODO:  Make sure the cameras have been initialized
    double avgResidual = 0.0;
    int count = 0;
    for (int i=0; i<(int)p_pointsHash.count(); i++) {
      if (p_pointsHash[p_pointIds[i]].IsIgnored()) continue;
      avgResidual += p_pointsHash[p_pointIds[i]].GetAverageResidual();
      count++;
    }

    if (count == 0) return avgResidual;
    return avgResidual / count;
  }


  /**
   * Returns the camera list from the given image number
   *
   * @param index The image number
   *
   * @return Isis::Camera* The pointer to the resultant camera list
   */
  Isis::Camera *ControlNet::Camera(int index) {
    return p_cameraList[index];
  }


  /**
   * Return the Created Date
   *
   * @author Sharmila Prasad (10/6/2010)
   *
   * @return std::string 
   */
  iString ControlNet::CreatedDate() const {
    return p_created;
  }


  /**
   * Return the description of the network 
   *
   * @return The description of this Control Network 
   */
  iString ControlNet::Description() const {
    return p_description;
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
    if(p_pointsHash.contains(point.GetId())) {
      return true;
    }
    return false;
  }


 /**
  * Finds and returns a pointer to the ControlPoint with the same id
  *
  * @param point The ControlPoint with a matching ID
  *
  * @return <B>ControlPoint*</B> Pointer to the ControlPoint with
  *         the same ID
  */
  ControlPoint *ControlNet::Find(const ControlPoint &point) {
    return Find(point.GetId());
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
  ControlPoint *ControlNet::Find(const iString &id) {
    if(!p_pointIds.contains(QString::fromStdString(id))) {
      iString msg = "A ControlPoint matching the id [" + id
        + "] was not found in the ControlNet";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    return &p_pointsHash.find(QString::fromStdString(id)).value();
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
  ControlPoint *ControlNet::FindClosest(const iString &serialNumber,
                                        double sample, double line) {

    ControlPoint *savePoint=NULL;
    double dist;
    double minDist=99999.;
    for (int i=0; i < (int)p_pointsHash.count(); i++) {
      for (int j=0; j < p_pointsHash[p_pointIds[i]].Size(); j++) {
        if (p_pointsHash[p_pointIds[i]][j].GetCubeSerialNumber() != serialNumber) continue;
        //Find closest line sample & return that controlpoint
        dist = fabs(sample - p_pointsHash[p_pointIds[i]][j].GetSample()) +
               fabs(line - p_pointsHash[p_pointIds[i]][j].GetLine());
        if (dist < minDist) {
          minDist = dist;
          savePoint = &p_pointsHash[p_pointIds[i]];
        }
      }
    }

    return savePoint;
  }


  //! Return if the control point is invalid
  bool ControlNet::Invalid() const {
    return p_invalid;
  }


  /**
   * Determine the maximum error of all points in the network 
   * @return <B>double</B> Max error of points 
   *
   * @history 2010-01-12  Tracie Sucharski - Renamed from MaximumError 
   */
  double ControlNet::MaximumResidual() {
    // TODO:  Make sure the cameras have been initialized
    double maxResidual = 0.0;
    for (int i=0; i<(int)p_pointsHash.count(); i++) {
      double residual = p_pointsHash[p_pointIds[i]].GetMaximumResidual();
      if (residual > maxResidual) maxResidual = residual;
    }
    return maxResidual;
  }


  iString ControlNet::NetworkId() const {
    return p_networkId;
  }


  /**
   * Return the total number of edit locked measures for all control points in the 
   * network 
   *
   * @return Number of edit locked measures 
   */
  int ControlNet::NumEditLockMeasures() {
    int numLockedMeasures = 0;
    for (int cp = 0; cp < Size(); cp++) {
      ControlPoint &pt = p_pointsHash[p_pointIds[cp]];
      numLockedMeasures += pt.Size() - pt.GetNumLockedMeasures();
    }
    return numLockedMeasures;
  }


  /**
   * Returns the number of edit locked control points 
   *
   * @return Number of edit locked control points 
   */
  int ControlNet::NumEditLockPoints() {
    int size = 0;
    for(int cp = 0; cp < Size(); cp ++) {
      if (p_pointsHash[p_pointIds[cp]].IsEditLocked()) size ++;
    }
    return size;
  }


  /**
   * Return the total number of ignored measures for all control points in the 
   * network 
   *
   * @return Number of valid measures 
   */
  int ControlNet::NumIgnoredMeasures() {
    int numIgnoredMeasures = 0;
    for (int cp = 0; cp < Size(); cp++) {
      ControlPoint &pt = p_pointsHash[p_pointIds[cp]];
      numIgnoredMeasures += pt.Size() - pt.GetNumValidMeasures();
    }
    return numIgnoredMeasures;
  }


  /**
   * Returns the total number of measures for all control points in the network 
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


  //! Return the number of control points in the network
  int ControlNet::NumPoints() const {
    return p_pointsHash.size();
  }


  /**
   * Return the number of valid (non-ignored) measures for all control points 
   * in the network 
   *
   * @return Number of valid measures 
   */
  int ControlNet::NumValidMeasures() {
    int numValidMeasures = 0;
    for (int cp = 0; cp < Size(); cp++) {
      numValidMeasures += p_pointsHash[p_pointIds[cp]].GetNumValidMeasures();
    }
    return numValidMeasures;
  }


  /**
   * Returns the number of non-ignored control points 
   *
   * @return Number of valid control points 
   */
  int ControlNet::NumValidPoints() {
    int size = 0;
    for(int cp = 0; cp < Size(); cp ++) {
      if(!p_pointsHash[p_pointIds[cp]].IsIgnored()) size ++;
    }
    return size;
  }


  //! Return the number of control points in the network
  int ControlNet::Size() const {
    return p_pointsHash.size();
  }


  //! Return the target name
  iString ControlNet::Target() const {
    return p_targetName;
  }


  //! Return the user name
  iString ControlNet::UserName() const {
    return p_userName;
  }


  /**
   * Set the creation time 
   *
   * @param date The date this Control Network was created 
   */
  void ControlNet::SetCreatedDate(const iString &date) {
    p_created = date;
  }


  /**
    * Set the description of the network 
    *
    * @param desc The description of this Control Network
    */
  void ControlNet::SetDescription(const iString &newDescription) {
    p_description = newDescription;
  }


  /**
   * Creates the ControlNet's image cameras based on an input file
   *
   * @param imageListFile The list of images
   */
  void ControlNet::SetImages (const iString &imageListFile) {
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
      iString serialNumber = list.SerialNumber(i);
      iString filename = list.Filename(i);
      Pvl pvl(filename);

      try {
        Isis::Camera *cam = CameraFactory::Create(pvl);
        p_cameraMap[serialNumber] = cam;
        p_cameraList.push_back(cam);
      }
      catch (Isis::iException &e) {
        iString msg = "Unable to create camera for cube file ";
        msg += filename;
        throw Isis::iException::Message(Isis::iException::System,msg,_FILEINFO_);
      }

      if (progress != NULL) progress->CheckStatus();
    }

    // Loop through all measures and set the camera
    for (int p=0; p<Size(); p++) {
      for (int m=0; m<p_pointsHash[p_pointIds[p]].Size(); m++) {
        if (p_pointsHash[p_pointIds[p]][m].IsIgnored()) continue;
        iString serialNumber = p_pointsHash[p_pointIds[p]][m].GetCubeSerialNumber();
        if (list.HasSerialNumber(serialNumber)) {
          p_pointsHash[p_pointIds[p]][m].SetCamera(p_cameraMap[serialNumber]);
        }
        else {
          iString msg = "Control point [" +
              p_pointsHash[p_pointIds[p]].GetId() + "], measure [" +
              p_pointsHash[p_pointIds[p]][m].GetCubeSerialNumber() +
              "] does not have a cube with a matching serial number";
          throw Isis::iException::Message(iException::User, msg,
              _FILEINFO_);
          // TODO: DO we allow to continue or not?
        }
      }
    }
  }


  /**
   * Set the last modified date 
   *
   * @param date The last date this Control Network was modified
   */
  void ControlNet::SetModifiedDate(const iString &date) {
    p_modified = date;
  }


  /**
   * Set the network id 
   *
   * @param id The Id of this Control Network
   */
  void ControlNet::SetNetworkId(const iString &id) {
    p_networkId = id;
  }


  /**
   * Set the target name
   *
   * @param target The name of the target of this Control Network
   */
  void ControlNet::SetTarget(const iString &target) {
    p_targetName = target;
  }


  /**
   * Set the user name 
   *
   * @param name The name of the user creating or modifying this Control Net
   */
  void ControlNet::SetUserName(const iString &name) {
    p_userName = name;
  }


  ControlPoint ControlNet::operator[](int index) const {
    if(index >= p_pointIds.size()) {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return (*this)[p_pointIds.at(index)];
  }


  ControlPoint ControlNet::operator[](iString id) const {
    QHash <QString, ControlPoint>::const_iterator
        result = p_pointsHash.find(id);

    if(result == p_pointsHash.end()) {
      iString msg = "The control network has no control points with an ID "
          "equal to [" + id + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return p_pointsHash[id];
  }


  /**
   * Return the ith control point 
   *
   * @param index Control Point index 
   *
   * @return The Control Point at the provided index
   *
  ControlPoint &ControlNet::operator[](int index) {
    return p_pointsHash[p_pointIds[index]];
  }*/
}
