#ifndef PvlKeyword_h
#define PvlKeyword_h
/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/06/25 20:42:35 $
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
#include <map>
#include <iostream>

#include <QString>
#include <QVariant>
#include <QVarLengthArray>

#include "Constants.h"
#include "IString.h"

namespace Isis {
  class PvlSequence;
  class PvlFormat;

  /**
   * @brief A single keyword-value pair.
   *
   * This class is used to create a single PVL keyword-value pair. PvlContainer
   * can combine PvlKeyword objects and organize them so they look clean on
   * output.
   *
   * @ingroup Parsing
   *
   * @author 2002-10-11 Jeff Anderson
   *
   * @internal
   *  @history 2005-04-08 Leah Dahmer - wrote class documentation.
   *  @history 2005-04-08 Leah Dahmer - added the WriteWithWrap() method so
   *                                    keyword values will now be wrapped when
   *                                    the length exceeds 80 characters.
   *  @history 2005-05-18 Jeff Anderson - Fixed minor problems with wrapping code
   *  @history 2006-04-05 Elizabeth Miller - Added
   *           AddCommentWrapped method
   *  @history 2006-09-05 Stuart Sides - Added ability to format keywords in
   *                                     different ways using the PvlFormat class
   *  @history 2007-08-20 Brendan George - Added checking to ensure Keyword Name
   *                                       contains no whitespace
   *  @history 2008-02-08 Christopher Austin - Altered
   *           WriteWithWrap to not bomb when 2 or more single
   *           statement lines in a row are over 78 characters
   *           long.
   *  @history 2008-07-03 Steven Lambright - Added const functionality
   *  @history 2008-07-10 Steven Lambright - StringEqual is now static, all
   *           AddComments methods are public
   *  @history 2008-09-30 Christopher Austin - replaced all std::endl in the <<
   *           operator as well as WriteWithWrap() with PvlFormat.FormatEOL(), and
   *           formatted wraps accordingly
   *  @history 2009-08-18 Eric Hyer - Added both SetUnits methods and ASSERT macro
   *  @history 2009-09-09 Steven Lambright - Removed ASSERT macro, fixed
   *           formatting of error in SetUnits, and fixed text wrapping when a
   *           single array element needed split up into multiple lines.
   *  @history 2009-12-07 Steven Lambright - Added stream input operator for
   *           reading
   *  @history 2010-01-19 Travis Addair - Added SetCharLimit
   *           method allowing users to set the point at which a
   *           keyword value is output to the next line down.
   *  @history 2010-02-04 Travis Addair - Moved the SetCharLimit
   *           method to PvlFormat class
   *  @history 2010-04-13 Eric Hyer - Added copy constructor
   *                                  Added assignment operator
   *  @history 2010-06-25 Steven Lambright - NULLs ('\0') now count as binary
   *  @history 2010-09-27 Sharmila Prasad - API to Validate a Keyword for type and values
   *  @history 2010-10-18 Sharmila Prasad - Added more options for the keyword validation
   *  @history 2011-04-12 Steven Lambright - Lessened the memory footprint by
   *            changing p_comments and p_units to pointers, p_values to a
   *            QVarLengthArray and p_name to a char *.
   *  @history 2011-07-07 Sharmila Prasad - While validating keyword, display appropriate 
   *                 error msg when converting string to integer which has a double value.
   */
  class PvlKeyword {
    public:
      PvlKeyword();
      PvlKeyword(QString name);
      PvlKeyword(QString name, QString value,
                 QString unit = "");
      PvlKeyword(const PvlKeyword &other);
      ~PvlKeyword();

      void SetName(QString name);

      /**
       * Returns the keyword name.
       *
       * @return The name of the keyword.
       */
      QString Name() const {
        if(p_name)
          return p_name;
        else
          return "";
      };
      /**
       * Determines whether two PvlKeywords have the same name or not.
       *
       * @param name The name of the keyword to compare with this one.
       * @return True if the names are equal, false if not.
       */
      bool IsNamed(QString name) const {
        return StringEqual(name, Name());
      };

      void SetValue(QString value, QString unit = "");

      void SetUnits(QString units);
      void SetUnits(QString value, QString units);

      PvlKeyword &operator=(QString value);

      void AddValue(QString value, QString unit = "");
      PvlKeyword &operator+=(QString value);

      //! Returns the number of values stored in this keyword
      int Size() const {
        return p_values.size();
      };
      bool IsNull(const int index = 0) const;
      void Clear();

