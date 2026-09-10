/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "PsmrtsShapeModel.h"

#include <numeric>
#include <utility>

#include <QtGlobal>
#include <QVector>

#include "IException.h"
#include "NaifStatus.h"
#include "SpecialPixel.h"
#include "Spice.h"
#include "SurfacePoint.h"
#include "Target.h"


using namespace std;

namespace Isis {


  inline std::string bool_s( const bool torf ) {
    return ( ( torf == true ) ? "true" : "false" );
  }

  /** Generic constructor sets type to a TIN */
  PsmrtsShapeModel::PsmrtsShapeModel() : 
                                     ShapeModel(),
                                     m_parameters(),
                                     m_shape_ray_t(),
                                     m_latlon_ray_t(),
                                     m_tracer( "isis" ),
                                     m_tolerance( DefaultDistanceTolerance ),
                                     m_psmrts_debug( false ) {
    // defaults for ShapeModel parent class include:
    //     name = empty string
    //     surfacePoint = null sp
    //     hasIntersection = false
    //     hasNormal = false
    //     normal = (0,0,0)
    //     hasEllipsoidIntersection = false
    setName("PSMRTS");
    clearSurfacePoint();
  }


  /**
   * @brief Constructor provided for instantiation from an ISIS cube
   *
   * This constructor is typically used for and ISIS cube that has been
   * initialized by spiceinit.  The DEM name should be that of a NAIF DSK file.
   * This constructor will throw an exception if it fails to open the DSK file.
   * 
   * The "parameters" can provided content that overrides shape model
   * preferences in a user specified (or the default state) preferences files
   * such as IsisPreferences. If there is no content in "parameters", it will
   * load ISIS preferences and apply the contents otherwise it will not load
   * preferencs and overide values in the preferecnes state.
   *
   * @author 2026-05-10 Kris Becker
   *
   * @param target     Target object describing the observed body
   * @param pvl        ISIS Cube label.  Extract the name of the DEM from the 
   *                   Kernels group
   * @param parameters Override IsisPreferences if this parameter has content.
   */
  PsmrtsShapeModel::PsmrtsShapeModel( Target *target, Pvl &pvl,
                                      const PvlFlatMap &parameters ) : 
                                      ShapeModel(target),
                                      m_parameters(),
                                      m_shape_ray_t(),
                                      m_latlon_ray_t(),
                                      m_tracer( ),
                                      m_tolerance( DefaultDistanceTolerance ),
                                      m_psmrts_debug( false ) {

    // Debug preferences

    PvlFlatMap psmrts_p = parameters;
    if ( psmrts_p.size() == 0 ) psmrts_p = get_shapemodel_preferences();

    m_psmrts_debug = toBool( psmrts_p.get( "PsmrtsDebug", "false" ) );
    // m_psmrts_debug = toBool( psmrts_p.get( "PsmrtsDebug", "true" ) );

    if ( m_psmrts_debug ) std::cout << "ISIS/PSMRTS Constructor running..." << std::endl;

    setName("PSMRTS");
    std::string name_t = qt_to_string( get_file_name( pvl, "isis_label" ) );
    auto tracer_s = psmrts::PsmrtsTracerSystem( name_t, 
                                                create_isis_path_translator( "isisdata" ) );

    PvlGroup &kernels = pvl.findGroup("Kernels", Pvl::Traverse);
    PvlFlatMap kmap_t = PvlFlatMap( kernels );
    
    // Load the shape model preferences and override labels if specifed
    bool pref_shapemodel_override = toBool( psmrts_p.get("PsmrtsShapeModelOverride", "false" ) );
    if ( true == pref_shapemodel_override ) {
      kmap_t.merge( psmrts_p );
    }

    if ( kmap_t.exists("ElevationModel") ) {
      m_parameters.add( keyword_vector( "ShapeModel", kmap_t.allValues( "ElevationModel") ) );
    }
    else { // if (kernels.hasKeyword("ShapeModel")) {
      m_parameters.add( keyword_vector( "ShapeModel", kmap_t.allValues( "ShapeModel") ) );
    }

    try {
      if ( m_parameters.count( "ShapeModel") == 1 ) {
        QString fext = FileName( m_parameters.get( "ShapeModel" ) ).extension();
        if ( ( "txt" ==  fext ) || ( "lis" == fext ) ) {
          (void) load_shape_list( m_parameters.get("ShapeModel" ), psmrts_p );
        }
        else if ( ( "conf" ==  fext ) || ( "pvl" == fext ) ) {
          (void) load_pvl_config( m_parameters.get("ShapeModel" ), psmrts_p );
        }
        else {
          psmrts_p.add( m_parameters.keyword( "ShapeModel" ) );
        }
      }
      else {
        psmrts_p.merge( m_parameters );
      }

    }
    catch ( const IException &ie ) {
      QString mess = "Failed to load shape file(s) or invalid ShapeModel (" +
                      m_parameters.get( "ShapeModel", "") +
                     ") for cube " + QString::fromStdString( name_t );
      throw IException( ie, IException::User, mess, _FILEINFO_ );      
    }


    // Convert the shape model list to a std:vector<std::string> list
    std::vector<std::string> v_shapefiles;
    for ( const QString &s : psmrts_p.allValues( "ShapeModel") ) {

      // Don't support/add ISIS DEMs!
      const std::string file_s = qt_to_string( s );
      if ( psmrts::psmrts_file_extension( file_s ) == "cub" ) {
        QString mess = "PSMRTS does not currenly support ISIS 2.5 DEMs (" +
                        s + "),in ISIS cube " + QString::fromStdString( name_t );
       throw IException( IException::User, mess, _FILEINFO_ );      
      }

      // Add the file to the list
      v_shapefiles.push_back( file_s );
    }

    // Attempt to initialize the DSK file - exception ensues if errors occur
    // error thrown if ShapeModel=Null (i.e. Ellipsoid)
    tracer_s = psmrts::PsmrtsTracerSystem( name_t, v_shapefiles, tracer_s.translations( ) );
    NaifStatus::CheckErrors();

    // Add the ellipsoid as defined in target
    std::vector<double> radii;
    for ( const auto &radius : this->targetRadii() ) {
      if ( radius.isValid() ) {
        radii.push_back( radius.kilometers() );
      }
    }

    // If it is defined set up the reference ellipsoid
    if ( radii.size() > 0 ) {
      tracer_s.set_reference_ellipsoid( qt_to_string( target->systemName() ), 
                                        radii );
                                         
    }

    // Check for tolerance amd apply if given found
    QString tolerance_s = kmap_t.get( "Tolerance", "" );
    if ( tolerance_s.size() == 0 ) {
      tolerance_s = psmrts_p.get( "Tolerance", tolerance_s );
    }

    // Apply it!
    if ( "" !=  tolerance_s )  {
      m_tolerance = toDouble( tolerance_s );
      psmrts_p.add( "Tolerance", tolerance_s );
    }


    //Check for errors and throw if any occur
    if ( tracer_s.error_count() > 0 ) {
      tracer_s.throw_errors();
    }

    // Now extract the priority tracer which allows discard of the
    // PsmrtsTracerSystem
    m_tracer = tracer_s.create_priority_tracer();

    // Update the labels with PSMRTS data/info
    psmrts_p.add( "RayTraceEngine", "psmrts" );
    m_parameters = psmrts_p;
    if ( !psmrtsUpdateIsisLabel( pvl, psmrts_p ) ) {
      QString mess = "Failed to update ISIS labels in cube " + QString::fromStdString( name_t );
      throw IException( IException::User, mess, _FILEINFO_ );
    }

    // Set up for tracing
    clearSurfacePoint();
    if ( m_psmrts_debug) std::cout << "PsmrtsShapeModel constructor done!" << std::endl;
  }


/**
   * @brief Constructor for creating new shape model from the same DSK file
   *
   * This constructor provides the ability to create a formal shape model from the
   * NAIF DSK plate model file already opened. This approach allows multiple
   * threads to access the same DSK file interface without the overhead of opening
   * many instances of the same file.
   *
   * @author 2014-02-12 Kris Becker
   *
   * @param model DSK plate model from an existing NaifDskPlateModel (see the
   *              model() method
   */
  PsmrtsShapeModel::PsmrtsShapeModel(const psmrts::PsmrtsTracerSystem &tracer_s,
                                     const PvlFlatMap &parameters ) :
                                     ShapeModel(),
                                     m_parameters( parameters ),
                                     m_shape_ray_t(),
                                     m_latlon_ray_t(),
                                     m_tracer( tracer_s.get_shape_tracer() ),
                                     m_tolerance( DefaultDistanceTolerance ),
                                     m_psmrts_debug( false ) {

    setName("PSMRTS");
    m_psmrts_debug = toBool( m_parameters.get( "PsmrtsDebug", "false" ) );
    clearSurfacePoint();
  }

