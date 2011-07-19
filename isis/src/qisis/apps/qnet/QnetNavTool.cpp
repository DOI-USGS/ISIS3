#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QTabWidget>
#include <QWidget>


#include "QnetCubeDistanceFilter.h"
#include "QnetCubeNameFilter.h"
#include "QnetCubePointsFilter.h"
#include "QnetNavTool.h"
#include "QnetPointCubeNameFilter.h"
#include "QnetPointDistanceFilter.h"
#include "QnetPointJigsawErrorFilter.h"
#include "QnetPointRegistrationErrorFilter.h"
#include "QnetPointGoodnessFilter.h"
#include "QnetPointIdFilter.h"
#include "QnetPointImagesFilter.h"
#include "QnetPointMeasureFilter.h"
#include "QnetPointRangeFilter.h"
#include "QnetPointTypeFilter.h"
#include "QnetSetAprioriDialog.h"

#include "Angle.h"
#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"

#include "qnet.h"
using namespace Qisis::Qnet;
using namespace Isis;
using namespace std;

namespace Qisis {
  /**
   * Consructs the Navigation Tool window
   *
   * @param parent The parent widget for the navigation tool
   *
   * @internal
   *   @history  2008-12-09 Tracie Sucharski - Added p_filtered indicating whether
   *                           the listBox contains filtered or unfiltered
   *                           list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null.
   *
   */
  QnetNavTool::QnetNavTool(QWidget *parent) : Qisis::Tool(parent) {
    p_navDialog = NULL;
    p_filter = NULL;
    p_tie = NULL;
    p_multiIgnore = NULL;
    p_multiDelete = NULL;
    p_setApriori = NULL;
    p_filterStack = NULL;
    p_listCombo = NULL;
    p_listBox = NULL;
    p_filterCountLabel = NULL;
    p_aprioriDialog = NULL;
    createNavigationDialog(parent);
    connect(this, SIGNAL(deletedPoints()),
        this, SLOT(refreshList()));
    p_filtered = false;
  }

