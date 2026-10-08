#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include <QTemporaryDir>
#include <QString>
#include <QVector>

#include "BulletShapeModel.h"
#include "Camera.h"
#include "CameraPointInfo.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "NaifDskShape.h"
#include "IsisPsmrtsUtilities.hpp"
#include "PsmrtsShapeModel.h"
#include "Pvl.h"
#include "PvlFlatMap.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "ShapeModel.h"
#include "ShapeModelFactory.h"
#include "TempFixtures.h"
#include "TestUtilities.h"
#include "TextFile.h"
#include "UserInterface.h"

#include "ocams2isis.h"
#include "spiceinit.h"

#include <Eigen/Geometry>

#include "gtest/gtest.h"

using namespace Isis;

class PsmrtsSpiceinit : public TempTestingFiles {
  private:
    static inline QString APP_XML_OCAMS2ISIS = FileName("$ISISROOT/bin/xml/ocams2isis.xml").expanded();
    static inline QString APP_XML_SPICEINIT  = FileName("$ISISROOT/bin/xml/spiceinit.xml").expanded();

  protected:
    using XmlParameters = QVector<QString>;

    inline QString run_ocams2isis( const QString &from ) const {
      QString to = make_temp_filename( FileName( from ).setExtension( "cub" ).name() );
      XmlParameters args = { "from=" + from, "to=" + to };
      UserInterface ui( APP_XML_OCAMS2ISIS, args );
      try {
      ocams2isis(ui);
      }
      catch (IException &e) {
        QString mess = "Unable to run ocams2isis file " + from;
        throw IException( e, IException::Programmer, mess, _FILEINFO_ );
      }
      
      return ( to );
    }

    inline Cube *run_spiceinit( const QString from,
                                const XmlParameters &spiceinit_parameters, 
                                const PvlFlatMap &preferences = PvlFlatMap() ) {
      XmlParameters args = { "from=" + from };
      args += spiceinit_parameters;

      if ( preferences.size() > 0 ) {
        Pvl pvl = make_preferences( preferences );
        args += "-preferences=" + write_preferences_file( "prefs.pvl", pvl );
      }

      // Run spiceinit
      UserInterface ui(APP_XML_SPICEINIT, args);
      try {
      spiceinit(ui);
      }
      catch (IException &e) {
        QString mess = "Unable to spiceinit file: " + from;
        throw IException( e, IException::Programmer, mess, _FILEINFO_ );
      }

      return ( new Cube( from ) );
    }

    /** Return the temporary directory path */
    inline QString tmpdir() const {
      return ( tempDir.path() );
    }

    /** Create a Pvl Preferences file with a named group, with "ShapeModel" default */
    inline Pvl make_preferences( const PvlFlatMap &parameters, 
                                 const QString &grpnam = "ShapeModel" ) const {
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

    inline QString make_temp_filename( const QString &basename ) const {
      FileName base_t( tmpdir() + basename );
      FileName pvlfile = FileName::createTempFile( base_t );
      return ( pvlfile.toString() );
    }

    inline QString write_preferences_file( const QString &filename,
                                           Pvl &pvl_prefs ) const {
      QString ofile = make_temp_filename( filename );
      pvl_prefs.write( ofile );
      return ( ofile );
    }

    inline QString write_list_file( const QString &filename,
                                    std::vector<QString> &lines ) const {
      QString ofile = make_temp_filename( filename );
      TextFile txtfile( ofile, "output", lines );
      txtfile.Close();
      return ( ofile );
    }    
};

/** Expose the Camera and Cube pointers for enhanced functionality */
class EnhancedCameraPointInfo : public CameraPointInfo {
  public:
    EnhancedCameraPointInfo() : CameraPointInfo() {}
    virtual ~EnhancedCameraPointInfo() = default;

    inline Camera *camera() {
     return ( CameraPointInfo::camera() );
    }

    inline Cube *cube() {
     return ( CameraPointInfo::cube() );
    }

