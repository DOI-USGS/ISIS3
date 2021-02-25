#ifndef Environment_h
#define Environment_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

class QString;

namespace Isis {

  /**
   * @author 2011-04-01 Eric Hyer & Steven Lambright
   *
   * @internal
   *   @history 2011-09-20 Steven Lambright - Added support for 4 line version
   *                           file.
   *   @history 2012-03-06 Steven Lambright - Added automatic environment
   *                           setup that works with every Isis application.
   *                           This includes the 'qisis' applications. The
   *                           automatic setup is setting the Qt plugin path
   *                           because picking up a plugin from the OS means
   *                           crashing with an error like:
   *                             Cannot mix incompatible Qt library (version
   *                             0x40704) with this library (version 0x40800)
   *                           Fixes #742.
   *   @history 2012-11-02 Steven Lambright - Updated automatic environment setup to fix
   *                           a library dependency problem (that is ignoring our plugin path
   *                           setting) with QDBus. Fixes #1228.
   *   @history 2106-06-20 Kris Becker - Porting to Qt5.6 requires a different Qt startup
   *                           strategy. See QStartup().
   */
  class Environment {
    public:
      ~Environment() {}

      static QString userName();
      static QString hostName();
      static QString isisVersion();

      static QString getEnvironmentValue(QString, QString);
    protected:
      Environment();


    private:
      /**
       *  Construct an environment in static space to initialize some
       *    global Isis environment options. This initialization
       *    applies to anything that links against the Isis library.
       */
      static Environment automaticEnvironmentSetup;
  };
}

#endif
