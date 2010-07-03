#ifndef hiLab_h
#define hiLab_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/14 21:07:26 $
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

#include "Cube.h"
#include "Pvl.h"

using namespace Isis;
/**                                                                       
 * @brief Process HiRise label
 *                                                                        
 * This class retrieves label keyword values from an Isis3
 * HiRise cube file. This class receives a Cube object from
 * an opened HiRise cube file and has methods to return HiRise
 * specific keyword values from the label.
 *                                                                        
 * @ingroup MarsReconnaissanceOrbiter
 *                                                                        
 * @author 2005-06-29 unknown
 *                                                                                                                                                                                      
 * @internal                                                              
 *  @history 2005-06-29 unknown - Original Version 
 *  @history 2006-08-17 Debbie A. Cook - Added members p_bin and p_tdi
 *                      along with methods to retrieve them and the ccd
 *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
 */                                                                       
class HiLab{
  public:
    HiLab(Cube *cube);

    //! Returns the cpmm number read from the labels of a hiris cube
    int getCpmmNumber(){return p_cpmmNumber;};

    //! Returns the channel read from the labels of a hirise cube
    int getChannel(){return p_channel;};

    //! Returns the bin read as Summing from the labels of a hirise cube
    int getBin(){return p_bin;};

    //! Returns the tdi read from the labels of a hirise cube
    int getTdi(){return p_tdi;};

    int getCcd();

  private:
    int p_cpmmNumber;
    int p_channel;
    int p_bin;
    int p_tdi;
};
#endif
