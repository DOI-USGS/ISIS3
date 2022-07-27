#ifndef SpicePosition_h
#define SpicePosition_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <vector>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Table.h"
#include "PolynomialUnivariate.h"

// ale includes
#include "ale/States.h"

#include <nlohmann/json.hpp>

namespace Isis {
  class NumericalApproximation;

  /**
   * @brief Obtain SPICE position information for a body
   *
   * This class will obtain the J2000 body position between a target and
   * observer body.  For example, a spacecraft and Mars or the Sun and Mars.
   * It is essentially a C++ wrapper to the NAIF spkez_c routine.  Therefore,
   * appropriate NAIF kernels are expected to be loaded prior to using this
   * class.  The position can be returned with or without one way light time
   * corrections between the two bodies.  See
   * NAIF required reading for more information regarding this subject
   * at
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/ascii/individual_docs/spk.req
   * <p>
   * An important functionality of this class is the ability to cache the
   * positions so they do not have to be constantly read from the NAIF kernels.
   * Onced the data is cached the NAIF kernels can be unloaded too.
   *
   * @code
   *
   * @endcode
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2005-12-01 Jeff Anderson
   *
   * @internal
   *   @history 2003-12-01 Jeff Anderson Original Version derived from Spice
   *                           class
   *   @history 2006-03-23 Jeff Anderson Added check to SetEphemeris to return
   *                           if the time did not change.  Should speed up line
   *                           scan cameras
   *   @history 2007-07-10 Debbie A. Cook Added else to method
   *                           SetAberrationCorrection to separate error section
   *                           from the rest of the code
   *   @history 2007-08-24 Debbie A. Cook Added members p_coefficients, enums
   *                           PartialType and Coefficient, and methods ReloadCache,
   *                           SetPolynomial, GetPolynomial, ComputeBaseTime,
   *                           DPolynomial, and CoordinatePartial to support solving
   *                           for instrument position in BundleAdjust
   *   @history 2008-01-10 Debbie A. Cook The function was changed from Parabola
   *                           to Poly1D. New methods GetBaseTime and
   *                           SetOverrideBaseTime were added
   *   @history 2008-02-07 Jeannie Walldren  Poly1D was changed to
   *                           PolynomialUnivariate
   *   @history 2008-02-10 Debbie A. Cook Removed the (-1.) in case A of
   *                           DPolynomial since it was not actually part of the
   *                           position derivation but how it was being applied in
   *                           BundleAdjust.
   *   @history 2008-06-18 Steven Lambright Fixed documentation
   *   @history 2008-06-25 Debbie A. Cook  Added members p_velocity,
   *                           p_cacheVelocity, and p_hasVelocity; also added
   *                           methods Velocity() and HasVelocity() to support
   *                           miniRF.
   *   @history 2008-06-26 Debbie A. Cook Replaced Naif error handling calls with
   *                           Isis NaifStatus
   *   @history 2009-08-03 Jeannie Walldren - Added methods ReloadCache( table ),
   *                           HermiteIndices(), Memcache2HermiteCache() to allow
   *                           for downsized instrument position tables in labels of
   *                           Isis cubes and added methods SetEphemerisTimeSpice(),
   *                           SetEphemerisTimeHermiteCache(), and
   *                           SetEphemerisTimeMemcache() to make software more
   *                           readable.
   *   @history 2009-08-14 Debbie A. Cook - Corrected loop counter in
   *                           HermiteIndices
   *   @history 2009-08-27 Jeannie Walldren - Added documentation.
   *   @history 2009-10-20 Debbie A. Cook - Corrected calculation of extremum in
   *                           ReloadCache
   *   @history 2009-11-06 Debbie A. Cook - Added velocity partial derivative
   *                           method
   *   @history 2009-12-15 Debbie A. Cook - Changed enumerated partial types and
   *                           argument list for CoordinatePartial and
   *                           VelocityPartial
   *   @history 2010-03-19 Debbie A. Cook - Added argument coeffIndex to method
   *                           Velocity Partial
   *   @history 2010-12-07 Steven Lambright - Moved the cubic hermite splines to
   *                           member scope for efficiency. It was a significant
   *                           overhead to keep reconstructing these. Created
   *                           ClearCache() to help increase code reusability.
   *   @history 2011-02-12 Debbie A. Cook - Added PolyFunction to source types and
   *                           new method SetEphemerisTimePolyFunction to support the
   *                           new type.  Also added method SetPolynomialDegree to
   *                           allow the degree of the polynomials fit to the
   *                           position coordinates to be changed.  The fit
   *                           polynomial was changed from a fixed 2nd order
   *                           polynomial to an nth degree polynomial with one
   *                           independent variable.  Added private class members
   *                           p_fullCacheStartTime, p_fullCacheEndTime, and
   *                           p_fullCacheSize.  Added p_timeScale, GetBaseTime(),
   *                           SetOverrideBaseTime, GetTimeScale(),
   *                           Extrapolate(double timeEt), and
   *                           ComputeVelocityInTime(PartialType var).  The function
   *                           was changed from a Parabola to Polynomial1Variable,
   *                           now called PolynomialUnivariate.
   *   @history 2011-02-14 Debbie A. Cook - Corrected previous update for Hermite
   *                           case.  Created a special member, p_overrideScale, for
   *                           Hermite case.  Also removed obsolete enum Coefficient.
   *   @history 2011-02-17 Debbie A. Cook - Corrected missed problem with degree
   *                           forced to be 2 and corrected calculation of velocity
   *                           partial
   *   @history 2011-02-22 Debbie A. Cook - Corrected Extrapolation method
   *   @history 2011-02-28 Debbie A. Cook - Fixed typo in LoadCache and potential
   *                           memory problem in SetPolynomial
   *   @history 2011-04-10 Debbie A. Cook - Added GetSource method to support
   *                           spkwriter.
   *   @history 2011-05-27 Debbie A. Cook - Renamed old ReloadCache method for
   *                           converting polynomial functions to Hermite cubic
   *                           splines to LoadHermiteCache for use with spkwriter.
   *   @history 2012-03-31 Debbie A. Cook - Programmer notes: Added new
   *                           interpolation algorithm,
   *                           PolyFunctionOverHermiteConstant, and new supporting
   *                           method, SetEphemerisTimePolyOverHermiteConstant.  Also
   *                           added argument to SetPolynomial methods for type.
   *                           LineCache and ReloadCache were modified to allow
   *                           other function types beyond the PolyFunction.  Added
   *                           new method to return Hermite coordinate.
   *   @history 2012-10-25 Jeannie Backer - Brought class closer to Isis3
   *                           standards: Ordered includes in cpp file, replaced
   *                           quotation marks with angle braces in 3rd party
   *                           includes, fixed history indentation. References
   *                           #1181.
   *   @history 2012-10-31 Kris Becker - Added implementation for swapping of
   *                           observer/target and light time correction to
   *                           surface.  Added a new, special protected
   *                           constructor; virtualized the SetEphemerisTimeSpice
   *                           method which allows for specialized determination
   *                           of light time corrections; generalized the
   *                           retrieval of body state vectors
   *                           (computeStateVector); added many new, mostly
   *                           protected, ssupport routines for this
   *                           generalization. Fixes (mostly) #0909, #1136 and
   *                           #1223.
   *   @history 2013-03-28 Debbie A. Cook - Fixed bug causing jigsaw to fail with a singular definite
   *                           matrix on radar data.  The bug was in the method VelocityPartial and
   *                           occurred when the et = baseTime and the coeffIndex was 0.  This caused
   *                           the derivative equation to be 0 * 0 ** -1.  This update fixes issue #1582.
   *   @history 2015-02-20 Jeannie Backer - Improved error messages.
   *   @history 2015-07-21 Kristin Berry - Added additional NaifStatus::CheckErrors() to see if
   *                            any NAIF errors were signaled. References #2248.
   *   @history 2015-08-25 Kristin Berry - Moved check to make sure a polynomial exists to earlier in
   *                           LoadHermiteCache. Was failing due to lack of a polynomial with a
   *                           confusing error message before the check.
   *   @history 2017-08-18 Tyler Wilson, Summer Stapleton, Ian Humphrey -  Added opening/closing brackets
   *                           to SetEphemerisTimePolyFunction() so this class compiles without warnings
   *                           under C++14. References #4809.
   *   @history 2018-06-22 Ken Edmundson - Added scaledTime() method to return current scaled time.
   *   @history 2020-07-01 Kristin Berry - Updated to use ale::States for internal state cache.
   */
  class SpicePosition {
    public:
      //??? jw
      /**
       * This enum indicates the status of the object.  The class
       * expects functions to be after MemCache in the list.
       */
      enum Source { Spice,       //!< Object is reading directly from the kernels
                    Memcache,    //!< Object is reading from cached table
                    HermiteCache,//!< Object is reading from splined table
                    PolyFunction, //!< Object is calculated from nth degree polynomial
                    PolyFunctionOverHermiteConstant //!< Object is reading from splined
                    //                table and adding nth degree polynomial
                  };

