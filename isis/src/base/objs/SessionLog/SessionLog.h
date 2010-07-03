#if !defined(SessionLog_h)
#define SessionLog_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/07/08 22:16:43 $
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
#include "Pvl.h"
#include "iException.h"

namespace Isis {
/**                                        
 * 
 * @internal                                                              
 *  @history 2005-12-28 Elizabeth Miller - Added try/catch block in Write() to 
 *                                         catch an error that causes an abort
 *  @history 2008-01-09 Steven Lambright - Made Application a friend for
 *                                         destruction purposes
 *   @history 2008-07-08 Steven Lambright - Now uses atexit for destruction
 *  @todo 2005-02-15 Write class documentation                                                     
 */                                                                     
  class SessionLog : private Isis::Pvl {
    public:
      static SessionLog &TheLog(bool restart=false);
      static bool HasLog() { return p_log != NULL; };

      // Add the Results group
      void AddResults(Isis::PvlGroup &results);
  
      // Write the log to the screen and/or file
      void Write ();
  
      // Add an Error to the log
      void AddError (Isis::Pvl &e);
  
      // Will we be logging to the terminal?
      bool TerminalOutput() { return p_termOutput; };

    private:
      SessionLog();
      ~SessionLog();

      PvlObject *p_root;
      bool p_errorAdded;

      bool p_termOutput;
      bool p_fileOutput;
      std::string p_outputFile;
      std::string p_access;

      static SessionLog *p_log;

      bool p_acctAdded;
      void AddAccounting();
      friend std::ostream& operator<<(std::ostream &os, Isis::SessionLog &log);
      static void Shutdown();
  };
};

#endif
