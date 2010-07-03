#ifndef LineScanCameraRotation_h
#define LineScanCameraRotation_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/12/29 23:03:47 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#include "SpiceRotation.h"
#include "SpicePosition.h"
#include "Table.h"
#include "Quaternion.h"
#include "PolynomialUnivariate.h"
#include "PixelOffset.h"
#include "Spice.h"


namespace Isis {
  /**
   * @brief Obtain SPICE rotation information for a body
   *
   * This class will obtain the rotation from J2000 to the ICR frame (with axes
   * (defined in direction of in track, cross track, and radial) for HiRise.
   *
   * It is essentially used to convert position vectors from one frame to
   * another, making it is a C++ wrapper to the NAIF routines pxform_c and
   * mxv or mtxv.  Therefore, appropriate NAIF kernels are expected to be
   * loaded prior to using this class.  A position can be returned in either
   * the J2000 frame or the selected reference frame.  See NAIF required
   * reading for more information regarding this subject at
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/ascii/individual_docs/spk.req
   * <p>
   * An important functionality of this class is the ability to cache the
   * rotations so they do not have to be constantly read from the NAIF kernels
   * and they can be more conveniently updated.  Once the data is cached, the
   * NAIF kernels can be unloaded.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2005-12-01 Debbie A. Cook
   *
   * @internal
   *  @history 2005-12-01  Debbie A. Cook Original Version modified from
   *  SpicePosition class by Jeff Anderson
   *  @history 2006-03-23  Jeff Anderson modified SetEphemerisTime to return
   *                       if the time did not change to improve speed.
   *  @history 2006-10-18  Debbie A. Cook Added method, WrapAngle, to wrap
   *                        angles around 2 pi
   *  @history 2007-12-05  Debbie A. Cook added method SetPolynomialDegree to 
   *                        allow the degree of the polynomials fit to the 
   *                        camera angles to be changed.  Also changed the
   *                        polynomial from a fixed 2nd order polynomial to
   *                        an nth degree polynomial with one independent
   *                        variable.  PartialType was revised and the calls to
   *                        SetReferencePartial (has an added argument, coefficient index)
   *                        and DPolynomial (argument type changed to int) were revised.
   *                        The function was changed from Parabola
   *                        to Polynomial1Variable, now called
   *                        PolynomialUnivariate. New methods GetBaseTime
   *                        and SetOverrideBaseTime were added
   *  @history 2008-02-15  Debbie A. Cook added a new error message to handle the
   *                        case where the Naif reference frame code is not
   *                        recognized.
   *  @history 2008-06-18  Fixed documentation, added NaifStatus calls
   *  @history 2008-08-11  Debbie A. Cook Added method to set axes of rotation.
   *                        Default axes are still 3,1,3 so existing software will
   *                        not be affected by the change.
   *  @history 2008-12-12  Debbie A. Cook Added parameters for updated pitch rate and yaw
   *                        and related methods
   *  @history 2009-07-31  Debbie A. Cook Added new argument, tol, for call to CreateCache
   *                        method of Spice class
   *  @history 2009-10-01  Debbie A. Cook Modified methods to be compatible with changes made
   *                        to parent class, SpiceRotation, to separate rotation into a constant
   *                        rotation and a time-dependent rotation
   */
  class LineScanCameraRotation : public Isis::SpiceRotation {
    public:
      //! Constructors
//      LineScanCameraRotation( int frameCode, SpiceRotation *crot, SpiceRotation *prot, SpicePosition *spos );
      LineScanCameraRotation( int frameCode,  Isis::Pvl &lab, std::vector<double> timeCache, double tol);

      //! Destructor
//      virtual ~LineScanCameraRotation() { };
      virtual ~LineScanCameraRotation();

      void LoadCache();
      void SetJitter( PixelOffset *jitter) { p_jitter = jitter; };
      void ReloadCache();
      void ResetPitchRate( double pitchRate ) { p_pitchRate = pitchRate; };
      void ResetYaw( double yaw ) { p_yaw = yaw; };

    private:
      Isis::Spice *p_spi;
      SpiceRotation *p_crot;                              //!< Camera rotation [CJ]
      SpicePosition *p_spos;                              //!< Spacecraft position in J2000
      SpiceRotation *p_prot;                              //!< Planet rotation [PJ]
      std::vector<std::vector<double> > p_cacheIB;        //!< Cached rotations body-fixed to ICR
      PixelOffset *p_jitter;                              //!< Jitter rotations from nominal camera to truth (jittering camera)
      bool p_cachesLoaded;                                //!< Flag indicated p_cache and p_cacheIB are loaded
      double p_pitchRate;                                 //!< Optional update to pitch rate
      double p_yaw;                                       //!< Optional update to yaw
  };
};

#endif