      SpicePosition(int targetCode, int observerCode);

      //! Destructor
      virtual ~SpicePosition();

      void SetTimeBias(double timeBias);
      double GetTimeBias() const;

      virtual void SetAberrationCorrection(const QString &correction);
      virtual QString GetAberrationCorrection() const;
      double GetLightTime() const;

      virtual const std::vector<double> &SetEphemerisTime(double et);
      enum PartialType {WRT_X, WRT_Y, WRT_Z};

      //! Return the current ephemeris time
      virtual double EphemerisTime() const {
        return p_et;
      };

      double scaledTime() const;

      const std::vector<double> &GetCenterCoordinate();

      //! Return the current J2000 position
      virtual const std::vector<double> &Coordinate() {
        return p_coordinate;
      };

      //! Return the current J2000 velocity
      const std::vector<double> &Velocity();

      //! Return the flag indicating whether the velocity exists
      bool HasVelocity() {
        return p_hasVelocity;
      };

      void LoadCache(double startTime, double endTime, int size);
      void LoadCache(double time);
      void LoadCache(Table &table);
      void LoadCache(nlohmann::json &isd);

      Table LineCache(const QString &tableName);
      Table LoadHermiteCache(const QString &tableName);

      void ReloadCache();
      void ReloadCache(Table &table);

