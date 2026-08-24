#ifndef lrowaccal_h
#define lrowaccal_h

#include <cmath>

#include "Cube.h"
#include "UserInterface.h"


namespace Isis {
  extern void lrowaccal(UserInterface &ui);
  extern void lrowaccal(Cube *icube, UserInterface &ui);

  namespace LroWacCal {
    constexpr int POLAR_MODE_SAMPLES = 1024;
    constexpr int NO_POLAR_MODE_SAMPLES = 704;
    constexpr int BW_BANDS = 1;
    constexpr int VIS_LINES = 14;
    constexpr int COLOR_BANDS = 5;
    constexpr int UV_SAMPLES = 128;
    constexpr int UV_LINES = 4;
    constexpr int UV_BANDS = 2;
    constexpr double KM_PER_AU = 149597871;

    /**
     * @brief Structure for storing list of available dark file temps/times.
     */
    struct DarkFileInfo {
      double temp;
      int time;

      DarkFileInfo(double temp, int time) {
        this->temp = temp;
        this->time = time;
      }
    };

    /**
     * @brief DarkFileInfo comparison object.
     *
     * Used for sorting DarkFileInfo objects. Sort first by difference from WAC temp, then difference from WAC time
     */
    struct DarkComp {
      double wacTemp;
      int wacTime;

      DarkComp(double wacTemp, int wacTime) {
        this->wacTemp = wacTemp;
        this->wacTime = wacTime;
      }

      // sort dark files by distance from wac temp, then distance from wac time
      bool operator()(const DarkFileInfo &A, const DarkFileInfo &B) {
        if (std::abs(wacTemp - A.temp) < std::abs(wacTemp - B.temp)) {
          return true;
        }
        if (std::abs(wacTemp - A.temp) > std::abs(wacTemp - B.temp)) {
          return false;
        }
        if (std::abs(wacTime - A.time) < std::abs(wacTime - B.time)) {
          return true;
        }
        return false;
      }
    };

    /**
     * @brief Struct for holding calibration parameters.
     */
    struct CalParams {
      bool dark = true;
      bool flatfield = true;
      bool radiometric = true;
      bool iof = true;
      bool specpix = true;
      bool temperature = true;
      double exposure = 1.0;       //!< Exposure duration
      double solarDistance = 1.01; //!< average distance in [AU]
    };
  }
}

#endif
