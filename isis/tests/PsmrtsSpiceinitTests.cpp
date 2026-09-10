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

    /** Create a Pvl Preferences file with a ShapeModel group */
    inline Pvl make_preferences( const PvlFlatMap &parameters ) const {
      Pvl pvl;
      if ( parameters.size() > 0 ) {
        PvlGroup prefs( "ShapeModel" );
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


TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitBullet ) {

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
  
  EXPECT_STREQ( priority_t.tracers()[0].type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0].model().c_str(), "bullet" );
  EXPECT_EQ( priority_t.tracers()[0].name(),  bennu_t);

}


TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitNaifDsk ) {

  const QString bennu_t( "naifdsk::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
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
  
  EXPECT_STREQ( priority_t.tracers()[0].type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0].model().c_str(), "naifdsk" );
  // EXPECT_EQ( priority_t.tracers()[0].name(),  bennu_t.toStdString() );

}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitEllipsoid ) {

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
  
  EXPECT_STREQ( priority_t.tracers()[0].type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0].model().c_str(), "ellipsoid" );
  EXPECT_EQ( priority_t.tracers()[0].name(),  bennu_t.toStdString() );

}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitSpheroid ) {

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
  
  EXPECT_STREQ( priority_t.tracers()[0].type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[0].model().c_str(), "ellipsoid" );
  EXPECT_EQ( priority_t.tracers()[0].name(),  bennu_t.toStdString() );

}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitIsisBullet ) {

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
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "Bullet" );

  BulletShapeModel *bullet_t = dynamic_cast<BulletShapeModel *>( camera_t->target()->shape() );
  ASSERT_NE( bullet_t, nullptr );
  EXPECT_FALSE( bullet_t->isDEM() );
  BulletTargetShape *target_b = bullet_t->model().getTarget();
  ASSERT_NE( target_b, nullptr );
  // EXPECT_PRED_FORMAT2(AssertQStringsEqual,target_b->name(),  bennu_t );
}

TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitIsisNaifDsk ) {

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
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, camera_t->target()->shape()->name(), "DSK" );

  NaifDskShape *naifdsk_t = dynamic_cast<NaifDskShape *>( camera_t->target()->shape() );
  ASSERT_NE( naifdsk_t, nullptr );
  EXPECT_FALSE( naifdsk_t->isDEM() );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, naifdsk_t->model().filename(),  bennu_t );
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
  
  EXPECT_STREQ( priority_t.tracers()[0].type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[1].type().c_str(),  "tracer" );
  EXPECT_STREQ( priority_t.tracers()[2].type().c_str(),  "tracer" );

  EXPECT_STREQ( priority_t.tracers()[0].model().c_str(), "naifdsk" );
  EXPECT_STREQ( priority_t.tracers()[1].model().c_str(), "bullet" );
  EXPECT_STREQ( priority_t.tracers()[2].model().c_str(), "ellipsoid" );

  // EXPECT_EQ( priority_t.tracers()[0].name(),  bennu_list[1].toStdString() );
  EXPECT_EQ( priority_t.tracers()[1].name(),  bennu_list[2].toStdString() );
  EXPECT_EQ( priority_t.tracers()[2].name(),  bennu_list[3].toStdString() );

  // Lets trace the center pixel and see which tracer we get
  EXPECT_TRUE( camera_t->SetImage( 512.0, 512.0 ) );
  auto ray_sl = psmrts_t->get_shape_trace();
  EXPECT_TRUE( ray_sl.hasHit() );
  EXPECT_TRUE( ray_sl.isValid() );
  auto tracer_at_intercept = psmrts_t->tracer().get_tracer( ray_sl.trace() );
  EXPECT_EQ( tracer_at_intercept.model(), "bullet" );

  // Now go the other way
  Eigen::Vector3d llr = psmrts::xyz_to_lonlatrad_d (ray_sl.trace().xyz() );
  EXPECT_TRUE( camera_t->SetUniversalGround( llr[1], llr[0] ) );
  auto ray_llr = psmrts_t->get_shape_trace();
  EXPECT_TRUE( ray_llr.hasHit() );
  EXPECT_TRUE( ray_llr.isValid() );
  auto tracer_at_intercept_llr = psmrts_t->tracer().get_tracer( ray_llr.trace() );
  EXPECT_EQ( tracer_at_intercept_llr.model(), "bullet" );
  EXPECT_TRUE( ray_sl.trace().isNear( ray_llr.trace(), 0.00001 ) );

  EXPECT_NEAR( camera_t->Sample(), 512.0, 0.00001 );
  EXPECT_NEAR( camera_t->Line(), 512.0, 0.00001 );
}


