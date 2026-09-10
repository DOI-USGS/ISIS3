#include <vector>
#include <algorithm>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include "PsmrtsShapeModel.h"

#include "Distance.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifDskShape.h"
#include "BulletShapeModel.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "TestUtilities.h"


#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Pointwise;
using ::testing::DoubleNear;


using namespace Isis;

TEST( PsmrtsShapeModelTests, DefaultConstructor ) {
  PsmrtsShapeModel default_psm;
  EXPECT_EQ(default_psm.name(), "PSMRTS");
  EXPECT_FALSE((bool) default_psm.get_shape_trace().hasHit());

  // Get the PSMRTS tracer system and evaluate its default state
  const auto &tracer_s = default_psm.tracer();

  EXPECT_EQ(tracer_s.name(), "isis" );
  EXPECT_EQ(tracer_s.size(), 0 );
}

TEST( PsmrtsShapeModelTests, IsisTracerComparisons ) {

  // This will also test the PSMRTS ISIS path translator system
  const double tolerance_km = 1.0e-6;

  const std::string dsk     = "$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds";
  const std::vector<double> bennu_radii = { 0.283065, 0.271215, 0.249720 };

  // Vector value generator
  auto make_vector = []( const double start, const double stop, const size_t num ) -> std::vector<double> {
    std::vector<double> v(num);
    double step = ( stop - start ) / (num -1 );
    size_t i = 0;
    std::generate( v.begin(), v.end(), [&i, start, step]() {
      return ( start + ( i++ * step ) );
    });

    return ( v );
  };

  // Set up various tracers for comparisons
  std::vector<std::string> bullet_psmrts_s  = { "bullet::" + dsk };
  std::vector<std::string> naifdsk_psmrts_s = { "naifdsk::" + dsk };

  psmrts::PsmrtsTracer ellipsoid_t( psmrts::EllipsoidTracer( Eigen::Vector3d( bennu_radii.data() ), "psmrts_ellipsoid" ) );
  psmrts::PsmrtsTranslations isis_path_t = create_isis_path_translator( "isisdata" );

  psmrts::PsmrtsTracerSystem bullet_tracer_s("bullet_psmrts", bullet_psmrts_s, isis_path_t );
  bullet_tracer_s.set_reference_ellipsoid( ellipsoid_t );

  psmrts::PsmrtsTracerSystem naifdsk_tracer_s("naifdsk_psmrts", naifdsk_psmrts_s, isis_path_t );
  naifdsk_tracer_s.set_reference_ellipsoid( "psmrts_radii", bennu_radii );

  // Set up ISIS target
  std::vector<Distance> target_d = { Distance( bennu_radii[0], Distance::Kilometers),
                                     Distance( bennu_radii[1], Distance::Kilometers),
                                     Distance( bennu_radii[2], Distance::Kilometers) };
  Target target_t;
  target_t.setName("isis_target");
  target_t.setRadii( target_d );

  Pvl pvl;  // Dummy parameter for shape models

  // PSMRTS Tracers
  PsmrtsShapeModel psmrts_bullet_t  = PsmrtsShapeModel( bullet_tracer_s );
  PsmrtsShapeModel psmrts_naifdsk_t = PsmrtsShapeModel( naifdsk_tracer_s );

  // ISIS Tracers
  QString dem_t = QString::fromStdString( dsk );
  BulletShapeModel isis_bullet_t( BulletTargetShape::loadDSK( dem_t ), &target_t, pvl );
  NaifDskShape     isis_naifdsk_t( NaifDskPlateModel( dem_t ), &target_t );

    // Do some basic checks
  EXPECT_PRED_FORMAT2( AssertQStringsEqual, psmrts_bullet_t.name(),  "PSMRTS" );
  EXPECT_PRED_FORMAT2( AssertQStringsEqual, psmrts_naifdsk_t.name(), "PSMRTS" );
  EXPECT_PRED_FORMAT2( AssertQStringsEqual, isis_bullet_t.name(),    "Bullet" );
  EXPECT_PRED_FORMAT2( AssertQStringsEqual, isis_naifdsk_t.name(),   "DSK" );

  EXPECT_FALSE( psmrts_bullet_t.isDEM() );
  EXPECT_FALSE( psmrts_naifdsk_t.isDEM() );
  EXPECT_FALSE( isis_bullet_t.isDEM() );
  EXPECT_FALSE( isis_naifdsk_t.isDEM() );

  const double max_radius = ellipsoid_t.maximum_radius();
  const double observer_scalar = 1.5;  // km

  for ( const double &latitude : make_vector( -90.0, 90.0, 7 ) ) {
    for ( const double &longitude : make_vector( 0.0, 360.0, 13 ) ) {
      Eigen::Vector3d llr_t( { longitude, latitude, max_radius } );
      Eigen::Vector3d observer_t = psmrts::lonlatrad_to_xyz_d( llr_t ) * observer_scalar;
      Eigen::Vector3d lookdir_t  = -observer_t.normalized();
      psmrts::PsmrtsRayTrace ray_t( observer_t, lookdir_t );

      std::vector<double> observer_v = { observer_t[0], observer_t[1], observer_t[2] };
      std::vector<double> lookdir_v  = { lookdir_t[0],  lookdir_t[1],  lookdir_t[2] };

      Longitude lon_t( longitude, Angle::Degrees );
      Longitude lat_t( latitude,  Angle::Degrees );
      Distance  radius_t( max_radius, Distance::Kilometers );

      SurfacePoint surfpt_i( lat_t, lon_t, radius_t );
      
      // Generalized comparison lambda function for ray tracers
      auto CompareTrace = [&] ( const std::string &method_s ) {
        SCOPED_TRACE("TraceMethod: " + method_s + " - Longitude = " + qt_to_string( toString( longitude ) ) + ", Latitude = " + qt_to_string( toString( latitude ) ) );
        
        EXPECT_TRUE( psmrts_bullet_t.hasIntersection() );
        EXPECT_TRUE( psmrts_naifdsk_t.hasIntersection() );
        EXPECT_TRUE( isis_bullet_t.hasIntersection() );
        EXPECT_TRUE( isis_naifdsk_t.hasIntersection() );

        EXPECT_TRUE( observer_t.isApprox( psmrts_bullet_t.get_shape_trace().trace().observer(), tolerance_km ) );
        EXPECT_TRUE( observer_t.isApprox( psmrts_naifdsk_t.get_shape_trace().trace().observer(), tolerance_km ) );
        EXPECT_TRUE( lookdir_t.isApprox( psmrts_bullet_t.get_shape_trace().trace().lookdir(), tolerance_km ) );
        EXPECT_TRUE( lookdir_t.isApprox( psmrts_bullet_t.get_shape_trace().trace().lookdir(), tolerance_km ) );

        EXPECT_TRUE( psmrts_bullet_t.hasNormal() );
        EXPECT_TRUE( psmrts_naifdsk_t.hasNormal() );
        EXPECT_FALSE( isis_bullet_t.hasNormal() );
        EXPECT_FALSE( isis_naifdsk_t.hasNormal() );

        EXPECT_TRUE( psmrts_bullet_t.hasLocalNormal() );
        EXPECT_TRUE( psmrts_naifdsk_t.hasLocalNormal() );
        EXPECT_TRUE( isis_bullet_t.hasLocalNormal() );
        EXPECT_TRUE( isis_naifdsk_t.hasLocalNormal() );

        const auto &psmrts_bullet_ray  = psmrts_bullet_t.get_shape_trace().trace();
        const auto &psmrts_naifdsk_ray = psmrts_naifdsk_t.get_shape_trace().trace();

        EXPECT_TRUE( psmrts_bullet_ray.raypt().isApprox( psmrts_naifdsk_ray.raypt() , tolerance_km ) );
        EXPECT_TRUE( psmrts_bullet_ray.xyz().isApprox( psmrts_naifdsk_ray.xyz() , tolerance_km ) );

        EXPECT_NEAR( psmrts_bullet_ray.radius(), psmrts_naifdsk_ray.radius() , tolerance_km );
        EXPECT_NEAR( psmrts_bullet_ray.slant_distance(), psmrts_naifdsk_ray.slant_distance(), tolerance_km );
        EXPECT_NEAR( psmrts_bullet_ray.distance( psmrts_naifdsk_ray ), 0.0, tolerance_km );
        EXPECT_NEAR( psmrts::PsmrtsRayTrace::separation_angle( psmrts_bullet_ray.xyz(), psmrts_naifdsk_ray.xyz() ), 0.0, tolerance_km );

        EXPECT_TRUE( psmrts_bullet_t.isVisibleFrom( observer_v, lookdir_v ) );
        EXPECT_TRUE( psmrts_naifdsk_t.isVisibleFrom( observer_v, lookdir_v ) );
        // EXPECT_TRUE( isis_bullet_t.isVisibleFrom( observer_v, lookdir_v ) );
        EXPECT_TRUE( isis_naifdsk_t.isVisibleFrom( observer_v, lookdir_v ) );
      };

      // Test vector traces of observer/lookdir
      EXPECT_TRUE( psmrts_bullet_t.intersectSurface( observer_v, lookdir_v ) );
      EXPECT_TRUE( psmrts_naifdsk_t.intersectSurface( observer_v, lookdir_v ) );
      EXPECT_TRUE( isis_bullet_t.intersectSurface( observer_v, lookdir_v ) );
      EXPECT_TRUE( isis_naifdsk_t.intersectSurface( observer_v, lookdir_v ) );
      CompareTrace( "intersectSurface(observer, lookdir)" );

      // Test lat/lon traces from observer
      EXPECT_TRUE( psmrts_bullet_t.intersectSurface( lat_t, lon_t, observer_v) );
      EXPECT_TRUE( psmrts_naifdsk_t.intersectSurface( lat_t, lon_t, observer_v ) );
      EXPECT_TRUE( isis_bullet_t.intersectSurface( lat_t, lon_t, observer_v, false ) ); // Fails occlusion test
      EXPECT_TRUE( isis_naifdsk_t.intersectSurface( lat_t, lon_t, observer_v ) );
      lookdir_t = psmrts_bullet_t.get_shape_trace().trace().lookdir(); // Tests need lookdir updated
      CompareTrace( "intersectSurface(lat, lon, observer)" ); 
      lookdir_t  = -observer_t.normalized(); // Update lookdir after this trace

      // Test surface intercept point of observer
      EXPECT_TRUE( psmrts_bullet_t.intersectSurface( surfpt_i, observer_v) );
      EXPECT_TRUE( psmrts_naifdsk_t.intersectSurface( surfpt_i, observer_v ) );
      EXPECT_TRUE( isis_bullet_t.intersectSurface( surfpt_i, observer_v, false ) ); // Fails occlusion test
      EXPECT_TRUE( isis_naifdsk_t.intersectSurface( surfpt_i, observer_v ) );
      lookdir_t = psmrts_bullet_t.get_shape_trace().trace().lookdir(); // Tests need lookdir updated
      CompareTrace( "intersectSurface(surfpt, observer)" ); 
      lookdir_t  = -observer_t.normalized(); // Update lookdir after this trace
    }
  }
}