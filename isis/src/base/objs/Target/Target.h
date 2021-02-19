#ifndef Target_h
#define Target_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   * This class is used to create and store valid Isis targets.
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
   *  @history 2017-08-14 Stuart Sides - Added the ability to use a target code and the
   *                          NaifKeywords to find the radii. Added so osirisrex and spicelib v66.
   *                          References #4947.
   *  @history 2018-10-02 Debbie A. Cook - Fixed method lookupNaifBodyCode to look
   *                          up the Naif body code instead of the Naif body frame code.  We may
   *                          need to add a method to look up the Naif body frame code as well.
   *                          Also moved the try loop attempting to find the radii tagged with the
   *                          Naif body code ahead of the try loop that attempts to find the radii
   *                           tagged with the body frame code in the method radiiGroup.  Fixed
   *                           any mention of Naif body frame code that should be Naif body code.
   *                           These are not the same.  Naif tags the body radii keyword with the
   *                           Naif body code.  The Naif body frame code refers to the orientation
   *                           (SpiceRotation) of the body.  References #4649 and #501.
   *  @history 2021-02-17 Kristin Berry, Jesse Mapel, and Stuart Sides - Added the ability to
   *                          create a Target without SPICE data and later set the sensor
   *                          model pointer.
   */
  class Target {

    public:
      // constructors
      Target(Spice *spice, Pvl &label);
      Target(Pvl &label);
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
      void setName(QString name);
      void setSpice(Spice *spice);

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
