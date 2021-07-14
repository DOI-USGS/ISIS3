#ifndef Portal_h
#define Portal_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>
#include "PixelType.h"
#include "Buffer.h"

namespace Isis {
  /**
   * @brief  Buffer for containing a two dimensional section of an image
   *
   * This class is a Buffer. The shape of the buffer is two dimensional in the
   * line and sample directions only. The band dimension is always one. This
   * class provides a random access window into a cube. The position can be set
   * to any line, sample and band, including outside the image.
   *
   * If you would like to see Portal being used in implementation,
   * see the ProcessRubberSheet class
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2002-10-09 Stuart Sides
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2005-02-28 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   */
  class Portal : public Isis::Buffer {

    private:
      double p_hotSample;  /**<The sample position within the buffer which is
                               the point of interest. This position is zero
                               based and has no default.*/
      double p_hotLine;    /**<The line position within the buffer which is the
                               point of interest. This position is zero based
                               and has no default.*/

    public:
      // Constructors and Destructors

      /**
       * Constructs a Portal object. The hotspot defaults of (-0.5,-0.5) cause
       * the nearest neighbor to the requested pixel to be returned in the top
       * left corner of the portal buffer
       *
       * @param bufSamps The number of samples in the portal.
       *
       * @param bufLines The number of lines in the portal.
       *
       * @param type Type of pixel in raw buffer
       *
       * @param hotSamp The point of interest within the buffer in the sample
       *                direction. When a buffer is being setup, the hotSamp
       *                will be subtracted from the point of interest to give a
       *                buffer of the requested size around the hot spot. This
       *                number is zero based. Defaults to 0.5
       *
       * @param hotLine The point of interest within the buffer in the line
       *                direction. When a buffer is being setup, the hotLine
       *                will be subtracted from the point of interest to give a
       *                buffer of the requested size around the hot spot. This
       *                number is zero based. Defaults to 0.5
       */
      Portal(const int bufSamps, const int bufLines, const Isis::PixelType type,
             const double hotSamp = -0.5, const double hotLine = -0.5) :
        Isis::Buffer(bufSamps, bufLines, 1, type) {
        p_hotSample = hotSamp;
        p_hotLine = hotLine;
      };

      //! Destroys the Portal object
      ~Portal() {};

      /**
       * Sets the line and sample position of the buffer. The hotspot location
       * is subtracted from this position to set the upper left corner of the
       * buffer.
       *
       * @param sample The sample position of the buffer.
       *
       * @param line The line position of the buffer.
       *
       * @param band The band position of the buffer.
       */
      inline void SetPosition(const double sample, const double line,
                              const int band) {
        Isis::Buffer::SetBasePosition((int)floor(sample - p_hotSample),
                                      (int)floor(line - p_hotLine), band);
      };

      /**
       * Sets the line and sample offsets for the buffer. The defaults of
       * (-0.5,-0.5) cause the nearest neighbor to the requested pixel to be
       * returned in the top left corner of the portal buffer
       *
       * @param sample The sample offset for the buffer. A zero for this
       *               parameter will cause the buffer to be alligned with the
       *               hot spot at the left edge of the buffer. Defaults to 0.5
       *
       * @param line The line offset for the buffer. A zero for this parameter
       *             will cause the buffer to be alligned with the hot spot at
       *             the top edge of the buffer. Defaults to 0.5
       */
      inline void SetHotSpot(const double sample = -0.5, const double line = -0.5) {
        p_hotSample = sample;
        p_hotLine = line;
      };
  };
};

#endif

