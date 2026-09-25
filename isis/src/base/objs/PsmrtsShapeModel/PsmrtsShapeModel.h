#ifndef PsmrtsShapeModel_h
#define PsmrtsShapeModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ShapeModel.h"

#include <vector>

#include <QString>

#include "Latitude.h"
#include "Longitude.h"
#include "Pvl.h"
#include "PvlFlatMap.h"
#include "SurfacePoint.h"

#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>
#include "IsisPsmrtsUtilities.hpp"


namespace Isis {
  class Target;

  /**
   * Implementation of the PSMRTS shape model class
   *
   * @author 2026-03-04 Kris J. Becker
   *
   * @internal
   *   @history 2026-03-04 - Kris J. Becker - Original Version
 
   */
  class PsmrtsShapeModel : public ShapeModel {
    public:
    /** Set default surface point tolerance to millimeter precision */
      static inline const double DefaultDistanceTolerance = 1.0e-6;
      static inline const double LastTraceTolerance = 1.0e-9;

      // Constructors
      PsmrtsShapeModel();
      PsmrtsShapeModel(Target *target, Pvl &pvl, 
                       const PvlFlatMap &parameters = PvlFlatMap() );
      PsmrtsShapeModel(const psmrts::PsmrtsTracerSystem &tracer_s, 
                       const PvlFlatMap &parameters = PvlFlatMap() );                       
      PsmrtsShapeModel(const psmrts::PsmrtsPriorityTracer &tracer_t, 
                       const PvlFlatMap &parameters = PvlFlatMap() );

      // Destructor
      ~PsmrtsShapeModel();

      // Psmrts shape model creator methods */
      static PsmrtsShapeModel *create(Target *target, Pvl &pvl, 
                                      const bool throw_errors = true);

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);
      virtual bool intersectSurface(const Latitude &lat, const Longitude &lon,
                                    const std::vector<double> &observerPos,
                                    const bool &checkOcclusion = true);
      virtual bool intersectSurface(const SurfacePoint &surfpt, 
                                    const std::vector<double> &observerPos,
                                    const bool &checkOcclusion = true);


