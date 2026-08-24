#ifndef lrowaccal_h
#define lrowaccal_h

#include <array>
#include <cmath>
#include <vector>

#include <QString>

#include "Buffer.h"
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

    void CopyCubeIntoBuffer(QString &fileString, Buffer *&data);

    /**
     * @brief Finds 2 best dark files for WAC calibration.
     *
     * GetDark will find the 2 closest available dark file temperatures matching the given file name
     * pattern. Then find the dark file at each temperature with the time closest to the WAC temperature.
     * If there is only one temperature, it will pick the 2 closest times at that temperature.
     *
     *
     * @param fileString String pattern defining dark files to search (ie. lro/calibration/wac_darks/WAC_COLOR_Offset68_*C_*T_Dark.????.cub)
     * @param temp Temperature of WAC being calibrated
     * @param time Time of WAC being calibrated
     * @param data1 Buffer to hold dark file 1 cub data
     * @param data2 Buffer to hold dark file 2 cub data
     * @param temp1 Temperature of dark file 1
     * @param temp2 Temperature of dark file 2
     * @param file1 Filename of dark file 1
     * @param file2 Filename of dark file 2
     */
    void GetDark(const QString &fileString, double temp, double time, Buffer *&data1, Buffer *&data2, double &temp1,
                 double &temp2, QString &file1, QString &file2);

    void GetMask(QString &fileString, double temp, Buffer *&data);

    /**
    * @brief Returns a QString containing the path of an LRO calibration directory
    *
    * @param calibrationType The type of calibration data
    *
    * @return @b QString Path of the calibration directory
    *
    * @internal
    *   @history 2008-11-05 Jeannie Walldren - Original version
    *   @history 2016-08-16 Victor Silva - Added option for base calibration directory
    */
    QString GetCalibrationDirectory(QString calibrationType);

    void CorrectDark(const Buffer &in, Buffer &out, int correctBand, double startTemp, double endTemp, int frame,
                     int frameHeight, int frameSize, int numFrames, double frameTemp, Buffer *darkCube1, double temp1,
                     Buffer *darkCube2, double temp2);

    void CorrectFlatfield(const Buffer &in, Buffer &out, int correctBand, int frame, int frameHeight, int frameSize,
                          Buffer *flatCube);

    void CorrectRadiometric(Buffer &out, double exposure, double solarDistance, bool iof,
                            const std::vector<double> &iofResponsivity,
                            const std::vector<double> &radianceResponsivity);

    void CorrectSpecialPixels(const Buffer &in, Buffer &out, int correctBand, int frame, int frameHeight,
                              int frameSize, Buffer *specpixCube);

    void CorrectTemperature(Buffer &out, int correctBand, double frameTemp,
                            const std::array<std::array<double, 2>, 7> &temperatureConstants);
  }
}

#endif
