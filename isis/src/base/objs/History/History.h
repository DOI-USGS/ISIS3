#if !defined(History_h)
#define History_h

/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/14 19:17:59 $
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

#include "Blob.h"

namespace Isis {
  /**
   *  
   * @internal 
   *   @history 2006-12-11 Kris Becker Fixed bug in WriteInit method using a
   *                           temporary string to reference a char pointer to
   *                           its contents.  The string remain after the
   *                           reference or its toast.
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *
   * @internal
   * @todo This class needs documentation.
  */
  class History : public Isis::Blob {
    public:
      History(const std::string &name);
      History(const std::string &name, const std::string &file);
      ~History();

      void AddEntry();
      void AddEntry(Isis::PvlObject &obj);
      Pvl ReturnHist();
      void Read(const Isis::Pvl &pvl, std::istream &is);

    protected:
      void WriteInit();

    private:
      Pvl p_history; //!< History Pvl
  };
};

#endif

