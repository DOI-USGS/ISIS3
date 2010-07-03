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
#ifndef Isis_Filename_h
#define Isis_Filename_h

#include <string>
#include <vector>

#include <QFileInfo>
#include <QDir>

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
 *                       variables within a filename.
 *   @history 2003-01-27 Jeff Anderson - added a method to allow full file
 *                       specification to be extracted (includes the attributes.
 *   @history 2002-02-12 Stuart Sides - fixed bug with incorrect parsing when
 *                       filename did not have a path.
 *   @history 2003-05-16 Stuart Sides - modified schema from
 *                       astrogeology...isis.astrogeology.
 *   @history 2003-07-21 Stuart Sides - modified to use "+" as the attribute
 *                       delimiter instead of a ":".
 *   @history 2003-10-17 Added default constructor, operator "=" and temporary
 *                       members.
 *   @history 2003-10-30 Added new members: HighestVersion and Exists.
 *   @history 2003-12-03 Added capability to expand Preferences in the
 *                       DataDirectory group only.
 *   @history 2004-01-14 Added new member OriginalPath.
 *   @history 2004-01-22 Added new member Name. Name returns the filename only
 *                       without any path, extension or attributes.
 *   @history 2004-01-27 Tool all references to attributes out. IsisFilename
 *                       now ignores all cube attributes.
 *   @history 2004-01-27 Removed the member FullSpecification, because
 *                       FullSpecification without the cube attributes now does
 *                       the same thing as Filename.
 *   @history 2004-01-27 Added a new constructor with two parameters. This new
 *                       constructor will create a temporary filename using the
 *                       path from the Preference DataDirectory/Temporary the
 *                       filename from argument one appended with a number from
 *                       100000 to 199999 and the extension of argument two.
 *   @history 2004-05-17 Added new member MakeDirectory.
 *   @history 2005-07-28 Drew Davidson - added new member NewVersion.
 *   @history 2007-10-03 Steven Koechle - Fixed Temporary() so if a path was
 *                       specified it will have the cwd put on in front of it.
 *   @history 2009-01-07 Steven Lambright - Expanded(...) no longer behaves
 *                       differently for unit tests
 * 
 * 
 */
  class Filename : public QFileInfo {
    public:
      // Create an empty filename
      Filename ();

      // Create a new Filename with the string
      Filename (const std::string &filename);

      // Create a new Filename using the users temporary directory
      // preference with the name and extension specified
      Filename (const std::string &Name, const std::string &extension);

      // Destroys the Filename Object
      ~Filename ();

      // Set the filename to the argument
      void operator=(const std::string &filename);
      void operator=(const char *filename);

      // Return only the path
      std::string Path() const;

      // Return the name without the extension or path
      std::string Basename () const; // equiv to qt baseName
                                     // renamed from Name

      // Return only the name with extension and without the path
      std::string Name () const; // equiv to qt fileName
                                 // renamed from Basename

      // Return only the extension (no path, name or ".")
      std::string Extension () const; // equiv to qt extension

      // Return the expanded filename (path, name & extension)
      std::string Expanded () const; // mostly qt absFilePath without $ expansion

      // Return the original path without "$" expansion if any
      std::string OriginalPath() const;

      // Add an extension to an existing Filename
      // Doesn't do anything if an extension already exists
      void AddExtension (const std::string &extension);

      // Remove the extension
      void RemoveExtension ();

      // Find the highest version of a filename
      void HighestVersion ();

      // Find the highest version + 1 of a filename
      void NewVersion();

      // Return true if the file exists
      bool Exists ();

      // Create a directory
      void MakeDirectory ();

      // Create a temporary filename using the
      // Isis::Preference DataDirectory/Temporary
      // directory with the name and extension of the arguments
      void Temporary (const std::string &name, const std::string &extension);

    private:

      // Expand any "$xxxx" into Isis preferences and environment variables
      // THe "DataDirectory" is the only group searched in IsisPreferences
      std::string Expand (const std::string &file);

      void CheckVersion () const;

      std::string p_original; //!< The original filename saved at construction

  };
};

#endif