  /**
   * @brief Constructor for creating new shape model from the same DSK file
   *
   * This constructor provides the ability to create a formal shape model from the
   * NAIF DSK plate model file already opened. This approach allows multiple
   * threads to access the same DSK file interface without the overhead of opening
   * many instances of the same file.
   *
   * @author 2014-02-12 Kris Becker
   *
   * @param model DSK plate model from an existing NaifDskPlateModel (see the
   *              model() method
   */
  PsmrtsShapeModel::PsmrtsShapeModel(const psmrts::PsmrtsPriorityTracer &tracer_t,
                                     const PvlFlatMap &parameters ) :
                                     ShapeModel(),
                                     m_parameters( parameters ),
                                     m_shape_ray_t(),
                                     m_latlon_ray_t(),
                                     m_tracer( tracer_t ),
                                     m_tolerance( DefaultDistanceTolerance ),
                                     m_psmrts_debug( false ) {

    setName("PSMRTS");
    m_psmrts_debug = toBool( m_parameters.get( "PsmrtsDebug", "false" ) );
    clearSurfacePoint();
  }


  /** Destructor - cleanup is handled automagically */
  PsmrtsShapeModel::~PsmrtsShapeModel() {
    if ( this->isDebug() ) {
      std::cout << "\nPsmrtsShapeModel() shutting down!" << std::endl;
      std::cout << "RunTime (s):    " << m_shape_ray_t.tracker().runtime_s() << std::endl;
      std::cout << "ShapeRayCount:  " << m_shape_ray_t.tracker().count() << std::endl;
      std::cout << "LatLotRayCount: " << m_latlon_ray_t.tracker().count() << std::endl;
      for ( const auto &tracer : m_tracer.tracers() ) {
        auto uid = tracer.uid();
        std::cout << "\nTracerUid:   " << uid << std::endl;
        std::cout << "Name:        " << tracer.name() << std::endl;
        std::cout << "Type:        " << tracer.type() << std::endl;
        std::cout << "Model:       " << tracer.model() << std::endl;
        auto tracker_t = tracer.product().timestamp();
        std::cout << "TraceCount:  " << tracker_t.count() << std::endl;
        std::cout << "RunTime (s): " << tracker_t.runtime_s() << std::endl;
      }      
    }
  }


