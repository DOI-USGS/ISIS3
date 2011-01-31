#include "ControlNet.h"

#include <cmath>
#include <sstream>

#include <QtAlgorithms>
#include <QPair>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "Application.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlSerialNumber.h"
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


namespace Isis
{
  void ControlNet::Nullify()
  {
    points = NULL;
    serials = NULL;
    pointIds = NULL;
  }

  //!Creates an empty ControlNet object
  ControlNet::ControlNet()
  {
    Nullify();

    points = new QHash< QString, ControlPoint * >;
    serials = new QHash< QString, ControlSerialNumber * >;
    pointIds = new QStringList;

    p_invalid = false;
    p_numMeasures = 0;
    p_numIgnoredMeasures = 0;
    p_created = Application::DateTime();
    p_modified = Application::DateTime();
  }

  ControlNet::ControlNet(const ControlNet & other)
  {
    Nullify();

    points = new QHash< QString, ControlPoint * >;
    serials = new QHash< QString, ControlSerialNumber * >;
    pointIds = new QStringList;

    QHashIterator< QString, ControlPoint * > pointsIterator(*other.points);
    while (pointsIterator.hasNext())
    {
      pointsIterator.next();

      ControlPoint * newPoint = new ControlPoint(*pointsIterator.value());
      QString newPointId = newPoint->GetId();
      points->insert(newPointId, newPoint);
      pointIds->append(newPointId);

      QList< QString > newPointsSerials = newPoint->GetCubeSerialNumbers();
      for (int i = 0; i < newPointsSerials.size(); i++)
      {
        QString key = newPointsSerials[i];
        ControlMeasure * newMeasure = newPoint->GetMeasure(key);

        if (serials->contains(key))
        {
          (*serials)[key]->AddMeasure(newPointId, newMeasure);
        }
        else
        {
          ControlSerialNumber * newControlSerialNumber =
            new ControlSerialNumber(key);

          newControlSerialNumber->AddMeasure(newPointId, newMeasure);
          serials->insert(key, newControlSerialNumber);
        }
      }
    }

    p_targetName = other.p_targetName;
    p_networkId = other.p_networkId;
    p_created = other.p_created;
    p_modified = other.p_modified;
    p_description = other.p_description;
    p_userName = other.p_userName;
    p_numMeasures = other.p_numMeasures;
    p_numIgnoredMeasures = other.p_numIgnoredMeasures;
    p_invalid = other.p_invalid;
    p_cameraMap = other.p_cameraMap;
    p_cameraList = other.p_cameraList;
  }

  /**
   * Creates a ControlNet object with the given list of control points and cubes
   *
   * @param ptfile Name of file containing a Pvl list of control points
   * @param progress A pointer to the progress of reading in the control points
   */
  ControlNet::ControlNet(const iString & ptfile, Progress * progress)
  {
    Nullify();

    points = new QHash< QString, ControlPoint * >;
    serials = new QHash< QString, ControlSerialNumber * >;
    pointIds = new QStringList;

    p_invalid = false;
    p_numMeasures = 0;
    p_numIgnoredMeasures = 0;
    ReadControl(ptfile, progress);
  }


  ControlNet::~ControlNet()
  {
    if (points)
    {
      QHashIterator< QString, ControlPoint * > i(*points);
      while (i.hasNext())
      {
        i.next();
        delete(*points)[i.key()];
        (*points)[i.key()] = NULL;
      }
      delete points;
      points = NULL;
    }

    if (serials)
    {
      QHashIterator< QString, ControlSerialNumber * > i(*serials);
      while (i.hasNext())
      {
        i.next();
        delete(*serials)[i.key()];
        (*serials)[i.key()] = NULL;
      }
      delete serials;
      serials = NULL;
    }

    if (pointIds)
    {
      delete pointIds;
      pointIds = NULL;
    }
  }


