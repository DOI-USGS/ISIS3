#ifndef BundleConstraint_h
#define BundleConstraint_h
/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/27 07:01:33 $
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

namespace Isis {

  /**
   * @brief Base class for all BundleConstraint utility child classes.
   *
   * This abstract class serves as the parent for all child classes implementing constraints in the
   * bundle adjustment.
   *
   * @ingroup ControlNetworks
   *
   * @author 2017-03-03 Ken Edmundson
   *
   * @internal
   *   @history 2017-03-03 Ken Edmundson - Original version.
   */
  class BundleConstraint {

    public:
      BundleConstraint();

      virtual ~BundleConstraint();

//    virtual std::vector<double> stateVector(double et) const = 0;
//    virtual bool hasVelocity() const = 0;
//    double startTime() const;
//    double endTime() const;

    private:
//    double m_startTime; //!< The beginning time for this segment.
//    double m_endTime;   //!< The ending time for this segment.
  };
};

#endif
