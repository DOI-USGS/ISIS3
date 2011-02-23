#ifndef SpiceRotation_h
#define SpiceRotation_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
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
#include "Table.h"
#include "Quaternion.h"
#include "PolynomialUnivariate.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#define J2000Code    1

namespace Isis {
  /**
   * @brief Obtain SPICE rotation information for a body
   *
   * This class will obtain the rotation from J2000 to a particular reference
   * frame, for example the rotation from J2000 to MOC NA.
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
   * NAIF kernels can be unloaded.  If the rotation has a fixed part and a time-
   * based part, the rotation is computed and stored in those two parts.
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
   *  @history 2008-11-26  Debbie A. Cook Added method to set axes of rotation.
   *                        Default axes are still 3,1,3 so existing software will
   *                        not be affected by the change.  Also added timeScale to the
   *                        the class and made some parameters protected instead of private
   *                        so they are available to inheriting classes.
   *  @history 2008-12-12  Debbie A. Cook Added method to return frame code
   *  @history 2009-01-26  Debbie A. Cook Added wrap of 3rd camera angle when crossing +-180
   *  @history 2009-04-21  Debbie A. Cook Added methods MinimizeCache and LoadTimeCache, variable p_minimizeCache, and
   *                        enum constants DownsizeStatus
   *  @history 2009-06-29  Debbie A. Cook Fixed memory overwrite problem in LoadTimeCache when reading a type 3 ck
   *  @history 2009-07-24  Debbie A. Cook Removed downsizing for Nadir instrument pointing tables (LoadTimeCache) so that
   *                        radar instruments will work.  Current downsizing code requires sclk and radar has no sclk.
   *  @history 2009-10-01  Debbie A. Cook Divided the rotation into a constant (in time) part and a time-based part and
   *                        added keywords listing the frame chains for both the constant part and the time-based part.
   *  @history 2009-10-09  Debbie A. Cook Added angular velocity when it is available
   *  @history 2009-10-30  Modified J2000Vector and ReferenceVector to work on either length 3 vectors (position only)
   *                        or lenght 6 vectors (position and velocity) and added private method StateTJ()
   *  @history 2009-12-03  Debbie A. Cook Modified tests in LoadTimeCache to allow observation to cross segment boundary
   *                        for LRO
   *  @history 2010-03-19  Debbie A. Cook Revised ReloadCache including removing obsolete arguments.  Added
   *                        initialization of members p_fullCacheStartTime, p_fullCacheEndTime, and p_fullCacheSize.  Added these
   *                        same values to the table label in method Cache and the reading of these values to the method
   *                        LoadCache(table).  Improved error message in FrameTrace.  Also corrected a comment in StateTJ
   *  @history 2010-09-23  Debbie A. Cook Revised to write out line cache for updated pointing when cache size is 1. If the
   *                        original pointing had an angular velocity in this case, the original angular velocity is written
   *                        out along with the updated quaternion.  Also added method Extrapolate, to extrapolate pointing
   *                        assuming a constant angular velocity.  This method was designed to compute the pointing at the
   *                        start and end of the exposure for framing cameras to create a ck that would cover a single framing
   *                        observation.
   *  @history 2010-12-22  Debbie A. Cook  Added new method SetFullCacheParameters to upgrade appjit to current instrument
   *                        Rotation group labels.
   *  @history 2011-02-17  Debbie A. Cook  Fixed bug in method LineCache and fixed computation of angular velocity in
   *                        method DCJdt (derivative was with respect to scaled et instead of et)
   *  @history 2011-02-22 Debbie A. Cook - Corrected Extrapolation method
   *  @todo Downsize using Hermite cubic spline and allow Nadir tables to be downsized again.
   */
  class SpiceRotation {
    public:
      //! Constructors
      SpiceRotation(int frameCode);
      /*      SpiceRotation( int NaifCode );
      We would like to call refchg instead to avoid the strings.  Currently Naif does
      not have refchg_c, but only the f2c'd refchg.c.*/
      SpiceRotation(int frameCode, int targetCode);

      //! Destructor
      virtual ~SpiceRotation() { }

      //! Change the frame (has no effect if cached)
      void SetFrame(int frameCode) {
        p_constantFrames[0] = frameCode;
      };
      int Frame() {
        return p_constantFrames[0];
      };

      void SetTimeBias(double timeBias);
      /**
       * The rotation can come from one of 3 places for an Isis cube:  Cache,
       * Naif Spice kernels, or Nadir computations.
       */
      enum Source { Spice, Nadir, Memcache, Function};

      enum PartialType {WRT_RightAscension, WRT_Declination, WRT_Twist};

      enum DownsizeStatus {Yes, Done, No};
      enum NaifFrameType { INERTL = 1, PCK = INERTL + 1, CK = PCK + 1, TK = CK + 1, DYN = TK + 1};

      void SetEphemerisTime(double et);

      //! Return the current ephemeris time
      double EphemerisTime() const {
        return p_et;
      };

      std::vector<double> Matrix();
      std::vector<double> AngularVelocity() {
        return p_av;
      };
      std::vector<double> &ConstantMatrix() {
        return p_TC;
      };
      std::vector<double> &TimeBasedMatrix() {
        return p_CJ;
      };

      std::vector<double> J2000Vector(const std::vector<double>& rVec);

      std::vector<double> ReferenceVector(const std::vector<double>& jVec);

      //! Set the downsize status
      void MinimizeCache(DownsizeStatus status) {
        p_minimizeCache = status;
      };

      void LoadCache(double startTime, double endTime, int size);

      void LoadCache(double time);

      void LoadCache(Table &table);

