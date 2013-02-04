#ifndef OriginalLabel_h
#define OriginalLabel_h

/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/14 19:20:28 $
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
#include "Pvl.h"

namespace Isis {
  /**
   * @brief Read and store original labels.
   *
   * This class provides a means to read and store the labels from the original
   * source.  The labels can be retreived in a Pvl object.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2006-02-23 Robert Wallace and Brian Lipkowitz
   *
   * @internal
   *   @history 2006-06-13 Brendan George - Modified to throw error when called
   *                           file does not contain an Original Label
   *   @history 2006-12-11 Kris Becker - Fixed bug in WriteInit method using a
   *                           temporary string to reference a char pointer
   *                           to its contents.  The string remain after the
   *                           reference or its toast.
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *
   */
  class OriginalLabel : public Isis::Blob {
    public:
      OriginalLabel();
      OriginalLabel(const QString &file);
      OriginalLabel(Pvl pvl);
      ~OriginalLabel();

      // Return the original labels
      Pvl ReturnLabels();

    protected:
      // prepare data for writing
      void WriteInit();

    private:
      // labels of original source
      Pvl m_originalLabel;
  };
};

#endif

