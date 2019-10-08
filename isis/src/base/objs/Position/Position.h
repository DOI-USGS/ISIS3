#ifndef Position_h
#define Position_h
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

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Table.h"
#include "PolynomialUnivariate.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Isis {
  class NumericalApproximation;

  class Position {
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

      Position(int targetCode, int observerCode);

      //! Destructor
      virtual ~Position();

      virtual void SetTimeBias(double timeBias);
      virtual double GetTimeBias() const;

      virtual void SetAberrationCorrection(const QString &correction);
      virtual QString GetAberrationCorrection() const;
      virtual double GetLightTime() const;

      virtual std::vector<std::vector<double>> SetEphemerisTime(double et);
      enum PartialType {WRT_X, WRT_Y, WRT_Z};

      //! Return the current ephemeris time
      virtual double EphemerisTime() const {
        return p_et;
      };

      virtual const std::vector<double> &GetCenterCoordinate();

      //! Return the current J2000 position
      virtual const std::vector<double> &Coordinate() {
        return p_coordinate;
      };

      //! Return the current J2000 velocity
      virtual const std::vector<double> &Velocity();

      //! Return the flag indicating whether the velocity exists
      virtual bool HasVelocity() {
        return p_hasVelocity;
      };

      virtual void LoadCache(double startTime, double endTime, int size);
      virtual void LoadCache(double time);
      virtual void LoadCache(Table &table);
      virtual void LoadCache(json &isd);

      virtual Table LineCache(const QString &tableName);
      virtual Table LoadHermiteCache(const QString &tableName);

      virtual void ReloadCache();
      virtual void ReloadCache(Table &table);

      virtual Table Cache(const QString &tableName);

      //! Is this position cached
      virtual bool IsCached() const {
        return (p_cache.size() > 0);
      };

      virtual void SetPolynomial(const Source type = PolyFunction);

      virtual void SetPolynomial(const std::vector<double>& XC,
                         const std::vector<double>& YC,
                         const std::vector<double>& ZC,
                         const Source type = PolyFunction);

      virtual void GetPolynomial(std::vector<double>& XC,
                         std::vector<double>& YC,
                         std::vector<double>& ZC);

      //! Set the polynomial degree
      virtual void SetPolynomialDegree(int degree);

      //! Return the source of the position
      virtual Source GetSource() {
        return p_source;
      };

      virtual void ComputeBaseTime();

      //! Return the base time for the position
      virtual double GetBaseTime() {
        return p_baseTime;
      };

      virtual void SetOverrideBaseTime(double baseTime, double timeScale);

      //! Return the time scale for the position
      virtual double GetTimeScale() {
        return p_timeScale;
      };

      virtual double DPolynomial(const int coeffIndex);

      virtual std::vector<double> CoordinatePartial(Position::PartialType partialVar, int coeffIndex);

      virtual std::vector<double> VelocityPartial(Position::PartialType partialVar, int coeffIndex);
      enum OverrideType {NoOverrides, ScaleOnly, BaseAndScale};
      virtual void Memcache2HermiteCache(double tolerance);
      virtual std::vector<double> Extrapolate(double timeEt);
      virtual std::vector<double> HermiteCoordinate();
      std::vector<double> LoadTimeCache(int startTime, int endTime, int size);

      virtual int getObserverCode() const;
      virtual int getTargetCode() const;
    protected:
      virtual void SetEphemerisTimeMemcache();
      virtual void SetEphemerisTimeHermiteCache();
      virtual void SetEphemerisTimeSpice();
      virtual void SetEphemerisTimePolyFunction();
      virtual void SetEphemerisTimePolyFunctionOverHermiteConstant();

      virtual std::vector<int> HermiteIndices(double tol, std::vector <int> indexList);

      //======================================================================
      // New methods support for light time correction and swap of observer/target
      Position(int targetCode, int observerCode, bool swapObserverTarget);
      virtual double getAdjustedEphemerisTime() const;
      virtual void computeStateVector(double et, int target, int observer,
                              const QString &refFrame,
                              const QString &abcorr,
                              double state[6], bool &hasVelocity,
                              double &lightTime) const;
      virtual void setStateVector(const double state[6], const bool &hasVelocity);
      virtual void setLightTime(const double &lightTime);
      std::vector<double> p_cacheTime;    //!< iTime for corresponding position
      std::vector<std::vector<double> > p_cache;         //!< Cached positions
      std::vector<std::vector<double> > p_cacheVelocity; //!< Cached velocities
      std::vector<double> p_coordinate;   //!< J2000 position at time et
      std::vector<double> p_velocity;     //!< J2000 velocity at time et
      int p_targetCode;                   //!< target body code
      int p_observerCode;                 //!< observer body code

      double p_timeBias;                  //!< iTime bias when reading kernels
      QString p_aberrationCorrection; //!< Light time correction to apply

      double p_et;                        //!< Current ephemeris time

      //! Hermite spline for x coordinate if Source == HermiteCache
      NumericalApproximation *p_xhermite;
      //! Hermite spline for y coordinate if Source == HermiteCache
      NumericalApproximation *p_yhermite;
      //! Hermite spline for z coordinate if Source == HermiteCache
      NumericalApproximation *p_zhermite;

      Source p_source;                    //!< Enumerated value for the location of the SPK information used
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
      //======================================================================

    private:
      void init(int targetCode, int observerCode,
                const bool &swapObserverTarget = false);
      void ClearCache();
      void LoadTimeCache();
      void CacheLabel(Table &table);
      double ComputeVelocityInTime(PartialType var);
  };
};

#endif
