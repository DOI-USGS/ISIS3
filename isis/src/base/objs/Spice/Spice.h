#ifndef Spice_h
#define Spice_h
/**
 * @file
 * $Revision: 1.21 $
 * $Date: 2010/04/09 22:31:16 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
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
#include "Pvl.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

namespace Isis {
/**
 * @brief Obtain SPICE information for a spacecraft
 *
 * This class initializes standard NAIF SPICE kernels in order to allow queries
 * of a spacecraft's position and attitude at a given time. It also allows for
 * access to the position of the sun and transformation matrices from J2000 to a
 * body-fixed reference frame for a given target (e.g., Mars). The constructor
 * for this class expects a PVL object with the following minimum information:
 *   @code
 *     Group = Instrument
 *       TargetName = Mars
 *     EndGroup
 *     Group = Kernels
 *       NaifFrameCode       = -94030
 *       LeapSecond          = naif0007.tls
 *       TargetAttitudeShape = pck00006.tpc
 *       TargetPosition      = de405.bsp
 *       InstrumentPointing  = (mgs_sc_ab1.bc,
 *       Instrument          = moc13.ti
 *       SpacecraftClock     = MGS_SCLKSCET.00045.tsc
 *       InstrumentPosition  = mgs_ab1.bsp
 *       InstrumentAddendum  = mocAddendum.ti
 *     EndGroup
 *   @endcode
 * This group is typically found in the image labels after it has been run
 * through the program "spiceinit" It is recommended you read NAIF documentation
 * to obtain a better understanding about the various types of SPICE kernels.
 * The NAIF toolkit accesses information from kernels on a
 * last-in-first-out (LIFO) basis. This means that the creation of a second
 * object can cause problems with the first object. To alleviate this problem we
 * have supplied the CreateCache method which should be invoked immediately
 * after the object is constructed. This caches information (spacecraft
 * position, pointing, etc) internally in the object and unloads all NAIF
 * kernels.
 *
 * @ingroup SpiceInstrumentsAndCameras
 *
 * @author 2003-03-13 Jeff Anderson
 *
 * @internal
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
 *                                     isis.astrogeology...
 *  @history 2003-10-15 Jeff Anderson - Added requirement for frame kernel in
 *                                      labels
 *  @history 2003-10-28 Jeff Anderson - Changed SpaceCraft to Spacecraft in
 *                                      labels and method names
 *  @history 2003-11-03 Jeff Anderson - Added SubSolarPoint and
 *                                      SubSpacecraftPoint methods
 *  @history 2003-11-12 Jeff Anderson - Added Target method
 *  @history 2004-01-14 Jeff Anderson - Changed how the SPK, CK, and Instrument
 *                                      codes where handled. The instrument code
 *                                      must be in the labels as NaifFrameCode
 *                                      and then the other two can be
 *                                      automatically computed.
 *  @history 2004-02-18 Jeff Anderson - Modified to ignore kernel labels which
 *                                      were blank
 *  @history 2004-03-25 Jeff Anderson - Modified NaifBodyCode method to convert
 *                                      Jupiter target code of 599 to 5
 *  @history 2004-03-25 Jeff Anderson - Fixed bug in destructor and added
 *                                      GetString method.
 *  @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
 *                                          documentation
 *  @history 2005-02-24 Jeff Anderson - Modified SubSolarPoint and
 *                                      SubSpacecraftPoint to return positive
 *                                      longitudes only
 *  @history 2005-09-12 Jeff Anderson - Check for case-insensitive values for
 *					TargetName of SKY
 *  @history 2005-09-20 Jeff Anderson - Added IsSky method
 *  @history 2006-01-05 Debbie A. Cook - Added units to comments
 *  @history 2006-03-28 Jeff Anderson - Refactored using SpiceRotation and
 *                                      SpicePosition classes.  Added Tables
 *                                      and nadir kernel information.
 *  @history 2006-03-31 Elizabeth Miller - Added TargetCenterDistance method
 *  @history 2006-04-19 Elizabeth Miller - Added SolarLongitude method
 *  @history 2007-01-30 Tracie Sucharski - Throw error in the load method before
 *                              calling furnish if the file does not exist.
 *  @history 2007-07-09 Steven Lambright - Frame kernel is now optional, added Extra kernel
 *                              support.
 *  @history 2007-07-10 Debbie A. Cook - Modified method ComputeSolarLongitude to use
 *                                       pxform instead of tipbod to get body-fixed to
 *                                       J2000 rotation matrix so that the correct frame
 *                                       will be used.  If the frame is different from the
 *                                       default IAU frame, the correct frame should be set
 *                                       in the iak file (see frames.req).  Also modified
 *                                       setting of p_bodyRotation frameCode.  The old code
 *                                       forced the IAU_ frame.  The new code uses the Naif
 *                                       routine cidfrm to get the frame associated with the
 *                                       body id.  These change will recognize any frame 
 *                                       changes made in the iak file.
 *  @history 2007-08-10 Steven Lambright - Added support for Nadir keyword in InstrumentPointing group 
 *                                       to not be the first element in the PvlKeyword.
 *  @history 2007-08-24 Debbie A. Cook - Removed p_sB so it is recalculated every time it is used
 *                                       insuring that any updates to the position or rotation are applied.
 *                                       Also removed p_BP since it is no longer used
 *  @history 2008-02-13 Steven Lambright - Added StartPadding and EndPadding caching capabilties
 *  @history 2008-02-13 Steven Lambright - Added Support Check for StartPadding and EndPadding caching capabilties;
 *                                        An clarified exception is thrown if a framing camera tries to use time padding
 *  @history 2008-02-27 Kris Becker - Modified so that planetary ephemeris SPKs
 *                                    are loaded before spacecraft SPKs so that
 *                                    missions that augment planet ephemerides
 *                                    will take precidence.
 *  @history 2008-06-23 Steven Lambright - Added NaifStatus error checking
 *  @history 2008-06-25 Debbie A. Cook - Added method InstrumentVelocity to support miniRF
 *  @history 2008-11-28 Debbie A. Cook - Added method HasKernels()
 *  @history 2009-03-18 Tracie Sucharski - Cleaned up some unnecessary,obsolete code.  Make sure the
 *                                    table is used if the kernel names follow the "Table" keyword value, due to change
 *                                    made to spiceinit to retain kernel names if the spice is written to blob.
 *  @history 2009-06-18 Debbie A. Cook - Modified to downsize instrument rotation table when loading cache
 *  @history 2009-07-01 Debbie A. Cook - Modified to downsize body rotation, and sun position tables when loading cache
 *  @history 2009-08-03 Debbie A. Cook - Added tolerance argument to method
 *                                       CreateCache to allow downsizing of
 *                                       instrument position Spice table.
 *  @history 2009-08-21 Kris Becker - Moved the NAIF code methods to public
 *                                    scope.
 *  @history 2010-01-29 Debbie A. Cook - Redid Tracie's change to make sure the table is loaded instead of the 
 *                                        kernels if the kernel keyword value lists "Table" before the kernel files.
 *  @history 2010-03-19 Debbie A. Cook - Added constructor and moved common constructor initialization into
 *                                        new method Init.  Also added parameter notab to method Load.
 *  @history 2010-04-09 Debbie A. Cook - Moved the loading of the "extra" kernel(s) from the middle of
 *                                       the loads to the end.
 *                                    
 *                                    
 */
  class Spice {
    public:
      // constructors
      Spice (Isis::Pvl &lab);
      Spice (Isis::Pvl &lab, bool notab);

      // destructor
      ~Spice ();

      // Methods
      void Init( Isis::Pvl &lab, bool notab);
      void SetEphemerisTime (const double time);
      void InstrumentPosition (double p[3]) const;
      void SunPosition (double p[3]) const;
      double TargetCenterDistance() const;
      double SolarLongitude();
      void InstrumentVelocity( double v[3] ) const;
      //! Sets flag to allow downsizing of kernels
//      void AllowDownsizing( bool allow) const { p_allowDownsizing = allow; };

     /**
      * Returns the ephemeris time in seconds which was used to obtain the
      * spacecraft and sun positions.
      *
      * @return double
      */
      inline double EphemerisTime () const { return p_et; };

      void Radii (double r[3]) const;

      void CreateCache (const double startTime, const double endTime,
                        const int size, double tol);
      void CreateCache (const double time, double tol);
      inline double CacheStartTime () const { return p_startTime; };
      inline double CacheEndTime () const { return p_endTime; };

      void SubSpacecraftPoint (double &lat, double &lon);
      void SubSolarPoint (double &lat, double &lon);

     /**
      * Returns the string name of the target
      *
      * @return string
      */
      std::string Target () const { return p_target; };

      //! Return if our target is the sky
      bool IsSky () const { return p_sky; };

      static SpiceDouble GetDouble (const std::string &key, int index=0);
      static SpiceInt GetInteger (const std::string &key, int index=0);
      static std::string GetString (const std::string &key, int index=0);

      SpicePosition *SunPosition() const { return p_sunPosition; };
      SpicePosition *InstrumentPosition() const { return p_instrumentPosition; };
      SpiceRotation *BodyRotation() const { return p_bodyRotation; };
      SpiceRotation *InstrumentRotation() const { return p_instrumentRotation; };

      bool HasKernels(Isis::Pvl &lab);

      SpiceInt NaifBodyCode () const;
      SpiceInt NaifSpkCode () const;
      SpiceInt NaifCkCode () const;
      SpiceInt NaifIkCode () const;
      SpiceInt NaifSclkCode () const;

    protected:
      // Leave these protected so that inheriting classes don't
      // have to convert between double and spicedouble
      // None of the below data elements are usable (except
      // p_radii) until SetEphemerisTime is invoked
      SpiceDouble p_uB[3];    /**<This contains the sun position (u) in the
                                  bodyfixed reference frame (B). It is left
                                  protected so that conversions between double
                                  and SpiceDouble do not have to occur in
                                  inheriting classes. Units are km */
      SpiceDouble p_BJ[3][3]; /**<This contains the transformation matrix from
                                  J2000 (J) to Body fixed (B). Recall that the
                                  transpose of this matrix JB will convert from
                                  body-fixed to J2000. It is left in protected
                                  space so that conversions between double and
                                  SpiceDouble do not have to occur in inheriting
                                  classes.*/
      SpiceDouble p_radii[3]; //!<The radii of the target in kilometers


    private:
      void Load(Isis::PvlKeyword &key, bool notab);
      void ComputeSolarLongitude(double et);

      SpiceDouble p_solarLongitude;
      SpiceDouble p_et;
      std::vector<std::string> p_kernels;
      std::string p_target;

      // cache stuff
      SpiceDouble p_startTime;
      SpiceDouble p_endTime;
      SpiceDouble p_cacheSize;

      SpiceDouble p_startTimePadding;
      SpiceDouble p_endTimePadding;

      SpicePosition *p_instrumentPosition;
      SpiceRotation *p_instrumentRotation;
      SpicePosition *p_sunPosition;
      SpiceRotation *p_bodyRotation;

      bool p_keepKernelsLoaded;
      bool p_allowDownsizing;

      // Constants
      SpiceInt p_bodyCode;
      SpiceInt p_spkCode;
      SpiceInt p_ckCode;
      SpiceInt p_ikCode;
      SpiceInt p_sclkCode;
      SpiceInt p_spkBodyCode;

      bool p_sky;
  };
};

#endif