      Table LineCache(const std::string &tableName);

      void ReloadCache();

      Table Cache(const std::string &tableName);
      void CacheLabel(Table &table);

      void LoadTimeCache();

      std::vector<double> Angles(int axis3, int axis2, int axis1);

      //! Is this rotation cached
      bool IsCached() const {
        return (p_cache.size() > 0);
      };

      void SetPolynomial();

      void SetPolynomial(const std::vector<double>& abcAng1,
                         const std::vector<double>& abcAng2,
                         const std::vector<double>& abcAng3);

      void GetPolynomial(std::vector<double>& abcAng1,
                         std::vector<double>& abcAng2,
                         std::vector<double>& abcAng3);

      //! Set the polynomial degree
      void SetPolynomialDegree(int degree);

      //! Return the source of the rotation
      Source GetSource() {
        return p_source;
      };

      //! Resets the source of the rotation
      void SetSource(Source source) {
        p_source = source;
        return;
      };

      void ComputeBaseTime();

      //! Return the base time for the rotation
      double GetBaseTime() {
        return p_baseTime;
      };

      //! Return the time scale for the rotation
      double GetTimeScale() {
        return p_timeScale;
      };

      void SetOverrideBaseTime(double baseTime, double timeScale);

      double DPolynomial(const int coeffIndex);

      std::vector<double> ToReferencePartial(std::vector<double>& lookJ,
                                             PartialType partialVar, int coeffIndex);
      double WrapAngle(double compareAngle, double angle);
      void SetAxes(int axis1, int axis2, int axis3);
      std::vector<double> GetFullCacheTime();
      void FrameTrace(double et);

      //! Return the frame chain for the constant part of the rotation (ends in target)
      std::vector<int>  ConstantFrameChain() {
        return p_constantFrames;
      };

      //! Return the frame chain for the rotation (begins in J2000)
      std::vector<int>  TimeFrameChain() {
        return p_timeFrames;
      };

      void InitConstantRotation(double et);
      std::vector<double> ConstantRotation();
      std::vector<double> TimeBasedRotation();
      void DCJdt(std::vector<double> &dRJ);

      //! Return whether or not the rotation has angular velocities
      bool HasAngularVelocity() {
        return p_hasAngularVelocity;
      };

      void ComputeAv();
      std::vector<double> Extrapolate(double timeEt);


    protected:
      void SetFullCacheParameters(double startTime, double endTime, int cacheSize);
      std::vector<double> p_cacheTime;  //!< iTime for corresponding rotation
      std::vector<std::vector<double> > p_cache;      //!< Cached rotations
      //!< Stored as rotation matrix from
      //    J2000 to 1st constant frame (CJ) or
      //    coefficients of polynomial
      //    fit to rotation angles
      int p_degree;                     //!< Degree of fit polynomial for angles
      int p_axis1;                      //!< Axis of rotation for angle 1 of rotation
      int p_axis2;                      //!< Axis of rotation for angle 2 of rotation
      int p_axis3;                      //!< Axis of rotation for angle 3 of rotation

    private:
      std::vector<int> p_constantFrames;  //!< Chain of Naif frame codes in constant rotation TC
      //    The first entry will always be the target frame code
      std::vector<int> p_timeFrames;      //!< Chain of Naif frame codes in time-based rotation CJ
      //    The last entry will always be 1 (J2000 code)
      double p_timeBias;                  //!< iTime bias when reading kernels

      double p_et;                        //!< Current ephemeris time
      Quaternion p_quaternion;            //!< Quaternion for J2000 to reference
      //   rotation at et

      bool p_matrixSet;                   //!< Flag indicating p_TJ has been set

      Source p_source;                    //!< The source of the rotation data
      int p_axisP;                        //!< The axis defined by the spacecraft
      //   vector for defining a nadir rotation
      int p_axisV;                        //!< The axis defined by the velocity
      //   vector for defining a nadir rotation
      int p_targetCode;                   //!< For computing Nadir rotation only

      double p_baseTime;                  //!< Base time used in fit equations
      double p_timeScale;                 //!< Time scale used in fit equations
      bool p_degreeApplied;               //!< Flag indicating whether or not a polynomial
      //    of degree p_degree has been created and
      //    used to fill the cache
      std::vector<double> p_coefficients[3];  //!< Coefficients defining functions fit to 3 pointing angles
      bool p_noOverride;                  //!< Flag to compute base time;
      double p_overrideBaseTime;          //!< Value set by caller to override computed base time
      double p_overrideTimeScale;         //!< Value set by caller to override computed time scale
      DownsizeStatus p_minimizeCache;     //!< Status of downsizing the cache (set to No to ignore)
      double p_fullCacheStartTime;        //!< Initial requested starting time of cache
      double p_fullCacheEndTime;          //!< Initial requested ending time of cache
      int p_fullCacheSize;                //!< Initial requested cache size
      std::vector<double> p_TC;           //!< Rotation matrix from first constant rotation (after all
      //    time-based rotations in frame chain from J2000 to target)
      //    to the target frame
      std::vector<double> p_CJ;           //!< Rotation matrix from J2000 to first constant rotation
      //    after all the time-based rotations in frame chain from
      std::vector<std::vector<double> > p_cacheAv;
      //!< Cached angular velocities for corresponding rotactions in p_cache
      std::vector<double> p_av;           //!< Angular velocity for rotation at time p_et
      bool p_hasAngularVelocity;          //!< Flag indicating whether the rotation includes angular velocity
      std::vector<double> StateTJ();      //!< State matrix (6x6) for rotating state vectors from J2000 to target frame
  };
};

#endif