  /**
   * Reads in the control points from the given file
   *
   * @param ptfile Name of file containing a Pvl list of control points
   * @param progress A pointer to the progress of reading in the control points
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
  void ControlNet::ReadControl(const iString & ptfile, Progress * progress)
  {
    Pvl p(ptfile);
    //Test to see if this is a binary control net file.
    if (p.HasObject("ProtoBuffer"))
    {
      ReadPBControl(ptfile);
    }
    else
    {
      try
      {
        PvlObject & cn = p.FindObject("ControlNetwork");

        if (cn.HasKeyword("NetworkId"))
          p_networkId = cn["NetworkId"][0];
        p_targetName = (std::string)cn["TargetName"];
        p_userName = (std::string)cn["UserName"];
        p_created = (std::string)cn["Created"];
        p_modified = (std::string)cn["LastModified"];
        if (cn.HasKeyword("Description"))
          p_description = cn["Description"][0];

        // Prep for reporting progress
        if (progress != NULL)
        {
          progress->SetText("Loading Control Points...");
          progress->SetMaximumSteps(cn.Objects());
          progress->CheckStatus();
        }
        for (int i = 0; i < cn.Objects(); i++)
        {
          try
          {
            if (cn.Object(i).IsNamed("ControlPoint"))
            {
              ControlPoint * newPoint = new ControlPoint;
              newPoint->Load(cn.Object(i));
              p_numMeasures += newPoint->GetNumMeasures();
              if (newPoint->IsIgnored())
              {
                p_numIgnoredMeasures += newPoint->GetNumMeasures();
              }
              else
              {
                QList< QString > measureIds = newPoint->GetCubeSerialNumbers();
                for (int i = 0; i < measureIds.size(); i++)
                {
                  if ((*newPoint)[measureIds[i]]->IsIgnored())
                    p_numIgnoredMeasures++;
                }
              }
              AddPoint(newPoint);
            }
          }
          catch (iException & e)
          {
            iString msg = "Invalid Control Point at position [" + iString(i)
                          + "]";
            throw iException::Message(iException::User, msg, _FILEINFO_);
          }
          if (progress != NULL)
            progress->CheckStatus();
        }
      }
      catch (iException & e)
      {
        iString msg = "Invalid Format in [" + ptfile + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
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
  void ControlNet::ReadPBControl(const iString & ptfile)
  {
    // Create an input file stream with the input file.
    Pvl protoFile(ptfile);

    PvlObject & protoBufferInfo = protoFile.FindObject("ProtoBuffer");
    PvlObject & protoBufferCore = protoBufferInfo.FindObject("Core");
    BigInt coreStartPos = protoBufferCore["StartByte"];
    BigInt coreLength =  protoBufferCore["Bytes"];

    fstream input(ptfile.c_str(), ios::in | ios::binary);
    if (!input.is_open())
    {
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
    try
    {
      if (!pbnet.ParsePartialFromCodedStream(&codedInStream))
      {
        string msg = "Failed to read input PB file " + ptfile;
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (...)
    {
      string msg = "Cannot parse binary PB file";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
    PBControlNetLogData logData;
    bool readLogData = protoBufferInfo.HasObject("LogData");
    if (readLogData)
    {
      PvlObject & logDataInfo = protoBufferInfo.FindObject("LogData");
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
      try
      {
        if (!logData.ParsePartialFromCodedStream(&codedLogInStream))
        {
          string msg = "Failed to read log data in PB file " + ptfile;
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
      catch (...)
      {
        string msg = "Cannot parse binary PB file's log data";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }

      if (logData.points_size() != pbnet.points_size() || logData.points_size() == 0)
      {
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
    for (int pnts = 0 ; pnts < pbnet.points_size(); pnts++)
    {
      if (!readLogData)
      {
        ControlPoint * point = new ControlPoint(pbnet.points(pnts));
        AddPoint(point);
      }
      else
      {
        ControlPoint * point = new ControlPoint(pbnet.points(pnts),
                                                logData.points(pnts));
        AddPoint(point);
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
  void ControlNet::Write(const iString & ptfile, bool pvl)
  {
    if (pvl)
    {
      WritePvl(ptfile);
    }
    else
    {
      WritePB(ptfile);
    }
  }


  /**
   *
   *
   *
   * @param ptfile
   */
  void ControlNet::WritePB(const iString & ptfile)
  {

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
    for (int i = 0; i < pointIds->size(); i++)
    {
      ControlPoint * point = points->value(pointIds->at(i));
      *pbnet.add_points() = point->ToProtocolBuffer();
      *logData.add_points() = point->GetLogProtocolBuffer();
    }

    const int labelBytes = 65536;
    fstream output(ptfile.c_str(), ios::out | ios::trunc | ios::binary);

    char * blankLabel = new char[labelBytes];
    memset(blankLabel, 0, labelBytes);
    output.write(blankLabel, labelBytes);

    streampos startCorePos = output.tellp();

    if (!pbnet.SerializeToOstream(&output))
    {
      string msg = "Failed to write output PB file [" + ptfile + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    streampos coreSize = output.tellp() - startCorePos;

    streampos startLogPos = output.tellp();
    if (!logData.SerializeToOstream(&output))
    {
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
    netInfo += Isis::PvlKeyword("NumberOfPoints", points->count());
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
  void ControlNet::WritePvl(const iString & ptfile)
  {

    Pvl p;
    PvlObject net("ControlNetwork");
    net += PvlKeyword("NetworkId", p_networkId);

    net += PvlKeyword("TargetName", p_targetName);
    net += PvlKeyword("UserName", p_userName);
    iString mod = iString(p_modified).UpCase();
    if (mod == "NULL"  ||  mod == "")
    {
      SetModifiedDate(Isis::iTime::CurrentLocalTime());
    }
    net += PvlKeyword("Created", p_created);
    net += PvlKeyword("LastModified", p_modified);
    net += PvlKeyword("Description", p_description);

    for (int i = 0; i < pointIds->size(); i++)
    {
      ControlPoint * point = points->value(pointIds->at(i));
      PvlObject cp = point->ToPvlObject();
      net.AddObject(cp);
    }
    
    p.AddObject(net);

    try
    {
      p.Write(ptfile);
    }
    catch (iException e)
    {
      iString message = "Unable to write PVL infomation to file [" +
                        ptfile + "]";
      throw Isis::iException::Message(Isis::iException::Io, message, _FILEINFO_);
    }
  }


  /**
   * Adds a ControlPoint to the ControlNet
   *
   * @param point Control point to be added
   *
   * @throws Isis::iException::Programmer - "ControlPoint must
   *             have unique Id"
   */
  void ControlNet::AddPoint(ControlPoint * point)
  {
    if (!point)
    {
      iString msg = "Null pointer passed to ControlNet::AddPoint!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (ContainsPoint(point->GetId()))
    {
      iString msg = "ControlPoint must have unique Id";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    QString pointId = point->GetId();
    points->insert(pointId, point);
    pointIds->append(pointId);

    point->parentNetwork = this;
    QList< QString > serials = point->GetCubeSerialNumbers();
    if (serials.size())
    {
      for (int i = 0; i < serials.size(); i++) {
        AddControlSerialNumber(point, point->GetMeasure(serials[i]));
      }
    }
  }


  void ControlNet::AddControlSerialNumber(const ControlPoint * point,
                                          ControlMeasure * measure)
  {
    iString msg;
    if (!point)
      msg = "Null point passed to ControlNet::AddMeasure!";

    if (!measure)
      msg += "\nNull measure passed to ControlNet::AddMeasure!";

    if (msg != "")
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);

    if (!ContainsPoint(point->GetId()))
    {
      msg = "ControlNet does not contain the point [";
      msg += point->GetId() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    QString key = measure->GetCubeSerialNumber();
    if (serials->contains(key))
    {
      (*serials)[key]->AddMeasure(point->GetId(), measure);
    }
    else
    {
      ControlSerialNumber * newControlSerialNumber =
        new ControlSerialNumber(measure->GetCubeSerialNumber());

      newControlSerialNumber->AddMeasure(point->GetId(), measure);
      serials->insert(measure->GetCubeSerialNumber(), newControlSerialNumber);
    }
  }

  void ControlNet::RemoveControlSerialNumber(ControlPoint * point,
      ControlMeasure * measure)
  {
    QString key = measure->GetCubeSerialNumber();
    ControlSerialNumber * serialNumber = (*serials)[key];

    QString pointId = point->GetId();
    serialNumber->RemoveMeasure(pointId);

    if (!serialNumber->GetNumMeasures())
    {
      serials->remove(key);

      delete serialNumber;
      serialNumber = NULL;
    }
  }


  /**
   */
  void ControlNet::DeletePoint(iString pointId)
  {
    if (!points->contains(pointId))
    {
      iString msg = "point Id [" + pointId + "] does not exist in the network";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    else
    {
      // See if removing this point qualifies for a re-check of validity
      bool check = false;
      if (p_invalid && points->value(pointId)->IsInvalid())
        check = true;

      points->remove(pointId);
      pointIds->removeAt(pointIds->indexOf(pointId));

      // Check validity if needed
      if (check)
      {
        p_invalid = false;

        QList< QString > keys = points->keys();
        for (int i = 0; i < keys.size() && !p_invalid; i++)
        {
          if (points->count(keys[i]) > 1)
            p_invalid = true;
        }
      }
    }
  }


  void ControlNet::DeletePoint(int index)
  {
    if (index < 0 || index >= pointIds->size())
    {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    DeletePoint(pointIds->at(index));
  }


  bool ControlNet::ContainsPoint(iString pointId) const
  {
    bool contains = false;

    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext())
    {
      i.next();
      if (i.value()->GetId() == (iString) pointId)
      {
        contains = true;
        break;
      }
    }

    return contains;
  }
  
  
  void ControlNet::ValidateSerialNumber(iString serialNumber) const
  {
    if (!serials->contains(serialNumber))
    {
      iString msg = "Cube Serial Number [" + serialNumber + "] not found in "
          "the network";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }
  
  
  /**
   * Get all the measures pertaining to a given cube serial number
   *
   * @returns A list of all measures which are in a given cube
   */
  QList< ControlMeasure * > ControlNet::GetMeasuresInCube(iString serialNumber)
  {
    ValidateSerialNumber(serialNumber);
    return (*serials)[serialNumber]->GetMeasures();
  }

  
  /**
   * Essentially removes a cube from the networkid
   *
   * @param serialNumber The cube serial number to be removed from the network
   */
  void ControlNet::DeleteMeasuresWithId(iString serialNumber)
  {
    ValidateSerialNumber(serialNumber);
    
    ControlSerialNumber * csn = (*serials)[serialNumber];
    QList< QString > pointIds = csn->GetPointIds();
    foreach (QString id, pointIds)
    {
      ControlPoint * point = GetPoint(id);
      iString serialNumber = csn->GetMeasure(id)->GetCubeSerialNumber();
 
      point->Delete(serialNumber);
    }
  }


  /**
   * Compute error for each point in the network
   *
   * @history 2010-01-11  Tracie Sucharski, Renamed from ComputeErrors
   */
  void ControlNet::ComputeResiduals()
  {
    // TODO:  Make sure the cameras have been initialized

    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext())
    {
      i.next();
      i.value()->ComputeResiduals();
    }
  }


  /**
   * Compute aprior values for each point in the network
   */
  void ControlNet::ComputeApriori()
  {
    // TODO:  Make sure the cameras have been initialized
    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext())
    {
      i.next();
      i.value()->ComputeApriori();
    }
  }


  /**
   * Compute the average error of all points in the network
   * @return <B>double</B> Average error of points
   *
   * @history 2010-01-12  Tracie Sucharski - Renamed from AverageError
   */
  double ControlNet::AverageResidual()
  {
    // TODO:  Make sure the cameras have been initialized
    double avgResidual = 0.0;
    int count = 0;
    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext())
    {
      i.next();
      ControlPoint * point = i.value();
      if (!point->IsIgnored())
      {
        avgResidual += point->GetAverageResidual();
        count++;
      }
    }

    if (count != 0)
      avgResidual /= count;

    return avgResidual;
  }


  /**
   * Returns the camera list from the given image number
   *
   * @param index The image number
   *
   * @return Isis::Camera* The pointer to the resultant camera list
   */
  Isis::Camera * ControlNet::Camera(int index)
  {
    return p_cameraList[index];
  }


  /**
   * Return the Created Date
   *
   * @author Sharmila Prasad (10/6/2010)
   *
   * @return std::string
   */
  iString ControlNet::CreatedDate() const
  {
    return p_created;
  }


  /**
   * Return the description of the network
   *
   * @return The description of this Control Network
   */
  iString ControlNet::Description() const
  {
    return p_description;
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
  ControlPoint * ControlNet::FindClosest(iString serialNumber,
                                         double sample, double line)
  {
    if (!serials->contains(serialNumber))
    {
      iString msg = "serialNumber [";
      msg += serialNumber;
      msg += "] not found in ControlNet";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    const double SEARCH_DISTANCE = 99999999.0;
    double minDist = SEARCH_DISTANCE;
    ControlPoint * closestPoint = NULL;

    ControlSerialNumber * csn = (*serials)[serialNumber];
    QList< QString > pointIds = csn->GetPointIds();
    for (int i = 0; i < pointIds.size(); i++)
    {
      ControlMeasure * measureToCheck = (*csn)[pointIds[i]];

      //Find closest line sample & return that controlpoint
      double dx = fabs(sample - measureToCheck->GetSample());
      double dy = fabs(line - measureToCheck->GetLine());

      double dist = sqrt((dx * dx) + (dy * dy));

      if (dist < minDist)
      {
        minDist = dist;
        closestPoint = measureToCheck->Parent();
      }
    }

    if (!closestPoint)
    {
      iString msg = "No point found within ";
      msg += iString(SEARCH_DISTANCE);
      msg += "pixels of sample/line [";
      msg += iString(sample);
      msg += ", ";
      msg += iString(line);
      msg += "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return closestPoint;
  }


  //! Return if the control point is invalid
  bool ControlNet::IsValid() const
  {
    return !p_invalid;
  }


  /**
   * Determine the maximum error of all points in the network
   * @return <B>double</B> Max error of points
   *
   * @history 2010-01-12  Tracie Sucharski - Renamed from MaximumError
   */
  double ControlNet::GetMaximumResidual()
  {
    // TODO:  Make sure the cameras have been initialized

    double maxResidual = 0.0;
    foreach(ControlPoint * p, *points)
    {
      double residual = p->GetMaximumResidual();
      if (residual > maxResidual)
        maxResidual = residual;
    }

    return maxResidual;
  }


  iString ControlNet::GetNetworkId() const
  {
    return p_networkId;
  }


  /**
   * Return the total number of edit locked measures for all control points in the
   * network
   *
   * @return Number of edit locked measures
   */
  int ControlNet::GetNumEditLockMeasures()
  {
    int numLockedMeasures = 0;
    foreach(ControlPoint * p, *points)
    numLockedMeasures += p->GetNumMeasures() - p->GetNumLockedMeasures();

    return numLockedMeasures;
  }


  /**
   * Returns the number of edit locked control points
   *
   * @return Number of edit locked control points
   */
  int ControlNet::GetNumEditLockPoints()
  {
    int editLockPoints = 0;
    foreach(ControlPoint * p, *points)
    {
      if (p->IsEditLocked())
        editLockPoints++;
    }

    return editLockPoints;
  }


  /**
   * Return the total number of ignored measures for all control points in the
   * network
   *
   * @return Number of valid measures
   */
  int ControlNet::GetNumIgnoredMeasures()
  {
    int numIgnoredMeasures = 0;
    foreach(ControlPoint * p, *points)
    numIgnoredMeasures += p->GetNumMeasures() - p->GetNumValidMeasures();

    return numIgnoredMeasures;
  }


  /**
   * Returns the total number of measures for all control points in the network
   *
   * @return Number of control measures
   */
  int ControlNet::GetNumMeasures() const
  {
    int numMeasures = 0;
    foreach(ControlPoint * p, *points)
    numMeasures += p->GetNumMeasures();

    return numMeasures;
  }


  //! Return the number of control points in the network
  int ControlNet::GetNumPoints() const
  {
    return points->size();
  }


  /**
   * Return the number of valid (non-ignored) measures for all control points
   * in the network
   *
   * @return Number of valid measures
   */
  int ControlNet::GetNumValidMeasures()
  {
    int numValidMeasures = 0;
    foreach(ControlPoint * p, *points)
    numValidMeasures += p->GetNumValidMeasures();

    return numValidMeasures;
  }


  /**
   * Returns the number of non-ignored control points
   *
   * @return Number of valid control points
   */
  int ControlNet::GetNumValidPoints()
  {
    int validPoints = 0;
    foreach(ControlPoint * p, *points)
    {
      if (!p->IsIgnored())
        validPoints++;
    }

    return validPoints;
  }


  //! Return the target name
  iString ControlNet::GetTarget() const
  {
    return p_targetName;
  }


  //! Return the user name
  iString ControlNet::GetUserName() const
  {
    return p_userName;
  }

  //! Return QList of ControlPoint Ids used in hash, in order of addition 
  QList< QString > ControlNet::GetPointIds() const
  {
    return *pointIds;
  }


  /**
   * Set the creation time
   *
   * @param date The date this Control Network was created
   */
  void ControlNet::SetCreatedDate(const iString & date)
  {
    p_created = date;
  }


  /**
    * Set the description of the network
    *
    * @param desc The description of this Control Network
    */
  void ControlNet::SetDescription(const iString & newDescription)
  {
    p_description = newDescription;
  }


  /**
   * Creates the ControlNet's image cameras based on an input file
   *
   * @param imageListFile The list of images
   */
  void ControlNet::SetImages(const iString & imageListFile)
  {
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
  void ControlNet::SetImages(SerialNumberList & list, Progress * progress)
  {
    // Prep for reporting progress
    if (progress != NULL)
    {
      progress->SetText("Setting input images...");
      progress->SetMaximumSteps(list.Size());
      progress->CheckStatus();
    }
    // Open the camera for all the images in the serial number list
    for (int i = 0; i < list.Size(); i++)
    {
      iString serialNumber = list.SerialNumber(i);
      iString filename = list.Filename(i);
      Pvl pvl(filename);

      try
      {
        Isis::Camera * cam = CameraFactory::Create(pvl);
        p_cameraMap[serialNumber] = cam;
        p_cameraList.push_back(cam);
      }
      catch (Isis::iException & e)
      {
        iString msg = "Unable to create camera for cube file ";
        msg += filename;
        throw Isis::iException::Message(Isis::iException::System, msg, _FILEINFO_);
      }

      if (progress != NULL)
        progress->CheckStatus();
    }

    // Loop through all measures and set the camera
    QHashIterator< QString, ControlPoint * > p(*points);
    while (p.hasNext())
    {
      p.next();
      ControlPoint * curPoint = p.value();

      QList< QString > serialNums = curPoint->GetCubeSerialNumbers();
      for (int m = 0; m < serialNums.size(); m++)
      {
        ControlMeasure * curMeasure = (*curPoint)[serialNums[m]];

        if (!curMeasure->IsIgnored())
        {
          iString serialNumber = curMeasure->GetCubeSerialNumber();
          if (list.HasSerialNumber(serialNumber))
          {
            curMeasure->SetCamera(p_cameraMap[serialNumber]);
          }
          else
          {
            iString msg = "Control point [" + curPoint->GetId() +
                          "], measure [" + curMeasure->GetCubeSerialNumber() +
                          "] does not have a cube with a matching serial number";
            throw Isis::iException::Message(iException::User, msg, _FILEINFO_);
            // TODO: DO we allow to continue or not?
          }
        }
      }
    }
  }


  /**
   * Set the last modified date
   *
   * @param date The last date this Control Network was modified
   */
  void ControlNet::SetModifiedDate(const iString & date)
  {
    p_modified = date;
  }


  /**
   * Set the network id
   *
   * @param id The Id of this Control Network
   */
  void ControlNet::SetNetworkId(const iString & id)
  {
    p_networkId = id;
  }


  /**
   * Set the target name
   *
   * @param target The name of the target of this Control Network
   */
  void ControlNet::SetTarget(const iString & target)
  {
    p_targetName = target;
  }


  /**
   * Set the user name
   *
   * @param name The name of the user creating or modifying this Control Net
   */
  void ControlNet::SetUserName(const iString & name)
  {
    p_userName = name;
  }

  const ControlNet & ControlNet::operator=(ControlNet other)
  {
    if (this == &other)
      return *this;

    if (points)
    {
      QHashIterator< QString, ControlPoint * > i(*points);
      while (i.hasNext())
      {
        i.next();
        delete(*points)[i.key()];
        (*points)[i.key()] = NULL;
      }
      delete points;
      points = NULL;
    }

    if (serials)
    {
      QHashIterator< QString, ControlSerialNumber * > i(*serials);
      while (i.hasNext())
      {
        i.next();
        delete(*serials)[i.key()];
        (*serials)[i.key()] = NULL;
      }
      delete serials;
      serials = NULL;
    }

    points = new QHash< QString, ControlPoint * >;
    serials = new QHash< QString, ControlSerialNumber * >;

    QHashIterator < QString, ControlPoint * > i(*other.points);
    while (i.hasNext())
    {
      i.next();

      ControlPoint * newPoint = new ControlPoint(*i.value());
      QString newPointId = newPoint->GetId();
      points->insert(newPointId, newPoint);

      QList< QString > newPointsSerials = newPoint->GetCubeSerialNumbers();
      for (int i = 0; i < newPointsSerials.size(); i++)
      {
        QString key = newPointsSerials[i];
        ControlMeasure * newMeasure = newPoint->GetMeasure(key);

        if (serials->contains(key))
        {
          (*serials)[key]->AddMeasure(newPointId, newMeasure);
        }
        else
        {
          ControlSerialNumber * newControlSerialNumber =
            new ControlSerialNumber(key);

          newControlSerialNumber->AddMeasure(newPointId, newMeasure);
          serials->insert(key, newControlSerialNumber);
        }
      }
    }

    p_targetName = other.p_targetName;
    p_networkId = other.p_networkId;
    p_created = other.p_created;
    p_modified = other.p_modified;
    p_description = other.p_description;
    p_userName = other.p_userName;
    p_numMeasures = other.p_numMeasures;
    p_numIgnoredMeasures = other.p_numIgnoredMeasures;
    p_invalid = other.p_invalid;
    p_cameraMap = other.p_cameraMap;
    p_cameraList = other.p_cameraList;

    return *this;
  }


  const ControlPoint * ControlNet::GetPoint(QString id) const
  {
    if (!points->contains(id))
    {
      iString msg = "The control network has no control points with an ID "
                    "equal to [" + id + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return points->value(id);
  }


  ControlPoint * ControlNet::GetPoint(QString id)
  {
    if (!points->contains(id))
    {
      iString msg = "The control network has no control points with an ID "
                    "equal to [" + id + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return (*points)[id];
  }


  const ControlPoint * ControlNet::GetPoint(int index) const
  {
    if (index < 0 || index >= pointIds->size())
    {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetPoint(pointIds->at(index));
  }


  ControlPoint * ControlNet::GetPoint(int index)
  {
    if (index < 0 || index >= pointIds->size())
    {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetPoint(pointIds->at(index));
  }


  const ControlPoint * ControlNet::operator[](QString id) const
  {
    return GetPoint(id);
  }


  ControlPoint * ControlNet::operator[](QString id)
  {
    return GetPoint(id);
  }


  const ControlPoint * ControlNet::operator[](int index) const
  {
    return GetPoint(index);
  }


  ControlPoint * ControlNet::operator[](int index)
  {
    return GetPoint(index);
  }
}