  /**
   * @brief Compute a DEM intersection from and observer and look direction
   *
   * This method computes a DEM intercept point given an observer location and
   * direction vector in body fixed coordinates.  This method provides true ray
   * intercept techiques as implemented by NAIF's DSK API.
   *
   * If the intercept is successful, its state is retain in this class for further
   * application.
   *
   * @author 2026-03-12 Kris Becker
   *
   * @param observerPos    Position of observer in body fixed coordiates
   * @param lookDirection  Look direction (ray) from the observer
   *
   * @return bool Returns true if an intercept was successful, false otherwise
   */
  bool PsmrtsShapeModel::intersectSurface(std::vector<double> observerPos,
                                          std::vector<double> lookDirection) {

    Eigen::Vector3d observer_t( observerPos.data() ); 
    Eigen::Vector3d lookdir_t( lookDirection.data() ); 

    m_shape_ray_t.set_trace( observer_t,lookdir_t );
    m_tracer.process( m_shape_ray_t );

    return ( this->updateTraceState( m_shape_ray_t ) );
  }


  /**
  * @brief Compute surface intersection with optional occlusion check
  *
  * This method sets the surface point at the given latitude, longitude. The
  * derived model is called to get the radius at that location to complete the
  * accuracy of the surface point, them the derived method is called to complete
  * the intersection.
  *
  * @author 2026-03-03 Kris J. Becker
  *
  * @param surfpt       Absolute point on the surface to check
  * @param observerPos  Position of the observer
  * @param backCheck    Flag to indicate occlusion check
  *
  * @return bool        True if the intersection point is valid (visable)
  */
  bool PsmrtsShapeModel::intersectSurface(const Latitude &lat, const Longitude &lon,
                                          const std::vector<double> &observerPos,
                                          const bool &backCheck ) {

    bool status = getSurfaceLatLonRadius( lat, lon, 
                                          m_tracer,
                                          m_shape_ray_t );

    // If successful, set observer and calculate lookdir
    if ( true == status ) {
      // Save off the surfpt intercept point in case backCheck is needed
      auto ray_t( std::move( m_shape_ray_t.trace() ) );
      auto &datum_r = m_shape_ray_t.trace().datum();

      Eigen::Vector3d observer_t( observerPos.data() );
      Eigen::Vector3d lookdir_t = datum_r.m_xyz - observer_t;

      datum_r.m_observer = observer_t;
      datum_r.m_lookdir  = lookdir_t;

      // Check for occlusions
      if ( true == backCheck ) {
        if ( emission_angle_isvalid( datum_r.m_normal, -datum_r.m_lookdir ) ) {
          if ( true == m_tracer.process( m_shape_ray_t ) ) {
            if ( !m_shape_ray_t.trace().isNear( ray_t, get_tolerance() ) ) {
              m_shape_ray_t.set_trace( observer_t, lookdir_t );
            }
          }
        }
        else {
          // Its not visible from the observer
          datum_r.m_hit = false;
        }
      }
    }

    // Update the shape trace state
    return ( this->updateTraceState( m_shape_ray_t ) );
  }

