/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <opencv2/opencv.hpp>

#include "Constants.h"
#include "Distance.h"
#include "FastGeom.h"
#include "ImageTransform.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SurfacePoint.h"

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
  m_tolerance = toDouble(m_parameters.get("FastGeomTolerance", "3.0"));
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
ImageTransform *FastGeom::compute(MatchImage &query, MatchImage &train,
                                  QLogger logger) const {

  // Shortcut...
  QDebugStreamType &logit = logger.logger();

  logit << "\n++++ Running FastGeom ++++\n";
  logit << "*** QueryImage: " << query.source().name() << "\n";
  logit << "*** TrainImage: " << train.source().name() << "\n";

  // Set default map to identity matrix
  cv::Mat mapper = cv::Mat::eye(3,3,CV_64FC1);

  // Image description
  RectArea qSize(0.0f, 0.0f, query.source().samples(), query.source().lines() );
  RectArea tSize(0.0f, 0.0f, train.source().samples(), train.source().lines() );

  // SANITY CHECK
  // Ensure both images have valid cameras
  QStringList errors;
  if ( !query.source().hasGeometry() ) {
    errors.append("Query image (" + query.source().name() +
                  ") does not support geometry operations (no camera/projection)!");
  }

  if ( !train.source().hasGeometry() ) {
    errors.append("Train image (" + train.source().name() +
                  ") does not support geometry operations (no camera/projection)!");
  }

  if ( errors.size() > 0 ) {
    logit << "--> Failed: " << errors.join("\n") << "\n";
    throw IException(IException::User, "--> FastGeom failed <--\n" + errors.join("\n"),
                     _FILEINFO_);
  }

  // Setup common processing parameters
  double q_samps = query.source().samples();
  double q_lines = query.source().lines();

  double t_samps = train.source().samples();
  double t_lines = train.source().lines();

  // Now get FOV tolerances
  double fg_q_sample_tolerance = toDouble(m_parameters.get("FastGeomQuerySampleTolerance", "0.0"));
  double fg_q_line_tolerance = toDouble(m_parameters.get("FastGeomQueryLineTolerance", "0.0"));
  double fg_sample_tolerance = toDouble(m_parameters.get("FastGeomSampleTolerance", "0.0"));
  double fg_line_tolerance = toDouble(m_parameters.get("FastGeomLineTolerance", "0.0"));

  // Train line/sample map restrictions
  double q_min_samp = 0.5 - fg_q_sample_tolerance;
  double q_max_samp = q_samps + 0.4999 + fg_q_sample_tolerance;
  double q_min_line = 0.5 - fg_q_line_tolerance;
  double q_max_line = q_lines + 0.4999 + fg_q_line_tolerance;
  FGFov  q_fov(q_min_samp, q_min_line, (q_max_samp-q_min_samp+1.0), (q_max_line-q_min_line+1.0) );

  double t_min_samp = 0.5 - fg_sample_tolerance;
  double t_max_samp = t_samps + 0.4999 + fg_sample_tolerance;
  double t_min_line = 0.5 - fg_line_tolerance;
  double t_max_line = t_lines + 0.4999 + fg_line_tolerance;
  FGFov  t_fov(t_min_samp, t_min_line, (t_max_samp-t_min_samp+1.0), (t_max_line-t_min_line+1.0) );

  // Set up line/sample mapping correspondance between images
  std::vector<FGPoint>      q_points, t_points;
  std::vector<bool>         t_inFOV;
  std::vector<SurfacePoint> q_surface_points;

  // Only the good points to matrix solver
  std::vector<FGPoint>      q_infov_points, t_infov_points;

  // Let's track some numbers
  int n_total_points(0);
  int n_image_points(0);
  int n_mapped_points(0);
  int n_train_fov(0);

  // Select the newer radial or earlier grid method. You gotta choose the
  // Grid option correctly or you will get the radial algorithm
  QString fg_algorithm = m_parameters.get("FastGeomAlgorithm", "Radial").toLower();
  bool radial_method = ( "grid" != fg_algorithm );
  logit <<   "  FastGeomAlgorithm:       " << fg_algorithm << "\n";
  logit <<   "  FastGeomPoints:          " << m_fastpts << "\n";
  logit <<   "  FastGeomTolerance:       " << m_tolerance << "\n";
  logit <<   "  FastGeomSampleTolerance: " << fg_sample_tolerance << "\n";
  logit <<   "  FastGeomLineTolerance:   " << fg_line_tolerance << "\n\n";

  // Run the requested algorithm
  if ( true == radial_method) {
    logit << "--> Using Radial Algorithm train-to-query mapping <--\n";

    // Compute maximum radial track at the center of the image to
    // the corner and scaling parameters
    double fg_max_radius    = std::sqrt( (q_samps * q_samps) + ( q_lines * q_lines) );
    double fg_radius_factor = toDouble(m_parameters.get("FastGeomRadiusFactor", "0.035"));
    double fg_point_factor  = toDouble(m_parameters.get("FastGeomPointFactor", "0.10"));

    // No check for explict specs
    // Allow user explicit radius delta increments - minimum is the length of the
    // diagonal of 1 pixel ( 2 * sqrt(1^2 + 1^2))
    double fg_radius_inc = qMax( 2.8284, (fg_max_radius * fg_radius_factor) );
    if ( m_parameters.exists("FastGeomRadiusIncrement") ) {
      fg_radius_inc = toDouble(m_parameters.get("FastGeomRadiusIncrement"));
    }

    // Allow user explicit point/radius increments. A 0 increment would only
    // get a single point at all radii at the end of the reference vector.
    // Probably bad, but we will allow it as an extreme.
    double fg_point_inc = qMax( 0.0, (fg_radius_inc * fg_point_factor) );
    if ( m_parameters.exists("FastGeomPointIncrement") ) {
      fg_point_inc = toDouble(m_parameters.get("FastGeomPointIncrement"));
    }

    // Lets report what we got
    logit <<   "  FGMaximumRadius:         " << fg_max_radius << "\n";
    logit <<   "  FGRadiusFactor:          " << fg_radius_factor << "\n";
    logit <<   "  FGPointFactor:           " << fg_point_factor << "\n";
    logit <<   "  FGRadiusIncrement:       " << fg_radius_inc << "\n";
    logit <<   "  FGPointIncrement:        " << fg_point_inc << "\n";
    logit <<   "  TotalRadialTracks:       " << fg_max_radius / fg_radius_inc << "\n";

    logit.flush();

    // Rounding constant
    const double FG_Epsilon(0.0001);

    // Unit vector on circle points east
    double v_x0 = 1.0;
    double v_y0 = 0.0;

    // Center of query image
    double c_x = q_samps / 2.0;
    double c_y = q_lines / 2.0;

    // The Circle Loop - compute points along a circle at radial
    // distance (pixels) from center
    double points_per_radius = 0.0; // Gets the center point on 1st iteration
    double fg_radius = 0.0;
    while ( fg_radius <= fg_max_radius ) {

      // Compute the number points / radial
      double r_points = qMax(1.0, points_per_radius);
      double d_theta = ( 360.0 / r_points ) + FG_Epsilon;

      // Compute the points on the circle by d_theta increments
      double r_angle = 0.0;
      while ( r_angle < 360.0 ) {

        // Potential point
        n_total_points++;

        // Compute angle from reference
        double theta = r_angle * DEG2RAD;
        double v_cos = std::cos(theta);
        double v_sin = std::sin(theta);

        // Rotation matrix relative to center of image. This is the
        // reference vector, (v_x0, v_y0), rotated counter-clockwise,
        // scaled by the radial pixel distance from the center of
        // the image at (c_x, c_y).
        double q_x = ( ((v_x0 * v_cos) + (v_y0 * -v_sin)) * fg_radius ) + c_x;
        double q_y = ( ((v_x0 * v_sin) + (v_y0 *  v_cos)) * fg_radius ) + c_y;

        // Check to see if the point is in the image FOV
        FGPoint q_coord(q_x, q_y);
        if ( in_fov(q_coord, q_min_samp, q_max_samp, q_min_line, q_max_line) ) {
          n_image_points++;

          // Convert the point and if valid add to list
          double t_x, t_y, t_r;
          SurfacePoint q_surfpt = query.source().getLatLon(q_y, q_x);
          if ( train.source().getLineSamp(q_surfpt, t_y, t_x, t_r) ) {
            n_mapped_points++;

            // Query q_coord set above
            FGPoint t_coord(t_x, t_y);
            q_points.push_back(q_coord);
            q_surface_points.push_back(q_surfpt);
            t_points.push_back(t_coord);

            // Test for those falling in train FOV
            bool inFOV = in_fov(t_coord, t_min_samp, t_max_samp, t_min_line, t_max_line);
            if ( inFOV ) {
              n_train_fov++;

              // Save the good points
              q_infov_points.push_back(q_coord);
              t_infov_points.push_back(t_coord);
            }
            t_inFOV.push_back(inFOV);
          }
        }

        // Update angle of rotation to the next point
        r_angle += d_theta;
      }

      // Update radial distance from image center and points/circle
      fg_radius         += fg_radius_inc;
      points_per_radius += fg_point_inc;
    }
  }
  else { // true == grid_method

#if USE_OLD_ALGORITHM
    mapper = train.source().getGeometryMapping(query.source(),
                                               m_fastpts,
                                               m_tolerance);
#endif

    logit << "--> Using Grid Algorithm train-to-query mapping <--\n";

     // Compute increment
    int   fg_minpts = qMax(m_fastpts, 16);
    int increment = (int) (std::sqrt(std::max(24.0, (double) (fg_minpts - 1))) + 1.0);

    double fg_max_axis = qMax( qMax(q_max_samp, q_max_line), qMax(t_max_samp, t_max_line) );
    int v_max_iter     = int( fg_max_axis / 2.0 );

    int fg_grid_start_iter = toInt(  m_parameters.get("FastGeomGridStartIteration", "0") );
    int fg_grid_stop_iter  = toInt(  m_parameters.get("FastGeomGridStopIteration", toString(v_max_iter)) );
    int fg_grid_iter_step  = toInt(  m_parameters.get("FastGeomGridIterationStep", "1") );
    bool fg_save_all       = toBool( m_parameters.get("FastGeomGridSaveAllPoints", "false") );

    logit <<   "\n  FastGeomGridStartIteration: " << fg_grid_start_iter << "\n";
    logit <<   "  FastGeomGridStopIteration:  " << fg_grid_stop_iter << "\n";
    logit <<   "  FastGeomGridIterationStep:  " << fg_grid_iter_step << "\n";
    logit <<   "  FastGeomGridSaveAllPoints:  " << toString(fg_save_all) << "\n";
    logit <<   "  FGPointIncrement:           " << increment << "\n";


    bool done = false;
    int iteration = fg_grid_start_iter;
    int currinc(0);
    double v_q_nsamps = q_max_samp - q_min_samp + 1.0;
    double v_q_nlines = q_max_line - q_min_line + 1.0;

    int n_iterations(0);
    while ( (iteration < fg_grid_stop_iter) && ( !done) ) {

      currinc = increment + ( iteration * 2 );
      n_iterations++;

      double sSpacing = qMax( 1.0, v_q_nsamps/(currinc*1.0) );
      double lSpacing = qMax( 1.0, v_q_nlines/(currinc*1.0) );

      if ( qMax( sSpacing, lSpacing ) <= 1.0 ) done = true; // Last possible loop

      // To get all points computed...
      if ( !fg_save_all ) {
        q_points.clear();
        t_points.clear();
        t_inFOV.clear();
        q_surface_points.clear();
      }

      // Clear these for the matrix transform
      q_infov_points.clear();
      t_infov_points.clear();
      n_train_fov = 0;

      // Run the loop
      for (int l = 0 ; l < currinc ; l++) {
        for ( int s = 0 ; s < currinc ; s++ ) {

          // Total point count
          n_total_points++;

          double q_y = (lSpacing / 2.0 + lSpacing * l + 0.5) + q_min_line;
          double q_x = (sSpacing / 2.0 + sSpacing * s + 0.5) + q_min_samp;

          // Check to see if the point is in the image FOV
          FGPoint q_coord(q_x, q_y);
          if ( in_fov(q_coord, q_min_samp, q_max_samp, q_min_line, q_max_line) ) {
            n_image_points++;

            // Convert the point and if valid add to list
            double t_x, t_y, t_r;
            SurfacePoint q_surfpt = query.source().getLatLon(q_y, q_x);
            if ( train.source().getLineSamp(q_surfpt, t_y, t_x, t_r) ) {
              n_mapped_points++;

              // Query q_coord set above
              FGPoint t_coord(t_x, t_y);
              q_points.push_back(q_coord);
              q_surface_points.push_back(q_surfpt);
              t_points.push_back(t_coord);

              // Test for those falling in train FOV
              bool inFOV = in_fov(t_coord, t_min_samp, t_max_samp, t_min_line, t_max_line);
              if ( inFOV ) {
                n_train_fov++;

                // Save the good points
                q_infov_points.push_back(q_coord);
                t_infov_points.push_back(t_coord);
              }
              t_inFOV.push_back(inFOV);
            }
          }
        }
      }

      // If on this iteration we have enough points we are done
      if ( n_train_fov >= m_fastpts ) done = true;

      // Next iteration + step
      iteration += fg_grid_iter_step;
    }

    logit <<   "  FGTotalGridIterations:      " << n_iterations << "\n";
  }

  //// Latitude/Longitude mapping complete! Check status

  // Log results
  logit <<   "\n\n==> Mapping complete <==\n";
  logit <<   "  TotalPoints:       " << n_total_points << "\n";
  logit <<   "  ImagePoints:       " << n_image_points << "\n";
  logit <<   "  MappedPoints:      " << n_mapped_points << "\n";
  logit <<   "  InTrainMapFOV:     " << n_train_fov << "\n";
  logit.flush();

  // Lets dump the data if requested
  if ( toBool(m_parameters.get("FastGeomDumpMapping","False")) ) {
    FileName q_file( query.name() );
    FileName t_file( train.name() );

    QString csvout = q_file.baseName() + "_" + t_file.baseName() + "." +
                     fg_algorithm + ".fastgeom.csv";
    logit <<   "  PointDumpFile:     " << csvout << "\n";
    QDebugStream csvstrm = QDebugLogger::create( csvout,
                                                (QIODevice::WriteOnly |
                                                 QIODevice::Truncate) );
    QDebugStreamType &csv(csvstrm->dbugout());


    // Write the header
    csv << "QuerySample,QueryLine,TrainSample,TrainLine,"
        << "Latitude,Longitude,Radius,X,Y,Z,InTrainFOV\n";

    // Write the data

    for ( unsigned int i = 0 ; i < q_points.size() ; i++) {
      const SurfacePoint &srfpt( q_surface_points[i] );

      csv << q_points[i].x << ", " << q_points[i].y << ", "
          << t_points[i].x << ", " << t_points[i].y << ", "
          << srfpt.GetLatitude().degrees() << ", "
          << srfpt.GetLongitude().degrees() << ", "
          << srfpt.GetLocalRadius().meters() << ", "
          << srfpt.GetX().meters() << ", "
          << srfpt.GetY().meters() << ", "
          << srfpt.GetZ().meters() << ", "
          << (t_inFOV[i] ? "True" : "False") << "\n";
    }
  }

  // Compute homography if enough point ater in common FOVs of both images,
  // otherwise we report failure
  if ( n_train_fov < m_fastpts ) {
    QString mess = "Failed to get FOV geometry mapping for " + train.name() +
                    " to " + query.name() + " needing " + QString::number(m_fastpts) +
                    " but got " + QString::number(n_train_fov) +" in train FOV.";
    logit << ">>> ERROR - " << mess << "\n";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }

  // Compute homography tranformation. Note the order of the point arrays.
  // This method computes the train to query transform. The inverse will
  // provide sample,line translations from query-to-train sample,line
  // coordinates
  std::vector<uchar> t_inliers;
  mapper = getTransformMatrix(t_infov_points, q_infov_points, t_inliers, m_tolerance, logger);

  // Report the transformation matrix
  logit <<   "\n  MatrixTransform:   \n";
  for (int i = 0 ; i < mapper.rows ; i++) {
    logit << "    ";
    QString comma("");
    for (int j = 0 ; j < mapper.cols ; j++) {
      logit << comma << mapper.at<double>(i,j);
      comma = ",";
    }
    logit << "\n";
  }
  logit << "\n";

  // The above matrix is for simply computing the direct fastgeom of the
  // training image into the image space of the query image, just like
  // cam2cam does.
  //
  // Now consider cropping to only the minimum common coverage or
  // preserving all the train image in the transformation (this option can
  // be really big and is not recommended for some situations!).

  // Set up the transform pointer for applying the desired affect. Do
  // preserve option first.
  QScopedPointer<GenericTransform> fastg(0);
  if ( "map" == m_geomtype ) {
    // Compute preserved image output size and translation matrix
    cv::Mat tMat;
    RectArea tSizeFull = ImageTransform::transformedSize(mapper,
                                                         tSize.size(),
                                                         tMat);
    // std::cout << "FullTrain: " << tSizeFull << "\n";

    // If it is within the area tolerance use it, otherwise just use the
    // same size as the output image below.
    if ( tSizeFull.area()  < (m_maxarea * qSize.area()) ) {
      // std::cout << "Use direct full mapping of Train-to-Query...\n";
      fastg.reset(new GenericTransform("FastGeomMap", mapper,
                                       tSizeFull));
    }
  }
  else if ( "crop" == m_geomtype ) {

    // Crop train image to only common area in query image using inverse
    // std::cout << "Compute train to query BB...\n";
    RectArea qbbox = ImageTransform::boundingBox( GenericTransform::computeInverse(mapper),
                                                  qSize,
                                                  tSize.size());
    // std::cout << "TrainBB: " << qbbox << "\n";


    // std::cout << "Compute query to train BB...\n";
    RectArea tbbox = ImageTransform::boundingBox( mapper, qbbox,
                                                   qSize.size());

    // std::cout << "MapBB: " << tbbox << "\n";
    fastg.reset(new GenericTransform("FastGeomCrop", mapper, tbbox));
  }

 // If a mapper has not been allocated, allocate mapping to query image size
 // (as cam2cam does).
  if ( fastg.isNull() ) {
    // std::cout << "FastGeom cam2map map created...\n";
    fastg.reset(new GenericTransform("FastGeomCamera", mapper, qSize.size()));
  }

  return ( fastg.take() );
}

