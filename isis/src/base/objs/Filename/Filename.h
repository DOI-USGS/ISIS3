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
#ifndef Filename_h
#define Filename_h

#include <QFileInfo>

#include "iString.h"

namespace Isis {
  /**
   * @brief Filename manipulation and expansion.
   *
   * This class is used for manipulating filenames. It allows access to the path,
   * extension, base name and attributes. A standard Isis filename takes the
   * form of
   * @code
   * /path/base.extension:attribute
   * @endcode
   * For example:
   * @code
   * /work1/mars.cub:5
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
   *                           IsisFilename now ignores all cube attributes.
   *   @history 2004-01-27 ?????? - Removed the member FullSpecification,
   *                           because FullSpecification without the cube
   *                           attributes now does the same thing as Filename.
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
   *   @history 2009-01-07 Steven Lambright - Expanded() no longer behaves
   *                           differently for unit tests
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *   @history 2011-08-24 Steven Lambright and Tracie Sucharski - Uses iString
   *                           instead of std::string for simplicity when used
   *                           with our GUI applications. Removed extra includes
   *                           and cleaned up code a little.
   */
  class Filename : public QFileInfo {
    public:
      // Create an empty filename
      Filename();

      // Create a new Filename with the string
      Filename(const iString &filename);

      // Create a new Filename using the users temporary directory
      // preference with the name and extension specified
      Filename(const iString &Name, const iString &extension);

      // Destroys the Filename Object
      ~Filename();

      // Set the filename to the argument
      void operator=(const iString &filename);
      void operator=(const char *filename);

      // Return only the path
      iString Path() const;

      // Return the name without the extension or path
      iString Basename() const;  // equiv to qt baseName
      // renamed from Name

      // Return only the name with extension and without the path
      iString Name() const;  // equiv to qt fileName
      // renamed from Basename

      // Return only the extension (no path, name or ".")
      iString Extension() const;  // equiv to qt extension

      // Return the expanded filename (path, name & extension)
      iString Expanded() const;  // mostly qt absFilePath without $ expansion

      // Return the original path without "$" expansion if any
      iString OriginalPath() const;

      // Add an extension to an existing Filename
      // Doesn't do anything if an extension already exists
      void AddExtension(const iString &extension);

      // Remove the extension
      void RemoveExtension();

      // Find the highest version of a filename
      void HighestVersion();

      bool IsVersioned() const;

      bool IsNumericallyVersioned() const;

      bool IsDateVersioned() const;

      void SetHighestNumericalVersion();

      void SetHighestDateVersion();

      // Find the highest version + 1 of a filename
      void NewVersion();

      void SetNewNumericalVersion();

      void SetNewDateVersion();

      // Return true if the file exists
      bool Exists();

      // Create a directory
      void MakeDirectory();

      // Create a temporary filename using the
      // Isis::Preference DataDirectory/Temporary
      // directory with the name and extension of the arguments
      void Temporary(const iString &name, const iString &extension);

    private:
      QDir GetDirectory() const;

      void CheckVersion() const;

      QString ReplacePattern(QString name, QString version);

      QString PadFront(QString string, QString padding, int minLength);

      QString GetHighestVersionNumber() const;

      QString GetDatePattern() const;

      // Expand any "$xxxx" into Isis preferences and environment variables
      // THe "DataDirectory" is the only group searched in IsisPreferences
      iString Expand(const iString &file);

      iString p_original; //!< The original filename saved at construction

  };
};

#endif