  /**
   * @brief Intersect a surface point from an observer position
   * 
   * This is a two phase intercept operation. First an intersection of a vector
   * from the body origin through the body-fixed latitude/longitude coordinate
   * is computed. The look direction from observerPos is computed of the true
   * actual surface point. A second trace is computed from this oberver/look
   * direction. If requested, occlusion is tested if backCheck == true. 
   * 
   * @param surfpt      Surface lat/lon point to compute look direction
   * @param observerPos Position of the observer
   * @param backCheck   If true, an occlusion trace is applied
   * @return true       True if all intersects succeed
   * @return false      False in any intercept fails
   */
  bool PsmrtsShapeModel::intersectSurface(const SurfacePoint &surfpt,
                                          const std::vector<double> &observerPos,
                                          const bool &backCheck ) {

    // Initialize/compute ground intercept
    bool status = getSurfaceLatLonRadius( surfpt.GetLatitude(), 
                                          surfpt.GetLongitude(), 
                                          m_tracer,
                                          m_shape_ray_t );

    if ( true == status ) {
      // Save off the surfpt intercept point in case backCheck is needed
      auto ray_t( std::move( m_shape_ray_t.trace() ) );  
      auto &datum_r = m_shape_ray_t.trace().datum();
      
      Eigen::Vector3d observer_t( observerPos.data() );
      Eigen::Vector3d lookdir_t = datum_r.m_xyz - observer_t;

      datum_r.m_observer = observer_t;
      datum_r.m_lookdir  = lookdir_t;
        
      // Check for occlusion if requested
      if ( true == backCheck ) {
        if ( emission_angle_isvalid( datum_r.m_normal, -datum_r.m_lookdir ) ) {
          if ( true == m_tracer.process( m_shape_ray_t ) ) {
            if ( !m_shape_ray_t.trace().isNear( ray_t, get_tolerance() ) ) {
              m_shape_ray_t.set_trace( observer_t, lookdir_t );
            }
          }
        }
        else {
          // Its not visible from the observer
          datum_r.m_hit = false;          
        }
      }
    }

    return ( this->updateTraceState( m_shape_ray_t ) );
  }                                            


  /**
   * @brief Calculate the normal from the ellipsoid intercept
   * 
   * If the local normal is not set or the ellipsoid has no surface intercept
   * an exception is thrown since it is based off the precomputed surface
   * intercept.
   * 
   * If the normal is not set the the ellipsoid normal is set.
   * 
   */
  void PsmrtsShapeModel::calculateDefaultlNormal() {
    // Check if none are set
    if ( !hasNormal() && !m_shape_ray_t.hasHit() ) {
      QString mess = "Intercept point does not exist - cannot provide normal vector";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }
    
    // Set the normal if not set
    if ( m_shape_ray_t.hasHit() ) {
      Eigen::Vector3d norm_t = m_shape_ray_t.trace().normal();
      setNormal(norm_t[0], norm_t[1], norm_t[2]); // this also takes care of setHasNormal(true);
    }
    
    return;      
  }


  /**
   * @brief Compute the normal for a local region of surface points
   *
   * This method will calculate the surface normal of an assumed very local
   * region of points.  This method is provided to fullfil the specs of the
   * ShapeModel class but this approach is not the most efficent means to
   * accomplish this for a pre-exising intercept point.  See
   * setLocalNormalFromIntercept() for this.
   *
   * The ShapeModel class makes the assumption that the four pixel corners of the
   * center intercept point forms a plane from which a surface normal can be
   * computed.  For the Naif DSK plate model, we have already identified the plate
   * (see m_intercept) from the DSK plate model (m_model) of the intercept point
   * that provides it directly.  That is what setLocalNormalFromIntercept()
   * provides.
   *
   * So, this implementation will compute the centroid of the neighboring points
   * and make a determination if it intercepts the current intercept plate as
   * defined by m_intercept - if it is valid.  If it does not exist or does not
   * intercept the plate, a new intercept point is computed and returned here.
   *
   * @author 2026-03-14 Kris Becker
   *
   * @param neighborPoints Input body-fixed points to compute normal for
   */
  void PsmrtsShapeModel::calculateLocalNormal(QVector<double *> neighborPoints) {
    // Sanity check
    if ( !( hasIntersection() && m_shape_ray_t.hasHit() ) ) { // hasIntersection()  <==>  !m_intercept.isNull()
      QString mess = "Intercept point does not exist - cannot provide normal vector";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }

    setLocalNormalFromIntercept();
    return;
  }
 

