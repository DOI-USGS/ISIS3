#include <vector>
#include <algorithm>

#include "IsisPsmrtsUtilities.hpp"
#include "PsmrtsShapeModel.h"

#include "Angle.h"
#include "Distance.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifDskShape.h"
#include "BulletShapeModel.h"
#include "Pvl.h"
#include "ShapeModel.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "TestUtilities.h"


#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Pointwise;
using ::testing::DoubleNear;


using namespace Isis;


/** Create a Pvl Preferences file with a named group, with "ShapeModel" default */
inline Pvl make_preferences( const PvlFlatMap &parameters, 
                             const QString &grpnam = "ShapeModel" ) {
  Pvl pvl;
  if ( parameters.size() > 0 ) {
    PvlGroup prefs( grpnam );
    for ( const PvlKeyword &key : parameters ) {
      prefs.addKeyword( key );
    }

    pvl.addGroup ( prefs );
  }
  return ( pvl );
}

/** Helper function to compute the  */
inline SurfacePoint trace_lon_lat( const double &longitude, 
                                   const double latitude,
                                   ShapeModel *shape_t,
                                   const double &distance_f = 1.5 ) {
  
  Longitude lon_t( longitude, Angle::Degrees );
  Latitude  lat_t( latitude, Angle::Degrees );

  Distance radius = shape_t->localRadius( lat_t, lon_t );
  EXPECT_TRUE( radius.isValid() );

  SurfacePoint surfpt( lat_t, lon_t, radius );
  Eigen::Vector3d observer;
  surfpt.ToNaifArray( observer.data() );
  observer = observer.normalized() * distance_f;
  std::vector<double> observer_s = { observer[0], observer[1], observer[2] };
  EXPECT_TRUE( shape_t->intersectSurface( lat_t, lon_t, observer_s ) );  

  return ( surfpt );
}


TEST( PsmrtsShapeModelTests, DefaultConstructor ) {
  PsmrtsShapeModel default_psm;
  EXPECT_EQ(default_psm.name(), "PSMRTS");
  EXPECT_FALSE((bool) default_psm.get_shape_trace().hasHit());

  // Get the PSMRTS tracer system and evaluate its default state
  const auto &tracer_s = default_psm.tracer();

  EXPECT_EQ(tracer_s.name(), "isis" );
  EXPECT_EQ(tracer_s.size(), 0 );
}


