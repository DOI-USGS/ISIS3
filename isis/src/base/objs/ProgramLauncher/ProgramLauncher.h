#ifndef ProgramLauncher_h
#define ProgramLauncher_h

/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/06/29 23:42:18 $
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

namespace Isis {
  class iString;

  /**
   * @brief Execute External Programs and Commands
   *
   * This class is designed to handle running any other programs or commands.
   *
   * @author Steven Lambright
   *
   * @internal
   *   @history 2010-12-03 Steven Lambright - Original version
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of
   *                           /tmp directory.
   */
  class ProgramLauncher {
    public:
      static void RunIsisProgram(iString isisProgramName, iString arguments);
      static void RunSystemCommand(iString commandLine);

    private:
      static void ProcessIsisMessageFromChild(iString code, iString msg);

    private:
      //! Construction is not allowed
      ProgramLauncher();
      //! Destruction is not allowed
      ~ProgramLauncher();

      /**
       * Copy construction is not allowed
       *
       * @param other
       */
      ProgramLauncher(ProgramLauncher &other);

      /**
       * Assignment is not allowed
       *
       * @param other
       * @returns
       */
      ProgramLauncher &operator=(ProgramLauncher &other);
  };
};

#endif