  /**
   * @brief Set the local normal vector to the intercept point normal
   *
   * This method will reassign the ShapeModel normal to the current intecept point
   * shape (which is a triangular plate) normal.  If an intercept point is not
   * defined, an error will ensue.
   *
   * @author 2026-03-14 Kris Becker
   */
  void PsmrtsShapeModel::setLocalNormalFromIntercept()  {

    // Sanity check
    if ( !( hasIntersection() && m_shape_ray_t.hasHit() ) ) { // hasIntersection()  <==>  !m_intercept.isNull()
      QString mess = "Intercept point does not exist - cannot provide normal vector";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }

    // Got it, use the existing intercept point (plate) normal
    Eigen::Vector3d norm_t = m_shape_ray_t.trace().normal();
    setLocalNormal(norm_t[0], norm_t[1], norm_t[2]); // this also takes care of setHasLocalNormal(true);
    return;
  }

  
  /**
   * @brief Determine if the intercept is visiable from the position/lookdir
   * 
   * This method determines if a previously intersected surface point is visible
   * from the observer position, observerPos, and a look direction vector
   * (lookDirection). It uses the state of save trace from a previous surface
   * intersect call. If there is no save intercept this method returns false.
   * 
   * @param observerPos   Position of the observer
   * @param lookDirection Look direction vector from the observer position
   * @return true         If the point intersects the saved intercept point
   * @return false        If the point is not intersected
   */
  bool PsmrtsShapeModel::isVisibleFrom(const std::vector<double> observerPos,
                                       const std::vector<double> lookDirection) {

    // Only check for visibility if we have an intersection
    if ( m_shape_ray_t.hasHit() && hasIntersection() ) { 
      psmrts::PRQRayTrace ray_t( Eigen::Vector3d( observerPos.data() ),
                                 Eigen::Vector3d( lookDirection.data() ) );

      // Trace it to the surface for the real radius
      if ( emission_angle_isvalid( m_shape_ray_t.trace().normal(), -ray_t.trace().lookdir() ) ) {
        if ( m_tracer.process( ray_t ) ) {
          if ( m_shape_ray_t.trace().isNear( ray_t.trace(), get_tolerance() ) ) {
            return ( true );
          }
        }
      }
    }                                   

    // All other situations return false
    return ( false );
  }


  /** Reset the surface point and internal tracing data */
  void PsmrtsShapeModel::clearSurfacePoint() {
    ShapeModel::clearSurfacePoint();    
    this->reset_all_rays();
  }

  // CSMCamera uses all of these so be sure they are implemented!!
  // Calculate the emission angle of the current intersection point
  double PsmrtsShapeModel::emissionAngle(const std::vector<double> & sB) {
    
    // Check for intersect 
    if ( !m_shape_ray_t.hasHit() ) {
      return ( ShapeModel::emissionAngle( sB ) );
    }

    // Return the normal
    return ( psmrts::radians_to_degrees( m_shape_ray_t.emission() ) );
  }

  // Calculate the incidence angle of the current intersection point
  double PsmrtsShapeModel::incidenceAngle(const std::vector<double> &uB) {
    
    // Check for intersect 
    if ( !m_shape_ray_t.hasHit() ) {
      return ( ShapeModel::emissionAngle( uB ) );
    }

    // Compute the vector from the surface to uB
    Eigen::Vector3d uB_v( uB.data() );
    Eigen::Vector3d pub_v = m_shape_ray_t.trace().xyz() - uB_v;
    double inc_r = psmrts::PsmrtsRayTrace::separation_angle( m_shape_ray_t.trace().normal(), pub_v );
    return ( psmrts::radians_to_degrees( inc_r ) );
  }

  /**
   * @brief Determine DEM radius at a given lat/lon grid point
   *
   * This method computes the radius value of a point on the shape.  A vector
   * from the center of the body through the lat/lon location on the ellipsiod.
   * From this, a look direction back toward the center of the body is generated
   * and then an intercept point is determined.
   *
   * This method does not retain state of the surface trace.
   * 
   * @author 2026-03-10 Kris Becker
   *
   * @param lat Latitude coordinate of grid point
   * @param lon Longitude coordinate of grid point
   *
   * @return Distance Radius value of the intercept grid point
   */
  Distance PsmrtsShapeModel::localRadius(const Latitude &lat,
                                         const Longitude &lon) {

    // Get surface intercept at the requested location
    psmrts::PRQRayTrace ray_t;                                         
    bool status = getSurfaceLatLonRadius( lat, lon, 
                                          m_tracer, 
                                          ray_t );
    // Trace it to the surface for the real radius
    if ( true == status ) {
      return ( Distance( ray_t.trace().radius(), Distance::Kilometers ) );
    }

    // Trace failed to intersect
    return ( Distance() );
  }

  /**
   * Indicates that this shape model is not from a DEM. Since this method
   * returns false for this class, the Camera class will not calculate the
   * local normal using neighbor points.
   *
   * @return bool Indicates that this is not a DEM shape model.
   */
  bool PsmrtsShapeModel::isDEM() const {
    return false;
  }