TEST_F(PsmrtsSpiceinit, PsmrtsSpiceinitComparison ) {

  const QString bennu_p( "bullet::$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  const QString bennu_i( "$osirisrex/kernels/dsk/bennu_g_12600mm_alt_obj_0000n00000_v021a.bds" );
  // const QString ocams_f( "data/osirisRexImages/ocams/20190223T024051S802_map_iofL2pan.fits" );
  const QString ocams_f( "data/osirisRexImages/ocams/20190328T200344S309_pol_iofL2pan.fits" );

  const double tolerance    = 1.0e-8;
  const double tolerance_sl = 1.0e-6;

  QString from_p, from_i;
  try {
    std::unique_ptr<Cube> cube_p, cube_i;

    from_p = run_ocams2isis( ocams_f );
    XmlParameters args_p = { "shape=user", "model=" + bennu_p };
    cube_p.reset( run_spiceinit( from_p, args_p ) );

    PvlFlatMap bullet_pref;
    bullet_pref.add( "RayTraceEngine", "bullet" );
    bullet_pref.add( "Tolerance", "1.0e-6" );
    Pvl pvl = make_preferences( bullet_pref );
    QString bullet_p = write_preferences_file( "bullet.pref", pvl );

    from_i = run_ocams2isis( ocams_f );
    XmlParameters args_i = { "shape=user", "model=" + bennu_i, "-pref="+bullet_p };
    cube_i.reset( run_spiceinit( from_i, args_i ) );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of OCAMS cube failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  // Create the enhanced camera point info 
  EnhancedCameraPointInfo enhanced_p, enhanced_i;
  try {
    enhanced_p.SetCube( from_p );
    enhanced_i.SetCube( from_i );
  }
  catch ( const IException &e ) {
    FAIL() << "Initialization of ehnanced camera point infos failed: " 
           << e.toString().toStdString().c_str()
           << std::endl;
  }

  ASSERT_NE( enhanced_p.shape(), nullptr );
  ASSERT_NE( enhanced_i.shape(), nullptr );

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, enhanced_p.shape()->name(), "PSMRTS" );
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, enhanced_i.shape()->name(), "Bullet" );

  PsmrtsShapeModel *psmrts_t = dynamic_cast<PsmrtsShapeModel *>( enhanced_p.shape() );
  BulletShapeModel *bullet_t = dynamic_cast<BulletShapeModel *>( enhanced_i.shape() );

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

  QStringList char_key = { "filename", "emission", "incidence", "offnadirangle" };
  double samples = enhanced_p.camera()->Samples();
  double lines   = enhanced_p.camera()->Lines();
  size_t n_tested = 0;
  for ( const double &line : make_vector( 12.0, lines-12, 10 ) ) {
    for ( const double &samp : make_vector( 12.0, samples-12, 10 ) ) {
      SCOPED_TRACE("Error in sample,line = (" + qt_to_string( toString( samp ) ) + ", " + qt_to_string( toString( line ) ) + ")" );
  
      std::unique_ptr<PvlGroup> points_p( enhanced_p.SetImage( samp, line ) );
      std::unique_ptr<PvlGroup> points_i( enhanced_i.SetImage( samp, line ) );

      EXPECT_EQ( psmrts_t->plate_index(), bullet_t->plate_index() );

      PvlFlatMap flat_p( *points_p );
      PvlFlatMap flat_i( *points_i );

      EXPECT_EQ( flat_p.size(), flat_i.size() );
      for ( const auto &key : flat_p.keys() ) {
        SCOPED_TRACE("Error in key " + qt_to_string( key ) );
        EXPECT_FALSE( flat_p.isNull( key ) );
        if ( !char_key.contains( key ) && flat_i.exists( key ) ) {
          for ( int i = 0 ; i < flat_p.count( key ) ; i++ ) {
            n_tested++;
            if ( flat_p.get( key, i) != flat_i.get( key, i ) ) {
              try {
                EXPECT_NEAR( toDouble( flat_p.get( key, i) ), 
                             toDouble( flat_i.get( key, i) ), 
                             tolerance );
              }
              catch ( IException &e ) {
                EXPECT_PRED_FORMAT2(AssertQStringsEqual, flat_p.get( key, i), flat_i.get( key, i) );
              }
            }
          }
        }
      }

      auto isLatLonGood = []( const PvlFlatMap &map_f ) ->bool {
        return ( !map_f.isNull("PlanetocentricLatitude" ) && !map_f.isNull( "PositiveEast360Longitude") );
      };

      auto getLatLon = []( const PvlFlatMap &map_f ) ->std::tuple<double, double> {
        double lat = toDouble( map_f.get( "PlanetocentricLatitude" ) );
        double lon = toDouble( map_f.get( "PositiveEast360Longitude" ) );
        return ( std::make_tuple( lat, lon ) );
      };

      // Now check if the point maps back to the samp, line
      if ( isLatLonGood( flat_p ) ) {
        auto [ lat, lon ] = getLatLon( flat_p );
        EXPECT_TRUE( enhanced_p.camera()->SetUniversalGround( lat, lon ) );
        EXPECT_NEAR( enhanced_p.camera()->Sample(), samp, tolerance_sl );
        EXPECT_NEAR( enhanced_p.camera()->Line(),   line, tolerance_sl );
      }

      if ( isLatLonGood( flat_i ) ) {
        auto [ lat, lon ] = getLatLon( flat_i );
        EXPECT_TRUE( enhanced_i.camera()->SetUniversalGround( lat, lon ) );
        EXPECT_NEAR( enhanced_i.camera()->Sample(), samp, tolerance_sl );
        EXPECT_NEAR( enhanced_i.camera()->Line(),   line, tolerance_sl );
      }
    }
  }
  EXPECT_NE( n_tested, 0 );
}