  /**
   * Creates and shows the dialog box for the navigation tool
   *
   * @param parent The parent widget for the navigation dialopg
   *
   * @internal
   *   @history  2008-10-29 Tracie Sucharski - Added filter count
   *   @history  2008-12-31 Jeannie Walldren - Added keyboard shortcuts
   *   @history  2010-11-04 Tracie Sucharski - Move listBox double-click
   *                            connection to the slot for changing the
   *                            listBox.
   */
  void QnetNavTool::createNavigationDialog(QWidget *parent) {
    // Create the combo box selector
    p_listCombo = new QComboBox();
    p_listCombo->addItem("Points");
    p_listCombo->addItem("Cubes");

    p_listBox = new QListWidget();
    p_listBox->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Create the filter area
    QLabel *filterLabel = new QLabel("Filters");
    filterLabel->setAlignment(Qt::AlignHCenter);
    p_filterStack = new QStackedWidget();

    connect(p_listCombo, SIGNAL(activated(int)),
        p_filterStack, SLOT(setCurrentIndex(int)));
    connect(p_listCombo, SIGNAL(activated(int)),
        this, SLOT(resetList()));
    connect(p_listCombo, SIGNAL(activated(int)),
        this, SLOT(enableButtons()));

    //  Create filter count label
    p_filterCountLabel = new QLabel("Filter Count: ");

    // Create action options
    QWidget *actionArea = new QWidget();
    QPushButton *load = new QPushButton("&View Cube(s)", actionArea);
    load->setAutoDefault(false);
    load->setToolTip("Open Selected Images");
    load->setWhatsThis("<b>Function: </b> Opens all selected images, or images \
                        that are associated with the given point or overlap.  \
                        <p><b>Hint: </b> You can select more than one item in \
                        the list by using the shift or control key.</p>");
    connect(load, SIGNAL(clicked()),
        this, SLOT(load()));
    p_tie = new QPushButton("&Modify Point", actionArea);
    p_tie->setAutoDefault(false);
    p_tie->setToolTip("Modify Selected Point");
    p_tie->setWhatsThis("<b>Function: </b> Opens the tie tool to modify the \
                         selected point from the list.  This option is only \
                         available when the nav tool is in point mode");
    connect(p_tie, SIGNAL(clicked()),
        this, SLOT(tie()));

    p_multiIgnore = new QPushButton("&Ignore Points", actionArea);
    p_multiIgnore->setAutoDefault(false);
    p_multiIgnore->setToolTip("Set selected points to Ignore");
    p_multiIgnore->setWhatsThis("<b>Function: </b> Sets the selected points \
                               Ignore = True.  You will not be able to preview \
                               in the Point Editor before their Ignore switch \
                               is set to true. \
                               <p><b>Hint: </b> You can select more than one \
                               item in the list by using the shift or control \
                               key.</p>");
    connect(p_multiIgnore, SIGNAL(clicked()),
        this, SLOT(ignorePoints()));

    p_multiDelete = new QPushButton("&Delete Points", actionArea);
    p_multiDelete->setAutoDefault(false);
    p_multiIgnore->setToolTip("Set selected points to Delete");
    p_multiIgnore->setWhatsThis("<b>Function: </b> Delete the selected points \
                               from control network.  You will not be able to \
                               preview in the Point Editor before they are \
                               deleted. \
                               <p><b>Hint: </b> You can select more than one \
                               item in the list by using the shift or control \
                               key.</p>");
    connect(p_multiDelete, SIGNAL(clicked()),
        this, SLOT(deletePoints()));

    p_setApriori = new QPushButton("&Set Apriori/Sigmas", actionArea);
    p_setApriori->setAutoDefault(false);
    p_setApriori->setToolTip("Set selected points apriori/sigmas");
    p_setApriori->setWhatsThis("<b>Function: </b> Set the apriori points \
                               and sigmas. \
                               <p><b>Hint: </b> You can select more than one \
                               item in the list by using the shift or control \
                               key.</p>");
    connect(p_setApriori, SIGNAL(clicked()), this, SLOT(aprioriDialog()));

    p_filter = new QPushButton("&Filter", actionArea);
    p_filter->setAutoDefault(true);
    p_filter->setToolTip("Filter Current List");
    p_filter->setWhatsThis("<b>Function: </b> Filters the current list by user \
                            specifications made in the selected filter. \
                            <p><b>Note: </b> Any filter options selected in a \
                            filter that is not showing will be ignored.</p>");
    connect(p_filter, SIGNAL(clicked()),
        this, SLOT(filter()));

    QPushButton *reset = new QPushButton("&Show All", actionArea);
    reset->setAutoDefault(false);
    reset->setToolTip("Reset the Current List to show all the values in the list");
    reset->setWhatsThis("<b>Function: </b> Resets the list of points, \
                         overlaps, or images to the complete initial list.  \
                         Any filtering that has been done will be lost.");
    connect(reset, SIGNAL(clicked()),
        this, SLOT(resetList()));
    connect(reset, SIGNAL(clicked()),
        this, SLOT(resetFilter()));

    // Set up layout
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(load);
    layout->addWidget(p_tie);
    layout->addWidget(p_multiIgnore);
    layout->addWidget(p_multiDelete);
    layout->addWidget(p_setApriori);
    layout->addWidget(p_filter);
    layout->addWidget(reset);

    // Create filter stacked widgets
    createFilters();
    p_filterStack->adjustSize();

    // Set up the main window
    p_navDialog = new QDialog(parent);
    p_navDialog->setWindowTitle("Control Network Navigator");

    // Layout everything in the dialog
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(p_listCombo, 0, 0);
    gridLayout->addWidget(filterLabel, 0, 1);
    gridLayout->addWidget(p_listBox, 1, 0);
    gridLayout->addWidget(p_filterStack, 1, 1);
    gridLayout->addWidget(p_filterCountLabel, 2, 0);
    gridLayout->addLayout(layout, 3, 0, 1, 2);
    p_navDialog->setLayout(gridLayout);

    // View the dialog
    p_navDialog->setShown(true);
  }

