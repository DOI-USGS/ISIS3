#ifndef MdisCamera_h
#define MdisCamera_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.9 $
 * $Date: 2009/08/31 15:12:30 $
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
#include "FramingCamera.h"

namespace Isis {
  namespace Messenger {
    /**                                                                       
     * @brief MESSENGER MDIS NAC and WAC Camera Model
     *                                        
     * This is the camera model for both MESSENGER MDIS Wide Angle (WAC) and
     * Narrow Angle (NAC) cameras.
     * 
     * This camera model is desinfed to be externally managed as much as
     * possible through the Messenger MDIS instrument kernel (IAK).  See the
     * file $ISIS3DATA/messenger/kernels/iak/mdisAddendum???.ti for details.
     * 
     * @ingroup SpiceInstrumentsAndCameras                                   
     * @ingroup MESSENGER
     *                                                                        
     * @author 2005-07-29 Kris Becker
     * 
     * @internal
     *   @history 2006-10-31 Jeff Anderson - Updated to accomodate Spice class
     *            refactory.
     *   @history 2007-04-24 Kris Becker - Corrected problems with setting Ephemeris
     *            time after the cache is created;  fixed problem with FPU binning
     *            geometry is mapped to detector map as described in the SIS.  This
     *            problem is due to an FPGA coding bug on the camera.
     *   @history 2007-09-05 Kris Becker - Removed test for jailbar imaging mode
     *            as the MDIS team reports it should be treated as a normal
     *            image.
     *   @history 2007-09-06 Kris Becker - Removed test for subframe imaging mode
     *            as the team provided calification on its implications.
     *   @history 2007-12-06 Kris Becker - Added camera distortion model provided 
     *            by Scott Turner, JHU/APL.
     *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
     *            method instead of CreateCache(...).
     *   @history 2009-01-21 Kris Becker Added a new implementation of the MDIS
     *            NAC distortion model contributed by Scott Turner and Lillian
     *            Nguyen at JHU/APL.
     *   @history 2009-06-11 Steven Lambright - Documentation fixes
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class MdisCamera : public FramingCamera {
      public:
        MdisCamera (Isis::Pvl &lab);

        /**
         * This destroys the MdisCamera object
         */
        ~MdisCamera () {};
    };
  };
};

#endif
