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
            if (cp.Ignore()) {
              p_numIgnoredMeasures += cp.Size();
            }
            else {
              for (int m=0; m<cp.Size(); m++) {
                if (cp[m].Ignore()) p_numIgnoredMeasures++;
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
  void ControlNet::ReadPBControl(const iString &ptfile){
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

    //  Clear message before reading new
    p_pbnet.Clear();

    // Now stream the rest of the input into the google protocol buffer.
    try {
      if (!p_pbnet.ParsePartialFromCodedStream(&codedInStream)) {
        string msg = "Failed to read input PB file " + ptfile;
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (...) {
      string msg = "Cannot parse binary PB file";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }

    int actNumControlPoints = 0;

    // Set the private variable to the read in values from the input file.
    p_networkId = p_pbnet.networkid();
    p_targetName = p_pbnet.targetname();
    p_created = p_pbnet.created();
    p_modified = p_pbnet.lastmodified();
    p_description = p_pbnet.description();
    p_userName = p_pbnet.username();

    // Produce numbers available in ControlNet
    int NumValidPoints(0), NumMeasures(0), NumValidMeasures(0),
        NumIgnoredMeasures(0);


    // Create a PvlObject for each point and create an Isis::ControlPoint
    // and call the Load(PvlObject &p, bool forceBuild = false) command with the pvlobject.
    for ( int pnts = 0 ; pnts < p_pbnet.points_size() ; pnts++ ) {
      ControlPoint *point = new ControlPoint(p_pbnet.points(pnts).id());
      switch(p_pbnet.points(pnts).type()) {
        case PBControlNet_PBControlPoint_PointType_Tie:
            point->SetType(ControlPoint::Tie);
            break;
        case PBControlNet_PBControlPoint_PointType_Ground:
            point->SetType(ControlPoint::Ground);
            break;
      }
      point->SetIgnore(p_pbnet.points(pnts).ignore());
      point->SetRejected(p_pbnet.points(pnts).jigsawrejected());

      const PBControlNet_PBControlPoint &pbPoint = p_pbnet.points(pnts);
      NumMeasures += pbPoint.measures_size();
      if (pbPoint.ignore()) {
        NumIgnoredMeasures += pbPoint.measures_size();
      }
      else {
        NumValidPoints++;
      }

      // Read apriori keywords
      if (pbPoint.has_apriorixyzsource()) {
        switch (pbPoint.apriorixyzsource()) {
          case PBControlNet_PBControlPoint_AprioriSource_None:
            point->SetAprioriSurfacePointSource(
                ControlPoint::SurfacePointSource::None);
            break;

          case PBControlNet_PBControlPoint_AprioriSource_User:
            point->SetAprioriSurfacePointSource(
                ControlPoint::SurfacePointSource::User);
            break;

          case PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures:
            point->SetAprioriSurfacePointSource(
                ControlPoint::SurfacePointSource::AverageOfMeasures);
            break;

          case PBControlNet_PBControlPoint_AprioriSource_Reference:
            point->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Reference);
            break;

          case PBControlNet_PBControlPoint_AprioriSource_Basemap:
            point->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Basemap);
            break;

          case PBControlNet_PBControlPoint_AprioriSource_BundleSolution:
            point->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::BundleSolution);
            break;

          default:
            break;
        }
      }

      if (pbPoint.has_apriorixyzsourcefile()){
        point->SetAprioriSurfacePointSourceFile(pbPoint.apriorixyzsourcefile());
      }

      if (pbPoint.has_aprioriradiussource()) {
        switch (pbPoint.aprioriradiussource()) {
          case PBControlNet_PBControlPoint_AprioriSource_None:
            point->SetAprioriRadiusSource(ControlPoint::RadiusSource::None);
            break;
          case PBControlNet_PBControlPoint_AprioriSource_User:
            point->SetAprioriRadiusSource(ControlPoint::RadiusSource::User);
            break;
          case PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures:
            point->SetAprioriRadiusSource(ControlPoint::RadiusSource::AverageOfMeasures);
            break;
          case PBControlNet_PBControlPoint_AprioriSource_Ellipsoid:
            point->SetAprioriRadiusSource(ControlPoint::RadiusSource::Ellipsoid);
            break;
          case PBControlNet_PBControlPoint_AprioriSource_DEM:
            point->SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
            break;
          case PBControlNet_PBControlPoint_AprioriSource_BundleSolution:
            point->SetAprioriRadiusSource(ControlPoint::RadiusSource::BundleSolution);
            break;
          default:
            break;
        }
      }

      if (pbPoint.has_aprioriradiussourcefile()) {
        point->SetAprioriRadiusSourceFile(pbPoint.aprioriradiussourcefile());
      }

      if (pbPoint.has_apriorix() && pbPoint.has_aprioriy() &&
          pbPoint.has_aprioriz()) {
        SurfacePoint apriori(pbPoint.apriorix(),
                             pbPoint.aprioriy(),
                             pbPoint.aprioriz());

        if (pbPoint.aprioricovar_size() > 0) {
          symmetric_matrix<double,upper> covar;
          covar.resize(3);
          covar.clear();
          covar(0,0) = pbPoint.aprioricovar(0);
          covar(0,1) = pbPoint.aprioricovar(1);
          covar(0,2) = pbPoint.aprioricovar(2);
          covar(1,1) = pbPoint.aprioricovar(3);
          covar(1,2) = pbPoint.aprioricovar(4);
          covar(2,2) = pbPoint.aprioricovar(5);
          apriori.SetRectangularMatrix(covar);
        }

        point->SetAprioriSurfacePoint(apriori);
      }

      if (pbPoint.has_x() && pbPoint.has_y() && pbPoint.has_z()) {
        SurfacePoint apost(pbPoint.x(),
                           pbPoint.y(),
                           pbPoint.z());

        if (pbPoint.apostcovar_size() > 0) {
          symmetric_matrix<double,upper> covar;
          covar.resize(3);
          covar.clear();
          covar(0,0) = pbPoint.aprioricovar(0);
          covar(0,1) = pbPoint.aprioricovar(1);
          covar(0,2) = pbPoint.aprioricovar(2);
          covar(1,1) = pbPoint.aprioricovar(3);
          covar(1,2) = pbPoint.aprioricovar(4);
          covar(2,2) = pbPoint.aprioricovar(5);
          apost.SetRectangularMatrix(covar);
        }

        point->SetSurfacePoint(apost);
      }

      for ( int m = 0 ; m < pbPoint.measures_size() ; m++ ) {
        // Create a PControlMeasure and fill in it's info.
        // with the values from the input file.
        ControlMeasure *measure = new ControlMeasure();
        measure->SetCubeSerialNumber(QString::fromStdString(pbPoint.measures(m).serialnumber()));
        switch(pbPoint.measures(m).type()) {
          case PBControlNet_PBControlPoint_PBControlMeasure::Reference:
            measure->SetType(ControlMeasure::Reference);
            break;
          case PBControlNet_PBControlPoint_PBControlMeasure::Candidate:
            measure->SetType(ControlMeasure::Candidate);
            break;
          case PBControlNet_PBControlPoint_PBControlMeasure::Manual:
            measure->SetType(ControlMeasure::Manual);
            break;
          case PBControlNet_PBControlPoint_PBControlMeasure::RegisteredPixel:
            measure->SetType(ControlMeasure::RegisteredPixel);
            break;
          case PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel:
            measure->SetType(ControlMeasure::RegisteredSubPixel);
            break;
          case PBControlNet_PBControlPoint_PBControlMeasure::Ground:
            measure->SetType(ControlMeasure::RegisteredSubPixel);
            break;
        }

        measure->SetIgnore(pbPoint.measures(m).ignore());
        measure->SetRejected(pbPoint.measures(m).jigsawrejected());
        measure->SetCoordinate(pbPoint.measures(m).measurement().sample(),
                               pbPoint.measures(m).measurement().line());
        if (pbPoint.measures(m).measurement().has_sampleresidual() ||
            pbPoint.measures(m).measurement().has_lineresidual()) {
          measure->SetResidual(pbPoint.measures(m).measurement().sampleresidual(),
                               pbPoint.measures(m).measurement().lineresidual());
        }

        if (pbPoint.measures(m).has_diameter()) {
          measure->SetDiameter(pbPoint.measures(m).diameter());
        }
        if (pbPoint.measures(m).has_apriorisample()) {
          measure->SetAprioriSample(pbPoint.measures(m).apriorisample());
        }
        if (pbPoint.measures(m).has_aprioriline()) {
          measure->SetAprioriLine(pbPoint.measures(m).aprioriline());
        }
        if (pbPoint.measures(m).has_samplesigma()) {
          measure->SetSampleSigma(pbPoint.measures(m).samplesigma());
        }
        if (pbPoint.measures(m).has_linesigma()) {
          measure->SetLineSigma(pbPoint.measures(m).linesigma());
        }

        measure->SetChooserName(QString::fromStdString(pbPoint.measures(m).choosername()));
        measure->SetDateTime(QString::fromStdString(pbPoint.measures(m).datetime()));
        measure->SetEditLock(pbPoint.measures(m).editlock());
        point->Add(*measure, false, false);
        delete measure;

        if ( measure->Ignore() ) NumIgnoredMeasures++;
        else NumValidMeasures++;
      }

      point->SetChooserName(p_pbnet.points(pnts).choosername());
      point->SetDateTime(p_pbnet.points(pnts).datetime());
      point->SetEditLock(p_pbnet.points(pnts).editlock());
      p_numMeasures += point->Size();
      Add(*point, true);
      actNumControlPoints++;
      delete point;
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
  void ControlNet::WritePB(const iString &ptfile){

    //  Clear message before writing new
    p_pbnet.Clear();
    //  Gotta assign the Pedigree explicitly even though they default, otherwise
    //  they do not make it to the output file and error out!  Yes, this is by
    //  design.
    p_pbnet.mutable_pedigree()->set_version(p_pbnet.pedigree().version());
    p_pbnet.mutable_pedigree()->set_date(p_pbnet.pedigree().date());

    p_pbnet.set_networkid(p_networkId);
    p_pbnet.set_targetname(p_targetName);
    p_pbnet.set_created(p_created);
    p_pbnet.set_lastmodified(p_modified);
    p_pbnet.set_description(p_description);
    p_pbnet.set_username(p_userName);

    //  Now create ControlPoints
    int numPtsWritten = 0;
    for (int pnts = 0 ; pnts < p_pointsHash.size() ; pnts++ ) {
      ControlPoint &point = p_pointsHash[p_pointIds[pnts]];
      PBControlNet_PBControlPoint *pbPoint = p_pbnet.add_points();

      pbPoint->set_id(point.Id());
      switch (point.Type()) {
        case ControlPoint::Tie:
          pbPoint->set_type(PBControlNet_PBControlPoint::Tie);
          break;
        case ControlPoint::Ground:
          pbPoint->set_type(PBControlNet_PBControlPoint::Ground);
          break;
      }

      if (!point.ChooserName().empty()) {
        pbPoint->set_choosername(point.ChooserName());
      }
      if (!point.DateTime().empty()) {
        pbPoint->set_datetime(point.DateTime());
      }
      if (point.EditLock()) pbPoint->set_editlock(true);
      if (point.Ignore()) pbPoint->set_ignore(true);
      if (point.IsRejected()) pbPoint->set_jigsawrejected(true);

      switch (point.AprioriSurfacePointSource()) {
        case ControlPoint::SurfacePointSource::None:
          break;
        case ControlPoint::SurfacePointSource::User:
          pbPoint->set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_User);
          break;
        case ControlPoint::SurfacePointSource::AverageOfMeasures:
          pbPoint->set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures);
          break;
        case ControlPoint::SurfacePointSource::Reference:
          pbPoint->set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_Reference);
          break;
        case ControlPoint::SurfacePointSource::Basemap:
          pbPoint->set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_Basemap);
          break;
        case ControlPoint::SurfacePointSource::BundleSolution:
          pbPoint->set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_BundleSolution);
          break;
        default:
          break;
      }
      if (!point.AprioriSurfacePointSourceFile().empty()) {
        pbPoint->set_apriorixyzsourcefile(point.AprioriSurfacePointSourceFile());
      }
      switch (point.AprioriRadiusSource()) {
        case ControlPoint::RadiusSource::None:
          break;
        case ControlPoint::RadiusSource::User:
          pbPoint->set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_User);
          break;
        case ControlPoint::RadiusSource::AverageOfMeasures:
          pbPoint->set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures);
          break;
        case ControlPoint::RadiusSource::Ellipsoid:
          pbPoint->set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_Ellipsoid);
          break;
        case ControlPoint::RadiusSource::DEM:
          pbPoint->set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_DEM);
          break;
        case ControlPoint::RadiusSource::BundleSolution:
          pbPoint->set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_BundleSolution);
          break;
        default:
          break;
      }
      if (!point.AprioriRadiusSourceFile().empty()) {
        pbPoint->set_aprioriradiussourcefile(point.AprioriRadiusSourceFile());
      }

      if (point.GetAprioriSurfacePoint().Valid()) {
        SurfacePoint apriori = point.GetAprioriSurfacePoint();
        pbPoint->set_apriorix(apriori.GetX());
        pbPoint->set_aprioriy(apriori.GetY());
        pbPoint->set_aprioriz(apriori.GetZ());

        symmetric_matrix<double,upper> covar = apriori.GetRectangularMatrix();
        if (covar(0,0) != 0. || covar(0,1) != 0. ||
            covar(0,2) != 0. || covar(1,1) != 0. ||
            covar(1,2) != 0. || covar(2,2) != 0.) {
          pbPoint->add_aprioricovar(covar(0,0));
          pbPoint->add_aprioricovar(covar(0,1));
          pbPoint->add_aprioricovar(covar(0,2));
          pbPoint->add_aprioricovar(covar(1,1));
          pbPoint->add_aprioricovar(covar(1,2));
          pbPoint->add_aprioricovar(covar(2,2));
        }
      }


      if (point.GetSurfacePoint().Valid()) {
        SurfacePoint apost = point.GetSurfacePoint();
        pbPoint->set_x(apost.GetX());
        pbPoint->set_y(apost.GetY());
        pbPoint->set_z(apost.GetZ());

        symmetric_matrix<double,upper> covar = apost.GetRectangularMatrix();
        if (covar(0,0) != 0. || covar(0,1) != 0. ||
            covar(0,2) != 0. || covar(1,1) != 0. ||
            covar(1,2) != 0. || covar(2,2) != 0.) {
          pbPoint->add_apostcovar(covar(0,0));
          pbPoint->add_apostcovar(covar(0,1));
          pbPoint->add_apostcovar(covar(0,2));
          pbPoint->add_apostcovar(covar(1,1));
          pbPoint->add_apostcovar(covar(1,2));
          pbPoint->add_apostcovar(covar(2,2));
        }
      }

      //  Process all measures in the point
      for ( int meas = 0 ; meas < point.Size() ; meas++ ) {
        const ControlMeasure &measure = point[meas];
        PBControlNet_PBControlPoint_PBControlMeasure *pbmeasure = pbPoint->add_measures();
        pbmeasure->set_serialnumber(measure.CubeSerialNumber());
        switch (measure.Type()) {
          case ControlMeasure::Reference:
            pbmeasure->set_type(PBControlNet_PBControlPoint_PBControlMeasure::Reference);
            break;
          case ControlMeasure::Candidate:
            pbmeasure->set_type(PBControlNet_PBControlPoint_PBControlMeasure::Candidate);
            break;
          case ControlMeasure::Manual:
            pbmeasure->set_type(PBControlNet_PBControlPoint_PBControlMeasure::Manual);
            break;
          case ControlMeasure::RegisteredPixel:
            pbmeasure->set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredPixel);
            break;
          case ControlMeasure::RegisteredSubPixel:
            pbmeasure->set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel);
            break;
          case ControlMeasure::Ground:
            pbmeasure->set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel);
            break;
        }

        if (measure.ChooserName() != "") {
          pbmeasure->set_choosername(measure.ChooserName());
        }
        if (measure.DateTime() != "") {
          pbmeasure->set_datetime(measure.DateTime());
        }
        if (measure.EditLock()) pbmeasure->set_editlock(true);

        if (measure.Ignore()) pbmeasure->set_ignore(true);

        if (measure.IsRejected()) pbmeasure->set_jigsawrejected(true);

        if (measure.Sample() != 0. && measure.Line() != 0. ) {
          PBControlNet_PBControlPoint_PBControlMeasure::PBMeasure *m = pbmeasure->mutable_measurement();
          m->set_sample(measure.Sample());
          m->set_line(measure.Line());
          if (measure.SampleResidual() != Isis::Null) {
            m->set_sampleresidual(measure.SampleResidual());
          }
          if (measure.LineResidual() != Isis::Null) {
            m->set_lineresidual(measure.LineResidual());
          }
        }

        if (measure.Diameter() != Isis::Null) pbmeasure->set_diameter(measure.Diameter());
        if (measure.AprioriSample() != Isis::Null) {
          pbmeasure->set_apriorisample(measure.AprioriSample());
        }
        if (measure.AprioriLine() != Isis::Null) {
          pbmeasure->set_aprioriline(measure.AprioriLine());
        }
        if (measure.SampleSigma() != Isis::Null) {
          pbmeasure->set_samplesigma(measure.SampleSigma());
        }
        if (measure.LineSigma() != Isis::Null) {
          pbmeasure->set_linesigma(measure.LineSigma());
        }

      }

      numPtsWritten++;
    }

    const int labelBytes =  65536;
    fstream output(ptfile.c_str(), ios::out | ios::trunc | ios::binary);

    char *blankLabel = new char[labelBytes];
    memset(blankLabel, 0, labelBytes);
    output.write(blankLabel, labelBytes);

    streampos startCorePos = output.tellp();