  /**
   * @brief Returns the plate index (0-based for consistency) of the intercept
   * 
   * This method will return the plate index of the surface intercept facet
   * determined from shape tracer system. 
   * 
   * A 0-based index is returned to maintain consistency with other shape models
   * that return a 0-based index. To convert this to a real plate ID you can add
   * 1 to the value returned. 
   * 
   * Returned values greater that 0 and less or equal to the number of plates
   * minus 1 are valid numbers.
   * 
   * @return int Returns the plate index of the intercept. Returns a -1 if there
   *               is no intercept or its invalid.
   *
   */
  int PsmrtsShapeModel::plate_index() const {
    return ( this->get_shape_trace().trace().plateid() );
  }

  //----------------------------------------------------------------------------
  //  Creator methods used in generation of a PsmrtsShapeModel from different
  //  sources.
  //----------------------------------------------------------------------------

  bool PsmrtsShapeModel::load_shape_list( const QString &shapelist_f, 
                                          PvlFlatMap &flat_p ) const {
    
    // Check for *.txt or *.lis
    FileName fname( shapelist_f );
    std::string filelist = qt_to_string( fname.expanded() ); 

    try {
      std::vector<std::string> files;
      size_t nfiles = psmrts::read_list_file( filelist, files );
      if ( 0 == nfiles ) {
        QString mess = "No ShapeModel files loaded for ISIS cube from " + shapelist_f;
       throw IException( IException::User, mess, _FILEINFO_ );      
      }

      // Add content for labels
      flat_p.add( PvlKeyword( "ShapeModel", files ) );
      flat_p.add( "RayTraceEngine", "psmrts" );
    }
    catch ( const IException &ie ) {
      QString mess = "Unable to load shape list file " + shapelist_f;
      throw IException( ie, IException::User, mess, _FILEINFO_ );
    }
    catch ( const std::runtime_error &re ) {
      QString mess = "PSMRTS error reading txt/lis file: " + 
                     QString::fromStdString( filelist );
      mess += "\n" + QString( re.what() );
      throw IException( IException::User, mess, _FILEINFO_ );
    }
    return ( true );
  }


  bool PsmrtsShapeModel::load_pvl_config( const QString &pvlconf_f,
                                          PvlFlatMap &flat_p ) const {
    try {
      Pvl conf( pvlconf_f );
      PvlFlatMap conf_t( extract_pvl_group( conf, "Kernels" ) );        
      if ( !conf_t.exists( "ShapeModel" ) ) {
        return ( false );
      }
      else {
        flat_p.merge( conf_t );
      }
    }                                            
    catch ( const IException &ie ) {
      QString mess = "Unable to load PSMRTS config file: " + pvlconf_f;
      throw IException( ie, IException::User, mess, _FILEINFO_ );
    }
    return ( true );
  }
      

  bool PsmrtsShapeModel::psmrtsUpdateIsisLabel( Pvl &pvl, 
                                                const PvlFlatMap &psmrts_data ) { 

    PvlGroup &kernels = pvl.findGroup("Kernels", Pvl::Traverse);
    std::vector<QString> keys_p = { "ShapeModel", "RayTraceEngine", 
                                    "OnError",    "Tolerance" };
    for ( const auto &key : keys_p ) {
      if ( psmrts_data.exists( key ) ) {
        kernels.addKeyword( psmrts_data.keyword( key ), PvlContainer::Replace );
      }
    }

    return ( true );
  }



