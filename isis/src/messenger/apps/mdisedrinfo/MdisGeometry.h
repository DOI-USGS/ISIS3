#if !defined(MdisGeometry_h)
#define MdisGeometry_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $
 * $Date: 2009/09/19 00:12:10 $
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

#include <string>
#include <vector>
#include <iostream>

#include "Pvl.h"
#include "Cube.h"
#include "Camera.h"
#include "SpiceManager.h"
#include "iException.h"

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
   * @history 2008-04-14 Kris Becker - Removed the isTargetValid() method as it 
   *          was not implemented; Corrected computation of
   *          SC_TARGET_POSITION_VECTOR and TARGET_CENTER_DISTANCE when target
   *          is anything but Sky (it was previously only computed when the
   *          center reference pixel intersected the target).
   * @history 2009-09-18 Debbie A. Cook - corrected pxlscl and explen in MdisGeometry
   */
class MdisGeometry {
    public:

        /** Default constructor */
      MdisGeometry() : _label(), _orglabel(), _nSubframes(0), _camera(0),
                       _digitsPrecision(_defaultDigits), 
                       _NullDefault("\"N/A\""), _doUpdate(true), _spice() { }
      MdisGeometry(const std::string &filename);
      MdisGeometry(Cube &cube);
      virtual ~MdisGeometry() { delete _camera; }

      void setCube(const std::string &filename);
      static bool validateTarget(Pvl &label, bool makeValid = true);
      virtual void refCenterCoord(double &sample, double &line) const;
      virtual void refUpperLeftCoord(double &sample, double &line) const;
      virtual void refUpperRightCoord(double &sample, double &line) const;
      virtual void refLowerLeftCoord(double &sample, double &line) const;
      virtual void refLowerRightCoord(double &sample, double &line) const;
      Pvl getGeometry(const std::string &filename);


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
      void setNull(const std::string &nullstring) {
        _NullDefault = nullstring;
      }

      /**
       * @brief Value in use for uncomputable values
       * 
       * @return std::string Uncomputed value string
       */
      std::string getNull() const {
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
    const Camera &getCamera() const { return (*_camera); }
    /** Return reference to Camera model */
    Camera &getCamera() { return (*_camera); }

  private:
    enum { _defaultDigits = 5 };     //!< (PDS) Default digits of precision
    Pvl           _label;            //!< Label used to initialize camera model
    Pvl           _orglabel;         //!< Original label of PDS product
    int           _nSubframes;       //!< Number subframes in image
    Camera       *_camera;           //!< Camera model initiated from cube label
    int           _digitsPrecision;  //!< Current digits of precision
    std::string   _NullDefault;      //!< Current null string
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

    PvlKeyword format(const std::string &name, const double &value, 
                      const std::string &unit = "") const;
    PvlKeyword format(const std::string &name, const std::vector<double> &values, 
                      const std::string &unit = "") const;
    PvlKeyword format(const std::string &name, 
                      const std::vector<std::string> &values, 
                      const std::string &unit = "") const;
    iString DoubleToString(const double &value) const;
};

}     // namespace Isis
#endif

