#ifndef SpiceDbGen_h
#define SpiceDbGen_h

/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/05/02 18:39:28 $
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

#include <iostream>
#include "FileName.h"
#include "Pvl.h"
#include "iString.h"
#include "naif/SpiceUsr.h"
#include "qstringlist.h"
#include "KernelDb.h"
#include "IException.h"

using namespace std;
using namespace Isis;

/**
 * @brief Utility for SPICE kernel interrogation and kerneldb generation
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2010-04-27 Stuart Sides - Modified Direct member to use a vector
 *                           of filters instead of a single string
 *
 */
class SpiceDbGen {

  public:
    SpiceDbGen(iString type);
    PvlObject Direct(iString quality, iString location,
                     std::vector<std::string> & filter);
    void FurnishDependencies(string sclk, string fk);
  private:
    QStringList GetFiles(FileName location, iString filter);
    PvlGroup AddSelection(FileName fileIn);
    PvlGroup FormatIntervals(SpiceCell &coverage, string type);
    PvlGroup GetIntervals(SpiceCell &cover);
    //private instance variables
    iString p_type;
    static const char *calForm;
};

#endif
