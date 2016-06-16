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

#include <QSharedPointer>

#include <vector>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

class QString;

namespace Isis {
  class Angle;
  class Distance;
  class Pvl;
  class PvlGroup;
  class ShapeModel;
  class Spice;

  /**
   * This class is used to create and store valid Isis3 targets.
   *
   * @author 2012-03-20 Debbie A. Cook
   *
   * @internal
   *  @history 2015-07-31 Kristin Berry - Added additional NaifStatus::CheckErrors() to see if any
   *                          NAIF errors were signaled. References #2248.
   *  @history 2016-05-18 Jeannie Backer - Moved TProjection::TargetRadii() methods to
   *                          Target::radiiGroup() methods. Added overloaded 
   *                          lookupNaifBodyCode(QString) to have a generic static method that 
   *                          takes the TargetName as an input parameter. Added overloaded 
   *                          lookupNaifBodyCode(Pvl) to use the label passed into Target's 
   *                          constructor to find the code if not found using
   *                          the name or spice pointer provided. References #3934.
   *  @history 2016-05-18 Jeannie Backer - Removed unused lookupNaifBodyCode() method that takes no
   *                          input parameters (since it was replaced with lookupNaifBodyCode(Pvl)).
   *                          References #3934.
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
      SpiceInt naifPlanetSystemCode() const;
      QString name() const;
      QString systemName() const;
      std::vector<Distance> radii() const;
      void restoreShape();
      void setShapeEllipsoid();
      void setRadii(std::vector<Distance> radii);
      ShapeModel *shape() const;
      Spice *spice() const;

      int frameType();

      std::vector<Angle> poleRaCoefs();
      std::vector<Angle> poleDecCoefs();
      std::vector<Angle> pmCoefs();

      static SpiceInt lookupNaifBodyCode(QString name);
      // Static conversion methods
      static PvlGroup radiiGroup(QString target);
      static PvlGroup radiiGroup(Pvl &cubeLab, const PvlGroup &mapGroup);

      std::vector<double> poleRaNutPrecCoefs();
      std::vector<double> poleDecNutPrecCoefs();

      std::vector<double> pmNutPrecCoefs();

      std::vector<Angle> sysNutPrecConstants();
      std::vector<Angle> sysNutPrecCoefs();

    private:
      SpiceInt lookupNaifBodyCode(Pvl &lab) const;
      static PvlGroup radiiGroup(int bodyFrameCode);
      SpiceInt *m_bodyCode;          /**< The NaifBodyCode value, if it exists in the
                                       labels. Otherwise, if the target is sky,
                                       it's the SPK code and if not sky then it's
                                       calculated by the NaifBodyCode() method.*/
      SpiceInt *m_systemCode;        /**< The NaifBodyCode of the targets planetary system
                                       If the target is sky, then what should this be???*/
      QString *m_name;               //!< target name
      QString *m_systemName;         //!< name of the planetary system of the target
      std::vector<Distance> m_radii; //!< target radii
      ShapeModel *m_originalShape;   //!< target original shape model
      ShapeModel *m_shape;           //!< target shape model
      bool m_sky;                    //!< flag indicating target is the sky

      // TODO should this be an enum(ring, sky, or naifBody), created Naif body for sky, or ???
      // TODO should the target body kernels go in here too bodyRotation and position??? I don't
      //           think so.  They are SPICE kernels and belong in the Spice class (DAC).  What do others
      //           think.
      Spice *m_spice;                /**< parent Spice object, needed to get pixel resolution in
                                       ShapeModels*/
  };

  typedef QSharedPointer<Target> TargetQsp;
}

#endif