      // Calculate the surface normal of the current intersection point
      void calculateDefaultlNormal();
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints); // use default normal
      void setLocalNormalFromIntercept();


      // Determine if the internal intercept is occluded from the observer/lookdir
      virtual bool isVisibleFrom(const std::vector<double> observerPos,
                                 const std::vector<double> lookDirection);

      virtual void clearSurfacePoint();

      // CSMCamera uses all of these so be sure they are implemented!!
      // Calculate the emission angle of the current intersection point
      virtual double emissionAngle(const std::vector<double> & sB);

      // Calculate the incidence angle of the current intersection point
      virtual double incidenceAngle(const std::vector<double> &uB);

      Distance localRadius(const Latitude &lat, const Longitude &lon);
    
      bool isDEM() const;

      int plate_index() const;

      /** Return the composite tracer system reference */
      inline const psmrts::PsmrtsPriorityTracer &tracer() const {
        return ( m_tracer );
      }

      /** Return the shape model ray trace request object */
      inline const psmrts::PRQRayTrace &get_shape_trace() const {
        return ( m_shape_ray_t );
      }

      /** Return the occlusion tolerance (km) */
      inline double get_tolerance() const {
        return ( m_tolerance );
      }

      /** Set/reset the occlusion tolerance (km) */
      inline void set_tolerance( const double tolerance = DefaultDistanceTolerance ) {
        m_tolerance = tolerance;
        return;
      }

      /**
       * @brief Get the Surface Lat Lon Radius object with PRQ trace object
       * 
       * 
       * This method computes the surface point on the shape DEM given a
       * latitude/longitude mapping coordinate. This is useful for
       * orthorectified mapping operations to determined precision surface
       * intercept points and determining that radius at that coordinate. It
       * does not require an observer position as it is computed as the vector
       * from the body origin through the latitude/longitude coordinate extended
       * beyond the surface 1.5 times the maximum radius of the shape model. The
       * look direction is the negative of the this observer position. The
       * resulting trace of this configuration is returned as the surface
       * intercept point of this coordinate.
       * 
       * Note this implementation exploits a common occurance in mapping ISIS
       * operations where the radius computed for the surface point using the 
       * LocalRadius( lat, lon ) method implemented in this class. Efforts are
       * made to reuse that trace when calling interceptSurface( SurfacePoint,..).
       * 
       * @tparam T     Tracer, which could be a priority trace or a PsmrtsTraxcer
       * @param lat    Latitude on the surface
       * @param lon    Longitude on the surface 
       * @param ray    A ray PRQ to return the trace results
       * @return true  Indicates the trace was successful
       * @return false The trace failed to intersect the surface
       */
      template <typename T>
        bool getSurfaceLatLonRadius( const Latitude &lat, 
                                     const Longitude &lon,
                                     const T &tracer,
                                     psmrts::PRQRayTrace &ray ) 
                                     const {

          // Compute the observer position ensuring it is above the shape                                          
          double max_r = tracer.maximum_radius();
          Eigen::Vector3d llr( lon.degrees(), lat.degrees(), max_r * 1.5 );
          Eigen::Vector3d observer = psmrts::lonlatrad_to_xyz_d( llr );
          Eigen::Vector3d lookdir = -observer;  // Just negate the observer

          // Check to see if the last trace satisfies this observation. If not,
          // run the requested trace to save it for subsequent traces.
          ray.reset();
          bool status = m_latlon_ray_t.hasHit();
          if ( !( observer.isApprox( m_latlon_ray_t.trace().observer(), LastTraceTolerance ) &&
                  lookdir.isApprox( m_latlon_ray_t.trace().lookdir(),   LastTraceTolerance ) ) ) {

            status = tracer.process( m_latlon_ray_t.set_trace( observer, lookdir ) );
          }

          // Carefully copy the result to the return parameter
          ray.trace() = std::move( m_latlon_ray_t.trace() );
          if ( m_latlon_ray_t.error_count() > 0 ) ray.add_error( m_latlon_ray_t.errors_to_string() );

          // Return status of last trace activity
          return ( status );
        }

      /**
       * @brief Compute the surface intersept point given a lat/lon coordinate
       * 
       * @param lat   Latitude of the mapping coordinate
       * @param lon   Longitude of the mapping coordinate
       * @return psmrts::PRQRayTrace Result of the shape model trace 
       */
      template <typename T>
        psmrts::PRQRayTrace getSurfaceLatLonRadius( const Latitude &lat, 
                                                    const Longitude &lon,
                                                    const T &tracer ) 
                                                    const {

          // Trace it to the surface for intersect potential
          psmrts::PRQRayTrace ray_t;
          (void) getSurfaceLatLonRadius( lat, lon, tracer, ray_t );
          return ( ray_t );
        }

      bool load_pvl_config( const QString &pvlconf_f, PvlFlatMap &flat_p ) const;
      bool load_shape_list( const QString &shapelist_f, PvlFlatMap &flat_p ) const;
      static bool requires_psmrts( const Pvl &pvl );


      /** Return a reference to the configuration keywords */
      inline const PvlFlatMap &parameters() const {
        return ( m_parameters );
      }

      /** Return PSMRTS debug status */
      inline bool isDebug() const {
        return ( m_psmrts_debug );
      }

      /** Set the PSMRTS debug status */
      inline void set_debug( const bool p_debug = false ) {
        m_psmrts_debug = p_debug;
      }

    private:
      // Disallow copying because ShapeModel is not copyable
      Q_DISABLE_COPY(PsmrtsShapeModel)

      PvlFlatMap                   m_parameters;
      psmrts::PRQRayTrace          m_shape_ray_t;
      mutable psmrts::PRQRayTrace  m_latlon_ray_t;
      psmrts::PsmrtsPriorityTracer m_tracer;
      double                       m_tolerance;
      bool                         m_psmrts_debug;

      static bool psmrtsUpdateIsisLabel( Pvl &pvl, const PvlFlatMap &psmrts_data ); 
 
      /** Reset/reinit all ray trace states to default conditions */
      inline void reset_all_rays( ) {
        m_shape_ray_t.reset();
      }

      inline bool emission_angle_isvalid( const Eigen::Vector3d &v1, 
                                          const Eigen::Vector3d &v2 ) const {
        double angle = psmrts::PsmrtsRayTrace::separation_angle( v1, v2 );
        if ( angle > M_PI_2 ) return ( false );
        return ( true );
      }

      bool updateTraceState( const psmrts::PRQRayTrace &ray );
  };
}

#endif
