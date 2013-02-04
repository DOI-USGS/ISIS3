#ifndef Target_h
#define Target_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/06/19 23:35:38 $
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

#include <vector>

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

class QString;

namespace Isis {
  class Distance;
  class Pvl;
  class ShapeModel;
  class Spice;

  /**
   * This class is used to create and store valid Isis3 targets.
   *
   * @author 2012-03-20 Debbie A. Cook
   *
   * @internal
   */
  class Target {
    public:
      // constructors
      Target(Spice *spice, Pvl &label);
      Target();

      //! Destroys the Target
      ~Target();

      void init();
      bool isSky() const;
      SpiceInt naifBodyCode() const;
      QString name() const;
      std::vector<Distance> radii() const;
      void restoreShape();
      void setShapeEllipsoid();
      void setRadii(std::vector<Distance> radii);
      ShapeModel *shape() const;
      Spice *spice() const;


    private:
      SpiceInt lookupNaifBodyCode() const;
      SpiceInt *m_bodyCode;    /**< The NaifBodyCode value, if it exists in the
                                    labels. Otherwise, if the target is sky,
                                    it's the SPK code and if not sky then it's
                                    calculated by the NaifBodyCode() method.*/
      QString *m_name;   //!< Name of the target
      std::vector<Distance> m_radii; //!< The radii of the target
      ShapeModel *m_originalShape; //!< The shape model of the target
      ShapeModel *m_shape; //!< The shape model of the target
      bool m_sky; //!< Indicates whether the target of the observation is the sky
      // TODO should this be an enum(ring, sky, or naifBody), created Naif body for sky, or ???
      // TODO should the target body kernels go in here too bodyRotation and position??? I don't
      //           think so.  They are SPICE kernels and belong in the Spice class (DAC).  What do others
      //           think.
      Spice *m_spice;     //!< The parent Spice object.  This is needed to get pixel resolution in ShapeModels
  };
};

#endif