    inline ShapeModel *shape() {
      if ( nullptr != this->camera() ) {
        if ( nullptr != this->camera()->target() ) {
          return ( this->camera()->target()->shape() );
        }
      }

      return ( nullptr );
    }
};

/** Convenience functon to create a std::vector from an Eigen vector */
std::vector<double> eigen_to_vector( const Eigen::Vector3d &v ) {
  std::vector<double> std_v{ v[0], v[1], v[2] };
  return ( std_v );
}

/** Convenience functon to create a std::vector from an Eigen vector */
Eigen::Vector3d  vector_to_eigen( const std::vector<double> &v ) {
  return ( Eigen::Vector3d{ v.data() } );
}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitBullet ) {

  const double tolerance_u   = 1.0e-9;
  const double tolerance_d   = 1.0e-9;
  const double tolerance_km  = 1.0e-9;

  const QString bennu_t( "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;
  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( psmrts_t, nullptr );

  const psmrts::PsmrtsPriorityTracer &priority_t = psmrts_t->tracer();
  ASSERT_EQ( priority_t.size(), 1 );
  
  EXPECT_STREQ( priority_t.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0]->model().c_str(), "bullet" );
  EXPECT_EQ( priority_t.tracers()[0]->name(),  bennu_t );

  // Run some shapemodel tests
  ShapeModel *shape_t = camera_t->target()->shape();
  ASSERT_NE( shape_t, nullptr );
  EXPECT_FALSE( shape_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "PSMRTS" );

  // Set observer location
  Latitude latitude(45.0, Angle::Degrees );
  Longitude longitude(50.0, Angle::Degrees );
  Distance radius = shape_t->localRadius( latitude, longitude );
  EXPECT_TRUE( radius.isValid() );
  EXPECT_NEAR( radius.kilometers(), 0.21677833612799632, tolerance_km );

  SurfacePoint surfpt( latitude, longitude, radius );
  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  EXPECT_NEAR( point_t.norm(), 0.21677833612799632, tolerance_km );

  std::vector<double> observer_s = eigen_to_vector( point_t.normalized() * 1.5 );
  EXPECT_TRUE( shape_t->intersectSurface( latitude, longitude, observer_s ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  const auto &trace_s = psmrts_t->get_shape_trace();
  Eigen::Vector3d ray_normal_t = trace_s.trace().normal();

  EXPECT_NEAR( ray_normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( ray_normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( ray_normal_t[2], 0.78940041431260366, tolerance_u );

  SurfacePoint *spoint_t = shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t->Valid() );
  EXPECT_NEAR( spoint_t->GetLatitude().degrees(), latitude.degrees(),  tolerance_d );
  EXPECT_NEAR( spoint_t->GetLongitude().degrees(), longitude.degrees(), tolerance_d );
  EXPECT_NEAR( spoint_t->GetLocalRadius().meters(), radius.meters(), tolerance_km );

  std::vector<double> normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.78940041431260366, tolerance_u );

  std::vector<double> local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  QVector<double *> dummy = { nullptr, nullptr, nullptr, nullptr };
  shape_t->calculateLocalNormal( dummy );
  shape_t->calculateDefaultNormal();
  Eigen::Vector3d calc_normal_t = psmrts_t->get_shape_trace().trace().normal();

  EXPECT_NEAR( calc_normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( calc_normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( calc_normal_t[2], 0.78940041431260366, tolerance_u );

  EXPECT_TRUE( shape_t->hasNormal() );
  EXPECT_TRUE( shape_t->hasLocalNormal() );

  normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.78940041431260366, tolerance_u );

  local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  EXPECT_NEAR( shape_t->emissionAngle( observer_s ), 17.27549376523487, tolerance_d );

  EXPECT_EQ( psmrts_t->plate_index(), 1734 );
  
  Eigen::Vector3d observer_t = vector_to_eigen( observer_s );
  Eigen::Vector3d raypt_t  = point_t - observer_t;
  EXPECT_NEAR( raypt_t.norm(), 1.2832216638720038, tolerance_km );

  // Set sun position using sub-solar coordinates
  Latitude latitude_ss( 0.0, Angle::Degrees );
  Longitude longitude_ss( 0.0, Angle::Degrees );  
  Distance radius_ss = shape_t->localRadius( latitude_ss, longitude_ss );
  EXPECT_TRUE( radius_ss.isValid() );

  Eigen::Vector3d sunpos_subsolar_t{ longitude_ss.degrees(), 
                                     latitude_ss.degrees(), 
                                     radius_ss.kilometers() };
  Eigen::Vector3d sunpos_t = psmrts::lonlatrad_to_xyz_d( sunpos_subsolar_t ) * 100.0;

  std::vector<double> sunpos_s = eigen_to_vector( sunpos_t );

  EXPECT_NEAR( shape_t->incidenceAngle( sunpos_s ), 80.632868916142385, tolerance_d );
  EXPECT_NEAR( shape_t->phaseAngle( observer_s, sunpos_s ), 63.369378458433317, tolerance_d );

  std::vector<double> sun_lookdir_s = eigen_to_vector( point_t - sunpos_t );
  EXPECT_TRUE( shape_t->isVisibleFrom( sunpos_s, sun_lookdir_s ) );
}


TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitNaifDsk ) {

  const double tolerance_u   = 1.0e-8;
  const double tolerance_d   = 1.0e-8;
  const double tolerance_km  = 1.0e-8;

  const QString bennu_t( "naifdsk::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );
  const std::string bennu_name = psmrts::string_tokenizer_substring( bennu_t.toStdString(), "::").back();

  QString from;
  std::unique_ptr<Cube> cube_t;
  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( psmrts_t, nullptr );

  const psmrts::PsmrtsPriorityTracer &priority_t = psmrts_t->tracer();
  ASSERT_EQ( priority_t.size(), 1 );
  
  EXPECT_STREQ( priority_t.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0]->model().c_str(), "naifdsk" );
  EXPECT_EQ( priority_t.tracers()[0]->name(),  bennu_name );

 // Run some shapemodel tests
  ShapeModel *shape_t = camera_t->target()->shape();
  ASSERT_NE( shape_t, nullptr );
  EXPECT_FALSE( shape_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "PSMRTS" );

  // Set observer location
  Latitude latitude(45.0, Angle::Degrees );
  Longitude longitude(50.0, Angle::Degrees );
  Distance radius = shape_t->localRadius( latitude, longitude );
  EXPECT_TRUE( radius.isValid() );
  EXPECT_NEAR( radius.kilometers(), 0.21677833612799632, tolerance_km );

  SurfacePoint surfpt( latitude, longitude, radius );
  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  EXPECT_NEAR( point_t.norm(), 0.21677833612799632, tolerance_km );

  std::vector<double> observer_s = eigen_to_vector( point_t.normalized() * 1.5 );
  EXPECT_TRUE( shape_t->intersectSurface( latitude, longitude, observer_s ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  const auto &trace_s = psmrts_t->get_shape_trace();
  Eigen::Vector3d ray_normal_t = trace_s.trace().normal();

  EXPECT_NEAR( ray_normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( ray_normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( ray_normal_t[2], 0.78940041431260366, tolerance_u );

  SurfacePoint *spoint_t = shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t->Valid() );
  EXPECT_NEAR( spoint_t->GetLatitude().degrees(), latitude.degrees(),  tolerance_d );
  EXPECT_NEAR( spoint_t->GetLongitude().degrees(), longitude.degrees(), tolerance_d );
  EXPECT_NEAR( spoint_t->GetLocalRadius().meters(), radius.meters(), tolerance_km );

  std::vector<double> normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.78940041431260366, tolerance_u );

  std::vector<double> local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  QVector<double *> dummy = { nullptr, nullptr, nullptr, nullptr };
  shape_t->calculateLocalNormal( dummy );
  shape_t->calculateDefaultNormal();
  Eigen::Vector3d calc_normal_t = psmrts_t->get_shape_trace().trace().normal();

  EXPECT_NEAR( calc_normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( calc_normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( calc_normal_t[2], 0.78940041431260366, tolerance_u );

  EXPECT_TRUE( shape_t->hasNormal() );
  EXPECT_TRUE( shape_t->hasLocalNormal() );

  normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.16970215030368818, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.58995607130140981, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.78940041431260366, tolerance_u );

  local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  EXPECT_NEAR( shape_t->emissionAngle( observer_s ), 17.27549376523487, tolerance_d );

  EXPECT_EQ( psmrts_t->plate_index(), 1734 );
  
  Eigen::Vector3d observer_t = vector_to_eigen( observer_s );
  Eigen::Vector3d raypt_t  = point_t - observer_t;
  EXPECT_NEAR( raypt_t.norm(), 1.2832216638720038, tolerance_km );

  // Set sun position using sub-solar coordinates
  Latitude latitude_ss( 0.0, Angle::Degrees );
  Longitude longitude_ss( 0.0, Angle::Degrees );  
  Distance radius_ss = shape_t->localRadius( latitude_ss, longitude_ss );
  EXPECT_TRUE( radius_ss.isValid() );

  Eigen::Vector3d sunpos_subsolar_t{ longitude_ss.degrees(), 
                                     latitude_ss.degrees(), 
                                     radius_ss.kilometers() };
  Eigen::Vector3d sunpos_t = psmrts::lonlatrad_to_xyz_d( sunpos_subsolar_t ) * 100.0;

  std::vector<double> sunpos_s = eigen_to_vector( sunpos_t );

  EXPECT_NEAR( shape_t->incidenceAngle( sunpos_s ), 80.632868916142385, tolerance_d );
  EXPECT_NEAR( shape_t->phaseAngle( observer_s, sunpos_s ), 63.369378458433317, tolerance_d );

  std::vector<double> sun_lookdir_s = eigen_to_vector( point_t - sunpos_t );
  EXPECT_TRUE( shape_t->isVisibleFrom( sunpos_s, sun_lookdir_s ) );
}


TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitEllipsoid ) {

  const double tolerance_u   = 1.0e-9;
  const double tolerance_d   = 1.0e-9;
  const double tolerance_km  = 1.0e-9;

  const QString bennu_t( "ellipsoid::0.283065,0.271215,0.249720" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;
  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( psmrts_t, nullptr );

  const psmrts::PsmrtsPriorityTracer &priority_t = psmrts_t->tracer();
  ASSERT_EQ( priority_t.size(), 1 );
  
  EXPECT_STREQ( priority_t.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0]->model().c_str(), "ellipsoid" );
  EXPECT_EQ( priority_t.tracers()[0]->name(),  bennu_t.toStdString() );

 // Run some shapemodel tests
  ShapeModel *shape_t = camera_t->target()->shape();
  ASSERT_NE( shape_t, nullptr );
  EXPECT_FALSE( shape_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "PSMRTS" );

  // Set observer location
  Latitude latitude(45.0, Angle::Degrees );
  Longitude longitude(50.0, Angle::Degrees );
  Distance radius = shape_t->localRadius( latitude, longitude );
  EXPECT_TRUE( radius.isValid() );
  EXPECT_NEAR( radius.kilometers(), 0.26184541692759, tolerance_km );

  SurfacePoint surfpt( latitude, longitude, radius );
  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  EXPECT_NEAR( point_t.norm(), 0.26184541692759, tolerance_km );

  std::vector<double> observer_s = eigen_to_vector( point_t.normalized() * 1.5 );
  EXPECT_TRUE( shape_t->intersectSurface( latitude, longitude, observer_s ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  const auto &trace_s = psmrts_t->get_shape_trace();
  Eigen::Vector3d ray_normal_t = trace_s.trace().normal();

  EXPECT_NEAR( ray_normal_t[0], 0.38688333944146408, tolerance_u );
  EXPECT_NEAR( ray_normal_t[1], 0.5022401575162746, tolerance_u );
  EXPECT_NEAR( ray_normal_t[2], 0.77335380379270691, tolerance_u );

  SurfacePoint *spoint_t = shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t->Valid() );
  EXPECT_NEAR( spoint_t->GetLatitude().degrees(), latitude.degrees(),  tolerance_d );
  EXPECT_NEAR( spoint_t->GetLongitude().degrees(), longitude.degrees(), tolerance_d );
  EXPECT_NEAR( spoint_t->GetLocalRadius().meters(), radius.meters(), tolerance_km );

  std::vector<double> normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.38688333944146408, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.5022401575162746, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.77335380379270691, tolerance_u );

  std::vector<double> local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  QVector<double *> dummy = { nullptr, nullptr, nullptr, nullptr };
  shape_t->calculateLocalNormal( dummy );
  shape_t->calculateDefaultNormal();
  Eigen::Vector3d calc_normal_t = psmrts_t->get_shape_trace().trace().normal();

  EXPECT_NEAR( calc_normal_t[0], 0.38688333944146408, tolerance_u );
  EXPECT_NEAR( calc_normal_t[1], 0.5022401575162746, tolerance_u );
  EXPECT_NEAR( calc_normal_t[2], 0.77335380379270691, tolerance_u );

  EXPECT_TRUE( shape_t->hasNormal() );
  EXPECT_TRUE( shape_t->hasLocalNormal() );

  normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.38688333944146408, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.5022401575162746, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.77335380379270691, tolerance_u );

  local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  EXPECT_NEAR( shape_t->emissionAngle( observer_s ), 5.8788016580356004, tolerance_d );

  EXPECT_EQ( psmrts_t->plate_index(), -1 );
  
  Eigen::Vector3d observer_t = vector_to_eigen( observer_s );
  Eigen::Vector3d raypt_t  = point_t - observer_t;
  EXPECT_NEAR( raypt_t.norm(), 1.23815458307240, tolerance_km );

  // Set sun position using sub-solar coordinates
  Latitude latitude_ss( 0.0, Angle::Degrees );
  Longitude longitude_ss( 0.0, Angle::Degrees );  
  Distance radius_ss = shape_t->localRadius( latitude_ss, longitude_ss );
  EXPECT_TRUE( radius_ss.isValid() );

  Eigen::Vector3d sunpos_subsolar_t{ longitude_ss.degrees(), 
                                     latitude_ss.degrees(), 
                                     radius_ss.kilometers() };
  Eigen::Vector3d sunpos_t = psmrts::lonlatrad_to_xyz_d( sunpos_subsolar_t ) * 100.0;

  std::vector<double> sunpos_s = eigen_to_vector( sunpos_t );

  EXPECT_NEAR( shape_t->incidenceAngle( sunpos_s ), 67.711942871523064, tolerance_d );
  EXPECT_NEAR( shape_t->phaseAngle( observer_s, sunpos_s ), 63.440058191359142, tolerance_d );

  std::vector<double> sun_lookdir_s = eigen_to_vector( point_t - sunpos_t );
  EXPECT_TRUE( shape_t->isVisibleFrom( sunpos_s, sun_lookdir_s ) );
}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitSpheroid ) {

  const double tolerance_u   = 1.0e-9;
  const double tolerance_d   = 1.0e-9;
  const double tolerance_km  = 1.0e-9;

  const QString bennu_t( "ellipsoid::0.283065,0.271215" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;
  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( psmrts_t, nullptr );

  const psmrts::PsmrtsPriorityTracer &priority_t = psmrts_t->tracer();
  ASSERT_EQ( priority_t.size(), 1 );
  
  EXPECT_STREQ( priority_t.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0]->model().c_str(), "ellipsoid" );
  EXPECT_EQ( priority_t.tracers()[0]->name(),  bennu_t.toStdString() );

  // Run some shapemodel tests
  ShapeModel *shape_t = camera_t->target()->shape();
  ASSERT_NE( shape_t, nullptr );
  EXPECT_FALSE( shape_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "PSMRTS" );

  // Set observer location
  Latitude latitude(45.0, Angle::Degrees );
  Longitude longitude(50.0, Angle::Degrees );
  Distance radius = shape_t->localRadius( latitude, longitude );
  EXPECT_TRUE( radius.isValid() );
  EXPECT_NEAR( radius.kilometers(), 0.27695004401143336, tolerance_km );

  SurfacePoint surfpt( latitude, longitude, radius );
  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  EXPECT_NEAR( point_t.norm(), 0.27695004401143336, tolerance_km );

  std::vector<double> observer_s = eigen_to_vector( point_t.normalized() * 1.5 );
  EXPECT_TRUE( shape_t->intersectSurface( latitude, longitude, observer_s ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  const auto &trace_s = psmrts_t->get_shape_trace();
  Eigen::Vector3d ray_normal_t = trace_s.trace().normal();

  EXPECT_NEAR( ray_normal_t[0], 0.43469710823117308, tolerance_u );
  EXPECT_NEAR( ray_normal_t[1], 0.51805184042481456, tolerance_u );
  EXPECT_NEAR( ray_normal_t[2], 0.73665508532006896, tolerance_u );

  SurfacePoint *spoint_t = shape_t->surfaceIntersection();
  EXPECT_TRUE( spoint_t->Valid() );
  EXPECT_NEAR( spoint_t->GetLatitude().degrees(), latitude.degrees(),  tolerance_d );
  EXPECT_NEAR( spoint_t->GetLongitude().degrees(), longitude.degrees(), tolerance_d );
  EXPECT_NEAR( spoint_t->GetLocalRadius().meters(), radius.meters(), tolerance_km );

  std::vector<double> normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.43469710823117308, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.51805184042481456, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.73665508532006896, tolerance_u );

  std::vector<double> local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  QVector<double *> dummy = { nullptr, nullptr, nullptr, nullptr };
  shape_t->calculateLocalNormal( dummy );
  shape_t->calculateDefaultNormal();
  Eigen::Vector3d calc_normal_t = psmrts_t->get_shape_trace().trace().normal();

  EXPECT_NEAR( calc_normal_t[0], 0.43469710823117308, tolerance_u );
  EXPECT_NEAR( calc_normal_t[1], 0.51805184042481456, tolerance_u );
  EXPECT_NEAR( calc_normal_t[2], 0.73665508532006896, tolerance_u );

  EXPECT_TRUE( shape_t->hasNormal() );
  EXPECT_TRUE( shape_t->hasLocalNormal() );

  normal_t = shape_t->normal();
  EXPECT_NEAR( normal_t[0], 0.43469710823117308, tolerance_u );
  EXPECT_NEAR( normal_t[1], 0.51805184042481456, tolerance_u );
  EXPECT_NEAR( normal_t[2], 0.73665508532006896, tolerance_u );

  local_normal_t = shape_t->localNormal();
  EXPECT_NEAR( local_normal_t[0], normal_t[0], tolerance_u );
  EXPECT_NEAR( local_normal_t[1], normal_t[1], tolerance_u );
  EXPECT_NEAR( local_normal_t[2], normal_t[2], tolerance_u );

  EXPECT_NEAR( shape_t->emissionAngle( observer_s ), 2.4472542839952514, tolerance_d );

  EXPECT_EQ( psmrts_t->plate_index(), -1 );
  
  Eigen::Vector3d observer_t = vector_to_eigen( observer_s );
  Eigen::Vector3d raypt_t  = point_t - observer_t;
  EXPECT_NEAR( raypt_t.norm(), 1.2230499559885666, tolerance_km );

  // Set sun position using sub-solar coordinates
  Latitude latitude_ss( 0.0, Angle::Degrees );
  Longitude longitude_ss( 0.0, Angle::Degrees );  
  Distance radius_ss = shape_t->localRadius( latitude_ss, longitude_ss );
  EXPECT_TRUE( radius_ss.isValid() );

  Eigen::Vector3d sunpos_subsolar_t{ longitude_ss.degrees(), 
                                     latitude_ss.degrees(), 
                                     radius_ss.kilometers() };
  Eigen::Vector3d sunpos_t = psmrts::lonlatrad_to_xyz_d( sunpos_subsolar_t ) * 100.0;

  std::vector<double> sunpos_s = eigen_to_vector( sunpos_t );

  EXPECT_NEAR( shape_t->incidenceAngle( sunpos_s ), 64.735109840451472, tolerance_d );
  EXPECT_NEAR( shape_t->phaseAngle( observer_s, sunpos_s ), 63.467526367039198, tolerance_d );

  std::vector<double> sun_lookdir_s = eigen_to_vector( point_t - sunpos_t );
  EXPECT_TRUE( shape_t->isVisibleFrom( sunpos_s, sun_lookdir_s ) );

}

/** Checks ISIS Bullet is instantiated instead of PSMRTS confirming existing functionality */
TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitIsisBullet ) {

  const double tolerance_km  = 1.0e-9;

  const QString bennu_t( "$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;

  PvlFlatMap bullet_pref;
  bullet_pref.add( "RayTraceEngine", "bullet" );
  bullet_pref.add( "Tolerance", "1.0e-6" );
  Pvl pvl = make_preferences( bullet_pref );
  QString bullet_p = write_preferences_file( "bullet.pref", pvl );

  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t, "-pref="+bullet_p };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  ShapeModel *shape_t = camera_t->target()->shape();
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "Bullet" );

  BulletShapeModel *bullet_t = dynamic_cast<BulletShapeModel *>( shape_t );
  ASSERT_NE( bullet_t, nullptr );
  EXPECT_FALSE( bullet_t->isDEM() );
  BulletTargetShape *target_b = bullet_t->model().getTarget();
  ASSERT_NE( target_b, nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,bullet_t->model().name(),  bennu_t );

  ASSERT_NE( shape_t, nullptr );
  EXPECT_FALSE( shape_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "Bullet" );

  // Set observer location
  Latitude latitude(45.0, Angle::Degrees );
  Longitude longitude(50.0, Angle::Degrees );
  Distance radius = shape_t->localRadius( latitude, longitude );
  EXPECT_TRUE( radius.isValid() );
  EXPECT_NEAR( radius.kilometers(), 0.21677833612799632, tolerance_km );

  SurfacePoint surfpt( latitude, longitude, radius );
  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  EXPECT_NEAR( point_t.norm(), 0.21677833612799632, tolerance_km );

  std::vector<double> observer_s = eigen_to_vector( point_t.normalized() * 1.5 );
  EXPECT_TRUE( shape_t->intersectSurface( latitude, longitude, observer_s ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  EXPECT_EQ( shape_t->plate_index(), 1734 );
  EXPECT_EQ( shape_t->plate_index(), bullet_t->plate_index() );
}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitWithConfFile ) {

  const QString bennu_t( "$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;

  PvlFlatMap bullet_pref;
  bullet_pref.add( "RayTraceEngine", "bullet" );
  bullet_pref.add( "Tolerance", "1.0e-6" );
  bullet_pref.add( "ShapeModel", bennu_t );
  Pvl pvl = make_preferences( bullet_pref, "Kernels" );
  QString bullet_p = write_preferences_file( "bullet.conf", pvl );

  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bullet_p };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( psmrts_t, nullptr );
  EXPECT_FALSE( psmrts_t->isDEM() );

  auto flat_t = extract_pvl_group( *cube_t->label(), "Kernels" );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, flat_t.get("ShapeModel"), bennu_t );
}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitIsisNaifDsk ) {

  const double tolerance_km  = 1.0e-9;

  const QString bennu_t( "$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;

  PvlFlatMap bullet_pref;
  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  ShapeModel *shape_t = camera_t->target()->shape();
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, shape_t->name(), "DSK" );

  NaifDskShape *naifdsk_t = dynamic_cast<NaifDskShape *>( shape_t );
  ASSERT_NE( naifdsk_t, nullptr );
  EXPECT_FALSE( naifdsk_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, naifdsk_t->model().filename(),  bennu_t );

  // Set observer location
  Latitude latitude(45.0, Angle::Degrees );
  Longitude longitude(50.0, Angle::Degrees );
  Distance radius = shape_t->localRadius( latitude, longitude );
  EXPECT_TRUE( radius.isValid() );
  EXPECT_NEAR( radius.kilometers(), 0.21677833612776354, tolerance_km );

  SurfacePoint surfpt( latitude, longitude, radius );
  EXPECT_TRUE( surfpt.Valid() );
  Eigen::Vector3d point_t;
  surfpt.ToNaifArray( point_t.data() );
  EXPECT_NEAR( point_t.norm(), 0.21677833612776354, tolerance_km );

  std::vector<double> observer_s = eigen_to_vector( point_t.normalized() * 1.5 );
  EXPECT_TRUE( shape_t->intersectSurface( latitude, longitude, observer_s ) );
  EXPECT_TRUE( shape_t->hasIntersection() );
  EXPECT_EQ( shape_t->plate_index(), 1734 );
  EXPECT_EQ( shape_t->plate_index(), naifdsk_t->plate_index() );  
}


TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitPriorityTest ) {

  std::vector<QString> bennu_list = { 
    "# This loads a regional shape using NAIF, a global with Bullet and an ellipsoid",
    "naifdsk::$osirisrex/kernels/dsk/bennu_l_00050mm_alt_dtm_1148n05547_v021.bds",
    "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds",
    "ellipsoid::0.283065,0.271215,0.249720" 
  };
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  QString from;
  std::unique_ptr<Cube> cube_t;
  QString shapelist = write_list_file( "bennu_shapes.lis", bennu_list );

  try {
    from = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + shapelist };
    cube_t.reset( run_spiceinit( from, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( cube_t.get(),     nullptr );
  Camera *camera_t = cube_t->camera();

  ASSERT_NE( camera_t->target(), nullptr );
  ASSERT_NE( camera_t->target()->shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( psmrts_t, nullptr );

  const psmrts::PsmrtsPriorityTracer &priority_t = psmrts_t->tracer();
  ASSERT_EQ( priority_t.size(), 3 );
  
  EXPECT_STREQ( priority_t.tracers()[0]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[1]->type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[2]->type().c_str(),  "tracer" );

  EXPECT_STREQ( priority_t.tracers()[0]->model().c_str(), "naifdsk" );
  EXPECT_STREQ( priority_t.tracers()[1]->model().c_str(), "bullet" );
  EXPECT_STREQ( priority_t.tracers()[2]->model().c_str(), "ellipsoid" );

  // EXPECT_EQ( priority_t.tracers()[0]->name(),  bennu_list[1].toStdString() );
  EXPECT_EQ( priority_t.tracers()[1]->name(),  bennu_list[2].toStdString() );
  EXPECT_EQ( priority_t.tracers()[2]->name(),  bennu_list[3].toStdString() );

  // Lets trace the center pixel and see which tracer we get
  EXPECT_TRUE( camera_t->SetImage( 512.0, 512.0 ) );
  auto ray_sl = psmrts_t->get_shape_trace();
  EXPECT_TRUE( ray_sl.hasHit() );
  EXPECT_TRUE( ray_sl.isValid() );
  auto tracer_at_intercept = psmrts_t->tracer().get_tracer( ray_sl.trace() );
  EXPECT_EQ( tracer_at_intercept->model(), "bullet" );

  // Now go the other way
  Eigen::Vector3d llr = psmrts::xyz_to_lonlatrad_d (ray_sl.trace().xyz() );
  EXPECT_TRUE( camera_t->SetUniversalGround( llr[1], llr[0] ) );
  auto ray_llr = psmrts_t->get_shape_trace();
  EXPECT_TRUE( ray_llr.hasHit() );
  EXPECT_TRUE( ray_llr.isValid() );
  auto tracer_at_intercept_llr = psmrts_t->tracer().get_tracer( ray_llr.trace() );
  EXPECT_EQ( tracer_at_intercept_llr->model(), "bullet" );
  EXPECT_TRUE( ray_sl.trace().isNear( ray_llr.trace(), 0.00001 ) );

  EXPECT_NEAR( camera_t->Sample(), 512.0, 0.00001 );
  EXPECT_NEAR( camera_t->Line(), 512.0, 0.00001 );
}


TEST_F(PsmrtsSpiceinit, PsmrtsTwoWayCamptComparison ) {

  const QString bennu_t( "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  const double tolerance_large = 1.0e-4;
  const double tolerance_small = 1.0e-8;
  const double tolerance_sl    = 1.0e-5;

  QString from_t;

  try {
    std::unique_ptr<Cube> cube_t;

    from_t = run_ocams2isis( ocams_f );
    XmlParameters args_t = { "shape=user", "model=" + bennu_t };
    cube_t.reset( run_spiceinit( from_t, args_t ) );
  }
  catch ( const IException &e ) {
    FAIL() << "OCAMS import/spiceinit failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  // Create the enhanced camera point info 
  EnhancedCameraPointInfo enhanced_t;
  try {
    enhanced_t.SetCube( from_t );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of enhanced camera point infos failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( enhanced_t.shape(), nullptr );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, enhanced_t.shape()->name(), "PSMRTS" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( enhanced_t.shape() );
  ASSERT_NE( psmrts_t, nullptr );

  // Check values at the center pixel
  std::unique_ptr<PvlGroup> campt_c( enhanced_t.SetCenter() );
  ASSERT_NE( campt_c, nullptr );

  const PvlGroup cpt = *campt_c;
  EXPECT_NEAR( toDouble( cpt["Sample"] ), 512.0, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["Line"] ), 512.0, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["PixelValue"] ), 0.014208084, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["RightAscension"] ), 159.36649322592, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["Declination"] ), -37.119791880649, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["PlanetocentricLatitude"] ), -1.208550453232, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["PlanetographicLatitude"] ), -1.5527036106704, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["PositiveEast360Longitude"] ), 33.591471940917, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["PositiveEast180Longitude"] ), 33.591471940917, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["PositiveWest360Longitude"] ), 326.40852805908, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["PositiveWest180Longitude"] ), -33.591471940917, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["BodyFixedCoordinate"][0] ), 0.22745066819249, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["BodyFixedCoordinate"][1] ), 0.1510690689204, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["BodyFixedCoordinate"][2] ), -0.0057603239468131, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["LocalRadius"] ), 273.10959590915 , tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SampleResolution"] ), 0.047211549557682, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LineResolution"] ), 0.047211549557682, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["ObliqueDetectorResolution"] ), 0.055862387332971, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["ObliquePixelResolution"] ), 0.055862387332971, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["ObliqueLineResolution"] ), 0.055862387332971, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["ObliqueSampleResolution"] ), 0.055862387332971, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["SpacecraftPosition"][0] ), 2.4508427760406, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SpacecraftPosition"][1] ), 1.6808462152007, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SpacecraftPosition"][2] ), -2.2173509771884, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["SpacecraftAzimuth"] ), 287.63411249488, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SlantDistance"] ), 3.4892440155467, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["TargetCenterDistance"] ), 3.7078996302575, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SubSpacecraftLatitude"] ), -36.727312596629, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SubSpacecraftLongitude"] ), 34.443255564474, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SpacecraftAltitude"] ), 3.4669529356865, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["OffNadirAngle"] ), -3.2144548089471, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SubSpacecraftGroundAzimuth"] ), 178.82511288584, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["SunPosition"][0] ), 70633378.109504, 1.0e-3 );
  EXPECT_NEAR( toDouble( cpt["SunPosition"][1] ), 142837223.62645, 1.0e-3 );
  EXPECT_NEAR( toDouble( cpt["SunPosition"][2] ), 6078864.0202888, tolerance_large );

  EXPECT_NEAR( toDouble( cpt["SubSolarAzimuth"] ), 16.681494836056, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SolarDistance"] ), 1.0659453789984, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SubSolarLatitude"] ), 2.1846905951309, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SubSolarLongitude"] ), 63.687484875237, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SubSolarGroundAzimuth"] ), 83.584029101401, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["Phase"] ),49.397794783906, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["Incidence"] ), 23.337863628169, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["Emission"] ), 32.313039060303, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["NorthAzimuth"] ), 106.26700905481, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["EphemerisTime"] ), 607075493.45972, tolerance_small );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cpt["UTC"], "2019-03-28T20:03:44.274074" );
  EXPECT_NEAR( toDouble( cpt["LocalSolarTime"] ), 9.993599137712, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["SolarLongitude"] ), 62.759024139382, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["LookDirectionBodyFixed"][0] ), -0.63721313211158, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LookDirectionBodyFixed"][1] ), -0.43842653006332, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LookDirectionBodyFixed"][2] ),  0.63383089385198, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["LookDirectionJ2000"][0] ), -0.74622675831374, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LookDirectionJ2000"][1] ),  0.28098635895935, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LookDirectionJ2000"][2] ), -0.60348346394523, tolerance_small );

  EXPECT_NEAR( toDouble( cpt["LookDirectionCamera"][0] ), -1.35313080826016e-05, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LookDirectionCamera"][1] ), -1.35306343101369e-05, tolerance_small );
  EXPECT_NEAR( toDouble( cpt["LookDirectionCamera"][2] ),  0.99999999981691, tolerance_small );

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

  auto isLatLonGood = []( const PvlFlatMap &map_f ) ->bool {
    return ( !map_f.isNull("PlanetocentricLatitude" ) && !map_f.isNull( "PositiveEast360Longitude") );
  };

  auto getLatLon = []( const PvlFlatMap &map_f ) ->std::tuple<double, double> {
    double lat = toDouble( map_f.get( "PlanetocentricLatitude" ) );
    double lon = toDouble( map_f.get( "PositiveEast360Longitude" ) );
    return ( std::make_tuple( lat, lon ) );
  };  

  QStringList sensitive_keys = { "line", "sample", "spacecraftazimuth" };
  double samples = enhanced_t.camera()->Samples();
  double lines   = enhanced_t.camera()->Lines();

  size_t n_tested = 0;
  for ( const double &line : make_vector( 12.0, lines-12, 10 ) ) {
    for ( const double &samp : make_vector( 12.0, samples-12, 10 ) ) {
      SCOPED_TRACE("Error in sample,line = (" + qt_to_string( toString( samp ) ) + ", " + qt_to_string( toString( line ) ) + ")" );
  
      std::unique_ptr<PvlGroup> points_p( enhanced_t.SetImage( samp, line ) );
      PvlFlatMap flat_p( *points_p );

      int plate_index_p = psmrts_t->plate_index();

      // Now check if the point maps back to the samp, line
      ASSERT_TRUE( isLatLonGood( flat_p ) );
      auto [ lat, lon ] = getLatLon( flat_p );
      EXPECT_TRUE( enhanced_t.camera()->SetUniversalGround( lat, lon ) );
      EXPECT_NEAR( enhanced_t.camera()->Sample(), samp, tolerance_sl );
      EXPECT_NEAR( enhanced_t.camera()->Line(),   line, tolerance_sl );

      std::unique_ptr<PvlGroup> points_d( enhanced_t.SetGround( lat, lon ) );
      PvlFlatMap flat_d( *points_d );

      int plate_index_d = psmrts_t->plate_index();
      EXPECT_EQ( plate_index_p, plate_index_d );

      EXPECT_EQ( flat_p.size(), flat_d.size() );
      for ( const auto &key : flat_p.keys() ) {
        SCOPED_TRACE("Error in key " + qt_to_string( key ) );
        EXPECT_FALSE( flat_p.isNull( key ) );
        EXPECT_FALSE( flat_d.isNull( key ) );
        double tolerance = sensitive_keys.contains( key ) ? tolerance_large : tolerance_small;
        for ( int i = 0 ; i < flat_p.count( key ) ; i++ ) {
          n_tested++;
          if ( flat_p.get( key, i) != flat_d.get( key, i ) ) {
            try {
              EXPECT_NEAR( toDouble( flat_p.get( key, i) ), 
                           toDouble( flat_d.get( key, i) ), 
                           tolerance );
            }
            catch ( IException &e ) {
              EXPECT_PRED_FORMAT2(AssertQStringsEqual, flat_p.get( key, i), flat_d.get( key, i) );
            }
          }
        }
      }
    }
  }
  EXPECT_NE( n_tested, 0 );
}

