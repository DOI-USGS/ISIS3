#if !defined(HiBlob_h)
#define HiBlob_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $
 * $Date: 2008/05/12 18:17:33 $
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
#include "Blobber.h"
#include "HiCalTypes.h"

namespace Isis {

/**                                                   
 * @brief BLOB extraction class
 *  
 * This class provides access and processing of HiRISE data as
 * stored in an ISIS BLOB (Table).
 * 
 * Note that the file provided must be a HiRISE ISIS cube file
 * that is freshly converted from a PDS compatable EDR
 * (hi2isis).  It must contain an ISIS Table named "HiRISE
 * Calibration Image".  From that table, data is extracted from
 * the "Calibration" field.
 * 
 * @ingroup MarsReconnaissanceOrbiter                                      
 *                                                    
 * @author 2007-10-09 Kris Becker
 * @see Blobber
 * 
 * @internal
 *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
 */
  class HiBlob : public Blobber {
    public:
      /**
       * @brief Default, mostly useless constructor
       */
      HiBlob(): Blobber() { }

      /**
       * @brief Constructor providing interface to an ISIS Cube object
       */
      HiBlob(Cube &cube, const std::string &tblname, 
             const std::string &field, 
             const std::string &name = "HiBlob") : 
               Blobber(cube, tblname, field, name)  { }

      /** Destructor */
      virtual ~HiBlob() { }

      /** Return a reference to the data */
      const HiMatrix &buffer() const { return (ref()); }

   };
};

#endif
