#ifndef QnetNavTool_h
#define QnetNavTool_h

/**
 * @file
 * $Date: 2010/07/01 19:04:52 $ $Revision: 1.20 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */


#include "ControlPoint.h"
#include "Tool.h" // parent

// forward declarations
class QComboBox;
class QDialog;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;
class QString;
class QWidget;

using namespace std;
namespace Qisis {
  /**
   * @brief Qnet Navigation Tool
   *
   * @ingroup Visualization Tools
   *
   * @author Elizabeth Ribelin - 2006-11-07
   *
   * @internal
   *   @history 2007-06-05 Tracie Sucharski - Added enumerators for filter indices
   *   @history 2008-11-24 Jeannie Walldren - Replace references to PointEdit
   *            class with ControlPointEdit
   *   @history 2008-11-26 Jeannie Walldren - Added GoodnessOfFit to
   *            PointFilterIndex enumeration
   *   @history 2008-11-26 Tracie Sucharski - Remove all polygon/overlap
   *                          references, this functionality will be qmos
   *   @history 2008-12-09 Tracie Sucharski - Cleaned up some signal/slot
   *                          connections between QnetTool and QnetNavTool for
   *                          deleting or adding ControlPoints. Also added
   *                          p_filtered indicating whether the listBox contains
   *                          filtered or unfiltered lists.
   *   @history 2008-12-29 Jeannie Walldren - Added question boxes to the "Delete
   *                          Points" and "Ignore Points" buttons to verify that
   *                          the user wants to delete or ignore the selected
   *                          points
   *   @history 2008-12-30 Jeannie Walldren - Modified updateEditPoint() method to
   *                          set current item rather than simply highlight the
   *                          new point. Now the point does not have to be clicked
   *                          before "Delete Point(s)" is chosen. Removed "std::"
   *                          in cpp file since we are using std namespace.
   *   @history  2008-12-31 Jeannie Walldren - Added keyboard shortcuts to
   *                          createNavigationDialog() and createFilters()
   *                          methods.
   *   @history  2009-01-08 Jeannie Walldren - In resetList(), fill filtered lists
   *                           with all points in control net and all images in
   *                           serial number list so that filters can remove
   *                           unwanted members from this list. In filter() remove
   *                           command to clear these lists so that we may filter
   *                           filtered lists rather than start with the entire
   *                           points/image list each time it is called.
   *   @history  2009-01-26 Jeannie Walldren - The following changes were made in
   *                           order to create a Cube Names filter on the Points
   *                           list: Added CubeNames to PointFilterIndex
   *                           enumerated values. Added resetCubeList() slot and
   *                           serialListModified() signal. Modified filter() and
   *                           createFilters().
   *   @history  2010-06-02 Jeannie Walldren - Changed tab labels from "Point
   *                           Type" to "Point Properties" and "Measure Type(s)"
   *                           to "Measure Properties" for better accuracy.  Also
   *                           updated "What's This?" description for "Measure
   *                           Properties".
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null in
   *                          constructor. Modified tie() and load() methods.
   *   @history 2010-07-01 Jeannie Walldren - Added showNavTool() slot method.
   *                          This method is connected to the QnetTool in order to
   *                          reopen the navigator dialog whenever the Tie tool
   *                          button or "Show Nav Tool" button are activated.
   *                          Replaced #includes with forward class declarations
   *                          and moved #include to .cpp file.
   *  
   *  
   */
  class QnetNavTool : public Tool {
    Q_OBJECT

    public:
      QnetNavTool (QWidget *parent);

      enum FilterIndex {
        Points,
        Cubes
      };
      enum PointFilterIndex {
        Errors,
        Id,
        NumberImages,
        Type,
        LatLonRange,
        Distance,
        MeasureType,
        GoodnessOfFit,
        CubeName
      };
      enum CubeFilterIndex {
        Name,
        NumberPoints,
        PointDistance
      };


    public slots:
      void resetList();
      void refreshList();
      void updateEditPoint(string pointId);

    private slots:
      void load();
      void tie();
      void filter();
      void editPoint(QListWidgetItem *ptItem);
      void filterList();
      void resetFilter();
      void enableButtons();
      void ignorePoints();
      void deletePoints();
      void resetCubeList();
      void showNavTool();

    signals:
      void loadPoint (Isis::ControlPoint *);
      void loadImage (const QString &);
      void modifyPoint(Isis::ControlPoint *);
      void ignoredPoints();
      void deletedPoints();
      void netChanged();
      void serialListModified();

    private:
      void createNavigationDialog(QWidget *parent);
      void createFilters();


      QDialog *p_navDialog;
      QPushButton *p_filter;
      bool p_filtered;
      QPushButton *p_tie;
      QPushButton *p_multiIgnore;
      QPushButton *p_multiDelete;
      QStackedWidget *p_filterStack;
      QComboBox *p_listCombo;
      QListWidget *p_listBox;
      QLabel *p_filterCountLabel;
      int p_filterCount;

      string p_editPointId;
  };
};

#endif
