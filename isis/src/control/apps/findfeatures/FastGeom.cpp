/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <opencv2/opencv.hpp>

#include "FastGeom.h"
#include "ImageTransform.h"

namespace Isis {

/* Constructor */
FastGeom::FastGeom() : m_fastpts(25), m_tolerance(1.0), m_geomtype("camera"),
                       m_maxarea(3.0), m_parameters() {
  validate(m_geomtype);
}

/* Construct with parameters */
FastGeom::FastGeom(const PvlFlatMap &parameters) : m_fastpts(25),
                                                   m_tolerance(1.0),
                                                   m_geomtype("camera"),
                                                   m_maxarea(3.0),
                                                   m_parameters(parameters) {
  m_fastpts   = toInt(m_parameters.get("FastGeomPoints", "25"));
  m_tolerance = toDouble(m_parameters.get("FastGeomTolerance", "1.0"));
  m_geomtype  = m_parameters.get("GeomType", "camera").toLower();
  m_maxarea   = toDouble(m_parameters.get("GeomScaleLimit", "3.0"));
  validate(m_geomtype);
}

/* Construct with individual parameters to compute the fast geom transform */
FastGeom::FastGeom(const int maxpts, const double tolerance, const bool crop,
         const bool preserve, const double &maxarea) :
         m_fastpts(maxpts), m_tolerance(tolerance), m_geomtype("camera"),
         m_maxarea(maxarea), m_parameters() {
  validate(m_geomtype);
}

/* Destructor */
FastGeom::~FastGeom() { }

/**
 * @brief Compute train->query fast geom transformation
 *
 *  This method computes a fast geometric transform that "projects" the train
 *  image into the query image space. This method assumes both images have
 *  camera models or map projections that can compute longitude/latitude
 *  coordinates from sample/line coordinates and back.
 *
 *  The query line/samples coordinates are converted to latitude/longitude
 *  coordinates. The latitude/longitude coordinates are then used in the train
 *  image to compute its corresponding line/sample. A minimum number of points
 *  (m_fastpts) are computed to create a perspective transform that maps image
 *  coordinates of the two images.
 *
 * @param query            Query image
 * @param train            Train image
 * @return ImageTransform* Pointer to FastGeom transform
 */
ImageTransform *FastGeom::compute(MatchImage &query, MatchImage &train)  {
  // std::cout << "\nQuery: " << query.source().name() << "\n";
  // std::cout << "Train: " << train.source().name() << "\n";

  // Image description
  RectArea qSize(0.0f, 0.0f, query.source().samples(), query.source().lines() );
  RectArea tSize(0.0f, 0.0f, train.source().samples(), train.source().lines() );

  // std::cout << "Train-to-query mapping...\n";
  cv::Mat t_to_q = train.source().getGeometryMapping(query.source(),
                                                     m_fastpts,
                                                     m_tolerance);

  // The above matrix is for simply computing the direct fastgeom of the
  // training image into the image space of the query image, just like
  // cam2cam does.
  //
  // Now consider cropping to only the mininmum common coverage or
  // preserving all the train image in the transformation (this option can
  // be really big and is not recommended for some situations!).

  // Set up the transform pointer for applying the desired affect. Do
  // preserve option first.
  QScopedPointer<GenericTiledTransform> fastg(0);
  if ( "map" == m_geomtype ) {
    // Compute preserved image output size and translation matrix
    cv::Mat tMat;
    RectArea tSizeFull = ImageTransform::transformedSize(t_to_q,
                                                         tSize.size(),
                                                         tMat);
    // std::cout << "FullTrain: " << tSizeFull << "\n";

    // If it is within the area tolerance use it, otherwise just use the
    // same size as the output image below.
    if ( tSizeFull.area()  < (m_maxarea * qSize.area()) ) {
      // std::cout << "Use direct full mapping of Train-to-Query...\n";
      fastg.reset(new GenericTiledTransform("FastGeomMap", t_to_q,
                                       tSizeFull, 30000));
    }
  }
  else if ( "crop" == m_geomtype ) {

    // Crop train image to only common area in query image using inverse
    // std::cout << "Compute train to query BB...\n";
    RectArea qbbox = ImageTransform::boundingBox( t_to_q.inv(), qSize,
                                                   tSize.size());
    // std::cout << "TrainBB: " << qbbox << "\n";


    // std::cout << "Compute query to train BB...\n";
    RectArea tbbox = ImageTransform::boundingBox( t_to_q, qbbox,
                                                   qSize.size());

    // std::cout << "MapBB: " << tbbox << "\n";
    fastg.reset(new GenericTiledTransform("FastGeomCrop", t_to_q, tbbox, 30000));
  }

 // If a mapper has not been allocated, allocate mapping to query image size
 // (as cam2cam does).
  if ( fastg.isNull() ) {
    // std::cout << "FastGeom cam2map map created...\n";
    fastg.reset(new GenericTiledTransform("FastGeomCamera", t_to_q, qSize.size(), 30000));
  }

  return ( fastg.take() );
}

void FastGeom::apply(MatchImage &query, MatchImage &train) {
  // Add the fastggeom mapping transform
  train.addTransform( compute(query, train) );
  return;
}

/**
 * @brief Checks for valid fast geom transform options
 *
 * This method checks that geomtype is "camera", "crop" or "map". If some other
 * value is found, an exception is thrown.
 *
 * @param geomtype String specification of geom type - must be camera, crop or
 *                 map
 */
void FastGeom::validate( const QString &geomtype ) const {
  QStringList options;
  options << "camera" << "crop" << "map";
  if ( !options.contains(geomtype, Qt::CaseInsensitive)  ) {
    QString mess = "FastGeom - invalid GEOMTYPE (" + geomtype + ")!"
                   " Must be CAMERA, CROP or MAP.";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }
}

}  // namespace Isis
