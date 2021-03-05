/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// These includes are needed for the 
#include <string>

#include <QString>

#include <SpiceZdf.h>

#include "Kernels.h"
#include "Pvl.h"
#include "PvlGroup.h"


namespace Isis {


//  Prototype: static declarations limits its link scope to code its used in.
    static bool getSpkAbCorrState(std::string &abcorr, 
                           const std::string &idtag = "ID:USGS_SPK_ABCORR");


   /**
     * @brief Check for light time/stellar aberration tag in SPK comments
     *  
     * This function will search through all SPK kernel file comments section 
     * looking for a specified tag.  If the tag is found in any of the loaded 
     * SPK files, then true is returned.  This is intended to indicate that the 
     * light time and stellar aberration correction option needs to be 
     * overridden when querying NAIF position states. 
     *  
     * Note that this routine is reentrant and can be called multiple times from 
     * multiple sources to make this determination. 
     * 
     * @param abcorr Returns the value the found in keyword portion of the tag. 
     *               (For now, we always return "NONE".)
     * @param idtag  Value of the tag to search for in the comments section of 
     *               the SPK.  Currently, the default is "ID:USGS_SPK_ABCORR".
     * 
     * @return bool  Returns true if the tag is found anywhere in the comments 
     *               section of the SPK, otherwise false if it is found.
     */
    bool getSpkAbCorrState(std::string &abcorr,const std::string &idtag) {

      //  Determine loaded-only kernels.  Our search is restricted to only 
      //  kernels that are loaded and, currently, only of SPK type is of 
      //  interest.
      Kernels kernels;
      kernels.Discover();

      //  Init the tag to Qt QString for effective searching
      QString qtag = QString::fromStdString(idtag);
      abcorr = "";

      //  Retrieve list of loaded SPKs from Kernel object
      QStringList spks = kernels.getKernelList("SPK");
      for ( int k = 0 ; k < spks.size() ; k++ ) {
        QString spkFile = spks[k];
        SpiceChar ktype[32];
        SpiceChar source[128];
        SpiceInt  handle;
        SpiceBoolean found;
        //  Get info on SPK kernel mainly the NAIF handle for comment parsing
        kinfo_c(spkFile.toLatin1().data(), sizeof(ktype), sizeof(source), ktype,
                source, &handle, &found);
        if (found == SPICETRUE) {
          // SPK is open so read and parse all the comments.
          SpiceChar commnt[1001];
          SpiceBoolean done(SPICEFALSE);
          SpiceInt n;

          // NOTE it is specially important to read all comments so this routine
          // is reentrant!  NAIF will automatically reset the pointer to the
          // first comment line when and only when the last comment line is
          // read.  This is not apparent in the NAIF documentation.
          while ( !done ) {
            dafec_c(handle, 1, sizeof(commnt), &n, commnt, &done);
            QString cmmt(commnt);
            int pos = 0;
            if ( (pos = cmmt.indexOf(qtag, pos, Qt::CaseInsensitive)) != -1 ) {
              //  We can put more effort into this when the need arises and
              //  we have a better handle on options.
               abcorr = "NONE";
            }
          }
          // Don't need to read any more kernel comments if we found one with
          // the tag in it.
          if ( !abcorr.empty() )  break;
        }
      }
        
      return (!abcorr.empty());
    }

}