//    p_pbnet.PrintDebugString();
    if (!p_pbnet.SerializeToOstream(&output)) {
//      if (!p_pbnet.SerializePartialToOstream(&output)) {
      string msg = "Failed to write output PB file " + ptfile;
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    streampos size = output.tellp() - startCorePos;

    Pvl p;
    PvlObject protoObj("ProtoBuffer");

    PvlObject protoCore("Core");
    protoCore.AddKeyword(PvlKeyword("StartByte", iString(startCorePos)));
    protoCore.AddKeyword(PvlKeyword("Bytes", iString(size)));
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
    netInfo += Isis::PvlKeyword("Proto_Version", p_pbnet.pedigree().version());
    protoObj.AddGroup(netInfo);

    p.AddObject(protoObj);

    output.seekp(0, ios::beg);
    output << p;
    output << '\n';
    output.close();
//    google::protobuf::ShutdownProtobufLibrary();

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
      PvlObject cp = p_pointsHash[p_pointIds[i]].CreatePvlObject();
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
    if(p_pointsHash.contains(point.Id())) {
      iString msg = "ControlPoint must have unique Id";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    else {
      p_pointsHash.insert(point.Id(), point);
      p_pointIds.push_back(point.Id());
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
      if( p_invalid && p_pointsHash[p_pointIds[index]].Invalid()) check = true;

      p_pointsHash.remove(p_pointIds[index]);
      p_pointIds.removeAt(index);

      // Check validity if needed
      if( check ) {
        p_invalid = false;
        for (int i=0; i<Size() && !p_invalid; i++) {
          if(p_pointsHash.contains(p_pointsHash[p_pointIds[i]].Id())) {
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
      if (p_pointsHash[p_pointIds[i]].Ignore()) continue;
      avgResidual += p_pointsHash[p_pointIds[i]].AverageResidual();
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
    if(p_pointsHash.contains(point.Id())) {
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
    return Find(point.Id());
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
      double residual = p_pointsHash[p_pointIds[i]].MaximumResidual();
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
      numLockedMeasures += pt.Size() - pt.NumLockedMeasures();
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
      if (p_pointsHash[p_pointIds[cp]].EditLock()) size ++;
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
      numIgnoredMeasures += pt.Size() - pt.NumValidMeasures();
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
      numValidMeasures += p_pointsHash[p_pointIds[cp]].NumValidMeasures();
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
      if(!p_pointsHash[p_pointIds[cp]].Ignore()) size ++;
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
        if (p_pointsHash[p_pointIds[p]][m].Ignore()) continue;
        iString serialNumber = p_pointsHash[p_pointIds[p]][m].CubeSerialNumber();
        if (list.HasSerialNumber(serialNumber)) {
          p_pointsHash[p_pointIds[p]][m].SetCamera(p_cameraMap[serialNumber]);
        }
        else {
          iString msg = "Control point [" +
              p_pointsHash[p_pointIds[p]].Id() + "], measure [" +
              p_pointsHash[p_pointIds[p]][m].CubeSerialNumber() +
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


  /**
   * Return the ith control point 
   *
   * @param index Control Point index 
   *
   * @return The Control Point at the provided index
   */
  ControlPoint &ControlNet::operator[](int index) {
    return p_pointsHash[p_pointIds[index]];
  }
}
