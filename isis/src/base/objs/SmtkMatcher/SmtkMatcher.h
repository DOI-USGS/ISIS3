#ifndef SmtkMatcher_h
#define SmtkMatcher_h

/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/09/09 23:42:41 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <memory>


#include <QSharedPointer>
#include "SmtkStack.h"
#include "Gruen.h"
#include "SmtkPoint.h"

#include "GSLUtility.h"
#include <gsl/gsl_rng.h>
#include "NaifContext.h"


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

    bool isValid(NaifContextPtr naif, const Coordinate &pnt);
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

    SmtkPoint Register(NaifContextPtr naif, const Coordinate &lpnt,
                       const AffineRadio &affrad = AffineRadio());
    SmtkPoint Register(NaifContextPtr naif, const PointPair &pnts,
                       const AffineRadio &affrad = AffineRadio());
    SmtkPoint Register(NaifContextPtr naif, const SmtkPoint &spnt,
                       const AffineRadio &affrad = AffineRadio());
    SmtkPoint Register(NaifContextPtr naif, const PointGeometry &lpg, const PointGeometry &rpg,
                       const AffineRadio &affrad  = AffineRadio());

    SmtkPoint Create(NaifContextPtr naif, const Coordinate &left, const Coordinate &right);
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

    Coordinate getLineSample(NaifContextPtr naif, Camera &camera, const Coordinate &geom);
    Coordinate getLatLon(NaifContextPtr naif, Camera &camera, const Coordinate &pnt);

    bool inCube(const Camera &camera, const Coordinate &point) const;

    SmtkPoint makeRegisteredPoint(NaifContextPtr naif, 
                                  const PointGeometry &left,
                                  const PointGeometry &right, Gruen *gruen);
};
} // namespace Isis

#endif
