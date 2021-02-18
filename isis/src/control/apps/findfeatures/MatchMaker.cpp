/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include <QList>
#include <QStringList>
#include <QTime>

#include <boost/foreach.hpp>

#include "Angle.h"
#include "Application.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlPoint.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MatcherSolution.h"
#include "MatchMaker.h"
#include "QDebugLogger.h"
#include "RobustMatcher.h"
#include "Statistics.h"

namespace Isis {

MatchMaker::MatchMaker() : QLogger(), m_name("MatchMaker"),
                           m_parameters(), m_query(),  m_trainers(),
                           m_geomFlag(None) { }

MatchMaker::MatchMaker(const QString &name,const PvlFlatMap &parameters,
                       const QLogger &logger) : QLogger(logger),
                       m_name(name), m_parameters(parameters),
                       m_query(), m_trainers(), m_geomFlag(None)  { }


QString MatchMaker::name() const {
  return ( m_name );
}

/** Returns the number of trainers to match */
int MatchMaker::size() const {
  return ( m_trainers.size() );
}

void MatchMaker::setQueryImage(const MatchImage &query) {
  m_query = query;
  return;
}

void MatchMaker::addTrainImage(const MatchImage &train) {
  m_trainers.push_back(train);
  return;
}


const MatchImage &MatchMaker::query() const {
  return ( m_query );
}

MatchImage &MatchMaker::query()  {
  return ( m_query );
}

const MatchImage &MatchMaker::train(const int &index) const {
  Q_ASSERT ( (index >= 0) && (index < size()) );
  return ( m_trainers[index] );
}

MatchImage &MatchMaker::train(const int &index) {
  Q_ASSERT ( (index >= 0) && (index < size()) );
  return ( m_trainers[index] );
}

void MatchMaker::setGeometrySourceFlag(const MatchMaker::GeometrySourceFlag &source) {
  if ( (Train == source) && ( size() > 1) )  {
    QString mess = "Cannot choose Train image as geometry source when matching "
                   "more than one train image to match";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }
  m_geomFlag = source;
}

MatchMaker::GeometrySourceFlag MatchMaker::getGeometrySourceFlag() const {
  return ( m_geomFlag );
}

MatchImage MatchMaker::getGeometrySource() const {
  if ( ( Train == m_geomFlag ) && ( size() > 1) )  {
    QString mess = "Cannot choose Train image as geometry source when matching "
                   "more than one train image to match";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }

  if ( Query == m_geomFlag )  return ( query() );
  if ( Train == m_geomFlag )  return ( train() );
  return ( MatchImage() );  // Got none
}

MatcherSolution *MatchMaker::match(const SharedRobustMatcher &matcher) {

  // Pass along logging status
  matcher->setDebugLogger( stream(), isDebug() );
  MatchImage query_copy = m_query.clone();
  QList<MatchImage> trainers_copy;
  for (int i = 0; i < m_trainers.size();i++) {
    trainers_copy.append(m_trainers[i].clone());
  }


  MatcherSolution *m(0);
  if ( m_trainers.size() == 1 ) {
    // Run a pair only matcher
    m = new MatcherSolution(matcher,
                            matcher->match(query_copy, trainers_copy[0]),
                            *this /* MatchMaker and/or QLogger */ );
  }
  else {
    // Run the multi-matcher
    m = new MatcherSolution(matcher,
                            matcher->match(query_copy, trainers_copy),
                            *this /* MatchMaker and/or QLogger */ );
  }
  return ( m );
}

MatcherSolutionList MatchMaker::match(const RobustMatcherList &matchers) {
  // Thread here!!!!
  MatcherSolutionList solutions;
  for (int i = 0 ; i < matchers.size() ; i++) {
    solutions.push_back(SharedMatcherSolution( match(matchers[i]) ));
  }
  return ( solutions );
}


