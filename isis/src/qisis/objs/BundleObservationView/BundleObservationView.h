#ifndef BundleObservationView_h
#define BundleObservationView_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractProjectItemView.h"
#include "FileItem.h"

namespace Isis{


  /**
   * View for displaying BundleObservation CSV files
   *
   * @author 2017-05-01 Tyler Wilson
   *
   * @internal
   *   @history 2017-05-01 Tyler Wilson - Original version.
   *   @history 2017-05-05 Tracie Sucharski - Changed for the serialization of BundleObservation
   *                           files. This was implemented create a new ProjectItem type called
   *                           FileItemQsp. Fixes #4839, #4840.
   *   @history 2018-03-21 Ken Edmundson - Added capability to display either csv or text files.
   *                           Fixed problem for display of multi-line headers for csv files.
   *                           Set SectionResizeMode to QHeaderView::ResizeToContents so columns are
   *                           displayed at the width of the maximum size of the column content.
   *                           Fixes #4850.
   *   @history 2018-03-26 Ken Edmundson - Modified displayTextFile method to query for system's
   *                           fixed width font.
   *   @history 2018-04-16 Ken Edmundson - Modified display of residuals.csv to properly show the
   *                           rejected column if there are rejected measures. Also displays
   *                           rejected measure row in red.
   *   @history 2018-06-06 Kaitlyn Lee - Set a central widget and removed layout (it is not needed
   *                           after setting a central widget) because AbstractProjectItemView was
   *                           updated to inherit from QMainWindow.
   */

  class BundleObservationView : public AbstractProjectItemView
  {
    Q_OBJECT
    public:
      BundleObservationView(FileItemQsp fileItem, QWidget *parent=0);
      ~BundleObservationView();

    private:
      void displayCsvFile(FileItemQsp fileItem);
      void displayTextFile(FileItemQsp fileItem);
  };
}
#endif
