#ifndef MdisGeometry_h
#define MdisGeometry_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <iostream>

#include "Pvl.h"
#include "Cube.h"
#include "Camera.h"
#include "SpiceManager.h"
#include "IException.h"

namespace Isis {

  /**
   * @brief Computes all MDIS Geometric keywords
   *
   * This class computes a set of MESSENGER/MIDS geometric parameters from an
   * ISIS cube file.  The cube file must have been initialized with SPICE
   * kernels (typically by spiceinit).
   *
   * Many of the parameters are provided by the ISIS Spice/Cameraclass
   * hierarchy, but there are a few that utilize NAIF toolkit
   * functionality/features that require loading of the kernels that otherwise
   * would not be necessary.  These are parameters that require velecity vectors
   * and pixel smear components (currently not provided directly via the ISIS
   * API).
   *
   * Some of the keywords may not be computable for several reasons.  There may
   * not be appropriate kernel data coverage for the specified image acquisition
   * time, the boresight pixel does not intersect the surface, or the corner
   * pixels are off planet.  In these cases, the null string is replaced if the
   * actions is set appropriately.
   *
   * @ingroup Utility
   * @author 2007-08-14 Kris Becker
   * @internal
   *   @history 2008-04-14 Kris Becker - Removed the isTargetValid() method as it was not
   *                           implemented. Corrected computation of SC_TARGET_POSITION_VECTOR and
   *                           TARGET_CENTER_DISTANCE when target is anything but
   *                           Sky (it was previously only computed when the
   *                           center reference pixel intersected the target).
   *   @history 2009-09-18 Debbie A. Cook - corrected pxlscl and explen in MdisGeometry
   *   @history 2012-04-06 Kris Becker - Corrected TWIST_ANGLE computation to
   *                           ensure it is restricted to the 0-360 degree domain.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards.  References #972..
   *   @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References
   *                           #775 and #1114.
   *   @history 2012-11-30 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2015-07-22 Kristin Berry -  Added NaifStatus::CheckErrors() to see if any NAIF
   *                           errors were signaled. References #2248.
   *   @history 2016-05-10 Jeannie Backer - Replaced call to NAIF routine bodn2c with call to static
   *                           method Isis::Target::lookupNaifBodyCode. Reference #3934
   */
  class MdisGeometry {
    public:

      /** Default constructor */
      MdisGeometry() : _label(), _orglabel(), _nSubframes(0), _camera(0),
        _digitsPrecision(_defaultDigits),
        _NullDefault("\"N/A\""), _doUpdate(true), _spice() { }
      MdisGeometry(const QString &filename);
      MdisGeometry(Cube &cube);
      virtual ~MdisGeometry() {
        delete _camera;
      }

      void setCube(const QString &filename);
      static bool validateTarget(Pvl &label, bool makeValid = true);
      virtual void refCenterCoord(double &sample, double &line) const;
      virtual void refUpperLeftCoord(double &sample, double &line) const;
      virtual void refUpperRightCoord(double &sample, double &line) const;
      virtual void refLowerLeftCoord(double &sample, double &line) const;
      virtual void refLowerRightCoord(double &sample, double &line) const;
      Pvl getGeometry(const QString &filename);


      /**
       *  @brief Sets digits of precision for floating point keywords
       *
       *  This method can be used to set the number of digits that follow the
       *  decimal point when formatting floating point keyword values.  PDS will
       *  typically specify 5 digits of precision (which is the defined by
       *  _defaultDigits).
       *
       */
      void setPrecision(int ndigits) {
        _digitsPrecision = ndigits;
      }

      /**
       * @brief Sets the string to be used for uncomputable values
       *
       * Use this method to set the string that will be used when values cannot
       * be computed.  PDS recommends the value of "N/A" for these cases.
       *
       * @param nullstring Value of the string to use
       */
      void setNull(const QString &nullstring) {
        _NullDefault = nullstring;
      }

      /**
       * @brief Value in use for uncomputable values
       *
       * @return QString Uncomputed value string
       */
      QString getNull() const {
        return (_NullDefault);
      }

      /**
       * @brief Select action when values cannot be computed
       *
       * When keyword values cannot be computed, the caller can set the action
       * taken for the keyword.  If action is true, the keyword value is set to
       * the null string.  If false, no action is taken and the keyword is not
       * generated.
       *
       * @param action True sets uncomputable keywords to null string,
       *               false is a noop.
       */
      void updateNullKeys(bool action = true) {
        _doUpdate = action;
      }

    protected:
      /** Return const reference to Camera model */
      const Camera &getCamera() const {
        return (*_camera);
      }
      /** Return reference to Camera model */
      Camera &getCamera() {
        return (*_camera);
      }

    private:
      enum { _defaultDigits = 5 };     //!< (PDS) Default digits of precision
      Pvl           _label;            //!< Label used to initialize camera model
      Pvl           _orglabel;         //!< Original label of PDS product
      int           _nSubframes;       //!< Number subframes in image
      Camera       *_camera;           //!< Camera model initiated from cube label
      int           _digitsPrecision;  //!< Current digits of precision
      QString   _NullDefault;      //!< Current null string
      bool          _doUpdate;         //!< Action when vlue is uncomputable
      SpiceManager _spice;             //!< SPICE kernel manager

      void init(Cube &cube);

      void GeometryKeys(Pvl &geom);
      void TargetKeys(Pvl &geom);
      void SubframeTargetKeys(Pvl &geom);
      bool getSubframeCoordinates(int frameno, double &sample, double &line,
                                  double &width, double &height);
      void SpacecraftKeys(Pvl &geom);
      void ViewingAndLightingKeys(Pvl &geom);
      bool SmearComponents(double &smear_magnitude, double &smear_azimuth);
      std::vector<double> ScVelocityVector();

      PvlKeyword format(const QString &name, const double &value,
                        const QString &unit = "") const;
      PvlKeyword format(const QString &name, const std::vector<double> &values,
                        const QString &unit = "") const;
      PvlKeyword format(const QString &name,
                        const std::vector<QString> &values,
                        const QString &unit = "") const;
      PvlKeyword format(const QString &name,
                        const std::vector<std::string> &values,
                        const QString &unit = "") const;
      QString DoubleToString(const double &value) const;
  };

}     // namespace Isis
#endif
