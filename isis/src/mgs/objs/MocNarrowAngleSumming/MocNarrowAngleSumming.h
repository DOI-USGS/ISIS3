#ifndef MocNarrowAngleSumming_h
#define MocNarrowAngleSumming_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  /**
   * Mars Global Surveyor MOC narrow angle summing class.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsGlobalSurveyor
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed Mgs
   *            namespace wrap inside Isis namespace. Added Isis Disclaimer to
   *            file.  Removed Mgs scope from unitTest and truth file.
   */
  class MocNarrowAngleSumming {
    public:
      /**
       * Constructs the MocNarrowAngleSumming object
       * @param csum
       * @param ss
       */
      MocNarrowAngleSumming(int csum, int ss) {
        p_csum = csum;
        p_ss = ((double) csum / 2.0) + 0.5 + (double)(ss - 1);
      }

      //! Destroys the MocNarrowAngleSumming object
      ~MocNarrowAngleSumming() {};

      /**
       * Given the sample value, this method computes the corresponding
       * detector.
       * @param sample
       * @return @b double Detector
       */
      inline double Detector(double sample) const {
        return (sample - 1.0) * (double) p_csum + p_ss;
      }

      /**
       * Given the detector value, this method computes the corresponding
       * sample.
       * @param detector
       * @return @b double Sample
       */
      inline double Sample(double detector) const {
        return (detector - p_ss) / ((double) p_csum) + 1.0;
      }

    private:
      int p_csum;
      double p_ss;

  };
};
#endif
