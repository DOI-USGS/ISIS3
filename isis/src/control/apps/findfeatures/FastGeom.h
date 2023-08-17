#ifndef FastGeom_h
#define FastGeom_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QList>
#include <QString>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>

#include "GenericTransform.h"
#include "MatchImage.h"
#include "PvlFlatMap.h"
#include "QDebugLogger.h"

namespace Isis {

class ImageTransform;

/**
 * @brief Compute fast geom transform for pair of images
 *
 * This class computes a fast geometric transformation of a query and train
 * image pair using available geometry.
 *
 * It is assumed the two images have overlapping regions and in these regions
 * there are geometries that map from one image to the other.
 *
 * There are three types of fast geometric transforms that are computed in this
 * class: camera, crop and map.
 *
 * The camera fast geom works in the same way that the ISIS application cam2map
 * works. It transforms the train image directly into camera space of the query
 * image. The crop fast transform option computes the common area of overlap of
 * the train image and minimumizes the outputs size of the transformed image.
 * The map fast transform behaves just like cam2map where the output image is
 * fully retained and projected into the camera space. This option has the
 * highest degree of problems that can occur particularly if the image scales
 * differ significantly.
 *
 * The resulting fast geom transform is added to the train transform list. This
 * approach is well suited for the template MatchMaker::foreachpPair method.
 *
 * @author 2015-10-01 Kris Becker
 * @internal
 *   @history 2015-10-01 Kris Becker - Original Version
 *   @history 2016-04-06 Kris Becker Created .cpp file and completed documentation
 *   @history 2021-10-29 Kris Becker Added const qualifier to all compute() and
 *                                     apply() methods
 *   @history 2022-02-02 Kris Becker Added Grid implementation resulting in
 *                                     consolidation/abstraction of Radial
 *                                     algorithm code
 *   @history 2022-02-07 Kris Becker Modifications in response to USGS/Astro
 *                                     code review in PR #4772
 */

class FastGeom {
  public:
    typedef GenericTransform::RectArea   RectArea;
    typedef cv::Point2d                  FGPoint;
    typedef cv::Rect2d                   FGFov;

    FastGeom();
    FastGeom(const PvlFlatMap &parameters);
    FastGeom(const int maxpts, const double tolerance, const bool crop = false,
             const bool preserve = false, const double &maxarea = 3.0);
    virtual ~FastGeom();

    // ImageTransform *compute(MatchImage &query, MatchImage &train) const;
    ImageTransform *compute(MatchImage &query, MatchImage &train,
                            QLogger logger = QLogger() ) const;

    int radial_algorithm(MatchImage &query, MatchImage &train,
                        const FGFov &q_fov, const FGFov &t_fov,
                        const PvlFlatMap &parameters,
                        std::vector<FGPoint> &q_infov_points,
                        std::vector<FGPoint> &t_infov_points,
                        QLogger logger = QLogger()) const;

    int  grid_algorithm(MatchImage &query, MatchImage &train,
                        const FGFov &q_fov, const FGFov &t_fov,
                        const PvlFlatMap &parameters,
                        std::vector<FGPoint> &q_infov_points,
                        std::vector<FGPoint> &t_infov_points,
                        QLogger logger = QLogger()) const;

    void apply(MatchImage &query, MatchImage &train, QLogger logger = QLogger() ) const;

    static cv::Mat getTransformMatrix(const std::vector<FGPoint> &querypts,
                                      const std::vector<FGPoint> &trainpts,
                                      std::vector<uchar>         &inliers,
                                      const double tolerance,
                                      QLogger logger = QLogger() );

    PvlFlatMap getParameters() const;

    void dump_point_mapping(MatchImage &query, MatchImage &train,
                          const QString &method, const PvlFlatMap &parameters,
                          const std::vector<FGPoint> &q_points,
                          const std::vector<FGPoint> &t_points,
                          const std::vector<SurfacePoint> &q_surface_points,
                          const std::vector<bool> &t_inFOV,
                          QLogger logger = QLogger()) const;

  private:
    int        m_fastpts;    //!< Number of points to use for geom
    double     m_tolerance;  //!< Tolerance of geom transformation outliers
    QString    m_geomtype;   /** Geometry type to use can be "camera", "crop"
                                 or "map" */
    double     m_maxarea;    //!< Maximum scale change to use "map" option
    PvlFlatMap m_parameters; //!< Parameters of transform

    void validate( const QString &geomtype ) const;

};

}  // namespace Isis
#endif
