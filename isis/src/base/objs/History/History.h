#ifndef History_h
#define History_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Blob.h"
#include "Pvl.h"

namespace Isis {
  class PvlObject;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal 
   *   @history 2006-12-11 Kris Becker Fixed bug in WriteInit method using a
   *                           temporary string to reference a char pointer to
   *                           its contents.  The string remain after the
   *                           reference or its toast.
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *   @history 2012-10-04 Jeannie Backer - Replaced if! defined with ifndef.
   *                           Added forward declaration for PvlObject and
   *                           include for Pvl since the include for Pvl was
   *                           removed from Blob.h. Added padding to control
   *                           statements. References #1169
   *  
   * @todo This class needs documentation.
  */
  class History : public Isis::Blob {
    public:
      History(const QString &name);
      History(const QString &name, const QString &file);
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