      Table Cache(const QString &tableName);

      //! Is this position cached
      bool IsCached() const {
        return (m_state != NULL);
      };

      //! Get the size of the current cached positions
      int cacheSize() const {
        if (m_state) {
          return m_state->getStates().size();
        }
        return 0;
      };

      void SetPolynomial(const Source type = PolyFunction);

      void SetPolynomial(const std::vector<double>& XC,
                         const std::vector<double>& YC,
                         const std::vector<double>& ZC,
                         const Source type = PolyFunction);

      void GetPolynomial(std::vector<double>& XC,
                         std::vector<double>& YC,
                         std::vector<double>& ZC);

      //! Set the polynomial degree
      void SetPolynomialDegree(int degree);

      //! Return the source of the position
      Source GetSource() {
        return p_source;
      };

      void ComputeBaseTime();

      //! Return the base time for the position
      double GetBaseTime() {
        return p_baseTime;
      };

      void SetOverrideBaseTime(double baseTime, double timeScale);

      //! Return the time scale for the position
      double GetTimeScale() {
        return p_timeScale;
      };

      double DPolynomial(const int coeffIndex);

      std::vector<double> CoordinatePartial(SpicePosition::PartialType partialVar, int coeffIndex);

      std::vector<double> VelocityPartial(SpicePosition::PartialType partialVar, int coeffIndex);
      enum OverrideType {NoOverrides, ScaleOnly, BaseAndScale};
      void Memcache2HermiteCache(double tolerance);
      std::vector<double> Extrapolate(double timeEt);
      std::vector<double> HermiteCoordinate();
    protected:
      void SetEphemerisTimeMemcache();
      void SetEphemerisTimeHermiteCache();
      virtual void SetEphemerisTimeSpice();
      void SetEphemerisTimePolyFunction();
      void SetEphemerisTimePolyFunctionOverHermiteConstant();

      //======================================================================
      // New methods support for light time correction and swap of observer/target
      SpicePosition(int targetCode, int observerCode, bool swapObserverTarget);
      int getObserverCode() const;
      int getTargetCode() const;
      double getAdjustedEphemerisTime() const;
      void computeStateVector(double et, int target, int observer,
                              const QString &refFrame,
                              const QString &abcorr,
                              double state[6], bool &hasVelocity,
                              double &lightTime) const;
      void setStateVector(const double state[6], const bool &hasVelocity);
      void setLightTime(const double &lightTime);
      //======================================================================

    private:
      void init(int targetCode, int observerCode,
                const bool &swapObserverTarget = false);
      void ClearCache();
      void LoadTimeCache();
      void CacheLabel(Table &table);
      double ComputeVelocityInTime(PartialType var);

      int p_targetCode;                   //!< target body code
      int p_observerCode;                 //!< observer body code

      double p_timeBias;                  //!< iTime bias when reading kernels
      QString p_aberrationCorrection; //!< Light time correction to apply

      double p_et;                        //!< Current ephemeris time
      std::vector<double> p_coordinate;   //!< J2000 position at time et
      std::vector<double> p_velocity;     //!< J2000 velocity at time et

      Source p_source;                    //!< Enumerated value for the location of the SPK information used
      std::vector<double> p_cacheTime;    //!< iTime for corresponding position
      std::vector<double> p_coefficients[3];             //!< Coefficients of polynomials fit to 3 coordinates

      double p_baseTime;                  //!< Base time used in fit equations
      double p_timeScale;                 //!< Time scale used in fit equations
      bool p_degreeApplied;               //!< Flag indicating whether or not a polynomial
      //    of degree p_degree has been created and
      //    used to fill the cache
      int p_degree;                       //!< Degree of polynomial function fit to the coordinates of the position
      double p_fullCacheStartTime;        //!< Original start time of the complete cache after spiceinit
      double p_fullCacheEndTime;          //!< Original end time of the complete cache after spiceinit
      double p_fullCacheSize;             //!< Orignial size of the complete cache after spiceinit
      bool p_hasVelocity;                 //!< Flag to indicate velocity is available
      OverrideType p_override;            //!< Time base and scale override options;
      double p_overrideBaseTime;          //!< Value set by caller to override computed base time
      double p_overrideTimeScale;         //!< Value set by caller to override computed time scale

      // Variables support observer/target swap and light time correction
      bool   m_swapObserverTarget;  ///!< Swap traditional order
      double m_lt;                 ///!<  Light time correction

      ale::States *m_state; ///!< State: stores times, positions, velocities;
  };
};

#endif
