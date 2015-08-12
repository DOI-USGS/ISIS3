/**
 * @file
 * $Revision: 1.24 $
 * $Date: 2010/04/09 22:31:16 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "Target.h"

#include "Distance.h"
#include "EllipsoidShape.h"
#include "IException.h"
#include "NaifStatus.h"
#include "Pvl.h"
#include "ShapeModelFactory.h"
#include "Spice.h"


using namespace std;

namespace Isis {

  /**
   * Constructs a Target object and loads target information.
   *
   * @param lab Label containing Instrument and Kernels groups.
   *
   * @author 2012-03-20 Debbie A. Cook
   *
   * @internal
   * @history 2012-10-11 Debbie A. Cook - Original version
   */

  // TODO: what is needed to initialize?
  Target::Target(Spice *spice, Pvl &lab) {

    // Initialize members
    init();
    m_bodyCode = new SpiceInt;
    m_radii.resize(3, Distance());
    m_spice = spice;

    // If we get this far, we know we have a kernels group.  Spice requires it.
    PvlGroup &kernels = lab.findGroup("Kernels", Pvl::Traverse);

    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    m_name = new QString;
    *m_name = inst["TargetName"][0];
    QString trykey = "NaifIkCode";
    if (kernels.hasKeyword("NaifFrameCode")) trykey = "NaifFrameCode";

    if (name().toUpper() == "SKY") {
      m_radii[0] = m_radii[1] = m_radii[2] = Distance(1000.0, Distance::Meters);
      m_sky = true;
      int ikCode = toInt(kernels[trykey][0]);
      *m_bodyCode  = ikCode / 1000;
      // Check for override in kernel group
      if (kernels.hasKeyword("NaifSpkCode"))
        *m_bodyCode = (int) kernels["NaifSpkCode"];
    }
    else {
      *m_bodyCode = lookupNaifBodyCode();
      m_sky = false;
      // IString radiiKey = "BODY" + IString((BigInt) naifBodyCode()) + "_RADII";
      // m_radii[0] = Distance(getDouble(radiiKey, 0), Distance::Kilometers);
      // m_radii[1] = Distance(getDouble(radiiKey, 1), Distance::Kilometers);
      // m_radii[2] = Distance(getDouble(radiiKey, 2), Distance::Kilometers);
    }
    // Override it if it exists in the labels
    if (kernels.hasKeyword("NaifBodyCode"))
      *m_bodyCode = (int) kernels["NaifBodyCode"];
    m_shape = ShapeModelFactory::create(this, lab);
  }


  /**
   * Constructs an empty Target object
   *
   * @author 2012-03-20 Debbie A. Cook
   *
   * @internal
   * @history 2012-08-29 Debbie A. Cook - Original version
   */

  Target::Target() {
    m_bodyCode = NULL;
    m_name = NULL;
    m_spice = NULL;
    init();
 }


  /**
   * Initialize member variables
   *
   * @author 2012-03-20 Debbie A. Cook
   *
   * @internal
   * @history 2012-08-31 Debbie A. Cook - Original version
   */
  void Target::init() {
    m_shape = NULL;
    m_originalShape = NULL;
    m_sky = false;
  }



  /**
   * Destroys the Target object
   */
  Target::~Target() {
    NaifStatus::CheckErrors();

    delete m_bodyCode;
    m_bodyCode = NULL;

    delete m_name;
    m_name = NULL;

    if (m_radii.size() != 0) {
      m_radii.clear();
    }

    delete m_originalShape;
    m_originalShape = NULL;

    delete m_shape;
    m_shape = NULL;
  }


  //! Return if our target is the sky
  bool Target::isSky() const {
    return m_sky;
  }


  /**
   * This returns the NAIF body code of the target indicated in the labels.
   *
   * @return @b SpiceInt NAIF body code
   *
   */
  SpiceInt Target::lookupNaifBodyCode() const {
    NaifStatus::CheckErrors();
    SpiceInt code;
    SpiceBoolean found;
    bodn2c_c(m_name->toAscii().data(), &code, &found);
    if (!found) {
      IString msg = "Could not convert Target [" + *m_name +
                   "] to NAIF code";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    NaifStatus::CheckErrors();
    return  code;
  }


  /**
   * This returns the NAIF body code of the target
   *
   * @return @b SpiceInt NAIF body code
   *
   */
  SpiceInt Target::naifBodyCode() const {
    return *m_bodyCode;
  }


  //! Return target name
  QString Target::name() const {
    return *m_name;
  }


  /**
   * Returns the radii of the body in km. The radii are obtained from the
   * appropriate SPICE kernel for the body specified by TargetName in the
   * Instrument group of the labels.
   */
  std::vector<Distance> Target::radii() const {
    return m_radii;
  }


  /**
   * Restores the shape to the original after setShapeEllipsoid has overridden it.
   */
  void Target::restoreShape() {
    if (m_shape->name()  != "Ellipsoid") {
      // Nothing to do
      return;
    }
    // If we don't have a saved original shape, just stick to the current shape
    else if (m_originalShape == NULL) {
      // Nothing to do
      return;
    }
    m_shape = m_originalShape;
    m_originalShape = NULL;
  }


  /**
   * Set the shape to the ellipsoid and save the original shape.
   */
  void Target::setShapeEllipsoid() {
    // Save the current shape to restore later
    m_originalShape = m_shape;
    m_shape = new EllipsoidShape(this);
  }


  /**
   * Sets the radii of the body.
   *
   * @param r[] Radii of the target in kilometers
   */
  void Target::setRadii(std::vector<Distance> radii) {
    m_radii[0] = radii[0];
    m_radii[1] = radii[1];
    m_radii[2] = radii[2];
  }


  /**
   * Return the shape 
   */
  ShapeModel *Target::shape() const {
    return m_shape;
  }


  /**
   * Return the spice object  
   */
  Spice *Target::spice() const {
    return m_spice;
  }
}
