/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
  FramingCamera::FramingCamera(Cube &cube) : Camera(cube) {
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

