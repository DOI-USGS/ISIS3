#include "Isis.h"

#include <QFile>
#include <QtGlobal>
#include <QTextStream>
#include <QScopedPointer>

// boost library
#include <boost/foreach.hpp>

#include "nanoflann/nanoflann.hpp"

#include "ControlNet.h"
#include "ControlPointCloudPt.h"
#include "ControlPointMerger.h"
#include "FileList.h"
#include "FileName.h"
#include "IString.h"
#include "PointCloud.h"
#include "PointCloudTree.h"
#include "ProcessByLine.h"
#include "Progress.h"
#include "SerialNumber.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "IException.h"

using namespace std;
using namespace Isis;

/** Format an incoming 3-point vector to a string */
inline QString formatVector(const double *v, const QString &sep = "") {
  QString s(sep);
  QString data;
  for ( int i = 0 ; i < 3 ; i++) {
    data += (s +  QString::number(v[i]));
    s = ",";
  }
  return (data);
}

// Control network manager
typedef QList<QSharedPointer<ControlNet> >    ControlNetList;

// ControlNet point cloud types. Dist3d distance works for all types.
typedef ControlPointCloudPt         PointType;
typedef Dist3d<ControlPointCloudPt> DistanceType;

typedef PointCloud<PointType, DistanceType>             CNetPointCloud;
typedef PointCloudTree<PointType, DistanceType>         CNetPointCloudTree;
typedef PointCloudSearchResult<PointType, DistanceType> ResultType;