      friend std::istream &operator>>(std::istream &is, PvlKeyword &result);
      friend std::ostream &operator<<(std::ostream &os,
                                      const PvlKeyword &keyword);

      //! Returns the first value  in this keyword converted to a double
      operator double() const {
        return toDouble(operator[](0));
      };
      //! Returns the first value  in this keyword converted to an integer
      operator int() const {
        return toInt(operator[](0));
      };
      //! Returns the first value  in this keyword converted to a BigInt
      operator Isis::BigInt() const {
        return toBigInt(operator[](0));
      };

      operator QString() const;

      const QString &operator[](int index) const;
      QString &operator[](int index);
      QString Unit(const int index = 0) const;

      void AddComment(QString comment);
      void AddCommentWrapped(QString comment);

      void AddComments(const std::vector<QString> &comments);

      //! Returns the number of lines of comments associated with this keyword
      int Comments() const {
        return (p_comments ? p_comments->size() : 0);
      };
      QString Comment(const int index) const;
      void ClearComments();

      /**
       * Returns true of the keyword names match
       *
       * @param key The keyword to compare names with
       */
      bool operator==(const PvlKeyword &key) const {
        if(!p_name && !key.p_name) return true;
        if(!p_name || !key.p_name) return false;

        return (StringEqual(p_name, key.p_name));
      };

      /**
       * Returns true of the keyword names do not match
       *
       * @param key The keyword to compare names with
       */
      bool operator!=(const PvlKeyword &key) const {
        return !(*this == key);
      };

      bool IsEquivalent(QString string1, int index = 0) const;

      /**
       *  The width of the longest keyword name (for formatting)
       *
       *  @param width the new width
       */
      void SetWidth(int width) {
        p_width = width;
      };

      /**
       * Sets the indent level when outputted(for formatting)
       *
       * @param indent The new indent
       */
      void SetIndent(int indent) {
        p_indent = indent;
      };

      //! Returns the current set longest keyword name
      int Width() const {
        return p_width;
      };

      //! Returns the current indent level
      int Indent() const {
        return p_indent;
      };

      PvlKeyword &operator=(Isis::PvlSequence &seq);

      void SetFormat(PvlFormat *formatter);
      PvlFormat *GetFormat();

      static bool StringEqual(const QString &string1,
                              const QString &string2);


      static QString ReadLine(std::istream &is, bool insideComment);

      static bool ReadCleanKeyword(QString keyword,
                                   std::vector< QString > &keywordComments,
                                   QString &keywordName,
                                   std::vector< std::pair<QString, QString> >
                                   &keywordValues);

      static QString ReadValue(QString &keyword, bool &quoteProblem);
      static QString ReadValue(QString &keyword, bool &quoteProblem,
                                   const std::vector< std::pair<char, char> > &
                                   otherDelimiters);

      const PvlKeyword &operator=(const PvlKeyword &other);

      //! Validate Keyword for type and required values
      void ValidateKeyword(PvlKeyword & pvlKwrd, QString psValueType="", PvlKeyword* pvlKwrdRange=NULL);

    protected:
      QString Reform(const QString &value) const;
      QString ToPvl(const QString &value) const;
      QString ToIPvl(const QString &value) const;
      std::ostream &WriteWithWrap(std::ostream &os,
                                  const QString &textToWrite,
                                  int startColumn,
                                  PvlFormat &format) const;

      //! Formatter object
      PvlFormat *p_formatter;

    private:
      //! The keyword's name... This is a c-string for memory efficiency
      char * p_name;

      /**
       * The values in the keyword. This is a QVarLengthArray purely for
       *   optimization purposes. The amount of memory consumed by other data
       *   types introduces very significant overhead relative to this type
       *   which is meant to be as cost-effective and cheap as possible. Most
       *   of the time we have one value per keyword so that is what we're
       *   allocating by default with this variable.
       */
      QVarLengthArray<QString, 1> p_values;

      //! The units for the values.
      std::vector<QString> *p_units;

      //! The comments for the keyword.
      std::vector<QString> *p_comments;

      void Init();

      void WriteSpaces(std::ostream &, int) const;

      /**
       * The width of the longest keyword. This is used for spacing out the
       * equals signs on output.
       */
      int p_width;
      /**
       * The number of indentations to make. This is based on whether the
       * keyword is in a group, etc.
       */
      int p_indent;
  };
};

#endif

