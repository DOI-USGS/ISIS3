#ifndef QnetNavTool_h
#define QnetNavTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "Tool.h"
// forward declarations
class QComboBox;
class QDialog;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;
class QString;
class QVBoxLayout;
class QWidget;


namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;
  class QnetSetAprioriDialog;
  class QnetTool;
  class SerialNumberList;

  /**
   * @brief Qnet Navigation Tool
   *
   * @ingroup Visualization Tools
   *
   * @author 2006-11-07 Elizabeth Ribelin
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
   *                          m_filtered indicating whether the listBox contains
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
   *   @history 2010-07-12 Jeannie Walldren - Fixed documentation.
   *   @history 2010-11-01 Tracie Sucharski - Changed updateEditPoint parameter
   *                          from std::string to QString.
   *   @history 2010-11-04 Tracie Sucharski - Added overloaded load slot for
   *                          double-clicking on the cube list.
   *   @history 2010-12-10 Tracie Sucharski - Renamed signal loadPoint to
   *                          loadPointImages and the ControlPoint is passed
   *                          by value so that the original point is preserved
   *                          until the user selects "Save Point".
   *   @history 2011-03-24 Tracie Sucharski - Added ability to enter Apriori Point values and
   *                          sigmas.
   *   @history  2013-05-14 Tracie Sucharski - Add Qt::UniqueConnection to the connect statements
   *                          in ::resetList to prevent multiple connections between the ListWidget
   *                          and edit point slot and load cube slot.  Fixes #1655.
   *   @history 2015-05-28 Makayla Shepherd and Ian Humphrey - When changing navigation types from
   *                          Points to Cubes, and vice versa, and then back to the original type,
   *                          the filtered data is saved rather than forcing the user to refilter
   *                          the data. Fixes #2144.
   *   @history 2016-11-18 Makayla Shepherd - Added disconnection and deletion of the Set Apriori
   *                          Dialog on close. Fixes #4490.
   *   @history 2017-01-04 Makayla Shepherd - Made showNavTool() public in order to fix a window
   *                          management issue that caused the NavTool to go behind the
   *                          ViewportMainWindow (parent widget) when using Gnome or Cinnamon. Fixes
   *                          #4541.
   *   @history 2018-01-10 Adam Goins - Added the m_historyLayout member variable the class to keep
   *                          a running history tab of edits made to points.
   *
   */
  class QnetNavTool : public Tool {
      Q_OBJECT

    public:
      QnetNavTool(QnetTool *qnetTool, QWidget *parent);
      ~QnetNavTool();

      enum FilterIndex {
        Points,
        Cubes
      };
      enum PointFilterIndex {
        JigsawErrors,
        RegistrationErrors,
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

      QList<int> &filteredImages();
      const QList<int> &filteredImages() const;

      QList<int> &filteredPoints();
      const QList<int> &filteredPoints() const;

      ControlNet *controlNet();
      const ControlNet *controlNet() const;

      SerialNumberList *serialNumberList();
      const SerialNumberList *serialNumberList() const;

    public slots:
      void resetList();
      void refreshList();
      void updateEditPoint(QString pointId);
      void disconnectAprioriDialog();
      void showNavTool();

    private slots:
      void load();
      void load(QListWidgetItem *);
      void tie();
      void filter();
      void editPoint(QListWidgetItem *ptItem);
      void filterList();
      void resetFilter();
      void enableButtons();
      void ignorePoints();
      void deletePoints();
      void resetCubeList();
      void updateActivityHistory(QString activityMessage);
      void aprioriDialog();
      void setAprioriDialogPoints();

    signals:
      void activityUpdate(QString);
      void loadPointImages (ControlPoint *);
      void loadImage(const QString &);
      void modifyPoint(ControlPoint *);
      void ignoredPoints();
      void deletedPoints();
      void pointChanged(QString pointId);
      void netChanged();
      void serialListModified();

    private:
      void createNavigationDialog(QWidget *parent);
      void createFilters();
//      void listLockedPoints();


      QDialog *m_navDialog;
      QPushButton *m_filter;
      bool m_filtered;
      QPushButton *m_tie;
      QPushButton *m_multiIgnore;
      QPushButton *m_multiDelete;
      QPushButton *m_setApriori;
      QStackedWidget *m_filterStack;
      QComboBox *m_listCombo;
      QListWidget *m_listBox;
      QLabel *m_filterCountLabel;
      QVBoxLayout *m_historyLayout;
      int m_filterCount;

      QString m_editPointId;

      QnetSetAprioriDialog *m_aprioriDialog;

      QnetTool *m_qnetTool;
      QList<int> m_filteredPoints;
      QList<int> m_filteredImages;
  };
}

#endif
