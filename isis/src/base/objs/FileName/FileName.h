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
   */
  class FileName {
    public:
      //! Constructs an empty FileName object.
      FileName();
      /**
       * Constructs a FileName object using a char pointer as a file name.
       * @param *fileName char pointer representing new filename
       */
      FileName(const char *fileName);
      /**
       * Constructs a FileName object using a QString as a file name.
       * @param &fileName Qstring representing new filename
       */
      FileName(const QString &fileName);
      //! Copy Constructor, creates a copy of a FileName object.
      /**
       * Constructs a copy of a FileName object using another FileName object.
       * @param &other FileName object to copy.
       */
      FileName(const FileName &other);
      //! Destroys the FileName object.
      ~FileName();


      /**
       * Returns the path of the original file name. For *nix operating
       * systems this includes everything up to but not including the
       * last slash "/". For filenames created without any slashes
       * the current working directory will be returned.
       *
       * @returns QString of the path portion of the original filename.
       * <pre>
       *   for a full file specification of:
       *    "/home/me/img/picture.jpg"
       *   originalPath() gives:
       *    "/home/me/img"
       * </pre>
       */
      QString originalPath() const;


      /**
       * Returns the path of the file name. For *nix operating
       * systems this includes everything up to but not including the
       * last slash "/". For filenames created without any slashes
       * the current working directory will be returned.
       *
       * @returns QString of the path portion of the filename.
       * <pre>
       *   for a full file specification of:
       *    "/home/me/img/picture.jpg"
       *   path() gives:
       *    "/home/me/img"
       * </pre>
       */
      QString path() const;


      /**
       * Returns a QString of the attributes in a filename, attributes are expected to be of type
       * CubeAttributeInput or CubeAttributeOutput. Filenames without any attributes return an
       * empty QString.
       *
       * @returns QString of the attributes specified in the filename.
       * <pre>
       *   for a full file specification of:
       *    "/tmp/Peaks.cub+Bsq"
       *   attributes() gives:
       *    "Bsq"
       * </pre>
       */
      QString attributes() const;


      /**
       * Returns the name of the file without the path and without extensions.
       *
       * @returns QString containing every character excluding the path and all extensions.
       * <pre>
       *   for a full file specification of:
       *    "/tmp/Peaks.cub.gz"
       *   baseName() gives:
       *    "Peaks"
       * </pre>
       */
      QString baseName() const;


      /**
       * Returns the name of the file excluding the path and the attributes in the file name.
       *
       * @returns QString containing every character in the file name exluding the path and attributes
       * of the file.
       * <pre>
       *   for a full file specification of:
       *    "/tmp/Peaks.cub+Bsq"
       *   name() gives:
       *    "Peaks.cub"
       * </pre>
       */
      QString name() const;


      /**
       * Returns the last extension of the file name.
       *
       * @returns QString containing every character in the file name after the last "." character.
       * <pre>
       *   for a full file specification of:
       *    "/tmp/Peaks.cub.gz"
       *   extension() gives:
       *    "gz"
       * </pre>
       */
      QString extension() const;


      /**
       * Returns a QString of the full file name including the file path, excluding the attributes.
       * Any Isis Preferences or environment variables indicated by $, are changed to what they
       * represent.
       *
       * @returns QString
       * <pre>
       *   for a full file specification of:
       *    QString(ISISROOT) + "/tmp/Peaks.cub+Bsq"
       *   expanded() gives:
       *    "/usgs/pkgs/isis3/isis/tmp/Peaks.cub"
       * </pre>
       */
      QString expanded() const;


      /**
       * Returns the full file name including the file path
       *
       * @returns QString containing every character in the file name and the path
       * <pre>
       *   for a full file specification of:
       *    QString(ISISROOT) + "/tmp/Peaks.cub+Bsq"
       *   original() gives:
       *    QString(ISISROOT) + "/tmp/Peaks.cub+Bsq"
       * </pre>
       */
      QString original() const;


      /**
       * Adds a new extension to the file name. If the current extension is the same as the
       * new extension it will return an unchanged FileName object.
       *
       * @param &extension The new file extension to be added at the end of the file name after all
       * exisiting extensions.
       *
       * @returns FileName object with added extension
       */
      FileName addExtension(const QString &extension) const;


      /**
       * Removes all extensions in the file name
       *
       * @returns FileName object with all extensions removed
       */
      FileName removeExtension() const;


      /**
       * Sets all current file extensions to a new extension in the file name.
       *
       * @param &extension The new file extension to replace any current file extensions with
       *
       * @returns FileName object with all existing extensions replaced by the new extension
       */
      FileName setExtension(const QString &extension) const;


      /**
       * Checks to see if a file name is versioned by date or numerically. Returns true if file is
       * versioned by date or numerically; returns false otherwise.
       *
       * @returns Boolean
       */
      bool isVersioned() const;


      /**
       * Checks if the file name is versioned numerically. Returns true if the file is versioned
       * numerically; returns false otherwise.
       *
       * @returns Boolean
       */
      bool isNumericallyVersioned() const;


      /**
       * Checks if the file name is versioned by date. Returns true if the file is versioned
       * by date; returns false otherwise.
       *
       * @returns Boolean
       */
      bool isDateVersioned() const;


      /**
       * Searches the directory specified in the file name for the highest version of the file name.
       * Returns a FileName object with the file name changed to reflect the highest version.
       *
       * @returns FileName object
       */
      FileName highestVersion() const;


      /**
       * Updates the file name to be the latest version. If the file is versioned by date the
       * newest version will be the current date. If the file is versioned numerically, the newest
       * version will be the current version plus one.
       *
       * @returns FileName object with the new version file name.
       *
       * @throws Isis::IException::Unknown
       */
      FileName newVersion() const;


      /**
       * Returns a FileName object of the same file name but versioned numerically by the
       * number passed in as a parameter.
       *
       * @param versionNumber number to version the new FileName object
       *
       * @returns FileName object with the new version file name.
       *
       * @throws Isis::IException::Unknown
       */
      FileName version(long versionNumber) const;


      /**
       * Returns a FileName object of the same file name but versioned by the
       * date passed in as a parameter.
       *
       * @param versionDate QDate to version the new FileName object
       *
       * @returns FileName object with the new version file name.
       *
       */
      FileName version(QDate versionDate) const;


      /**
       * Returns true if the file exists; false otherwise.
       * If the file is a symlink that points to a nonexistent file, false is returned.
       *
       * @returns Boolean
       */
      bool fileExists() const;


      /**
       * Returns the path of the file's parent directory as a QDir object
       *
       * @returns QDir
       * <pre>
       *   for a full file specification of:
       *    "/tmp/Peaks.cub+Bsq"
       *   dir() gives:
       *    "/tmp/"
       * </pre>
       */
      QDir dir() const;


      /**
       * Creates a temporary file and returns a FileName object created using the temporary file.
       *
       * @param templateFileName the file name used to create the temporary file.
       *
       * @returns FileName object created using the temporary file
       *
       * @throws Isis::IException::Io
       */
      static FileName createTempFile(FileName templateFileName = "$TEMPORARY/temp");


      /**
       * Returns a QString of the full file name including the file path, excluding the attributes
       * with any Isis Preferences or environment variables indicated by $, changed to what they
       * represent.
       *
       * @returns QString
       * <pre>
       *   for a full file specification of:
       *    QString(ISISROOT) + "/tmp/Peaks.cub+Bsq"
       *   toString() gives:
       *    "/usgs/pkgs/isis3/isis/tmp/Peaks.cub"
       * </pre>
       */
      QString toString() const;


      /**
       * Clears the current contents of the FileName object and reinitializes it with
       * the argument.
       *
       * @returns void
       *
       * @param rhs FileName to replace the current contents of the object.
       *
       */
      FileName &operator=(const FileName &rhs);


      /**
       * Compares equality of two FileName objects. Returns true if the two objects are equal
       * and false otherwise.
       *
       * @returns Boolean
       *
       * @param rhs FileName to compare the current FileName object to.
       */
      bool operator==(const FileName &rhs);


      /**
       * Compares equality of two FileName objects. Returns false if the two objects are equal
       * and true otherwise.
       *
       * @returns Boolean
       *
       * @param rhs FileName to compare the current FileName object to.
       */
      bool operator!=(const FileName &rhs);


    private:
      /**
       * This looks through the directory of the file and checks for the highest version date of
       * the file that is versioned date.
       *
       * @returns QDate
       */
      QDate highestVersionDate() const;


      /**
       * This looks through the directory of the file and checks for the highest version number of
       * the file that is versioned numerically.
       *
       * @returns long
       */
      long highestVersionNum() const;


      /**
       * This verifies the class invariant when using versioning - that the FileName is in an acceptable
       *     state to find file version numbers.
       */
      void validateVersioningState() const;

     /**
      * This changes the files format. Specifically quotes everything not in {} with single quotes
      * and removes the {} from the file name.
      *
      * @returns QString
      */
      QString fileNameQDatePattern() const;

     /**
      * This returns a QPair of the text (before, after) a version number in a file. Before being
      * the text before the version number and after being the text after the version number.
      *
      * @returns QPair
      */
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
          //! Data constructor, creates a new Data object.
          Data();


          /**
           * Data copy constructor, creates a copy of a Data object.
           * @param &other Data object to copy
           */
          Data(const Data &other);


          //! Destroys the Data object.
          ~Data();


         /**
          * Returns the original file name, stored in m_originalFileNameString. Boolean
          * parameter includeAttributes determines if the returned file name has the variables
          * included.
          *
          * @returns Qstring
          *
          * @param includeAttributes boolean to represent whether the attricubtes should be included.
          */
          QString original(bool includeAttributes) const;


         /**
          * Sets the original file name, stored in m_originalFileNameString. QString parameter
          * is the new file name to store in m_originalFileNameString. The expanded verison is also
          * set and stored in m_expandedFileNameString when this method is called.
          *
          * @param originalStr the new file name
          */
          void setOriginal(const QString &originalStr);


         /**
          * Returns the expanded file name, stored in m_expandedFileNameString. Boolean
          *  parameter includeAttributes determines if the returned file name has the variables
          * included.
          *
          * @returns Qstring
          *
          * @param includeAttributes boolean to represent whether the attricubtes should be included.
          */
          QString expanded(bool includeAttributes) const;


        private:
          //! Holds the original file name.
          QString *m_originalFileNameString;


          //! Holds the expanded file name.
          QString *m_expandedFileNameString;
      };

      //! @see QSharedDataPointer
      QSharedDataPointer<Data> m_d;
  };
};

#endif
