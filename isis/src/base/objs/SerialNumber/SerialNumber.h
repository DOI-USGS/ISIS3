#ifndef SerialNumber_h
#define SerialNumber_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.8 $                                                             
 * $Date: 2008/06/18 18:54:11 $                                                                 
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

#include <string>

#include "SerialNumberList.h"

namespace Isis {

/**                                                                       
 * @brief Serial Number composer
 * 
 * A Serial Number is a unique identification for some object such as an 
 * Isis cube. A Serial Number for a specific object must be repeatable. This
 * class is intended to be used to create all Serial Numbers for Isis.
 * 
 * @ingroup ControlNetworks
 *                                                                        
 * @author  2005-07-28 Stuart Sides
 *                                                                        
 * @internal                                                              
 * 
 *  @todo This is only a temporary version. The code needs to be modified
 *  to use a PVL file to determine which keywords to use to create the
 *  Serial Number
 * 
 *  @history 2005-07-28 Stuart Sides Original version
 *  @history 2005-08-02 Jeff Anderson added new static methods for
 *                      serial number composition from a cube or filename
 *  @history 2006-01-24 Jacob Danton Updated the SerialNumber Compose method
 *                                   to use .trn files specific to the mission
 *  @history 2006-06-15 Kris Becker Added the return of the filename as the
 *                                  fallback default condition.  "Unknown" will
 *                                  still be returned if the input source is a
 *                                  label created in memory and does not have
 *                                  a valid serial number signature and it was
 *                                  not read in from a file.  Add a test for
 *                                  this to the unitTest.
 *  @history 2006-12-08 Stuart Sides Added parameter the the Compose methods
 *                                   to allow or disallow defaulting to the
 *                                   filename. This parameter has a default of
 *                                   false. Which will cause "Unknown" to be
 *                                   returned for files where a SN could not be
 *                                   correctly produced.
 *  @history 2007-07-09 Steven Lambright Changed missions translation filename from Missions.trn to
 *                                   MissionName2DataDir.trn
 *  @history 2007-09-11 Steven Koechle Added three ComposeObservation methods
 *                                   and made code reusable by seperating
 *                                   existing code into two private methods.
 *  @history 2007-09-13 Steven Koechle Fixed boolean paramaters passed into
 *           FindSerialTranslation
 *  
 *  @history 2008-01-10 Christpher Austin Removed the use of the system default
 *           file in FindSerialTranslation() and detached ObservationNumber
 *           functionality into its own class
 *  @history 2008-05-09 Steven Lambright Optimized the FindSerialTranslation
 *           method
 *  @history 2008-05-18 Steven Lambright Fixed documentation
 */

  // Forward declarations
  class Pvl;
  class PvlGroup;
  class Cube;

  class SerialNumber {
    public:
      SerialNumber ();

      virtual ~SerialNumber ();

      static std::string Compose (Pvl &label, bool def2filename=false);

      static std::string Compose (Cube &cube, bool def2filename=false);

      static std::string Compose (const std::string &filename, bool def2filename=false);

      static std::string ComposeObservation ( const std::string &sn, SerialNumberList &list, bool def2filename=false );

    protected:

      static std::string CreateSerialNumber (PvlGroup &snGroup, int key);

    private:

      static PvlGroup FindSerialTranslation (Pvl &label);

  }; // End of Class
}; // End of namespace

#endif
