#ifndef Column_h
#define Column_h

/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/10/14 18:15:36 $
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
   *   @history 2009-10-14 Eric Hyer Added documentation;
   *                                 Moved from base/apps/cubediff to base/objs;
   */
  class Column {
  public:

    //! Alignment of data in the Column
    enum Align {
      //! no alignment
      NoAlign = 0,
      //! right alignment
      Right = 1,
      //! left alignment
      Left = 2,
      //! decimal alignment
      Decimal = 3
    };

    //! Type of data in the Column
    enum Type {
      //! No data type
      NoType = 0,
      //! Integer data type
      Integer = 1,
      //! Real data type
      Real = 2,
      //! String data type
      String = 3,
      //! Pixel data type
      Pixel = 4
    };
    
    Column ();
    Column (std::string name, int width, Column::Type type, Align align=Column::Right);
    void SetName (std::string name);
    void SetWidth (unsigned int width);
    void SetType (Column::Type type);
    void SetAlignment (Column::Align alignment);
    void SetPrecision (unsigned int precision);

    //! get the Column's name
    std::string Name() { return p_name; };

    //! get the Column's width
    unsigned int Width() { return p_width; };

    Column::Type DataType();

    //! get the Column's alignment
    Column::Align Alignment() { return p_align; };

    //! get the Column's precision
    unsigned int Precision() { return p_precision; };

  private:
    //! Name of the Column
    std::string p_name;
    
    //! Width of the Column
    unsigned int p_width;
    
    //! Type of the data in the Column
    Column::Type p_type;
    
    //! Alignment of the data in the Column
    Column::Align p_align;
    
    //! Precision of the data in the Column
    unsigned int p_precision;
  };
};

#endif
