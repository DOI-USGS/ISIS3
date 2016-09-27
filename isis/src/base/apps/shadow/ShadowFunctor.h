#ifndef ShadowFunctor_H
#define ShadowFunctor_H

// Default argument requires knowledge of the type
#include "Distance.h"

template <typename A, typename B> class QHash;
template <typename A, typename B> struct QPair;

namespace Isis {
  class Buffer;
  class Camera;
  class Cube;
  class FileName;
  class PvlGroup;
  class Spice;
  class Statistics;

  /**
   * @brief Shades and shadows a DEM
   *
   * This functor is designed to shade and shadow a DEM given a sun position. This class is neither
   *   re-entrant nor thread safe (re-entrancy blocked by underlying NAIF API, thread safety not
   *   implemented due to caching/optimizations that isn't locked). See the 'shadow' program
   *   documentation for a more detailed explanation.
   *
   * @author 2012-11-14 Steven Lambright
   *
   * @internal
   */
  class ShadowFunctor {
    public:
      /**
       * These settings profiles enable/disable optimizations to adjust CPU/RAM/accuracy
       *   in a clean (more user friendly) way.
       */
      enum QuickSettings {
        /**
         * Balanced performance sacrifices 1-2 pixels worth of shadow accuracy for significant speed
         *   improvements. Memory usage is significant in this preset.
         */
        BalancedPerformance,
        /**
         * High accuracy, though not perfect accuracy, ought to always be well within 1 pixel of
         *   accuracy but will take a significantly larger amount of time. The memory requirements
         *   for this are minimal since the caches are ignored.
         */
        HighAccuracy
      };

      ShadowFunctor(Cube *inputDem);
      ShadowFunctor(const ShadowFunctor &other);
      ~ShadowFunctor();

      PvlGroup report() const;

      void operator()(Buffer &input, Buffer &output) const;

      void setRayPrecision(double approxDemRayTracePrecisionInPixels);
      void setSunPosition(FileName cubeFileNameWithCamToMatch);
      void setSunPosition(Cube *cubeWithCamForSunPos);
      void setSunPosition(Camera *camForSunPosition);
      void setSunPosition(Spice *spiceWithTimeSet);
      void setSunPosition(double *lightTimeCorrectedSunPositionInJ2000);

      void enableInterpolatedOptimizations(bool enable);
      void enableLightCurtain(bool enable, bool adjustElevations = true, int targetCurtainSize = 0);
      void enableShadowCalculations(bool enable);
      void enableShadowTraceToSunEdge(bool enable, Distance sunRadius = Distance());
      void enableShadowMap(bool enable, int targetMapSize = 0);
      void enableWalkingOverShadows(bool enable, int maxSteps = 4);

      void setQuickSettings(QuickSettings settingsProfile);

      void swap(ShadowFunctor &other);
      ShadowFunctor &operator=(const ShadowFunctor &rhs);

    private:
      bool isShadowed(double *rayStartPointInBodyFixed,
                      int demSample, int demLine, Distance demStartRadius,
                      double *directionInBodyFixed, Buffer *quickCache) const;
      void shrinkCaches() const;

      void nullify();

    private:
      /**
       * The input DEM we're processing. This is necessary because we traverse the cube outside of
       *   the input brick (though the input brick is used whenever possible).
       */
      Cube *m_inputDem;
      //! Maximum radius of the DEM cube for ray cut-off purposes (in meters)
      double m_inputDemMax;

      //! Stores if we should do the shadow ray tracing at all
      bool m_enableShadowCalculations;

      //! 1D array with 3 elements (X, Y, Z); units are meters, coordinate system is body fixed.
      double *m_sunPositionInBodyFixed;

      //! Adjust the light curtain to be as low as possible
      bool m_adjustLightCurtainElevations;
      //! Store optimization data for stepped over pixels
      bool m_allowInterpolatedOptimizations;
      //! How many ray steps we can interpolate over shadowed pixels; 0 for disabled
      int m_walkOverShadowMaxSteps;
      //! How far (in input DEM pixels) to step the ray at a time
      double m_rayPrecision;
      //! Radius of the sun; only used if tracing light to the sun edge for shadow calculations
      Distance *m_sunRadius;
      //! Size to shrink shadowed positions cache to after every step
      int m_targetShadowedPositionsSize;
      //! Size to shrink lighted elevations cache to after every step
      int m_targetLightedElevationsSize;
      //! If true, try to calculate the highest point of the sun for the ray trace (else use center)
      bool m_traceLightToSunEdge;

      // These statistics are used for generating the report
      //! Statistics on all of the ray lengths for every shadow computation
      Statistics *m_rayLengthStats;
      //! Statistics on all of the computed azimuths
      Statistics *m_azimuthStats;
      //! Statistics on all of the computed elevations
      Statistics *m_elevationStats;
      //! Statistics containing 0s for in light, 1s for in shadow, special pixels present in input
      Statistics *m_shadowedStats;
      //! Statistics containing 0s for in light, 1s for in shadow, only when ray tracing might apply
      Statistics *m_shadowedByRayStats;


      // Optimization: caching results... violating const correctness here is OK.
      //! This is the "shadow map"
      mutable QHash< QPair<int, int>, bool > *m_shadowedPositions;
      //! This is the "light curtain"
      mutable QHash< QPair<int, int>, double > *m_lightedElevations;
  };
}

#endif