void IsisMain() {
 
  // We will be processing by line
  ProcessByLine p;
  UserInterface &ui = Application::GetUserInterface();

  QStringList cnetfiles;
  int nBase(0);
  if ( ui.WasEntered("CNETBASE") ) {
    cnetfiles.append(ui.GetAsString("CNETBASE")); 
    nBase++;
  }

  int nFrom(0);
  if ( ui.WasEntered("CNETFROM") ) {
    cnetfiles.append(ui.GetAsString("CNETFROM")); 
    nFrom++;
  }

  int nList(0);
  if ( ui.WasEntered("CNETLIST") ) {
    FileList list_o_nets(ui.GetFileName("CNETLIST"));
    if (list_o_nets.size() < 1) {
      QString mess = "The file provided for CNETLIST, [";
      mess += ui.GetAsString("CNETLIST");
      mess += "] is empty.";
      throw IException(IException::User, mess, _FILEINFO_);
    }
    BOOST_FOREACH ( FileName cfile, list_o_nets ) {
      cnetfiles.append( cfile.original() );
      nList++;
    }
  }

  // Check for any files at all
  int totalNetFiles = nBase + nFrom + nList;
  if ( totalNetFiles <= 0 ) {
    QString mess = "No input networks files provided!";
    throw IException(IException::User, mess, _FILEINFO_);
  }

  // Ok, if we end up with one net, it must be entered in CNETBASE.
  if ( (cnetfiles.size() == 1) && (1 != nBase) ) {
    QString mess = "It appears you are attempting to merge points from a "
                   "single network, which is fine, but it must be specified "
                   "in CNETBASE. Try again if that is your intent!";
    throw IException(IException::User, mess, _FILEINFO_);
  }

  // Now establish the type of processing mode we are dealing with. Can be 
  // IMAGE or GROUND.
  QString cmode = ui.GetString("MODE").toLower();
  ControlPointCloudPt::CoordinateType ctype = ( "image" == cmode) ? 
                                                ControlPointCloudPt::Image :
                                                ControlPointCloudPt::Ground ;
 
  // If image node is requested, get the image to use as a reference
  QString serialno;
  if ( "image" == cmode ) {
    if ( ui.WasEntered("REFERENCE")) {
      serialno = SerialNumber::Compose(ui.GetAsString("REFERENCE")); 
    }
  }

  // Create the point cloud container and load the control networks
  QScopedPointer<CNetPointCloud> cloud(new CNetPointCloud()); 
  // ControlNetList cnetlist;

  // Collect some stuff from input nets for the output net
  QString netid;
  QString target;
  QString description;
  QVector<Distance> radii;
  BigInt allPoints(0);
  BOOST_FOREACH ( QString cfile, cnetfiles ) {
    std::cout << "\nLoading " << cfile << "...\n";
    Progress c_progress;
    QScopedPointer<ControlNet> cnet( new ControlNet(cfile, &c_progress) );
    if ( netid.isEmpty() ) { 
      netid = cnet->GetNetworkId(); 
    }
    if ( target.isEmpty() ) { 
      target = cnet->GetTarget(); 
    }
    if ( description.isEmpty() ) { 
      description = cnet->Description(); 
    }
    if ( radii.isEmpty() )  { 
      radii = QVector<Distance>::fromStdVector(cnet->GetTargetRadii()); 
    }

    // Get all control points by taking ownership from the control net
    int npoints(0);
    QList<ControlPoint *> points = cnet->take();
    BOOST_FOREACH ( ControlPoint *point, points) {
      ControlPointCloudPt cpt(point, ctype, ControlPointCloudPt::Exclusive, serialno);
      if ( cpt.isValid() ) { 
        cloud->addPoint(cpt); 
        npoints++;
      }
    }
    std::cout << "Added " << npoints << " of " << points.size() << "\n";
    allPoints += points.size();

    // Instead of having to save the ControlNet instances, we take ownership
    // of all the points from ControlNet and turn it over to the cloud...
    // 
    //  cnetlist.append( QSharedPointer<ControlNet> ( cnet.take() ) );
  }
  std::cout << "\nTotal " << cloud->size() << " of " << allPoints << "\n";

  // Set up conditions for search 
  double image_tolerance  = ui.GetDouble("IMAGETOL");
  double ground_tolerance = ui.GetDouble("GROUNDTOL");
  double search_distance  = ui.GetDouble("DISTANCE");
  double search_radius_sq = search_distance * search_distance;

  std::cout << "\nCreating cloud kd-tree..\n";
  int kd_nodes = ui.GetInteger("KDNODES");
  CNetPointCloudTree cloud_t( cloud.take(), kd_nodes );

  // Get the cloud back to use in subsequent processing
  const CNetPointCloud &v_cloud = cloud_t.cloud();
  Progress progress;
  progress.SetText("merging");
  progress.SetMaximumSteps( v_cloud.size() );
  progress.CheckStatus();


  //  Run through all valid points. Note they may be invalided as processing
  //  is done through mergers, so validity must be checked!!!
  BigInt nfound(0);
  BigInt nMerged(0);
  for (int n = 0 ; n < v_cloud.size() ; n++) {
    const ControlPointCloudPt &point = v_cloud.point(n);
    if ( point.isValid() ) {
      ResultType results = cloud_t.radius_query(point, search_radius_sq); 
      nfound += results.size();
      ControlPointMerger merger(image_tolerance, ground_tolerance);
      results.forEachPair(merger);
      nMerged += merger.merge();
    }
    progress.CheckStatus();
  }

  // Now check all remaining points by ignoring all points with less than
  // MINMEASURES.
  BigInt nMinMeasures(0);
  bool setaprioribest = ui.GetBoolean("SETAPRIORIBEST");
  QString rejectedmeasures = ui.GetString("REJECTEDMEASURES").toLower();
  bool ignoreRejected = ( "ignore" == rejectedmeasures);
  bool removeRejected = ( "remove" == rejectedmeasures);

  int minmeasures = ui.GetInteger("MINMEASURES");
  bool savemins = ui.GetBoolean("SAVEMINS");

  // Now create the output control network
  BigInt oPoints(0);
  BigInt nRejected(0);

  ControlNet cnet;
  if ( ui.WasEntered("NETWORKID") ) {
    netid = ui.GetString("NETWORKID");
  }

  cnet.SetNetworkId(netid); 
  cnet.SetUserName(Application::UserName());

  if ( ui.WasEntered("DESCRIPTION") ) {  
    description = ui.GetString("DESCRIPTION"); 
  }

  cnet.SetDescription(description); 
  cnet.SetCreatedDate(Application::DateTime());
  cnet.SetTarget(target, radii);

  // Gotta transfer all points/measures
  for ( int i = 0 ; i < v_cloud.size() ; i++) {
    ControlPointCloudPt point = v_cloud.point(i);

    if ( point.isValid() ) {
      
      // Set up a reference for convenience
      ControlPoint &cp = point.getPoint();

      // Processes measures if requested
      if ( true == removeRejected ) {

        QList<ControlMeasure *> measures = cp.getMeasures( false );
        ControlMeasure *refm = cp.GetRefMeasure();
        bool removedRef = ( 0 == refm ) ? true : false;

        BOOST_FOREACH ( ControlMeasure *m, measures) {
          if ( m->IsRejected() ) {  
            if ( (0 != refm) && (*m == *refm)) { 
              removedRef = true; 
              refm = 0;
            }
            cp.Delete(m); 
            nRejected++;
          }
        }

        // If we removed the reference, simply set to the first measure
        if ( removedRef && (cp.GetNumValidMeasures() > 0 ) ) {
          cp.SetRefMeasure(0);
        }
      }
      // Set rejected points to ignore
      else if ( true == ignoreRejected ) {
        QList<ControlMeasure *> measures = cp.getMeasures( false );
        BOOST_FOREACH ( ControlMeasure *m, measures) {
          if ( m->IsRejected() ) {  
            m->SetIgnored( true ); 
            nRejected++;
          }
        }
      }

      // Check to see if we want to reset the apriori surface to the best
      // available measure.
      if ( true == setaprioribest) {
        cp.SetAdjustedSurfacePoint(cp.GetBestSurfacePoint());
      }

      //  Now save point if valid
      if (point.size() < minmeasures) {
        point.disable();
        nMinMeasures++;
        // Check if we are to save points that are less than the valid minimum
        // measures.
        if ( true == savemins ) {
          cnet.AddPoint( point.take() ); 
          oPoints++;
        }
      }
      else {
        cnet.AddPoint( point.take() ); 
        oPoints++;
      }
    }
  }


  // Write the resulting control network to the specfied ONET file
  if ( ui.WasEntered("ONET") ) {
    cnet.Write( ui.GetAsString("ONET") );
  }

  // Write out a report
  int pMerged = v_cloud.size() - oPoints;
  PvlGroup summary("Summary");
  summary += PvlKeyword("TotalInputPoints",  toString(v_cloud.size()));
  summary += PvlKeyword("TotalOutputPoints", toString(oPoints));
  summary += PvlKeyword("PointsEliminated",  toString(pMerged));
  summary += PvlKeyword("PointsEvaluated",   toString(nfound));
  summary += PvlKeyword("RejectedMeasures",  toString(nRejected));
  summary += PvlKeyword("MeasuresMerged",    toString(nMerged));
  summary += PvlKeyword("MinimumMeasures",   toString(nMinMeasures));
  Application::Log(summary);

  p.EndProcess();
}