void FastGeom::apply(MatchImage &query, MatchImage &train, QLogger logger) const {
  // Add the fastgeom mapping transform
  train.addTransform( compute(query, train, logger) );
  return;
}


cv::Mat FastGeom::getTransformMatrix(const std::vector<FGPoint> &querypts,
                                     const std::vector<FGPoint> &trainpts,
                                     std::vector<uchar> &inliers,
                                     const double tolerance,
                                     QLogger logger) {

  QDebugStreamType &logit = logger.logger();

  logit << "\n--> Running Homography Image Transform <---\n";

  logit << "  IntialPoints:       " << querypts.size() << "\n";
  logit << "  Tolerance:          " << tolerance << "\n";

  // Find homography using Least-Median robust method algorithm
  inliers.assign(querypts.size(), 0);
  cv::Mat mapper = cv::findHomography(querypts, trainpts, cv::LMEDS, tolerance, inliers);

  // Using the Least median method requires > 50% inliers.  Check that here.
  int nInliers(0);
  for (unsigned int i = 0 ; i < inliers.size() ; i++) {
    if ( 0 != inliers[i] )  {
      nInliers++;
    }
  }

  logit << "  TotalLmedsInliers:  " << nInliers << "\n";

  // If we have < 50% inliners, compute the RANSAC homography
  double inlierPercent = ((double) nInliers / (double) querypts.size()) * 100.0;
  logit << "  PercentPassing: " << inlierPercent << "\n";

  if ( 50.0 > inlierPercent ) {
    logit << "  LMEDS failed w/less than 50% inliers - computing RANSAC homography!\n";
    mapper = cv::findHomography(querypts, trainpts, cv::RANSAC, tolerance, inliers);

    int r_nInliers(0);
    for (unsigned int i = 0 ; i < inliers.size() ; i++) {
      if ( 0 != inliers[i] )  r_nInliers++;
    }
    logit << "  TotalRansacInliers: " << r_nInliers << "\n";

  }

  return (mapper);
}


/** Get parameters currently being used   */
PvlFlatMap FastGeom::getParameters() const {
  return (m_parameters);
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