 PvlGroup MatchMaker::network(ControlNet &cnet, const MatcherSolution &solution,
                              ID &pointMaker) const {


   PvlGroup cnetinfo("ControlNetInfo");
  if ( isDebug() ) {
    logger() << "Entering MatchMaker::network(cnet, solution, pointmaker)...\n";
    logger().flush();
  }

  cnetinfo += PvlKeyword("SolutionSize", toString(solution.size()) );
  if ( solution.size() <= 0 ) {
    cnetinfo += PvlKeyword("Error", "No matches, no network!!");
    return (cnetinfo);
  }

  MatchImage v_query = (*solution.begin()).query();
  QVector<ControlPoint *> points(v_query.size(), 0);
  int nMeasures(0);
  int nImages(0);
  MatcherSolution::MatchPairConstIterator v_pair = solution.begin();
  for ( ; v_pair != solution.end() ; ++v_pair) {
    if ( v_pair->size() > 0 ) { nImages++; }
    for (int m = 0 ; m < v_pair->size() ; m++) {
      int index = v_pair->match(m).queryIdx;
      nMeasures += addMeasure(&points[index], *v_pair,
                              (*v_pair).match(m), solution);
    }
  }

  if ( isDebug() ) {
    logger().flush();
  }

  // Create control network
  int nPoints = 0;
  int nBad = 0;
  Statistics pointStats;
  for (int i = 0 ; i < points.size() ; i++) {
    if ( points[i] != 0 ) {
      if ( points[i]->GetNumValidMeasures() > 1 ) {
        pointStats.AddData((double)points[i]->GetNumValidMeasures());
        points[i]->SetId(pointMaker.Next());
        cnet.AddPoint(points[i]);
        points[i] = 0;
        nPoints++;
      }
      else {
        // Be sure to delete single measure points!!  This is as good as any
        // other place to do it.
        delete points[i];
        points[i] = 0;
        nBad++;
      }
    }
  }

  cnetinfo += PvlKeyword("ImagesMatched", toString(nImages) );
  cnetinfo += PvlKeyword("ControlPoints", toString(nPoints) );
  cnetinfo += PvlKeyword("ControlMeasures", toString(nMeasures) );
  cnetinfo += PvlKeyword("SingleMeasurePoints", toString(nBad) );
  if ( isDebug() ) {
    logger() << "  Images Matched:                 " << nImages << "\n";
    logger() << "  ControlPoints created:          " << nPoints << "\n";
    logger() << "  ControlMeasures created:        " << nMeasures << "\n";
    logger() << "  Excluded single measure points: " << nBad << "\n";
    logger().flush();
  }

  if ( pointStats.ValidPixels() > 0 ) {
    cnetinfo += PvlKeyword("MinimumMeasures", toString(pointStats.Minimum()) );
    cnetinfo += PvlKeyword("MaximumMeasures", toString(pointStats.Maximum()) );
    cnetinfo += PvlKeyword("StdDevMeasures", toString(pointStats.StandardDeviation()) );
    if ( isDebug() ) {
      logger() << "  MinimumMeasures:       " << pointStats.Minimum() << "\n";
      logger() << "  MaximumMeasures:       " << pointStats.Maximum() << "\n";
      logger() << "  StdDevMeasures:        " << pointStats.StandardDeviation() << "\n";
      logger().flush();
    }
  }

  return ( cnetinfo );
}


int MatchMaker::addMeasure(ControlPoint **cpt, const MatchPair &mpair,
                           const cv::DMatch &point,
                           const MatcherSolution &solution) const {

  int nMade(0);
  Q_ASSERT ( ( point.queryIdx >= 0) &&
             ( point.queryIdx < mpair.query().size() ) );
  Q_ASSERT ( ( point.trainIdx >= 0) &&
             ( point.trainIdx < mpair.train().size()) );

  // If no point created at this query keypoint, create a ControlPoint and add
  // the query keypoint as the reference
  if ( !(*cpt) ) {
    // std::cout << "\nMaking new ControlPoint...\n";
    *cpt = new ControlPoint();
    (*cpt)->SetDateTime(Application::DateTime());
    ControlMeasure *reference = makeMeasure(mpair.query(), point.queryIdx,
                                            solution.matcher()->name());
    reference->SetType(ControlMeasure::Candidate);
    //std::cout << "  Image Coordinate: " << reference->GetSample() << ", "
    //                                    << reference->GetLine() << "\n";

    (*cpt)->Add(reference);
    (*cpt)->SetRefMeasure(reference);

    // Set lat/lon if requested for Query image
    if ( Query == m_geomFlag ) {
      // What to do when it fails??
      (void) setAprioriLatLon(*(*cpt), *reference, mpair.query());
    }
    nMade++;
  }

  // Add a measure to the existing control point std::cout << "Adding new
  // ControlMeasure...\n";
  QScopedPointer<ControlMeasure> trainpt(makeMeasure(mpair.train(),
                                                     point.trainIdx,
                                                     solution.matcher()->name() ) );

  trainpt->SetType(ControlMeasure::RegisteredSubPixel);

  const cv::KeyPoint &query = mpair.query().keypoint(point.queryIdx);
  const cv::KeyPoint &train = mpair.train().keypoint(point.trainIdx);

  // Compute the estimated point using the homography and the translation
  // chain from matched image to source image coordintes
  cv::Point2f est = mpair.train().imageToSource(mpair.forward(query.pt));

  cv::Point2f tpoint( trainpt->GetSample(), trainpt->GetLine() );
  cv::Point2f diff = tpoint - est;

  // Compute the estimated residual. Wildly disparate matches will not be added
  // to the point so single points should be checked for and not added to the
  // network. Use 2 * homography tolerance as a limit unless the user has added
  // a ResidualTolerance parameter to the matcher.
  double residual = std::sqrt( diff.x*diff.x + diff.y*diff.y);
  double residTol = toDouble(solution.matcher()->parameters().get("HmgTolerance")) * 2.0;
  residTol = toDouble(solution.matcher()->parameters().get("ResidualTolerance",
                                                          toString(residTol)));

  // Don't add the measure to the point if it exceeds tolerance
  if ( residual <= residTol ) {
    trainpt->SetResidual(diff.x, diff.y);
    trainpt->SetLogData(ControlMeasureLogData(ControlMeasureLogData::GoodnessOfFit,
                                              goodnessOfFit(query, train)));

    // Set lat/lon if requested for train image
    if ( Train == m_geomFlag ) {
      // What to do when it fails??
      (void) setAprioriLatLon(*(*cpt), *trainpt, mpair.train() );
    }

    nMade++;
    (*cpt)->Add( trainpt.take() );
  }
  return (nMade);
}

ControlMeasure *MatchMaker::makeMeasure(const MatchImage &image,
                                        const int &keyindex,
                                        const QString &name) const {
  QScopedPointer<ControlMeasure> v_measure(new ControlMeasure());
  v_measure->SetChooserName(name);
  v_measure->SetCubeSerialNumber(image.id());

  // imgpt is unused variable, so it has been commented out
  /*cv::Point2f imgpt = image.keypoint(keyindex).pt; */
  // std::cout << "ImageCoordinate: " << imgpt << "\n";

  cv::Point2f query = image.imageToSource(image.keypoint(keyindex).pt);
  // std::cout << "SourceCoordinate: " << query << "\n";
  v_measure->SetCoordinate(query.x, query.y, ControlMeasure::Candidate);
  return ( v_measure.take() );
}

bool MatchMaker::setAprioriLatLon(ControlPoint &point,
                                  const ControlMeasure &measure,
                                  const MatchImage &image) const {
  // Check if the source has geometry
  if ( !image.source().hasGeometry() ) { return (false); }

  // Compute lat/lon
  double samp = measure.GetSample();
  double line = measure.GetLine();
  SurfacePoint latlon = image.source().getLatLon(line,samp);

  // std::cout << "  Lat/Lon Coordinate: " << latlon.GetLatitude().degrees() << ", "
  //           << latlon.GetLongitude().degrees() << "\n";
  point.SetAprioriSurfacePoint(latlon);
  return ( latlon.Valid() );
}


double MatchMaker::getParameter(const QString &name,
                                const PvlFlatMap &parameters,
                                const double &defaultParm) const {
  double parm(defaultParm);
  if ( parameters.count(name) > 0 ) {
    QString value = parameters.get(name);
    if ( !value.isEmpty() ) {
      parm = toDouble(value);
    }
  }

  return ( parm );
}

double MatchMaker::goodnessOfFit(const cv::KeyPoint &query,
                                 const cv::KeyPoint &train) const {
 return ( (query.response + train.response) / 2.0 );
}



}// namespace Isis
