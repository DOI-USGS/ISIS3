#ifndef Environment_h
#define Environment_h

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
   * @author 2011-04-01 Eric Hyer & Steven Lambright
   *
   * @internal
   *   @history 2011-09-20 Steven Lambright - Added support for 4 line version
   *                           file.
   *   @history 2012-03-06 Steven Lambright - Added automatic environment
   *                           setup that works with every Isis 3 application.
   *                           This includes the 'qisis' applications. The
   *                           automatic setup is setting the Qt plugin path
   *                           because picking up a plugin from the OS means
   *                           crashing with an error like:
   *                             Cannot mix incompatible Qt library (version
   *                             0x40704) with this library (version 0x40800)
   *                           Fixes #742.
   */
  class Environment {
    public:
      ~Environment() {}
      
      static iString userName();
      static iString hostName();
      static iString isisVersion();
      
      
    protected:
      Environment();
      
      
    private:
      static iString getEnvironmentValue(iString, iString);
      /**
       *  Construct an environment in static space to initialize some
       *    global Isis 3 environment options. This initialization
       *    applies to anything that links against the Isis 3 library.
       */
      static Environment automaticEnvironmentSetup;
  };
}

#endif
