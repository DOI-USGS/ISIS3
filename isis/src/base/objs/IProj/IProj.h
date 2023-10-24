#ifndef IProj_h
#define IProj_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <proj.h>

#include "TProjection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief Proj Map Projection
   *
   * This class provides methods for the forward and inverse projection for
   * any ISIS map file through PROJ. We take a map file and convert it into
   * a PROJ string, that string is then fed into the PROJ projection engine.
   *
   *
   * Please see the TProjection/Projection class for a full accounting of all the methods
   * available.
   *
   * @ingroup MapProjection
   *
   * @author 2023-10-17 Adam Paquette
   *
   * @internal
   *   @history 2023-10-17 Adam Paquette - Built Class
   */

  class IProj : public TProjection {
    public:
      IProj(Pvl &label, bool allowDefaults = false);
      ~IProj();

      QString Name() const;
      QString Version() const;

      PvlGroup Mapping();

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);

      bool XYRange(double &minX, double &maxX,
                   double &minY, double &maxY);

    private:
      void addRadii(std::string &projString);

      QString *m_userOutputProjStr;
      QString *m_userOutputProjType;
      PJ_CONTEXT *m_C;
      PJ *m_geocentricProj;
      PJ *m_llaProj;
      PJ *m_outputProj;
      PJ *m_geocentProj2llaProj;
      PJ *m_llaProj2outputProj;
  };
};

#endif
