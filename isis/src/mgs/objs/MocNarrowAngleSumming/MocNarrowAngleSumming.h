#ifndef MocNarrowAngleSumming_h
#define MocNarrowAngleSumming_h
/**
 * @file
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

namespace Isis {
  /**
   * Mars Global Surveyor MOC narrow angle summing class. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsGlobalSurveyor
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
