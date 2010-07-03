#ifndef Isis_GuiFilenameParameter_h
#define Isis_GuiFilenameParameter_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2009/12/15 20:44:57 $                                                                 
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

#include "GuiParameter.h"

namespace Isis {

/**                                                                       
 * @author 2006-10-31 ??? 
 *                                                                        
 * @internal                                                              
 *   @history 2009-11-10 Mackenzie Boyd - Moved SelectFile
 *            method up to parent class GuiParameter, also moved
 *            member pointers to QToolButton and QLineEdit up.
 *   @history 2009-12-15 Travis Addair - Fixed bug with button
 *            for opening file dialog, made this class a parent
 *            for GuiCubeParameter, and moved SelectFile method
 *            back to this class, no longer prompting users to
 *            confirm overwriting a file.
 */

  class GuiFilenameParameter : public GuiParameter {

    Q_OBJECT

    public:

      GuiFilenameParameter (QGridLayout *grid, UserInterface &ui,
                        int group, int param);
      ~GuiFilenameParameter ();

      iString Value ();

      void Set (iString newValue);

    protected slots:
      virtual void SelectFile();
  };
};



#endif

