#if !defined(Column_h)
#define Column_h
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

#include <vector>
#include <string>

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
   * 
   * @internal
   *   @history 2007-06-18 Brendan George Fixed error message outputs and
   *           unitTest
   */
  class Column {
  public:

    enum Align { NoAlign = 0,
                 Right = 1, 
                 Left = 2, 
                 Decimal = 3 };
    enum Type { NoType = 0,
                Integer = 1,
                Real = 2,
                String = 3,
                Pixel = 4 };
    Column ();
    Column (std::string name, int width, Column::Type type, Align align=Column::Right);
    void SetName (std::string name);
    void SetWidth (unsigned int width);
    void SetType (Column::Type type);
    void SetAlignment (Column::Align alignment);
    void SetPrecision (unsigned int precision);

    std::string Name () {return p_name;};
    unsigned int Width (){return p_width;};
    Column::Type DataType ();
    Column::Align Alignment (){return p_align;};
    unsigned int Precision (){return p_precision;};

  private:
    std::string p_name;
    unsigned int p_width;
    Column::Type p_type;
    Column::Align p_align;
    unsigned int p_precision;
  };
};

#endif
