#ifndef Hillshade_H
#define Hillshade_H
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

class QDebug;
class QString;

namespace Isis {
  class Angle;
  class Buffer;

  /**
   * @brief Calculate light intensity reflected off a local slope of DEM
   *
   * This class basically does what the 'shade' application does. This calculates a shaded-relief
   *   cube given 3x3 topographic portals. Inputs include the sun angle (azimuth), solar
   *   elevation (zenith), and resolution (meters per pixel). This was abstracted out from the
   *   shade application, which uses the algorithm described at:
   *     http://people.csail.mit.edu/bkph/papers/Hill-Shading.pdf
   *
   * I (Steven Lambright) took the code, originally authored by Tracie Sucharski, from the shade
   *   program and re-implemented it in this class.
   *
   * This class is re-entrant and const methods are thread-safe.
   *
   * @author 2012-10-25 Steven Lambright
   *
   * @internal
   *   @history 2016-11-21 Makayla Shepherd - Corrected the shaded value algorithm in shadedValue. 
   *                           Fixes #4326.
   */
  class Hillshade {
    public:
      Hillshade();
      Hillshade(Angle azimuth, Angle zenith, double resolution);
      Hillshade(const Hillshade &other);
      ~Hillshade();

      void setAzimuth(Angle azimuth);
      void setZenith(Angle zenith);
      void setResolution(double resolution);

      Angle azimuth() const;
      Angle zenith() const;
      double resolution() const;

      double shadedValue(Buffer &input) const;

      void swap(Hillshade &other);
      Hillshade &operator=(const Hillshade &rhs);

      QString toString() const;

    private:
      //! This is direction of the light, with 0 at north
      Angle *m_azimuth;
      //! This is the altitide of the light, with 0 directly overhead and 90 at the horizon
      Angle *m_zenith;
      //! meters per pixel
      double m_pixelResolution;
  };

  QDebug operator<<(QDebug, const Hillshade &hillshade);
}

#endif