TEST(PsmrtsShapeModelTests, PsmrtsConstructorTests ) {
  const double tolerance_km = 1.0e-6;

  typedef std::unique_ptr<PsmrtsShapeModel> PsmrtsModelPtr;

  psmrts::PsmrtsFactory().liquidate();

  const std::vector<std::string> bennu_list = { 
    "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds",
    "ellipsoid::0.283065,0.271215,0.249720" 
  };

    // Set up ISIS target
  const std::vector<double> bennu_radii = { 0.283065, 0.271215, 0.249720 };
  std::vector<Distance> target_d = { Distance( bennu_radii[0], Distance::Kilometers),
                                     Distance( bennu_radii[1], Distance::Kilometers),
                                     Distance( bennu_radii[2], Distance::Kilometers) };

  // Set up a miminal target object for PSMRTS
  Target target_t;
  target_t.setName("isis_target");
  target_t.setRadii( target_d );

  // Set up a minimal label for PSMRTS
  PvlFlatMap flat_t;
  flat_t.add( PvlKeyword( "ShapeModel", bennu_list) );
  Pvl pvl_t0 = make_preferences( flat_t, "Kernels" );
  EXPECT_TRUE( PsmrtsShapeModel::requires_psmrts( pvl_t0 ) );

  PsmrtsShapeModel psmrts_t0( &target_t, pvl_t0 );
  EXPECT_FALSE( psmrts_t0.isDEM() );
  
  EXPECT_TRUE( psmrts_t0.tracer().isValid() );
  EXPECT_EQ( psmrts_t0.tracer().size(), 2 );

  EXPECT_STREQ( psmrts_t0.tracer().tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( psmrts_t0.tracer().tracers()[1]->type().c_str(),  "tracer" );

  EXPECT_STREQ( psmrts_t0.tracer().tracers()[0]->model().c_str(), "bullet" );
  EXPECT_STREQ( psmrts_t0.tracer().tracers()[1]->model().c_str(), "ellipsoid" );

  EXPECT_FALSE( psmrts_t0.isDebug() );
  psmrts_t0.set_debug( true );
  EXPECT_TRUE( psmrts_t0.isDebug() );

  EXPECT_NEAR( psmrts_t0.get_tolerance(), PsmrtsShapeModel::DefaultDistanceTolerance, tolerance_km );
  psmrts_t0.set_tolerance( 10.0 );
  EXPECT_NEAR( psmrts_t0.get_tolerance(), 10.0, tolerance_km );
  psmrts_t0.set_tolerance( );
  EXPECT_NEAR( psmrts_t0.get_tolerance(), PsmrtsShapeModel::DefaultDistanceTolerance, tolerance_km );

  EXPECT_TRUE( psmrts_t0.tracer().isValid() );
  EXPECT_EQ( psmrts_t0.tracer().size(), 2 );

  Pvl pvl_t1 = make_preferences( flat_t, "Kernels" );
  PsmrtsModelPtr psmrts_t1( PsmrtsShapeModel::create( &target_t, pvl_t1 ) );
  ASSERT_NE( psmrts_t1, nullptr );

  EXPECT_TRUE( psmrts_t1->tracer().isValid() );
  EXPECT_EQ( psmrts_t1->tracer().size(), 2 );

  EXPECT_STREQ( psmrts_t1->tracer().tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( psmrts_t1->tracer().tracers()[1]->type().c_str(),  "tracer" );

  EXPECT_STREQ( psmrts_t1->tracer().tracers()[0]->model().c_str(), "bullet" );
  EXPECT_STREQ( psmrts_t1->tracer().tracers()[1]->model().c_str(), "ellipsoid" );

  auto params_t = psmrts_t1->parameters();
  params_t.add( "Tolerance", "0.5" );
  PsmrtsShapeModel psmrts_tr( psmrts_t1->tracer(), params_t );
  EXPECT_FALSE( psmrts_tr.isDEM() );
  EXPECT_NEAR( psmrts_tr.get_tolerance(), 0.5, tolerance_km );
  
  EXPECT_TRUE( psmrts_tr.tracer().isValid() );
  EXPECT_EQ( psmrts_tr.tracer().size(), 2 );

  EXPECT_STREQ( psmrts_tr.tracer().tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( psmrts_tr.tracer().tracers()[1]->type().c_str(),  "tracer" );

  EXPECT_STREQ( psmrts_tr.tracer().tracers()[0]->model().c_str(), "bullet" );
  EXPECT_STREQ( psmrts_tr.tracer().tracers()[1]->model().c_str(), "ellipsoid" );

  EXPECT_EQ( psmrts::PsmrtsFactory().shape_count(),  1 );
  EXPECT_EQ( psmrts::PsmrtsFactory().tracer_count(), 2 );
}

TEST( PsmrtsShapeModelTests, ShapeModelMethods ) {

  const double tolerance = 1.0e-6;

  const std::vector<std::string> bennu_list = { 
    "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds"
  };

    // Set up ISIS target
  const std::vector<double> bennu_radii = { 0.283065, 0.271215, 0.249720 };
  std::vector<Distance> target_d = { Distance( bennu_radii[0], Distance::Kilometers),
                                     Distance( bennu_radii[1], Distance::Kilometers),
                                     Distance( bennu_radii[2], Distance::Kilometers) };

  // Set up a miminal target object for PSMRTS
  Target target_t;
  target_t.setName("isis_target");
  target_t.setRadii( target_d );

  // Set up a minimal label for PSMRTS
  PvlFlatMap flat_t;
  flat_t.add( PvlKeyword( "ShapeModel", bennu_list) );
  Pvl pvl_t = make_preferences( flat_t, "Kernels" );

  std::unique_ptr<ShapeModel> shape_t( new PsmrtsShapeModel( &target_t, pvl_t ) );
  ASSERT_NE( shape_t, nullptr );
  EXPECT_PRED_FORMAT2( AssertQStringsEqual, shape_t->name(),  "PSMRTS" );
  EXPECT_FALSE( shape_t->isDEM() );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( shape_t.get() );
  ASSERT_NE( psmrts_t, nullptr );

  // Set up observer (@3.5km) and check surface intercept
  double latitude = 90.0;
  double longitude = 0.0;
  auto surfpt = trace_lon_lat( longitude, latitude, shape_t.get(), 3.5 );

  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  Eigen::Vector3d latlonrad_t = psmrts::xyz_to_lonlatrad_d( point_t );
  EXPECT_NEAR( latlonrad_t[0], longitude, tolerance );
  EXPECT_NEAR( latlonrad_t[1], latitude, tolerance );
  EXPECT_NEAR( latlonrad_t[2], 0.25066253542900091, tolerance );
  double radius = latlonrad_t[2];

  auto ray_t = psmrts_t->get_shape_trace();
  EXPECT_TRUE( ray_t.hasHit() );
  EXPECT_TRUE( ray_t.isValid() );
  auto tracer_t = psmrts_t->tracer().get_tracer( ray_t.trace() );
  EXPECT_EQ( tracer_t->model(), "bullet" );

  Eigen::Vector3d observer_t = ray_t.trace().observer();
  EXPECT_NEAR( observer_t[0], 0.0, tolerance );
  EXPECT_NEAR( observer_t[1], 0.0, tolerance );
  EXPECT_NEAR( observer_t[2], 3.5, tolerance );

  Eigen::Vector3d lookdir_t = ray_t.trace().lookdir();
  EXPECT_NEAR( lookdir_t[0],  0.0, tolerance );
  EXPECT_NEAR( lookdir_t[1],  0.0, tolerance );
  EXPECT_NEAR( lookdir_t[2], -3.2493374645709991, tolerance );
 
  EXPECT_NEAR( ray_t.trace().radius(), 0.25066253542900152, tolerance );
  EXPECT_NEAR( ray_t.trace().slant_distance(), 3.2493374645709983, tolerance );
  EXPECT_NEAR( ray_t.trace().emission(), 0.45630030280164435, tolerance );
  EXPECT_EQ( ray_t.trace().get_tracer_id(), tracer_t->uid() );
  EXPECT_EQ( shape_t->plate_index(), 1056 );  
  EXPECT_EQ( shape_t->plate_index(), psmrts_t->plate_index() );  

  // XYZs of observer and look direction vectors
  std::vector<double> observer = { observer_t[0], observer_t[1], observer_t[2] };
  std::vector<double> lookdir  = { lookdir_t[0],  lookdir_t[1],  lookdir_t[2] };

  // Run surface intercept and inspect state data
  EXPECT_TRUE( shape_t->intersectSurface( observer, lookdir ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  EXPECT_TRUE( shape_t->hasNormal() );
  EXPECT_TRUE( shape_t->hasLocalNormal() );

  SurfacePoint spoint_t = *shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t.Valid() );
  EXPECT_NEAR( spoint_t.GetLatitude().degrees(), latitude,  tolerance );
  EXPECT_NEAR( spoint_t.GetLongitude().degrees(), longitude, tolerance );
  EXPECT_NEAR( spoint_t.GetLocalRadius().kilometers(), radius, tolerance );

  std::vector<double> normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.20608601240483881, tolerance );
  EXPECT_NEAR( normal_t[1], -0.38946541724151479, tolerance );
  EXPECT_NEAR( normal_t[2], 0.89768883487763473, tolerance );

  std::vector<double> local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance );

  QVector<double *> dummy = { nullptr, nullptr, nullptr, nullptr };
  shape_t->calculateLocalNormal( dummy );
  std::vector<double> calc_normal_t = shape_t->localNormal();

  EXPECT_NEAR( calc_normal_t[0], 0.20608601240483881, tolerance );
  EXPECT_NEAR( calc_normal_t[1], -0.38946541724151479, tolerance );
  EXPECT_NEAR( calc_normal_t[2], 0.89768883487763473, tolerance );

  shape_t->calculateDefaultNormal();
  normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.20608601240483881, tolerance );
  EXPECT_NEAR( normal_t[1], -0.38946541724151479, tolerance );
  EXPECT_NEAR( normal_t[2], 0.89768883487763473, tolerance );

  // Set up sun position/observer location (@1.0e5 km)
  double subsolarlat = 55.0;
  double subsolarlon = 210.0;
  Eigen::Vector3d ssb_observer_t = psmrts::lonlatrad_to_xyz_d( { subsolarlon, subsolarlat, 100000.0 } );
  Eigen::Vector3d ssb_lookdir_t = point_t - ssb_observer_t;

  // Check to see if its visible from the sun
  std::vector<double> ssb_observer = { ssb_observer_t[0], ssb_observer_t[1], ssb_observer_t[2] };
  std::vector<double> ssb_lookdir  = { ssb_lookdir_t[0],  ssb_lookdir_t[1],  ssb_lookdir_t[2] };
  EXPECT_TRUE( shape_t->isVisibleFrom( ssb_observer, ssb_lookdir ) );

  EXPECT_NEAR( shape_t->emissionAngle( observer ), 26.144081541075714, tolerance );
  EXPECT_NEAR( shape_t->incidenceAngle( ssb_observer ), 41.869448280909282, tolerance );
  EXPECT_NEAR( shape_t->phaseAngle( observer, ssb_observer ), 35.000082376674108, tolerance );
  
  EXPECT_EQ( shape_t->plate_index(), 1056 );

  // Test vector traces of observer/lookdir
  EXPECT_TRUE( shape_t->intersectSurface( spoint_t.GetLatitude(), spoint_t.GetLongitude(), observer ) );
  SurfacePoint spoint_t2 = *shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t2.Valid() );
  EXPECT_NEAR( spoint_t2.GetLatitude().degrees(), spoint_t.GetLatitude().degrees(),  tolerance );
  EXPECT_NEAR( spoint_t2.GetLongitude().degrees(), spoint_t.GetLongitude().degrees(), tolerance );
  EXPECT_NEAR( spoint_t2.GetLocalRadius().kilometers(), spoint_t.GetLocalRadius().kilometers(), tolerance );

  // Test lat/lon traces from observer
  EXPECT_TRUE( shape_t->intersectSurface( spoint_t2, observer) );
  SurfacePoint spoint_t3 = *shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t3.Valid() );
  EXPECT_NEAR( spoint_t3.GetLatitude().degrees(), spoint_t.GetLatitude().degrees(),  tolerance );
  EXPECT_NEAR( spoint_t3.GetLongitude().degrees(), spoint_t.GetLongitude().degrees(), tolerance );
  EXPECT_NEAR( spoint_t3.GetLocalRadius().kilometers(), spoint_t.GetLocalRadius().kilometers(), tolerance );

}


