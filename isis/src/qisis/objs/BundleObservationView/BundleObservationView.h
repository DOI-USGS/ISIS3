#ifndef BundleObservationView_h
#define BundleObservationView_h
/**
 * @file
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
