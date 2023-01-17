#ifndef SpiceRotation_h
#define SpiceRotation_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <ale/Orientations.h>

#include "Angle.h"
#include "Table.h"
#include "PolynomialUnivariate.h"
#include "Quaternion.h"



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
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/spk.html
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
   *   @history 2005-12-01 Debbie A. Cook Original Version modified from
   *                           SpicePosition class by Jeff Anderson
   *   @history 2006-03-23 Jeff Anderson modified SetEphemerisTime to return
   *                           if the time did not change to improve speed.
   *   @history 2006-10-18 Debbie A. Cook Added method, WrapAngle, to wrap
   *                           angles around 2 pi
   *   @history 2007-12-05 Debbie A. Cook added method SetPolynomialDegree to
   *                           allow the degree of the polynomials fit to the
   *                           camera angles to be changed.  Also changed the
   *                           polynomial from a fixed 2nd order polynomial to
   *                           an nth degree polynomial with one independent
   *                           variable.  PartialType was revised and the calls
   *                           to SetReferencePartial (has an added argument,
   *                           coefficient index) and DPolynomial (argument type
   *                           changed to int) were revised. The function was
   *                           changed from Parabola to Polynomial1Variable, now
   *                           called PolynomialUnivariate. New methods
   *                           GetBaseTime and SetOverrideBaseTime were added
   *   @history 2008-02-15 Debbie A. Cook added a new error message to handle
   *                           the case where the Naif reference frame code is
   *                           not recognized.
   *   @history 2008-06-18 Unknown - Fixed documentation, added NaifStatus calls
   *   @history 2008-11-26 Debbie A. Cook Added method to set axes of rotation.
   *                           Default axes are still 3,1,3 so existing software
   *                           will not be affected by the change.  Also added
   *                           timeScale to the the class and made some
   *                           parameters protected instead of private so they
   *                           are available to inheriting classes.
   *   @history 2008-12-12 Debbie A. Cook Added method to return frame code
   *   @history 2009-01-26 Debbie A. Cook Added wrap of 3rd camera angle when
   *                           crossing +-180
   *   @history 2009-04-21 Debbie A. Cook Added methods MinimizeCache and
   *                           LoadTimeCache, variable p_minimizeCache, and
   *                           enum constants DownsizeStatus
   *   @history 2009-06-29 Debbie A. Cook Fixed memory overwrite problem in
   *                           LoadTimeCache when reading a type 3 ck
   *   @history 2009-07-24 Debbie A. Cook Removed downsizing for Nadir
   *                           instrument pointing tables (LoadTimeCache) so
   *                           that radar instruments will work.  Current
   *                           downsizing code requires sclk and radar has no
   *                           sclk.
   *   @history 2009-10-01 Debbie A. Cook Divided the rotation into a constant
   *                           (in time) part and a time-based part and
   *                           added keywords listing the frame chains for both
   *                           the constant part and the time-based part.
   *   @history 2009-10-09 Debbie A. Cook Added angular velocity when it is
   *                           available
   *   @history 2009-10-30 Unknown - Modified J2000Vector and ReferenceVector
   *                           to work on either length 3 vectors (position
   *                           only) or lenght 6 vectors (position and velocity)
   *                           and added private method StateTJ()
   *   @history 2009-12-03 Debbie A. Cook Modified tests in LoadTimeCache to
   *                           allow observation to cross segment boundary for
   *                           LRO
   *   @history 2010-03-19 Debbie A. Cook Revised ReloadCache including removing
   *                           obsolete arguments.  Added initialization of
   *                           members p_fullCacheStartTime, p_fullCacheEndTime,
   *                           and p_fullCacheSize.  Added these same values to
   *                           the table label in method Cache and the reading
   *                           of these values to the method LoadCache(table).
   *                           Improved error message in FrameTrace.  Also
   *                           corrected a comment in StateTJ
   *   @history 2010-09-23 Debbie A. Cook Revised to write out line cache for
   *                           updated pointing when cache size is 1. If the
   *                           original pointing had an angular velocity in
   *                           this case, the original angular velocity is
   *                           written out along with the updated quaternion.
   *                           Also added method Extrapolate, to extrapolate
   *                           pointing assuming a constant angular velocity.
   *                           This method was designed to compute the pointing
   *                           at the start and end of the exposure for framing
   *                           cameras to create a ck that would cover a single
   *                           framing observation.
   *   @history 2010-12-22 Debbie A. Cook  Added new method
   *                           SetFullCacheParameters to upgrade appjit to
   *                           current instrument Rotation group labels.
   *   @history 2011-02-17 Debbie A. Cook  Fixed bug in method LineCache and
   *                           fixed computation of angular velocity in method
   *                           DCJdt (derivative was with respect to scaled et
   *                           instead of et)
   *   @history 2011-02-22 Debbie A. Cook - Corrected Extrapolation method
   *   @history 2011-03-25 Debbie A. Cook - Added method GetCenterAngles()
   *   @history 2011-07-20 Kris J Becker - Modified
   *                           SpiceRotation::LoadCache(Table &table) to be
   *                           reentrant.  This mod was necessitated by the Dawn
   *                           VIR instrument.
   *   @history 2012-05-28 Debbie A. Cook - Programmer notes - A new
   *                           interpolation algorithm, PolyFunctionOverSpice,
   *                           was added and new supporting methods:
   *                           SetEphemerisTimePolyOverSpice,  SetEphemerisTimeSpice,
   *                           SetEphemerisTimeNadir, SetEphemerisTimeMemcache,
   *                           and SetEphemerisTimePolyFunction.
   *                           PolyFunctionOverSpice is never output, but is
   *                           converted to a line cache and reduced.  Methods
   *                           LineCache and ReloadCache were modified to do the
   *                           reduction and a copy constructor was added to
   *                           support the reduction.  Also an argument was
   *                           added to SetPolynomial methods for function type,
   *                           since PolyFunction is no longer the only function
   *                           supported.  These changes help the BundleAdjust
   *                           applications to better fit line scan images where
   *                           the pointing was not modeled well with a regular
   *                           polynomial.
   *   @history 2012-10-25 Jeannie Backer - Brought class closer to Isis3
   *                           standards: Ordered includes in cpp file, replaced
   *                           quotation marks with angle braces in 3rd party
   *                           includes, fixed history indentation and line
   *                           length. References #1181.
   *   @history 2013-03-27 Jeannie Backer - Added methods for MsiCamera. Brought
   *                           class closer to Isis standards: moved method
   *                           implementation to cpp file, fixed documentation.
   *                           References #1248.
   *   @history 2013-11-12 Ken Edmundson Programmers notes - Commented out cout
   *                           debug statements on lines 637 and 642 that appeared
   *                           onscreen during jigsaw runs when images are updated.
   *                           References #1521.
   *   @history 2014-03-11 Tracie Sucharski - In the LoadTimeCache method, do not throw error if
   *                           if first segment in kernel is not type 3 or 5.  As long as the
   *                           segment needed is type 3 or 5, we're ok.  This was changed for
   *                           New Horizons which had ck's with both type 2 and type 3 segments.
   *   @history 2014-03-11 Stuart Sides - Programmers notes - Fixed a bug in the copy constructor
   *                           that was going out of array bounds.
   *   @history 2015-02-20 Jeannie Backer - Improved error messages.
   *   @history 2015-07-21 Kristin Berry - Added additional NaifStatus::CheckErrors() calls to see if
   *                           any NAIF errors were signaled. References #2248.
   *   @history 2015-08-05 Debbie A. Cook - Programmer notes - Modified LoadCache,
   *                           and ComputeAv.
   *                           Added new methods
   *                           loadPCFromSpice, loadPCFromTable, toJ2000Partial, poleRaCoefs,
   *                           poleDecCoefs, pmCoefs, poleRaNutPrecCoefs, poleDecNutPrecCoefs,
   *                           pmNutPrecCoefs, sysNutPrecConstants, sysNutPrecCoefs,
   *                           usePckPolynomial, setPckPolynomial(raCoef, decCoef, pmCoef),
   *                           getPckPolynomial, setEphemerisTimePckPolyFunction, getFrameType
   *                           and members m_frameType, m_tOrientationAvailable,
   *                           m_raPole, m_decPole, m_pm, m_raNutPrec, m_decNutPrec, m_pmNutPrec,
   *                           m_sysNutPrec0, m_sysNutPrec1, m_dscale, m_Tscale to support request for
   *                           solving for target body parameters.
   *                           Also added a new enumerated value for Source, PckPolyFunction,
   *                           and PartialType, WRT_RotationRate.
   *   @history 2016-02-15 Debbie A. Cook - Programmer notes - Added private method
   *                           setFrameType to set the frame type.  It also loads the planetary
   *                           constants for a PCK type.
   *   @history 2016-06-28 Ian Humphrey - Updated documentation and coding standards. Added new
   *                           tests to unit test. Fixes #3972.
   *   @history 2017-12-13 Ken Edmundson - Added "case DYN:" to methods ToReferencePartial and toJ2000Partial. Fixes #5251.
   *                           This problem was found when trying to bundle M3 images that had been spiceinited with nadir
   *                           pointing. The nadir frame is defined as a Dynamic Frame by Naif.
   *   @history 2018-04-21 Jesse Mapel - Modified frametype resolution to check if a body centered
   *                           frame uses a CK or PCK definition. This only occurs for bodies
   *                           where a pck cannot accurately define for the duration of a mission.
   *                           The current example is the comet 67P/CHURYUMOV-GERASIMENKO
   *                           imaged by Rosetta. Some future comet/astroid missions are expected
   *                           to use a CK defined body fixed reference frame. Fixes #5408.
   *
   *  @todo Downsize using Hermite cubic spline and allow Nadir tables to be downsized again.
   *  @todo Consider making this a base class with child classes based on frame type or
   *              storage type (polynomial, polynomial over cache, cache, etc.)
   */
  class SpiceRotation {
    public:
      // Constructors
      SpiceRotation(int frameCode);
      /*      SpiceRotation( int NaifCode );
      We would like to call refchg instead to avoid the strings.  Currently Naif does
      not have refchg_c, but only the f2c'd refchg.c.*/
      SpiceRotation(int frameCode, int targetCode);
      SpiceRotation(const SpiceRotation &rotToCopy);

      // Destructor
      virtual ~SpiceRotation();

      // Change the frame (has no effect if cached)
      void SetFrame(int frameCode);
      int Frame();

      void SetTimeBias(double timeBias);

      /**
       * The rotation can come from one of 3 places for an Isis cube.  The class
       * expects function to be after Memcache.
       *       Spice - the rotation is calculated by Naif Spice routines with data
       *                  read directly from Naif kernels.
       *       Nadir - the rotation is calculated using the Naif routine twovec with
       *                  the position and velocity vectors of the spacecraft.
       *       Memcache - the rotation is linearly interpolated from time-based
       *                  values in a table.
       *       PolyFunction - the rotation is calculated from an nth degree
       *                  polynomial in one variable (time in scaled seconds)
       *       PolyFunctionOverSpice - the rotation is calculated from an nth
       *                  degree polynomial fit over the Naif Spice results.
       *       PckPolyFunction - The rotation is calculated using the IAU fit
       *                  polynomials in one variable (time in Julian centuries and days).
       */
      enum Source {
        Spice,                   //!< Directly from the kernels
        Nadir,                   //!< Nadir pointing
        Memcache,                //!< From cached table
        PolyFunction,            //!< From nth degree polynomial
        PolyFunctionOverSpice ,  //!< Kernels plus nth degree polynomial
        PckPolyFunction          //!< Quadratic polynomial function with linear trignometric terms
      };

      /**
       * This enumeration indicates whether the partial derivative is taken with
       * respect to Right Ascension, Declination, or Twist (or Rotation).
       */
      enum PartialType {
        WRT_RightAscension, //!< With respect to Right Ascension
        WRT_Declination,    //!< With respect to Declination
        WRT_Twist           //!< With respect to Twist or Prime Meridian Rotation
      };

      /**
       * Status of downsizing the cache
       */
      enum DownsizeStatus {
        Yes,  //!< Downsize the cache
        Done, //!< Cache is downsized
        No    //!< Do not downsize the cache
      };

      /**
       * Enumeration for the frame type of the rotation
       */
      enum FrameType {
        UNKNOWN = 0,      //!< Isis specific code for unknown frame type
        INERTL = 1,       //!< See Naif Frames.req document for
        PCK  = 2,         //!< definitions
        CK = 3,           //!<
        TK = 4,           //!<
        DYN = 5,          //!<
        BPC = 6,          //!< Isis specific code for binary pck
        NOTJ2000PCK = 7   //!< PCK frame not referenced to J2000
      };

      void SetEphemerisTime(double et);
      double EphemerisTime() const;

      std::vector<double> GetCenterAngles();

      std::vector<double> Matrix();
      std::vector<double> AngularVelocity();

      // TC
      std::vector<double> ConstantRotation();
      std::vector<double> &ConstantMatrix();
      void SetConstantMatrix(std::vector<double> constantMatrix);

      // CJ
      std::vector<double> TimeBasedRotation();
      std::vector<double> &TimeBasedMatrix();
      void SetTimeBasedMatrix(std::vector<double> timeBasedMatrix);

      std::vector<double> J2000Vector(const std::vector<double> &rVec);

      std::vector<Angle> poleRaCoefs();

      std::vector<Angle> poleDecCoefs();

      std::vector<Angle> pmCoefs();

      std::vector<double> poleRaNutPrecCoefs();

      std::vector<double> poleDecNutPrecCoefs();

      std::vector<double> pmNutPrecCoefs();

      std::vector<Angle> sysNutPrecConstants();

      std::vector<Angle> sysNutPrecCoefs();

      std::vector<double> ReferenceVector(const std::vector<double> &jVec);

      std::vector<double> EvaluatePolyFunction();

      void loadPCFromSpice(int CenterBodyCode);
      void loadPCFromTable(const PvlObject &Label);

      void MinimizeCache(DownsizeStatus status);

      void LoadCache(double startTime, double endTime, int size);

      void LoadCache(double time);

      void LoadCache(Table &table);

      void LoadCache(nlohmann::json &isd);

      Table LineCache(const QString &tableName);

      void ReloadCache();

      Table Cache(const QString &tableName);
      void CacheLabel(Table &table);

      void LoadTimeCache();

      std::vector<double> Angles(int axis3, int axis2, int axis1);
      void SetAngles(std::vector<double> angles, int axis3, int axis2, int axis1);

      bool IsCached() const;

      void SetPolynomial(const Source type=PolyFunction);

      void SetPolynomial(const std::vector<double> &abcAng1,
                         const std::vector<double> &abcAng2,
                         const std::vector<double> &abcAng3,
                         const Source type = PolyFunction);

      void usePckPolynomial();
      void setPckPolynomial(const std::vector<Angle> &raCoeff,
                            const std::vector<Angle> &decCoeff,
                            const std::vector<Angle> &pmCoeff);

      void GetPolynomial(std::vector<double> &abcAng1,
                         std::vector<double> &abcAng2,
                         std::vector<double> &abcAng3);

      void getPckPolynomial(std::vector<Angle> &raCoeff,
                            std::vector<Angle> &decCoeff,
                            std::vector<Angle> &pmCoeff);

      // Set the polynomial degree
      void SetPolynomialDegree(int degree);
      Source GetSource();
      void SetSource(Source source);
      void ComputeBaseTime();
      FrameType getFrameType();
      double GetBaseTime();
      double GetTimeScale();

      void SetOverrideBaseTime(double baseTime, double timeScale);
      void SetCacheTime(std::vector<double> cacheTime);

      // Derivative methods
      double DPolynomial(const int coeffIndex);
      double DPckPolynomial(PartialType partialVar, const int coeffIndex);

      std::vector<double> toJ2000Partial(const std::vector<double> &lookT,
                                         PartialType partialVar, int coeffIndex);
      std::vector<double> ToReferencePartial(std::vector<double> &lookJ,
                                             PartialType partialVar, int coeffIndex);
      void DCJdt(std::vector<double> &dRJ);

      double WrapAngle(double compareAngle, double angle);
      void SetAxes(int axis1, int axis2, int axis3);
      std::vector<double> GetFullCacheTime();
      void FrameTrace(double et);

      // Return the frame chain for the constant part of the rotation (ends in target)
      std::vector<int>  ConstantFrameChain();
      std::vector<int>  TimeFrameChain();
      void InitConstantRotation(double et);
      bool HasAngularVelocity();

      void ComputeAv();
      std::vector<double> Extrapolate(double timeEt);

      void checkForBinaryPck();

      int cacheSize() {
        if (m_orientation) {
          return m_orientation->getRotations().size();
        }
        return 0;
      }

    protected:
      void SetFullCacheParameters(double startTime, double endTime, int cacheSize);
      void setEphemerisTimeMemcache();
      void setEphemerisTimeNadir();
      void setEphemerisTimeSpice();
      void setEphemerisTimePolyFunction();
      void setEphemerisTimePolyFunctionOverSpice();
      void setEphemerisTimePckPolyFunction();
      std::vector<double> p_cacheTime;  //!< iTime for corresponding rotation
      int p_degree;                     //!< Degree of fit polynomial for angles
      int p_axis1;                      //!< Axis of rotation for angle 1 of rotation
      int p_axis2;                      //!< Axis of rotation for angle 2 of rotation
      int p_axis3;                      //!< Axis of rotation for angle 3 of rotation
      ale::Orientations *m_orientation; //! Cached orientation information

    private:
      // method
      void setFrameType();
      std::vector<int> p_constantFrames;  /**< Chain of Naif frame codes in constant
                                               rotation TC. The first entry will always
                                               be the target frame code*/
      std::vector<int> p_timeFrames;      /**< Chain of Naif frame codes in time-based
                                               rotation CJ. The last entry will always
                                               be 1 (J2000 code)*/
      double p_timeBias;                  //!< iTime bias when reading kernels

      double p_et;                           //!< Current ephemeris time
      Quaternion p_quaternion;            /**< Quaternion for J2000 to reference
                                                                  rotation at et*/

      bool p_matrixSet;                    //!< Flag indicating p_TJ has been set
      bool m_tOrientationAvailable;  //!< Target orientation constants are available


      FrameType m_frameType;  //!< The type of rotation frame
      Source p_source;                    //!< The source of the rotation data
      int p_axisP;                        /**< The axis defined by the spacecraft
                                               vector for defining a nadir rotation*/
      int p_axisV;                        /**< The axis defined by the velocity
                                               vector for defining a nadir rotation*/
      int p_targetCode;                   //!< For computing Nadir rotation only

      double p_baseTime;                  //!< Base time used in fit equations
      double p_timeScale;                 //!< Time scale used in fit equations
      bool p_degreeApplied;               /**< Flag indicating whether or not a polynomial
                                               of degree p_degree has been created and
                                               used to fill the cache*/
      std::vector<double> p_coefficients[3];  /**< Coefficients defining functions fit
                                                   to 3 pointing angles*/
      bool p_noOverride;                  //!< Flag to compute base time;
      double p_overrideBaseTime;          //!< Value set by caller to override computed base time
      double p_overrideTimeScale;         //!< Value set by caller to override computed time scale
      DownsizeStatus p_minimizeCache;     //!< Status of downsizing the cache (set to No to ignore)
      double p_fullCacheStartTime;        //!< Initial requested starting time of cache
      double p_fullCacheEndTime;          //!< Initial requested ending time of cache
      int p_fullCacheSize;                //!< Initial requested cache size
      std::vector<double> p_TC;           /**< Rotation matrix from first constant rotation
                                          (after all time-based rotations in frame chain from
                                           J2000 to target) to the target frame*/
      std::vector<double> p_CJ;           /**< Rotation matrix from J2000 to first constant
                                               rotation*/
      std::vector<double> p_av;           //!< Angular velocity for rotation at time p_et
      bool p_hasAngularVelocity;          /**< Flag indicating whether the rotation
                                               includes angular velocity*/
      std::vector<double> StateTJ();      /**< State matrix (6x6) for rotating state
                                               vectors from J2000 to target frame*/
      // The remaining items are only used for PCK frame types.  In this case the
      // rotation is  stored as a cache, but the coefficients are available for display
      // or comparison, and the first three coefficient sets can be solved for and
      // updated in jigsaw.   The initial coefficient values are read from a Naif PCK.
      //
      // The general equation for the right ascension of the pole is
      //
      // raPole  =  raPole[0] + raPole[1]*Time  + raPole[2]*Time**2 + raNutPrec,
      //    where
      //    raNutPrec  =  raNutPrec1[0]*sin(sysNutPrec[0][0] + sysNutPrec[0][1]*Time) +
      //                  raNutPrec1[1]*sin(sysNutPrec[1][0] + sysNutPrec[1][1]*Time) + ...
      //                  raNutPrec1[N-1]*sin(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time) +
      // (optional for multiples of nutation precession angles)
      //                  raNutPrec2[0]*sin(2*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  raNutPrec2[1]*sin(2*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  raNutPrec2[N-1]*sin(2*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      //                  raNutPrecM[0]*sin(M*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  raNutPrecM[1]*sin(M*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  raNutPrecM[N-1]*sin(M*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      //
      // The general equation for the declination of the pole is
      //
      // decPole  =  p_decPole[0] + p_decPole[1]*Time  + p_decPole[2]*Time**2 + decNutPrec,
      //    where
      //    decNutPrec  =  decNutPrec1[0]*cos(sysNutPrec[0][0] + sysNutPrec[0][1]*Time) +
      //                   decNutPrec1[1]*cos(sysNutPrec[1][0] + sysNutPrec[1][1]*Time) + ...
      //                   decNutPrec1[N-1]*cos(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time) +
      //                   decNutPrec2[0]*cos(2*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                   decNutPrec2[1]*cos(2*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                   decNutPrec2[N-1]*cos(2*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      // (optional for multiples of nutation precession angles)
      //                   decNutPrecM[0]*sin(M*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                   decNutPrecM[1]*sin(M*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                   decNutPrecM[N-1]*sin(M*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time))
      //
      //     and Time is julian centuries since J2000.
      //
      // The general equation for the prime meridian rotation is
      //
      // pm  =  p_pm[0] + p_pm[1]*Dtime  + p_pm[2]*Dtime**2 + pmNutPrec,
      //    where
      //    pmNutPrec  =  pmNutPrec1[0]*sin(sysNutPrec[0][0] + sysNutPrec[0][1]*Time) +
      //                  pmNutPrec1[1]*sin(sysNutPrec[1][0] + sysNutPrec[1][1]*Time) + ...
      //                  pmNutPrec1[N-1]*sin(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time) +
      // (optional for multiples of nutation precession angles)
      //                  pmNutPrec2[0]*sin(2*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  pmNutPrec2[1]*sin(2*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  pmNutPrec2[N-1]*sin(2*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      //                  pmNutPrecM[0]*sin(M*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  pmNutPrecM[1]*sin(M*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  pmNutPrecM[N-1]*sin(M*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time))
      //
      //     Time is interval in Julian centuries since the standard epoch,
      //     dTime is interval in days from the standard epoch (J2000),
      //
      //     N is the number of nutation/precession terms for the planetary system of the target
      //     body,  (possibly including multiple angles as unique terms,
      //             ie. 2*sysNutPrec[0][0] + sysNutPrec[][1]*Time).
      //
      //     Many of the constants in this equation are 0. for a given body.
      //
      //     M is included as an option for future improvements.  M = highest multiple (period)
      //     of any of the nutation/precession angles included in the equations.
      //
      //     ***NOTE*** Currently Naif stores multiples (amplitudes) as if they were additional
      //                nutation/precession terms (periods) in the equation.  This method works as
      //                long as jigsaw does not solve for those values.  In order to solve for
      //                those values, the multiples will need to be known so that the partial
      //                derivatives can be correctly calculated.  Some possible ways of doing this
      //                are 1) Convince Naif to change their data format indicating the relation
      //                      2) Make an Isis version of the PCK data and have Isis software to
      //                          calculate the rotation and partials.
      //                      3) Have an Isis addendum file that identifies the repeated periods
      //                          and software to apply them when calculating the rotation and partials.
      //
      //                For now this software will handle any terms with the same period and different
      //                amplitudes as unique terms in the equation (raNutPrec, decNutPrec,
      //                and pmNutPrec).
      //
      // The next three vectors will have length 3 (for a quadratic polynomial) if used.
      std::vector<Angle>m_raPole;       //!< Coefficients of a quadratic polynomial fitting pole ra.
      std::vector<Angle>m_decPole;      //!< Coefficients of a quadratic polynomial fitting pole dec.
      std::vector<Angle>m_pm ;          //!< Coefficients of a quadratic polynomial fitting pole pm.
      //
      // Currently multiples (terms with periods matching other terms but varying amplitudes)
      // are handled as additional terms added to the end of the vector as Naif does (see
      // comments in any of the standard Naif PCK.
      std::vector<double>m_raNutPrec;    //!< Coefficients of pole right ascension nut/prec terms.
      std::vector<double>m_decNutPrec;  //!< Coefficients of pole decliniation nut/prec terms.
      std::vector<double>m_pmNutPrec;   //!< Coefficients of prime meridian nut/prec terms.

      // The periods of bodies in the same system are modeled with a linear equation
      std::vector<Angle>m_sysNutPrec0; //!< Constants of planetary system nut/prec periods
      std::vector<Angle>m_sysNutPrec1; //!< Linear terms of planetary system nut/prec periods

      // The following scalars are used in the IAU equations to convert p_et to the appropriate time
      // units for calculating target body ra, dec, and w.  These need to be initialized in every
      // constructor.
      //! Seconds per Julian century for scaling time in seconds
      static const double m_centScale;
      //! Seconds per day for scaling time in seconds to get target body w
      static const double m_dayScale;

  };
};

#endif
