#ifndef FileName_h
#define FileName_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/01/07 18:55:22 $
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

#include <QSharedData>

class QDate;
class QDir;
class QString;

template<typename A, typename B> struct QPair;

namespace Isis {
  /**
   * @brief File name manipulation and expansion.
   *
   * This class is used for manipulating filenames. It allows access to the path,
   * extension, base name and Isis attributes. A standard Isis filename takes the
   * form of
   * @code
   * /path/base.extension+attribute
   * @endcode
   * For example:
   * @code
   * /work1/mars.cub+5
   * @endcode
   *
   * @ingroup Parsing
   *
   * @author 2002-06-11 Jeff Anderson
   *
   * @internal
   *   @todo This class needs an example.
   *   @history 2002-11-27 Stuart Sides - added capability to expand environment
   *                           variables within a filename.
   *   @history 2003-01-27 Jeff Anderson - added a method to allow full file
   *                           specification to be extracted (includes the
   *                           attributes).
   *   @history 2002-02-12 Stuart Sides - fixed bug with incorrect parsing when
   *                           filename did not have a path.
   *   @history 2003-05-16 Stuart Sides - modified schema from
   *                           astrogeology...isis.astrogeology.
   *   @history 2003-07-21 Stuart Sides - modified to use "+" as the attribute
   *                           delimiter instead of a ":".
   *   @history 2003-10-17 ?????? - Added default constructor, operator "=" and
   *                           temporary members.
   *   @history 2003-10-30 ?????? - Added new members: HighestVersion and
   *                           Exists.
   *   @history 2003-12-03 ?????? - Added capability to expand Preferences in
   *                           the DataDirectory group only.
   *   @history 2004-01-14 ?????? - Added new member OriginalPath.
   *   @history 2004-01-22 ?????? - Added new member Name. Name returns the
   *                           filename only
   *                           without any path, extension or attributes.
   *   @history 2004-01-27 ?????? - Tool all references to attributes out.
   *                           IsisFileName now ignores all cube attributes.
   *   @history 2004-01-27 ?????? - Removed the member FullSpecification,
   *                           because FullSpecification without the cube
   *                           attributes now does the same thing as FileName.
   *   @history 2004-01-27 ?????? - Added a new constructor with two parameters.
   *                           This new constructor will create a temporary
   *                           filename using the path from the Preference
   *                           DataDirectory/Temporary the filename from
   *                           argument one appended with a number from 100000
   *                           to 199999 and the extension of argument two.
   *   @history 2004-05-17 ?????? - Added new member MakeDirectory.
   *   @history 2005-07-28 Drew Davidson - added new member NewVersion.
   *   @history 2007-10-03 Steven Koechle - Fixed Temporary() so if a path was
   *                           specified it will have the cwd put on in front of it.
   *   @history 2009-01-07 Steven Lambright - expanded() no longer behaves
   *                           differently for unit tests
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *   @history 2011-08-24 Steven Lambright and Tracie Sucharski - Uses QString
   *                           instead of QString for simplicity when used
   *                           with our GUI applications. Removed extra includes
   *                           and cleaned up code a little.
   *   @history 2012-04-14 Steven Lambright - Renamed FileName to FileName. No
   *                           longer inherits QFileInfo, paired down
   *                           functionality to file name related functionality.
   *                           Fixed temporary file naming to guarantee unique,
   *                           unpredictable (safe) names.
   *   @history 2012-06-04 Steven Lambright and Stuart Sides - Added operator!= and improved unit
   *                           test to include tests of operator== and operator!=. Fixes #903.
   *   @history 2014-07-23 Jeannie Backer - Moved ifndef to top of file to bring class closer to
   *                           ISIS coding standards and removed include for std::string.
   *   @history 2016-06-21 Kris Becker - Properly forward declare QPair as struct not class
   *   @history 2017-04-21 Cole Neubauer - Updated documentation for the class Fixes #4121
   *   @history 2018-04-06 Kaitlyn Lee - Moved method documentation to cpp file and
   *                           updated it for consistency. Fixes #5230.

   */
  class FileName {
    public:
      // Constructors
      FileName();
      FileName(const char *fileName);
      FileName(const QString &fileName);

      // Copy Constructor, creates a copy of a FileName object.
      FileName(const FileName &other);

      // Destroys the FileName Object
      ~FileName();

      // Methods
      QString originalPath() const;
      QString path() const;
      QString attributes() const;
      QString baseName() const;
      QString name() const;
      QString extension() const;
      QString expanded() const;
      QString original() const;

      FileName addExtension(const QString &extension) const;
      FileName removeExtension() const;
      FileName setExtension(const QString &extension) const;

      bool isVersioned() const;
      bool isNumericallyVersioned() const;
      bool isDateVersioned() const;

      FileName highestVersion() const;
      FileName newVersion() const;
      FileName version(long versionNumber) const;
      FileName version(QDate versionDate) const;

      bool fileExists() const;
      QDir dir() const;
      static FileName createTempFile(FileName templateFileName = "$TEMPORARY/temp");
      QString toString() const;
      FileName &operator=(const FileName &rhs);
      bool operator==(const FileName &rhs);
      bool operator!=(const FileName &rhs);


    private:

      QDate highestVersionDate() const;
      long highestVersionNum() const;
      void validateVersioningState() const;
      QString fileNameQDatePattern() const;
      QPair<QString, QString> splitNameAroundVersionNum() const;

    private:
      /**
       * This is the reference-counted data for FileName
       *
       * @author 2012-05-02 Steven Lambright
       *
       * @internal
       */
      class Data : public QSharedData {
        public:

          // Constructors
          Data();

          // Copy Constructor, creates a copy of a Data object.
          Data(const Data &other);

          // Destroys the Data Object
          ~Data();

          // Methods
          QString original(bool includeAttributes) const;
          void setOriginal(const QString &originalStr);
          QString expanded(bool includeAttributes) const;

        private:
          // Holds the original file name.
          QString *m_originalFileNameString;


          // Holds the expanded file name.
          QString *m_expandedFileNameString;
      };

      // @see QSharedDataPointer
      QSharedDataPointer<Data> m_d;
  };
};

#endif
