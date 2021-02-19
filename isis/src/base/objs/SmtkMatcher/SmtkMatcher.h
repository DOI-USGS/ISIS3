#ifndef SmtkMatcher_h
#define SmtkMatcher_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <memory>


#include <QSharedPointer>
#include "SmtkStack.h"
#include "Gruen.h"
#include "SmtkPoint.h"

#include "GSLUtility.h"
#include <gsl/gsl_rng.h>


namespace Isis {

/**
 * @brief Workhorse of stereo matcher
 *
 * This class provides stereo matching functionality to the SMTK toolkit.  It
 * registers points, clones them by adjusting parameters to nearby point
 * locations and manages point selection processes.
 *
 * The Gruen algorithm is initialized here and maintained for use in the
 * stereo matching process.
 *
 * @author 2011-05-28 Kris Becker
 *
 * @internal
 *   @history 2012-12-20 Debbie A. Cook - Removed unused Projection.h
 *                           References #775.
 *   @history 2017-08-18 Summer Stapleton, Ian Humphrey, Tyler Wilson - 
 *                           Changed auto_ptr reference to QSharedPointer
 *                           so this class compiles under C++14.  
 *                           References #4809.
 */
class SmtkMatcher {
  public:
    SmtkMatcher();
    SmtkMatcher(const QString &regdef);
    SmtkMatcher(const QString &regdef, Cube *lhImage, Cube *rhImage);
    SmtkMatcher(Cube *lhImage, Cube *rhImage);
    ~SmtkMatcher();

    void setImages(Cube *lhImage, Cube *rhImage);
    void setGruenDef(const QString &regdef);

    bool isValid(const Coordinate &pnt);
    bool isValid(const SmtkPoint &spnt);

    /** Return pattern chip */
    Chip *PatternChip() const {
      validate();
      return (m_gruen->PatternChip());
    }

    /** Return search chip */
    Chip *SearchChip() const {
      validate();
      return (m_gruen->SearchChip());
    }

    /** Returns the fit chip */
    Chip *FitChip() const {
      validate();
      return (m_gruen->FitChip());
    }

    void setWriteSubsearchChipPattern(const QString &fileptrn = "SmtkMatcher");

    SmtkQStackIter FindSmallestEV(SmtkQStack &stack);
    SmtkQStackIter FindExpDistEV(SmtkQStack &stack, const double &seedsample,
                                 const double &minEV, const double &maxEV);

    SmtkPoint Register(const Coordinate &lpnt,
                       const AffineRadio &affrad = AffineRadio());
    SmtkPoint Register(const PointPair &pnts,
                       const AffineRadio &affrad = AffineRadio());
    SmtkPoint Register(const SmtkPoint &spnt,
                       const AffineRadio &affrad = AffineRadio());
    SmtkPoint Register(const PointGeometry &lpg, const PointGeometry &rpg,
                       const AffineRadio &affrad  = AffineRadio());

    SmtkPoint Create(const Coordinate &left, const Coordinate &right);
    SmtkPoint Clone(const SmtkPoint &point, const Coordinate &left);

    inline BigInt OffImageErrorCount() const { return (m_offImage);  }
    inline BigInt SpiceErrorCount() const { return (m_spiceErr);  }

    /** Return Gruen template parameters */
    PvlGroup RegTemplate() { return (m_gruen->RegTemplate());  }
    /** Return Gruen registration statistics */
    Pvl RegistrationStatistics() { return (m_gruen->RegistrationStatistics()); }

  private:
    SmtkMatcher &operator=(const SmtkMatcher &matcher); // Assignment disabled
    SmtkMatcher(const SmtkMatcher &matcher);            // Copy const disabled

    Cube     *m_lhCube;                // Left image cube (not owned)
    Cube     *m_rhCube;                // Right image cube (not owned)
    QSharedPointer<Gruen> m_gruen;     // Gruen matcher
    BigInt  m_offImage;                // Offimage counter
    BigInt  m_spiceErr;                // SPICE distance error
    bool    m_useAutoReg;              // Select AutoReg features
    const gsl_rng_type * T;            // GSL random number type
    gsl_rng * r;                       // GSL random number generator

    void randomNumberSetup();
    bool validate(const bool &throwError = true) const;

    inline Camera &lhCamera() { return (*m_lhCube->camera());   }
    inline Camera &rhCamera() { return (*m_rhCube->camera());   }

    Coordinate getLineSample(Camera &camera, const Coordinate &geom);
    Coordinate getLatLon(Camera &camera, const Coordinate &pnt);

    bool inCube(const Camera &camera, const Coordinate &point) const;

    SmtkPoint makeRegisteredPoint(const PointGeometry &left,
                                  const PointGeometry &right, Gruen *gruen);
};
} // namespace Isis

#endif
