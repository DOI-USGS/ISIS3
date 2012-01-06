#ifndef iException_h
#define iException_h

/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2009/07/29 21:16:39 $
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

#include <exception>
#include <string>
#include <vector>
#include <QList>

class QReadWriteLock;
class QThread;

template <typename A, typename B> class QMap;

#define _FILEINFO_ __FILE__,__LINE__

namespace Isis {
  class Pvl;

  /**
   * @brief Isis exception class
   *
   * The general Isis exception class. Contains an enumeration for what type of
   * exception to throw.
   *
   * @author ????-??-?? Jeff Anderson
   *
   * @internal
   *   @history 2005-05-10 Leah Dahmer - Added class documentation
   *   @history 2005-12-28 Elizabeth Miller - Fixed bug in Pvl error output
   *   @history 2006-06-12 Tracie Sucharski - Change clear method to static
   *   @history 2006-11-02 Jeff Anderson - Fixed bug in Report method for
   *                           exit status
   *   @history 2007-12-31 Steven Lambright - Added stack trace
   *   @history 2008-05-23 Steven Lambright - Added stack trace
   *   @history 2008-06-18 Stuart Sides - Fixed doc error
   *   @history 2008-07-08 Steven Lambright - Changed memory cleanup; now uses
   *                           atexit
   *   @history 2008-10-30 Steven Lambright - iException::Message now takes a
   *                           const char* for the filename, instead of a chat*, 
   *                           issue pointed out by "novus0x2a" (Support Board
   *                           Member)
   *   @history 2008-12-15 Steven Lambright - iException::what no longer returns
   *                           deleted memory.
   *   @history 2009-07-29 Steven Lambright - Stack trace calculations moved to
   *                           IsisDebug.h
   *   @history 2011-08-05 Steven Lambright - Hacked to make multi-threaded
   *                           code which throws exceptions work.
   */
  class iException : public std::exception {
    public:
      iException(const iException &other);
      iException &operator=(const iException &other);

      //! Contains a set of throwable exception types.
      enum errType {
        None       = 0,
        User       = 1,
        Programmer = 2,
        Pvl        = 3,
        Io         = 5,
        Camera     = 6,
        Projection = 7,
        Parse      = 8,
        Math       = 9,
        Spice      = 10,
        Cancel     = 200,
        System     = 255
      };

      static iException &Message(errType t, const std::string &m, const char *f,
                                 int l);

      const char *what() const throw();
      errType Type() const;
      int Report(bool fileinfo = true);
      Isis::Pvl PvlErrors();
      std::string Errors();
      bool IsPvlFormat();
      void Clear();

      ~iException() throw();

    private:
      // This class really, really isn't a singleton... promise...
      static iException *p_exception;

      /**
       * @brief Exception information
       *
       * Contains information about an iException object. A helper class to
       * iException.
       *
       * @author ????-??-?? Unknown
       *
       * @internal
       *   @history 2005-05-10 Leah Dahmer - wrote class documentation
       *   @history 2008-07-08 Steven Lambright - Fixed destruction code
       *   @todo Find out author and date of class!
       */
      class Info {
        public:
          errType type;
          std::string message;
          std::string filename;
          int lineNumber;
      };

      iException();

      static void Shutdown();

      // This lock is for the QMap of p_list, NOT for the elements inside.
      //   The elements inside of p_list should only be accessed by its
      //   current thread - never anything else!
      static QReadWriteLock * p_exceptionsLock;
      static QMap< QThread *, QList<Info> > * p_list;

      static Info createStackTrace();
      std::string enumString(errType t) const;
      errType enumString(const std::string &s) const;

      void describe() const;

      /**
       * True or false, depending on whether the filename and line number
       * should be reported in the output.
       */
      bool p_reportFileLine;

      /**
       * True or false, depending on whether the exception should be output in
       * PVL format.
       */
      bool p_pvlFormat;

      /**
       * This is the return value of what(). Calculate and keep track of it
       * in order to prevent the caller from using deleted memory (what()
       * returns a char *, and we need to guarantee the char * is good).
       */
      mutable std::string *p_what;
  };
};

#endif