  /**
   * @brief Create a PSMRTS/ISIS priority tracer model
   * 
   * This constructor method creates a PSMRTS priority tracer from a shape model
   * configuration. This method supports single and multiple shape models files
   * of various formats as well as mathematical models such as ellipsoids,
   * spheroids and spheres.
   * 
   * This static constructor is intended to be called from ShapeModeFactory s it
   * is tailored for that environment regarding configuration and error
   * management.
   * 
   * The "pvl" parameter is expected to originate from an ISIS label that
   * contains a Kernels PVL group with a ShapeModel keyword. This method should
   * be used when various new use cases are encountered:
   * 
   *   - More than one shape model file is contained in the ShapeModel keyword
   *   - A single file with extentions *.lis, *.txt and *.conf are contained in
   *     the ShapeModel keyword.
   *   - An explicit request to use the PSMRTS is specified in the
   *     RayTraceEngine keyword either in the label or specified in the
   *     IsisPreferences file in the ShapeModel group.
   *   
   * These cases are detected in this method and processed with the PSMRTS
   * system and a priority tracer is created and returned as a PsmrtsShapeModel.
   * If after evaluation for the conditions described above, it is determined
   * that none of these use cases are specified, a nullptr is returned. This
   * would indicate preexisting ISIS ShapeModelFactory behavior should be
   * applied. However, if an error has occured, an exception is thrown.
   * 
   * For the case where more than one shape model files exists in the ShapeModel
   * keyword, each file is assumed to be a DSK, OBJ or PLY file. Each of these
   * files are supported by PSMRTS and a priority tracer will be created where
   * the default priority is dictated by the order in which they occur in the
   * ShapeModel keyword. In addition, a reference ellipsoid is created from the
   * Target parameter and remains separate in the PSMRTS system. This processing
   * technique is also applied from the results of processing the other text
   * file cases. 
   * 
   * For the case of file with the extension "txt" or "lis", the contents are
   * expected to contain a shape model file of the supported type, one per line.
   * The file may contain comments that start with with "#" and are ignored. 
   * 
   * A file with the extension "conf" is a PVL formatted file that contains a
   * ShapeModel keyword, with other PSMRTS based options, such as Tolerance and
   * RayTraceEngine and others that may vary over time. To explicitly use
   * PSMRTS, the RayTraceEngine = PSMRTS must exist in the CONF file. See the
   * PSMRTS documentation for available options. Use of PSMRTS is implied if
   * more than one file exists in the ShapeModel keyword regardless of other
   * parameters configurations.
   * 
   * Several options/formats of individual shape model file specifications is
   * supported. Each file can have the extended formmatted form:
   * 
   *  tracer::$envpath/kernels/dsk/shape.ext
   * 
   *   where "tracer" is one of "bullet", "naifdsk", or "ellipsoid";
   *          "envpath" preceded with a $ is an environment variable or
   *          parameter from the ISIS DataDirectory group in the
   *          IsisPreferences file; 
   *          followed by the remainder of the full path.
   * 
   * In the case of the "ellipsoid", after the "::" 1, 2 or 3 double values
   * separated by commas are expected. This value will look like:
   * 
   *  ellipsoid::0.283065,0.271215,0.249720  
   * 
   * 
   * @param target       ISIS Target object for the body
   * @param pvl          ISIS cube label to be updated
   * @param throw_errors Throw exceptions if errors occur. Only if the ray trace
   *                       engine is not PSMRTS will this be honored.
   * @return ShapeModel* Pointer to a new PsmrtsShapeModel on valid construction
   */
   PsmrtsShapeModel *PsmrtsShapeModel::create(Target *target, Pvl &pvl,
                                              const bool throw_errors ) {

    PsmrtsShapeModel *model_t( nullptr );

    // Load the shape model preferences
    PvlFlatMap preferences_t = get_shapemodel_preferences();
    bool pref_shapemodel_override = toBool( preferences_t.get("PsmrtsShapeModelOverride", "false" ) );
    bool psmrts_debug = toBool( preferences_t.get("PsmrtsDebug", "false" ) );

    // First check the RayTraceEngine setting in the ISIS configuration file.
    // If it has anything other than RayTraceEngine = "psmrts", return a NULL
    // pointer and let ShapeModelFactory continue on.
    bool psmrts_requested = false;
    PvlFlatMap kernels = extract_pvl_group( pvl, "Kernels" );
    if ( !kernels.exists( "RayTraceEngine" ) || ( true == pref_shapemodel_override ) ) {
      kernels = preferences_t;
    }

    // Check for PSMRTS engine and allow PSMRT override mode if requested!
    // Note this allows for changing of the ShapeModel using a preferences file.
    // This provides a runtime, per-program swap out of shape model and ray
    // tracing engine.
    if ( kernels.exists( "RayTraceEngine" ) ) {
      if ( kernels.get( "RayTraceEngine" ).toLower() != "psmrts" ) {
        if ( true == pref_shapemodel_override ) {
          PsmrtsShapeModel::psmrtsUpdateIsisLabel( pvl, preferences_t );
        }
        return ( nullptr );
      }
      psmrts_requested = true;
    }
    else {
      if ( requires_psmrts( pvl ) == false ) {
        return  ( nullptr );
      }
    }

    try {
      // Pvl label will be updated upon success
      model_t = new PsmrtsShapeModel( target, pvl, preferences_t );
    }
    catch ( const std::runtime_error &re ) {
      if ( psmrts_debug ) std::cout << "Did not get a PSMRTS model!" << std::endl;
      // Catch PSMRTS exceptions first
      if ( psmrts_requested || throw_errors ) {
        QString mess = "PsmrtsShapeModel::create() error occured: " + QString( re.what() );
        throw IException( IException::User, mess, _FILEINFO_ );
      }
      return ( nullptr );
    }
    catch ( const IException &ie ) {
      if ( psmrts_debug ) std::cout << "Did not get a PSMRTS model!" << std::endl;
      // ISIS exceptions
      if ( psmrts_requested || throw_errors ) {
        QString mess = "ISIS/PSMRTS shape model error occured in PsmrtsShapeModel";
        throw IException( ie, IException::User, mess, _FILEINFO_ );
      }      
      return ( nullptr );
    }

    auto print_vector = [&] ( const std::string &tag, auto &v ) { 
      std::cout << tag;
      for ( const auto &v_t : v ) {
        std::cout << " " << v_t.uid();
      }
      std::cout << std::endl;
    };

    // Success! Return the shape model.
    if ( model_t->isDebug() || pref_shapemodel_override ) {
      std::cout << "PsmrtsShapeModel::create() done!" << std::endl;
      std::cout << "FactoryShapeCount:  " << psmrts::PsmrtsFactory().shapes().size() << std::endl;
      std::cout << "FactoryTracerCount: " << psmrts::PsmrtsFactory().tracers().size() << std::endl;
      print_vector( "TracerIds: ", model_t->tracer().tracers() );
      for ( const auto &tracer : model_t->tracer().tracers() ) {
        auto uid = tracer.uid();
        std::cout << "\nTracerUid: " << uid << std::endl;
        std::cout << "Name:      " << tracer.name() << std::endl;
        std::cout << "Type:      " << tracer.type() << std::endl;
        std::cout << "Model:     " << tracer.model() << std::endl;
        std::cout << "Config:    " << tracer.config().to_json().dump(-1) << std::endl;
        std::cout << "Tracer:    " << model_t->tracer().inventory().find( uid ).name() << std::endl << std::endl;
      }
    }

    return ( model_t );
  }


