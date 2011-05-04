/**
 * @file
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

#include "iTime.h"
#include "Pvl.h"

using namespace std;
namespace Isis {
  /**
   * Constructs the FramingCamera object
   *
   * @param lab Pvl label used to create the parent Camera object
   */
  FramingCamera::FramingCamera(Pvl &lab) : Camera(lab) {
  }

  /**
   * Returns the shutter open and close times.  This is a pure virtual method
   * and so must be overridden by all child classes. Child classes may call this
   * implementation if the shutter open and close times are found the same way.
   * Namely, the time value is the shutter open time and the exposure duration
   * is added to get the shutter close time.
   *
   * @param exposureDuration Exposure duration value in seconds. Usually, this
   *                         parameter is found in the image labels.
   * @param time An ephemeris time value.  Usually this parameter is the 
   *             StartTime or SpacecraftClockCount value from the image labels.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */

  pair<iTime, iTime> FramingCamera::ShutterOpenCloseTimes(double time,
                                                          double exposureDuration) {
    pair<iTime, iTime> shuttertimes;
    // assume the time passed in is the shutter open time
    shuttertimes.first = time;
    // add exposure duration to get the shutter close time
    shuttertimes.second = time + exposureDuration;
    return shuttertimes;
  }
};

