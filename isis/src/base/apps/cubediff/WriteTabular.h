#if !defined(NewClass_h)
#define NewClass_h
/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2007/08/09 18:24:24 $
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

#include <ostream>

#include "Column.h"

namespace Isis {
  /**
   * @brief Format ascii tables
   * 
   * This class takes in a series of string vectors and writes them out to a
   * file as a table. Formatting options are up to the user.
   * 
   * @ingroup Utility
   * 
   * @author 2007-05-01 Brendan George
   */
  class WriteTabular {
    public:
      // Constructor and Destructor
      WriteTabular(std::ostream &strm, std::vector <Column> cols );
      WriteTabular(std::ostream &strm );

      void SetColumns( std::vector <Column> cols );
      void Write();
      void Write (int item);
      void Write (std::string item);
      void Write (double item);
      void SetDelimiter(std::string delim);

      int Columns() {return p_cols.size();};
      int Rows() {return p_rows;};

    private:
      std::string p_delimiter;
      std::ostream &p_outfile;
      unsigned int p_rows;
      std::vector <Column> p_cols;
      unsigned int p_curCol;

  };
}
#endif