TEST(PsmrtsShapeModelTests, ReverseShapePriorityTest ) {

  const double tolerance = 1.0e-6;

  std::vector<std::string> bennu_list = { 
    "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds",
    "ellipsoid::0.283065,0.271215,0.249720" 
  };

  PsmrtsShapeModel psmrts_t( "psmrts_priority_test", bennu_list );

  const psmrts::PsmrtsPriorityTracer &priority_b = psmrts_t.tracer();
  ASSERT_EQ( priority_b.size(), 2 );
  
  EXPECT_STREQ( priority_b.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_b.tracers()[1]->type().c_str(),  "tracer" );

  EXPECT_STREQ( priority_b.tracers()[0]->model().c_str(), "bullet" );
  EXPECT_STREQ( priority_b.tracers()[1]->model().c_str(), "ellipsoid" );

  auto spt_b = trace_lon_lat( 30.0, 45.0, &psmrts_t );
  EXPECT_TRUE ( spt_b.Valid() );
  auto ray_b = psmrts_t.get_shape_trace();
  EXPECT_TRUE( ray_b.hasHit() );
  EXPECT_TRUE( ray_b.isValid() );
  auto tracer_b = psmrts_t.tracer().get_tracer( ray_b.trace() );
  EXPECT_EQ( tracer_b->model(), "bullet" );

  Eigen::Vector3d observer_b = ray_b.trace().observer();
  EXPECT_NEAR( observer_b[0], 0.91855865354369182, tolerance );
  EXPECT_NEAR( observer_b[1], 0.5303300858899106, tolerance );
  EXPECT_NEAR( observer_b[2], 1.0606601717798212, tolerance );

  Eigen::Vector3d lookdir_b = ray_b.trace().lookdir();
  EXPECT_NEAR( lookdir_b[0], -0.78006781743292941, tolerance );
  EXPECT_NEAR( lookdir_b[1], -0.45037236438106559, tolerance );
  EXPECT_NEAR( lookdir_b[2],  -0.90074472876213119, tolerance );

  Eigen::Vector3d latlonrad_b = psmrts::xyz_to_lonlatrad_d( ray_b.trace().xyz() );
  EXPECT_NEAR( latlonrad_b[0], 30.0, tolerance );
  EXPECT_NEAR( latlonrad_b[1], 45.0, tolerance );
  EXPECT_NEAR( latlonrad_b[2], 0.22615458834851906, tolerance );

  EXPECT_NEAR( ray_b.trace().radius(), 0.22615458834851906, tolerance );
  EXPECT_NEAR( ray_b.trace().slant_distance(), 1.273845411651481, tolerance );
  EXPECT_NEAR( ray_b.trace().emission(), 0.1120722746125111, tolerance );
  EXPECT_EQ( ray_b.trace().get_tracer_id(), tracer_b->uid() );
  EXPECT_EQ( psmrts_t.plate_index(), 1869 );

  // Reverse tracer order and trace same location
  EXPECT_EQ( psmrts_t.reverse_priority(), 2 );
  const psmrts::PsmrtsPriorityTracer &priority_e = psmrts_t.tracer();
  ASSERT_EQ( priority_e.size(), 2 );
  
  EXPECT_STREQ( priority_e.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_e.tracers()[1]->type().c_str(),  "tracer" );

  EXPECT_STREQ( priority_e.tracers()[0]->model().c_str(), "ellipsoid" );  
  EXPECT_STREQ( priority_e.tracers()[1]->model().c_str(), "bullet" );

  auto spt_e = trace_lon_lat( 30.0, 45.0, &psmrts_t );
  EXPECT_TRUE ( spt_e.Valid() );
  auto ray_e = psmrts_t.get_shape_trace();
  EXPECT_TRUE( ray_e.hasHit() );
  EXPECT_TRUE( ray_e.isValid() );
  auto tracer_e = psmrts_t.tracer().get_tracer( ray_e.trace() );
  EXPECT_EQ( tracer_e->model(), "ellipsoid" );

  Eigen::Vector3d observer_e = ray_e.trace().observer();
  EXPECT_NEAR( observer_e[0], 0.91855865354369182, tolerance );
  EXPECT_NEAR( observer_e[1], 0.5303300858899106, tolerance );
  EXPECT_NEAR( observer_e[2], 1.0606601717798212,  tolerance );

  Eigen::Vector3d lookdir_e = ray_e.trace().lookdir();
  EXPECT_NEAR( lookdir_e[0], -0.75717000145894453, tolerance );
  EXPECT_NEAR( lookdir_e[1], -0.4371523041646308,  tolerance );
  EXPECT_NEAR( lookdir_e[2], -0.87430460832926182, tolerance );

  Eigen::Vector3d latlonrad_e = psmrts::xyz_to_lonlatrad_d( ray_e.trace().xyz() );
  EXPECT_NEAR( latlonrad_e[0], 30.0, tolerance );
  EXPECT_NEAR( latlonrad_e[1], 45.0, tolerance );
  EXPECT_NEAR( latlonrad_e[2], 0.26354656525546094, tolerance );

  EXPECT_NEAR( ray_e.trace().radius(), 0.26354656525546094, tolerance );
  EXPECT_NEAR( ray_e.trace().slant_distance(), 1.2364534347445388, tolerance );
  EXPECT_NEAR( ray_e.trace().emission(), 0.11572445032853541, tolerance );
  EXPECT_EQ( ray_e.trace().get_tracer_id(), tracer_e->uid() );
  EXPECT_EQ( psmrts_t.plate_index(), -1 );
}
