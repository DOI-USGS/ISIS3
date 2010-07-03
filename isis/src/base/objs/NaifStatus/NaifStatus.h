#ifndef NaifStatus_h
#define NaifStatus_h

/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2008/06/17 21:26:19 $
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
 * @brief Class for checking for errors in the NAIF library
 *
 * The Naif Status class looks for errors that have occurred in NAIF calls. If 
 * an error has occurred, it will be converted to an iException.
 * 
 * @author 2008-06-13 Steven Lambright
 *
 */
  class NaifStatus {
    public:
      static void CheckErrors(bool resetNaif = true);
    private:
      static bool initialized;
  };
};

#endif