  /**
   * Sets up the tabbed widgets for the different types of filters available
   *
   * @internal
   *   @history  2007-06-05 Tracie Sucharski - Added enumerators for the filter
   *                           indices to make it easier to re-order filters.
   *                           Also, re-ordered the filters to put commonly used
   *                           first. Comment out overlap/polygon code
   *                           temporarily.
   *   @history  2008-11-26 Jeannie Walldren - Added Goodness of Fit to the filter
   *                           tabs.
   *   @history  2008-12-31 Jeannie Walldren - Added keyboard shortcuts to tabs.
   *   @history  2009-01-26 Jeannie Walldren - Clarified tab names. Added points
   *                           cube name filter tab.
   *   @history  2010-06-02 Jeannie Walldren - Changed tab labels from "Type" to
   *                           "Properties". Updated "What's This?" documentation
   *                           for Measure Properties to explain use of ignore
   *                           status and measure type filters.
   */
  void QnetNavTool::createFilters() {
    // Set up the point filters
    QTabWidget *pointFilters = new QTabWidget();

    QWidget *jigsawErrorFilter = new QnetPointJigsawErrorFilter();
    connect(jigsawErrorFilter, SIGNAL(filteredListModified()),
            this, SLOT(filterList()));
    pointFilters->insertTab(JigsawErrors, jigsawErrorFilter, "&Jigsaw Errors");
    pointFilters->setTabToolTip(JigsawErrors, "Filter Points by Jigsaw Error");
    pointFilters->setTabWhatsThis(JigsawErrors,
        "<b>Function: </b> Filter points list by \
                                     the bundle adjust error value at each  \
                                     point.  You can filter for points that \
                                     have an error greater than, or less than \
                                     the entered value.");

    QWidget *registrationErrorFilter = new QnetPointRegistrationErrorFilter();
    connect(registrationErrorFilter, SIGNAL(filteredListModified()),
            this, SLOT(filterList()));
    pointFilters->insertTab(RegistrationErrors, registrationErrorFilter,
                            "&Registration Errors");
    pointFilters->setTabToolTip(RegistrationErrors,
                                "Filter Points by Registration Error");
    pointFilters->setTabWhatsThis(RegistrationErrors,
        "<b>Function: </b> Filter points list by \
                                     the registration pixel shift value at each  \
                                     point.  You can filter for points that \
                                     have an error greater than, or less than \
                                     the entered value.  The maximum for all \
                                     measures in the point is used");

    QWidget *ptIdFilter = new QnetPointIdFilter();
    connect(ptIdFilter, SIGNAL(filteredListModified()),
            this, SLOT(filterList()));
    pointFilters->insertTab(Id, ptIdFilter, "&Point ID");
    pointFilters->setTabToolTip(Id, "Filter Points by PointID");

    QWidget *ptImageFilter = new QnetPointImagesFilter();
    connect(ptImageFilter, SIGNAL(filteredListModified()),
            this, SLOT(filterList()));
    pointFilters->insertTab(NumberImages, ptImageFilter, "&Number of Measures");
    pointFilters->setTabToolTip(NumberImages, "Filter Points by Number of Images");
    pointFilters->setTabWhatsThis(NumberImages, "<b>Function: </b> Filter points list \
                                     by the number of images that are in  \
                                     each point. You can filter for         \
                                     points that have more than the given   \
                                     number of images, or less than the \
                                     given number of images.  Points with   \
                                     the exact number of images specified \
                                     will not be included in the filtered \
                                     list.");
    QWidget *typeFilter = new QnetPointTypeFilter();
    connect(typeFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    pointFilters->insertTab(Type, typeFilter, "Point Properties");
    pointFilters->setTabToolTip(Type, "Filter Points by Listed Properties");
    pointFilters->setTabWhatsThis(Type, "<b>Function: </b> Filter points list by \
                                     their Point Type, Ignore status, or Held status properties");
    QWidget *rangeFilter = new QnetPointRangeFilter();
    connect(rangeFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    pointFilters->insertTab(LatLonRange, rangeFilter, "&Range");
    pointFilters->setTabToolTip(LatLonRange, "Filter Points by Range");
    pointFilters->setTabWhatsThis(LatLonRange, "<b>Function: </b> Filters out points \
                                     that are within a user set range lat/lon \
                                     range.");
    QWidget *ptDistFilter = new QnetPointDistanceFilter();
    connect(ptDistFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    pointFilters->insertTab(Distance, ptDistFilter, "Dist&ance");
    pointFilters->setTabToolTip(Distance, "Filter Points by Distance");
    pointFilters->setTabWhatsThis(Distance,
        "<b>Function: </b> Filter points list by \
                                     a user specified maximum distance from \
                                     any other point.");
    QWidget *measureFilter = new QnetPointMeasureFilter();
    connect(measureFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    pointFilters->insertTab(MeasureType, measureFilter, "Measure Properties");
    pointFilters->setTabToolTip(MeasureType, "Filter Points by Listed Measure Properties");
    pointFilters->setTabWhatsThis(MeasureType,
        "<b>Function: </b> Filter points list by \
                                     the properties of their measures. User may \
                                     filter by Measure Type or Ignore status. \
                                     If one or more measure from a point is found to \
                                     match a selected measure type, and that measure \
                                     satisfies the ignore status selected, the point \
                                     will be left in the filtered list.  More \
                                     than one measure type can be selected. \
                                     Only one Ignore status may be selected.");

    QWidget *goodnessFilter = new QnetPointGoodnessFilter();
    connect(goodnessFilter, SIGNAL(filteredListModified()),
            this, SLOT(filterList()));
    pointFilters->insertTab(GoodnessOfFit, goodnessFilter, "&Goodness of Fit");
    pointFilters->setTabToolTip(GoodnessOfFit, "Filter Points by the Goodness of Fit of its measures");
    pointFilters->setTabWhatsThis(GoodnessOfFit,
                                  "<b>Function: </b> Filter points list by \
                                     the goodness of fit.");
    QWidget *cubeNamesFilter = new QnetPointCubeNameFilter();
    connect(cubeNamesFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    connect(this, SIGNAL(serialListModified()),
        cubeNamesFilter, SLOT(createCubeList()));

    pointFilters->insertTab(CubeName, cubeNamesFilter, "&Cube Name(s)");
    pointFilters->setTabToolTip(CubeName, "Filter Points by Cube Filename(s)");
    pointFilters->setTabWhatsThis(CubeName,
        "<b>Function: </b> Filter points list by \
                                     the filenames of cubes. This filter will \
                                     show all points contained in a single \
                                     image or all points contained in every \
                                     cube selected.");
    // Set up the cube filters
    QTabWidget *cubeFilters = new QTabWidget();

    QWidget *cubeNameFilter = new QnetCubeNameFilter();
    connect(cubeNameFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    cubeFilters->insertTab(Name, cubeNameFilter, "&Cube Name");
    cubeFilters->setTabToolTip(Name, "Filter Images by Cube Name");

    QWidget *cubePtsFilter = new QnetCubePointsFilter();
    connect(cubePtsFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    cubeFilters->insertTab(NumberPoints, cubePtsFilter, "&Number of Points");
    cubeFilters->setTabToolTip(NumberPoints, "Filter Images by Number of Points");
    cubeFilters->setTabWhatsThis(NumberPoints,
        "<b>Function: </b> Filter images list by \
                                    the number of points that are in each \
                                    image. You can filter for images that have \
                                    more than the given number of points, or \
                                    less than the given number of point.  \
                                    Images with the exact number of points \
                                    specified will not be included in the \
                                    filtered list.");
    QWidget *cubeDistFilter = new QnetCubeDistanceFilter();
    connect(cubeDistFilter, SIGNAL(filteredListModified()),
        this, SLOT(filterList()));
    cubeFilters->insertTab(PointDistance, cubeDistFilter, "Dist&ance");
    cubeFilters->setTabToolTip(PointDistance, "Filter Images by Distance between Points");
    cubeFilters->setTabWhatsThis(PointDistance,
        "<b>Function: </b> Filter images list by \
                                    a user specified distance between points \
                                    in the image. This may be calculated in \
                                    meters or by pixel distance.");

    // Add widgets to the filter stack
    p_filterStack->addWidget(pointFilters);
    p_filterStack->addWidget(cubeFilters);
  }

  /**
   * Resets the list box with whatever is in the global lists
   *
   * @internal
   *   @history  2007-06-05 Tracie Sucharski - Use enumerators to test which
   *                           filter is chosen.  Comment overlap/polygon code
   *                           temporarily.
   *   @history  2008-10-29 Tracie Sucharski - Added filter count 2008-11-26
   *                           Tracie Sucharski - Remove all polygon/overlap
   *                           references, this functionality will be qmos.
   *   @history  2008-12-09 Tracie Sucharski - Renamed method from updateList to
   *                           resetList since it it reseting all of the filtered
   *                           lists and the listBox to the entire network of
   *                           points and serial numbers.
   *   @history  2008-12-09 Tracie Sucharski - Added p_filtered indicating whether
   *                           the listBox contains filtered or unfiltered list.
   *   @history  2009-01-08 Jeannie Walldren - Reset filtered list with all points
   *                           in control net and all images in serial number
   *                           list.
   *   @history  2010-11-04 Tracie Sucharski - Added double-click connections.
   */
  void QnetNavTool::resetList() {
    p_filtered = false;
    // Dont do anything if there are no cubes loaded
    if (g_serialNumberList == NULL)
      return;

    // Clear the old list and filtered lists and update with the entire list
    p_listBox->setCurrentRow(-1);
    p_listBox->clear();
    g_filteredPoints.clear();
    g_filteredImages.clear();

    // copy the control net indices into the filtered points list
    for (int i = 0; i < g_controlNetwork->GetNumPoints(); i++) {
      g_filteredPoints.push_back(i);
    }
    // copy the serial number indices into the filtered images list
    for (int i = 0; i < g_serialNumberList->Size(); i++) {
      g_filteredImages.push_back(i);
    }

    // We are dealing with points so output the point numbers
    if (p_listCombo->currentIndex() == Points) {
      // disconnect any old signals
      disconnect(p_listBox, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(load(QListWidgetItem *)));
      connect(p_listBox, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(editPoint(QListWidgetItem *)));
      //p_listBox->setSelectionMode(QAbstractItemView::SingleSelection);
      for (int i = 0; i < g_controlNetwork->GetNumPoints(); i++) {
        string cNetId = (*g_controlNetwork)[i]->GetId();
        QString itemString = cNetId.c_str();
        p_listBox->insertItem(i, itemString);
        int images = (*g_controlNetwork)[i]->GetNumMeasures();
        p_listBox->item(i)->setToolTip(QString::number(images) + " image(s) in point");
      }
      //  Make sure edit point is selected and in view.
      updateEditPoint(p_editPointId);

      QString msg = "Filter Count: " + QString::number(p_listBox->count()) +
          " / " + QString::number(g_controlNetwork->GetNumPoints());
      p_filterCountLabel->setText(msg);
    }
    // We are dealing with images so output the cube names
    else if (p_listCombo->currentIndex() == Cubes) {
      disconnect(p_listBox, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(editPoint(QListWidgetItem *)));
      connect(p_listBox, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(load(QListWidgetItem *)));
      //p_listBox->setSelectionMode(QAbstractItemView::ExtendedSelection);
      for (int i = 0; i < g_serialNumberList->Size(); i++) {
        Isis::Filename filename = Isis::Filename(g_serialNumberList->Filename(i));
        string tempFilename = filename.Name();
        p_listBox->insertItem(i, tempFilename.c_str());
      }
      QString msg = "Filter Count: " + QString::number(p_listBox->count()) +
          " / " + QString::number(g_serialNumberList->Size());
      p_filterCountLabel->setText(msg);

    }
  }


  /**
   * Update the list showing the new point highlighted.
   *
   * @param pointId Value of the PointId keyword for the new point.
   * @internal
   *   @history  2008-12-30 Jeannie Walldren - Modified to setCurrentItem() rather
   *                           than simply highlight the new point using
   *                           setItemSelected() and scrollToItem().
   *   @history  2010-11-01 Tracie Sucharski - Changed pointId from std::string
   *                           to QString.
   */
  void QnetNavTool::updateEditPoint(QString pointId) {

    p_editPointId = pointId;
    if (pointId == "")
      return;

    QList<QListWidgetItem *> items = p_listBox->findItems(pointId, Qt::MatchExactly);
    if (items.isEmpty()) {
      p_listBox->clearSelection();
    }
    else {
      p_listBox->setCurrentItem(items.at(0));
    }
    return;
  }



  /**
   *   Slot to refresh the listBox
   *
   *   @internal
   *     @history  2008-12-09 Tracie Sucharski - Slot to refresh the ListBox
   */
  void QnetNavTool::refreshList() {

    if (p_filtered) {
      filter();
    }
    else {
      resetList();
    }

  }

  /**
   * Resets the visible filter to the default values
   */
  void QnetNavTool::resetFilter() {

  }

  /**
   * Updates the list box in the nav window with a new list from one of the
   * filters
   *
   * @internal
   *   @history  2007-06-05 Tracie Sucharski - Use enumerators for the filter
   *                           indices.  Comment out overlap/polygon code
   *                           temporarily.
   *   @history  2008-10-29 Tracie Sucharski - Added filter count
   *
   */
  void QnetNavTool::filterList() {
    // Don't do anything if there are no cubes loaded
    if (g_serialNumberList == NULL)
      return;

    // Clears the old list and puts the filtered list in its place
    p_listBox->setCurrentRow(-1);
    p_listBox->clear();

    // We are dealing with points so output the point numbers
    if (p_listCombo->currentIndex() == Points) {
      for (int i = 0; i < g_filteredPoints.size(); i++) {
        string cNetId = (*g_controlNetwork)[g_filteredPoints[i]]->GetId();
        QString itemString = cNetId.c_str();
        p_listBox->insertItem(i, itemString);
        int images = (*g_controlNetwork)[g_filteredPoints[i]]->GetNumMeasures();
        p_listBox->item(i)->setToolTip(QString::number(images) + " image(s) in point");
      }
      QString msg = "Filter Count: " + QString::number(p_listBox->count()) +
          " / " + QString::number(g_controlNetwork->GetNumPoints());
      p_filterCountLabel->setText(msg);
    }
    // We are dealing with images so write out the cube names
    else if (p_listCombo->currentIndex() == Cubes) {
      for (int i = 0; i < g_filteredImages.size(); i++) {
        Isis::Filename filename = Isis::Filename(g_serialNumberList->Filename(g_filteredImages[i]));
        string tempFilename = filename.Name();
        p_listBox->insertItem(i, tempFilename.c_str());
      }
      QString msg = "Filter Count: " + QString::number(p_listBox->count()) +
          " / " + QString::number(g_serialNumberList->Size());
      p_filterCountLabel->setText(msg);
    }
  }



  /**
   * Tells the filetool to load an image, slot for "View Cube(s)" button
   *
   * @internal
   *   @history  2007-06-05 Tracie Sucharski - Use enumerators for the filter
   *                           indices.  Comment out overlap/polygon code
   *                           temporarily.
   *   @history  2008-11-19 Jeannie Walldren - Added Qt::WaitCursor (i.e. clock or
   *                           hourglass) to indicate that there is background
   *                           activity while this method is running
   *   @history  2008-11-26 Tracie Sucharski - Remove all polygon/overlap
   *                           references, this functionality will be qmos.
   *   @history 2010-06-03 Jeannie Walldren - Modified "No file selected" warning
   *                          to check whether selected list is empty rather than
   *                          the index of current row.
   */
  void QnetNavTool::load() {
    // Dont do anything if no cubes are loaded
    if (g_serialNumberList == NULL)
      return;

    // TODO: How do we handle them opening 2 overlaps or points that have the
    //       cube in them, open 2 of the same file or only open 1???
    QList<QListWidgetItem *> selected = p_listBox->selectedItems();
    if (selected.size() < 1) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No file selected to load.");
      return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int i = 0; i < selected.size(); i++) {
      int index = p_listBox->row(selected[i]);
      // Tell the filetool to load all images for the given point
      if (p_listCombo->currentIndex() == Points) {
        if (g_filteredPoints.size() == 0) {
          emit loadPointImages((*g_controlNetwork)[index]);
        }
        else {
          emit loadPointImages((*g_controlNetwork)[g_filteredPoints[index]]);
        }
      }
      // Tell the filetool to load the given image
      else if (p_listCombo->currentIndex() == Cubes) {
        if (g_filteredImages.size() == 0) {
          string serialNumberList = (*g_serialNumberList).SerialNumber(index);
          QString sn = serialNumberList.c_str();
          emit loadImage(sn);
        }
        else {
          string serialNumberList = (*g_serialNumberList).SerialNumber(g_filteredImages[index]);
          QString sn = serialNumberList.c_str();
          emit loadImage(sn);
        }
      }
    }
    QApplication::restoreOverrideCursor();
    return;
  }




  /**
   * Slot for double-clicking cube list.  Needed this slot because the signal 
   * has a QListWidgetItem parameter.  TODO:  Clean this up by possibly combining 
   * the two different load slots??? 
   *
   * @author 2010-11-04 Tracie Sucharski
   *
   * @internal
   */
  void QnetNavTool::load(QListWidgetItem *) {
    load();
  }

  /**
   * Emits a modifyPoint signal
   *
   * @param ptItem
   */
  void QnetNavTool::editPoint(QListWidgetItem *ptItem) {

    int index = p_listBox->row(ptItem);
    if (g_filteredPoints.size() == 0) {
      emit modifyPoint((*g_controlNetwork)[index]);
    }
    else {
      emit modifyPoint((*g_controlNetwork)[g_filteredPoints[index]]);
    }

  }

  /**
   * Calls the qnet tool for the given control point.
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Modified "No file selected" warning
   *                          to check whether selected list is empty rather than
   *                          the index of current row.
   */
  void QnetNavTool::tie() {

    QList<QListWidgetItem *> selected = p_listBox->selectedItems();
    if (selected.size() > 1) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Only one point can be modified at a time");
      return;
    }
    else if (selected.size() < 1) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No point selected to modify.");
      return;
    }
    int index = p_listBox->row(selected[0]);
    if (g_filteredPoints.size() == 0) {
      emit modifyPoint((*g_controlNetwork)[index]);
    }
    else {
      emit modifyPoint((*g_controlNetwork)[g_filteredPoints[index]]);
    }
  }


  /**
   * Set Ignored=True for selected Points
   *
   * @author 2008-12-09 Tracie Sucharski
   * @internal
   *   @history  2008-12-29 Jeannie Walldren - Added question box to verify that
   *                           the user wants to set the selected points to
   *                           ignore=true.
   *
   */
  void QnetNavTool::ignorePoints() {
    // do nothing if no cubes are loaded
    if (g_serialNumberList == NULL) return;

    int index = p_listBox->currentRow();
    if (index < 0) {
      QApplication::restoreOverrideCursor();
      QMessageBox::information((QWidget *)parent(),
          "Error", "No point selected to ignore");
      return;
    }
    QList<QListWidgetItem *> selected = p_listBox->selectedItems();
    switch (QMessageBox::question((QWidget *)parent(),
        "Control Network Navigator - Ignore Points",
        "You have chosen to set "
        + QString::number(selected.size())
        + " point(s) to ignore. Do you want to continue?",
        "&Yes", "&No", 0, 0)) {
      case 0: // Yes was clicked or Enter was pressed, delete points
        QApplication::setOverrideCursor(Qt::WaitCursor);
        int lockedPoints = 0;
        for (int i = 0; i < selected.size(); i++) {
          int index = p_listBox->row(selected[i]);
          if (g_filteredPoints.size() == 0) {
            if ((*g_controlNetwork)[index]->SetIgnored(true) ==
                ControlPoint::PointLocked) lockedPoints++;
          }
          else {
            if ((*g_controlNetwork)[g_filteredPoints[index]]->SetIgnored(true)
                == ControlPoint::PointLocked) lockedPoints++;
          }
          emit pointChanged((*g_controlNetwork)[g_filteredPoints[index]]->GetId());
        }
        //  Print info about locked points if there are any
        if (lockedPoints > 0) {
          QMessageBox::information((QWidget *)parent(),"EditLocked Points",
                QString::number(lockedPoints) + " / "
                + QString::number(selected.size())
                + " points are EditLocked and were not set to Ignored.");
        }
        QApplication::restoreOverrideCursor();
        if (lockedPoints != selected.size()) emit netChanged();

        break;
        //  case 1: // No was clicked, close window and do nothing to points
    }
    return;
  }



  /**
   * Delete selected Points from control network
   *
   * @author 2008-12-09 Tracie Sucharski
   * @internal
   *   @history 2008-12-29 Jeannie Walldren - Added question box to verify that
   *                          the user wants to delete the selected points.
   */
  void QnetNavTool::deletePoints() {
    // do nothing if no cubes are loaded
    if (g_serialNumberList == NULL)
      return;

    int index = p_listBox->currentRow();
    if (index < 0) {
      QApplication::restoreOverrideCursor();
      QMessageBox::information((QWidget *)parent(),
          "Error", "No point selected to delete");
      return;
    }

    QList<QListWidgetItem *> selected = p_listBox->selectedItems();
    switch (QMessageBox::question((QWidget *)parent(),
        "Control Network Navigator - Delete Points",
        "You have chosen to delete "
        + QString::number(selected.size())
        + " point(s). Do you want to continue?",
        "&Yes", "&No", 0, 0)) {
      case 0: // Yes was clicked or Enter was pressed, delete points
        QApplication::setOverrideCursor(Qt::WaitCursor);

        int editPointIndex = 0;
        int lockedPoints = 0;
        vector<int> deletedRows;
        for (int i = 0; i < selected.size(); i++) {
          QString id = selected.at(i)->text();
          //  Keep track of rows #'s that are being deleted.
          deletedRows.push_back(p_listBox->row(selected.at(i)));
          if (id == p_editPointId) {
            editPointIndex = deletedRows[i];
          }
          if (g_controlNetwork->DeletePoint(id) == ControlPoint::PointLocked)
            lockedPoints++;
        }

        //  Print info about locked points if there are any
        if (lockedPoints > 0) {
          QMessageBox::information((QWidget *)parent(),"EditLocked Points",
                QString::number(lockedPoints) + " / "
                + QString::number(selected.size())
                + " points are EditLocked and were not deleted.");
        }

        QApplication::restoreOverrideCursor();
        emit deletedPoints();
        emit netChanged();
        break;
        //  case 1: // No was clicked, close window and do nothing to points
    }
    return;
  }



  /**
   * Bring up apriori dialog
   *
   * @author 2011-04-19 Tracie Sucharski 
   *  
   * @internal 
   * @todo  This method should be temporary until the control point editor 
   *           comes online.  If this stick around, needs to be re-disigned-
   *           put in a separate class??
   *  
   */ 
  void QnetNavTool::aprioriDialog() {
    // If no cubes are loaded, simply return
    if (g_serialNumberList == NULL)
      return;

    if (!p_aprioriDialog) {
      p_aprioriDialog = new QnetSetAprioriDialog();
      setAprioriDialogPoints();
      connect(p_listBox, SIGNAL(itemSelectionChanged()),
              this, SLOT(setAprioriDialogPoints()));
      connect(p_aprioriDialog, SIGNAL(pointChanged(QString)),
              this, SIGNAL(pointChanged(QString)));
      connect(p_aprioriDialog, SIGNAL(netChanged()),
              this, SIGNAL(netChanged()));
    }
    p_aprioriDialog->show();
    p_aprioriDialog->activateWindow();
  }



  /**
   * Slot to pass points selected in Nav List Widget to Apriori Dialog 
   *  
   * @internal 
   * @history 2011-05-04 Tracie Sucharski - Do not print error if no pts 
   *                        selected, simply return.
   */
  void QnetNavTool::setAprioriDialogPoints() {

    if (p_aprioriDialog == NULL) return;

    int index = p_listBox->currentRow();
    if (index < 0) return;

    QList<QListWidgetItem *> selected = p_listBox->selectedItems();
    p_aprioriDialog->setPoints(selected);

  }


  /**
   * Figures out what type of widget the filter was selected for and calls the
   * filter method for that filter class
   *
   * @internal
   *   @history  2007-06-05 Tracie Sucharski - Use enumerators for the filter
   *                           indices.  Comment out overlap/polygon code
   *                           temporarily.
   *   @history  2008-11-19 Jeannie Walldren - Added WaitCursor (i.e. clock or
   *                           hourglass) to indicate that there is background
   *                           activity while this method is running
   *   @history  2008-11-26 Tracie Sucharski - Remove all polygon/overlap
   *                           references, this functionality will be qmos.
   *   @history  2008-12-09 Tracie Sucharski - Added p_filtered indicating whether
   *                           the listBox contains filtered or unfiltered list.
   *   @history  2009-01-08 Jeannie Walldren - Removed command to clear filtered
   *                           points and images lists
   *   @history  2009-01-26 Jeannie Walldren - Added filter call for points cube
   *                           name filter.
   *
   */
  void QnetNavTool::filter() {

    p_filtered = true;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    p_filter->setEnabled(false);

    QTabWidget *tab = (QTabWidget *)(p_filterStack->currentWidget());

    // We're dealing with points
    if (p_listCombo->currentIndex() == Points) {
      PointFilterIndex pointIndex = (PointFilterIndex) tab->currentIndex();
      // We have a jigsaw errors filter
      if (pointIndex == JigsawErrors) {
        QnetPointJigsawErrorFilter *widget =
          (QnetPointJigsawErrorFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a registration errors filter
      if (pointIndex == RegistrationErrors) {
        QnetPointRegistrationErrorFilter *widget =
          (QnetPointRegistrationErrorFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a point id filter
      else if (pointIndex == Id) {
        QnetPointIdFilter *widget =
          (QnetPointIdFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a number of images filter
      else if (pointIndex == NumberImages) {
        QnetPointImagesFilter *widget =
          (QnetPointImagesFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a point type filter
      else if (pointIndex == Type) {
        QnetPointTypeFilter *widget =
          (QnetPointTypeFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a lat/lon range filter
      else if (pointIndex == LatLonRange) {
        QnetPointRangeFilter *widget =
          (QnetPointRangeFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a distance filter
      else if (pointIndex == Distance) {
        QnetPointDistanceFilter *widget =
          (QnetPointDistanceFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a measure filter
      else if (pointIndex == MeasureType) {
        QnetPointMeasureFilter *widget =
          (QnetPointMeasureFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a goodness of fit filter
      else if(pointIndex == GoodnessOfFit) {
        QnetPointGoodnessFilter *widget =
          (QnetPointGoodnessFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a cube name filter
      else if (pointIndex == CubeName) {
        QnetPointCubeNameFilter *widget =
          (QnetPointCubeNameFilter *)(tab->currentWidget());
        widget->filter();
      }
    }

    // We're dealing with cubes
    else if (p_listCombo->currentIndex() == Cubes) {
      CubeFilterIndex cubeIndex = (CubeFilterIndex) tab->currentIndex();
      // We have a cube name filter
      if (cubeIndex == Name) {
        QnetCubeNameFilter *widget =
          (QnetCubeNameFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a point filter
      else if (cubeIndex == NumberPoints) {
        QnetCubePointsFilter *widget =
          (QnetCubePointsFilter *)(tab->currentWidget());
        widget->filter();
      }
      // We have a distance filter
      else if (cubeIndex == PointDistance) {
        QnetCubeDistanceFilter *widget =
          (QnetCubeDistanceFilter *)(tab->currentWidget());
        widget->filter();
      }
    }
    p_filter->setEnabled(true);
    QApplication::restoreOverrideCursor();
    return;
  }

  /**
   * Enable/disable buttons depending on whether Points or Cubes is chosen
   *
   * @internal
   *   @history  2008-12-09 Tracie Sucharski - Renamed from enableTie to
   *                           enableButtons.  Added ignore and delete buttons.
   */
  void QnetNavTool::enableButtons() {
    if (p_listCombo->currentIndex() == Points) {
      p_tie->setEnabled(true);
      p_multiIgnore->setEnabled(true);
      p_multiDelete->setEnabled(true);
      p_setApriori->setEnabled(true);
    }
    else {
      p_tie->setEnabled(false);
      p_multiIgnore->setEnabled(false);
      p_multiDelete->setEnabled(false);
      p_setApriori->setEnabled(false);
      if (p_aprioriDialog != NULL) {
        p_aprioriDialog->close();
      }
    }
  }

  /**
   * This slot is connected to the file tool in qnet.cpp.
   * It emits a signal that the serial list has been modified so
   * the points cube name filter knows to change the list box
   * displayed.
   * @see QnetPointCubeNameFilter
   * @internal
   *   @history 2009-01-26 Jeannie Walldren - Original version.
   */
  void QnetNavTool::resetCubeList() {
    emit serialListModified();
  }

  /**
   * This method sets the Navigation Dialog window to shown=true.
   *
   * @author Jeannie Walldren
   * @internal
   *   @history 2010-07-01 Jeannie Walldren - Original version.
   */
  void QnetNavTool::showNavTool() {
    p_navDialog->setShown(true);
  }

#if 0
  /**
   * List selected points which are EditLocked and allow user to un-lock
   */
  void QnetNavTool::listLockedPoints() {

    QDialog lockDialog = new QDialog();
    lockDialog.setWindowTitle("Un-lock Points");
    lockDialog.setModal(true);

    QListWidget editLockPointsListBox = new QListWidget(lockDialog);
    connect(editLockPointsListBox, SIGNAL(itemChanged(QListWidgetItem *)),
            this, SLOT(unlockPoint(QListWidgetItem *)));

    QList<QListWidgetItem *> selected = p_listBox->selectedItems();
    QList<QListWidgetItem *> lockedPoints;

    // Fill editLock List Box
    for (int i=0; i<selected.size(); i++) {
      QString id = selected.at(i)->text();
      ControlPoint *pt = g_controlNetwork->GetPoint(id);
      if (pt->IsEditLocked()) {
        QListWidgetItem *item = new QListWidgetItem(*(selected[i]));
        item->setCheckState(Qt::Checked);
        editLockPointsListBox->addItem(item);
      }
    }

    if (lockDialog.exec()) {

    }



    switch (QMessageBox::question((QWidget *)parent(),
        "Control Network Navigator - Ignore Points",
        "You have chosen to set "
        + QString::number(selected.size())
        + " point(s) to ignore. Do you want to continue?",
        "&Yes", "&No", 0, 0)) {
      case 0: // Yes was clicked or Enter was pressed, delete points
        QApplication::setOverrideCursor(Qt::WaitCursor);
          emit pointChanged((*g_controlNetwork)[g_filteredPoints[index]]->GetId());
        }


  }

  void QnetNavTool::unlockPoint(QListWidgetItem *pointId) {
    ControlPoint *pt = g_controlNetwork->GetPoint(pointId->text());
    if (pt->IsEditLocked() && pointId->checkState() == Qt::Unchecked) {
      pt->SetEditLock(false);
      editLockPointsListBox->removeItemWidget(pointId);
      pointId->setHidden(true);
      editLockPointsListBox->repaint();
      this->repaint();
      emit netChanged();
    }

  }
#endif
}

