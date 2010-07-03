#ifndef SpicePosition_h
#define SpicePosition_h
/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/27 07:01:33 $
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
#include "PolynomialUnivariate.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

namespace Isis {
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
   *  @history 2003-12-01 Jeff Anderson Original Version derived from Spice
   *  class
   *  @history 2006-03-23 Jeff Anderson Added check to SetEphemeris to return
   *                      if the time did not change.  Should speed up line
   *			  scan cameras
   *  @history 2007-07-10 Debbie A. Cook Added else to method SetAberrationCorrection
   *                      to separate error section from the rest of the code
   *  @history 2007-08-24 Debbie A. Cook Added members p_coefficients, enums PartialType and Coefficient,
   *                      and methods ReloadCache, SetPolynomial, GetPolynomial, ComputeBaseTime,
   *                      DPolynomial, and CoordinatePartial to support solving for instrument position
   *                      in BundleAdjust
   *  @history 2008-01-10 Debbie A. Cook The function was changed from Parabola to Poly1D. New methods
   *                      GetBaseTime and SetOverrideBaseTime were added
   *  @history 2008-02-07 Jeannie Walldren  Poly1D was changed to
   *                      PolynomialUnivariate
   *  @history 2008-02-10 Debbie A. Cook Removed the (-1.) in case A of DPolynomial since it was
   *                      not actually part of the position derivation but how it was being applied
   *                      in BundleAdjust.
   *  @history 2008-06-18 Steven Lambright Fixed documentation
   *  @history 2008-06-25 Debbie A. Cook  Added members p_velocity, p_cacheVelocity,
   *                      and p_hasVelocity; also added methods Velocity() and HasVelocity()
   *                      to support miniRF.
   *  @history 2008-06-26 Debbie A. Cook Replaced Naif error handling calls with Isis NaifStatus
   *  @history 2009-08-03 Jeannie Walldren - Added methods
   *                      ReloadCache( table ), HermiteIndices(),
   *                      Memcache2HermiteCache() to allow for
   *                      downsized instrument position tables in
   *                      labels of Isis cubes and added methods
   *                      SetEphemerisTimeSpice(),
   *                      SetEphemerisTimeHermiteCache(), and
   *                      SetEphemerisTimeMemcache() to make
   *                      software more readable.
   *  @history 2009-08-14 Debbie A. Cook - Corrected loop counter in HermiteIndices
   *  @history 2009-08-27 Jeannie Walldren - Added documentation.
   *  @history 2009-10-20 Debbie A. Cook - Corrected calculation of extremum in ReloadCache
   *  @history 2009-11-06 Debbie A. Cook - Added velocity partial derivative method
   *  @history 2009-12-15 Debbie A. Cook - Changed enumerated partial types and argument list for
   *                       CoordinatePartial and VelocityPartial
  *  @history 2010-03-19 Debbie A. Cook - Added argument coeffIndex to method Velocity Partial
  */
  class SpicePosition {
    public:
      SpicePosition(int targetCode, int observerCode);

      //! Destructor
      virtual ~SpicePosition() {}

      void SetTimeBias(double timeBias);
      void SetAberrationCorrection(const std::string &correction);
      const std::vector<double> &SetEphemerisTime(double et);


//      enum PartialType {WRT_X0,WRT_X1,WRT_X2,
//                        WRT_Y0,WRT_Y1,WRT_Y2,
//                        WRT_Z0,WRT_Z1,WRT_Z2};
      enum PartialType {WRT_X, WRT_Y, WRT_Z};


      enum Coefficient { A, B, C };

      //! Return the current ephemeris time
      double EphemerisTime() const {
        return p_et;
      };

      //! Return the current J2000 position
      const std::vector<double> &Coordinate() {
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
      void ReloadCache(Isis::PolynomialUnivariate &function1, Isis::PolynomialUnivariate &function2,
                       Isis::PolynomialUnivariate &function3);
      void ReloadCache(Table &table);

      Table Cache(const std::string &tableName);

      //! Is this position cached
      bool IsCached() const {
        return (p_cache.size() > 0);
      };

      void SetPolynomial();

      void SetPolynomial(const std::vector<double>& XC,
                         const std::vector<double>& YC,
                         const std::vector<double>& ZC);

      void GetPolynomial(std::vector<double>& XC,
                         std::vector<double>& YC,
                         std::vector<double>& ZC);

      void ComputeBaseTime();

      //! Return the base time for the position
      double GetBaseTime() {
        return p_baseTime;
      };

      void SetOverrideBaseTime(double baseTime);

      double DPolynomial(const Coefficient coeffIndex);

      std::vector<double> CoordinatePartial(SpicePosition::PartialType partialVar, int coeffIndex);

      std::vector<double> VelocityPartial(SpicePosition::PartialType partialVar, int coeffIndex);


      //??? jw
      /**
       * This enum defines indicates the status of the object
       */
      enum Source { Spice,      //!< Object is reading directly from the kernels
                    Memcache,   //!< Object is reading from cached table
                    HermiteCache //!< Object is reading from splined table
                  };
      void Memcache2HermiteCache(double tolerance);
    protected:
      void SetEphemerisTimeMemcache();
      void SetEphemerisTimeHermiteCache();
      void SetEphemerisTimeSpice();
      std::vector<int> HermiteIndices(double tol, std::vector <int> indexList);

    private:
      int p_targetCode;                   //!< target body code
      int p_observerCode;                 //!< observer body code

      double p_timeBias;                  //!< iTime bias when reading kernels
      std::string p_aberrationCorrection; //!< Light time correction to apply

      double p_et;                        //!< Current ephemeris time
      std::vector<double> p_coordinate;   //!< J2000 position at time et
      std::vector<double> p_velocity;     //!< J2000 velocity at time et

      Source p_source;                    //!< Enumerated value for the location of the SPK information used
      std::vector<double> p_cacheTime;    //!< iTime for corresponding position
      std::vector<std::vector<double> > p_cache;         //!< Cached positions
      std::vector<std::vector<double> > p_cacheVelocity; //!< Cached velocities
      std::vector<double> p_coefficients[3];             //!< Coefficients of polynomials
      double p_baseTime;                  //!< Base time used in fit equations
      bool p_noOverride;                  //!< Flag to compute base time;
      double p_overrideBaseTime;          //!< Value set by caller to override computed base time
      bool p_hasVelocity;                 //!< Flag to indicate velocity is available
  };
};

#endif
