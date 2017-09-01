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

#include <QString>

namespace Isis {
  /**
   * @brief Format ascii tables
   *
   * This class takes in a series of string vectors and writes them out to a
   * file as a table. Formatting options are up to the user. This was developed for
   * cubediff's table output option, and is being utilized by the WriteTabular class
   * (which currently resides in the cubediff folder).
   * 
   *
   * @ingroup Utility
   *
   * @author 2007-05-01 Brendan George
   *
   * @internal
   *   @history 2007-06-18 Brendan George Fixed error message outputs and
   *                           unitTest
   *   @history 2009-10-14 Eric Hyer Added documentation;
   *                           Moved from base/apps/cubediff to base/objs;
   *   @history 2012-09-20 Steven Lambright - Improved unit test, fixed bug where uninitialized
   *                           member variables were causing errors to be thrown sometimes.
   *                           Fixes #1125.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   */
  class Column {
    public:

      /**
       * Alignment of data in the Column
       */
      enum Align {
        NoAlign = 0,  //!< no alignment
        Right   = 1,  //!< right alignment
        Left    = 2,  //!< left alignment
        Decimal = 3   //!< decimal alignment
      };

      /**
       * Type of data in the Column
       */
      enum Type {
        NoType  = 0,  //! No data type
        Integer = 1,  //! Integer data type
        Real    = 2,  //! Real data type
        String  = 3,  //! String data type
        Pixel   = 4   //! Pixel data type
      };

      Column();
      Column(QString name, int width, Column::Type type, Column::Align align = Right);
      void SetName(QString name);
      void SetWidth(unsigned int width);
      void SetType(Column::Type type);
      void SetAlignment(Column::Align alignment);
      void SetPrecision(unsigned int precision);

      QString Name() const; 
      unsigned int Width() const;
      Column::Type DataType() const;
      Column::Align Alignment() const;
      unsigned int Precision() const;


    private:
      //! Name of the Column
      QString p_name;

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
