#ifndef SerialNumberList_h
#define SerialNumberList_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/11/05 18:42:56 $
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
 *   $ISISROOT/doc/documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <map>
#include <vector>
#include "Progress.h"

namespace Isis {
/**
 * @brief Serial Number list generator
 *
 * Create a list of serial numbers from a list of files
 *
 * @ingroup ControlNetworks
 *
 * @author  2005-08-03 Jeff Anderson
 *
 * @internal
 *
 *  @history 2005-08-03 Jeff Anderson Original Version
 *  @history 2006-02-10 Jacob Danton Added SerialNumber function
 *  @history 2006-02-13 Stuart Sides Added checks to make sure all the serial
 *                      number items have the same target.
 * 
 *  @history 2006-05-31 Tracie Sucharski  Added filename
 *                         function that uses index instead of
 *                         serial number.
 *  @history 2006-06-15 Jeff Anderson Added GetIndex method
 *  @history 2006-06-22 Brendan George Added functions to get
 *                          index, modified/added functions to
 *                          get filename and serial number, and
 *                          modified so that index corresponds
 *                          to order files are input.
 *  @history 2006-08-16 Brendan George Added/fixed error
 *                          checking in FilenameIndex() and
 *                          SerialNumber(string filename).
 *  @history 2006-08-18 Brendan George Modified to use Expanded
 *                          Filename on input, allowing for
 *                          filenames that use environment
 *                          variables
 *  @history 2006-09-13 Steven Koechle Added method to get the
 *                          ObservationNumber when you give it
 *                          an index
 *  @history 2008-01-10 Christopher Austin - Adapted for the new
 *                          ObservationNumber class.
 *   @history 2008-10-30 Steven Lambright - Fixed problem with definition
 *                          of struct Pair, pointed out by "novus0x2a" (Support Board Member)
 *   @history 2009-10-20 Jeannie Walldren - Added Progress flag
 *                          to Constructor
 *   @history 2009-11-05 Jeannie Walldren - Modified number
 *                          of maximum steps for Progress flag
 *                          in Constructor
 */

  class SerialNumberList {
    public:
      SerialNumberList (bool checkTarget=true);
      SerialNumberList (const std::string &list, bool checkTarget=true, Progress *progress=NULL);
      virtual ~SerialNumberList ();

      void Add (const std::string &filename);
      bool HasSerialNumber (const std::string &sn);

      int Size () const;
      std::string Filename (const std::string &sn);
      std::string Filename (int index);
      std::string SerialNumber(const std::string &filename);
      std::string SerialNumber(int index);
      std::string ObservationNumber (int index);

      int SerialNumberIndex(const std::string &sn);
      int FilenameIndex(const std::string &filename);

      std::vector<std::string> PossibleSerialNumbers (const std::string &on);
      
    protected:
      struct Pair {
        std::string filename;
        std::string serialNumber;
        std::string observationNumber;
      };

      std::vector<Pair> p_pairs;
      std::map<std::string,int> p_serialMap;
      std::map<std::string,int> p_fileMap;

      bool p_checkTarget;
      std::string p_target;

  };
};

#endif