  /**
   * @brief Determine if the contents of the kernels group requires PSMRTS
   * 
   * This method will determine if the contents of ISIS labels require PSMRTS
   * shape models. The conditions that require PSMRST is if the keyword
   * RayTraceEngine = "psmrts". Also if there is more than one shape model file
   * in the ShapeModel keyword or the single file contains a "lis" or "txt"
   * extension. These conditions will return true which definitively requires
   * PSMRTS shape models.
   * 
   * @param pvl    ISIS labels containing a Kernels group that has been created
   *                  by spiceinit.
   * @return true  If PSMRTS is required to create the ShapeModel tracer system
   * @return false If PSMRTS is not required
   */
  bool PsmrtsShapeModel::requires_psmrts( const Pvl &pvl ) {
    PvlFlatMap kernels = extract_pvl_group( pvl, "Kernels" );

    // Check for PSMRTS ray trace engine
    if ( kernels.get( "RayTraceEngine", "none" ).toLower() == "psmrts") {
      return ( true );
    }

    // Check for PSMRTS file name requirements. If the ShapeModel keyword only
    // contains one filename, check its format
    if ( kernels.count( "ShapeModel") == 1 ) {
      QString fname = kernels.get( "ShapeModel" );

      // Does the filename contain "::" indicating unique PSMRT formatting
      if ( fname.contains( "::" ) ) {
        return ( true );
      }

      //Check for lists of shape models
      QString fext = FileName( fname ).extension();
      if ( ( "txt" ==  fext ) || ( "lis" == fext ) || 
           ( "conf" == fext ) ) {
        return ( true );
      }

      // Its just a regular shape model name format.
      return ( false );
    }

    // There is more that one shape model which requires PSMRTS
    return ( true );
  }


  /**
   * @brief Update the internal state with the result of a ray trace 
   * 
   * This method updates the internal state of this shape model object with
   * the results of the ray trace object. If the trace has a hit the surface
   * point is updated with the surface point and normal data.
   * 
   * If the is no hit or an error occured, then the surface point is cleared.
   * 
   * Both shape and ellipsoid normals cannot be set unless the intecept point is
   * set. However, if both intercept, this method sets the shape ray intercept
   * point and not the ellipsoid. This may cause confusion.
   * 
   * The state of the result is returned to the caller.
   * 
   * @param ray   Shape model ray trace object to set 
   * @param ray_e Ellipsoid ray trace object to set 
   */
  bool PsmrtsShapeModel::updateTraceState( const psmrts::PRQRayTrace &ray_s ) {

    clearSurfacePoint();

    if ( ray_s.hasHit() ) {
      setHasIntersection( ray_s.hasHit() ); 
      SurfacePoint point;
      point.FromNaifArray( ray_s.trace().xyz().data() );
      ShapeModel::setSurfacePoint( point );

      // Got the local normal so set it here
      Eigen::Vector3d normal_l = ray_s.trace().normal();
      setLocalNormal( normal_l[0], normal_l[1], normal_l[2] );
      setNormal( normal_l[0], normal_l[1], normal_l[2] );      
    }

    return ( ray_s.hasHit() );
  }


} // namespace Isis 
