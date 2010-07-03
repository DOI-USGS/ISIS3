#include "MeasureTool.h"

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

#include "geos/geom/Geometry.h"
#include "geos/geom/Point.h"

#include "Camera.h"
#include "Filename.h"
#include "MdiCubeViewport.h"
#include "ToolPad.h"


namespace Qisis {
  /**
   * MeasureTool constructor
   *
   *
   * @param parent
   */
  MeasureTool::MeasureTool(QWidget *parent) : Qisis::Tool(parent) {
    p_rubberBand = NULL;
    p_tableWin = new TableMainWindow("Measurements", parent);
    p_tableWin->setTrackListItems(true);

    // Create the action for showing the table
    p_action = new QAction(parent);
    p_action->setText("Measuring ...");

    connect(p_action, SIGNAL(triggered()), p_tableWin, SLOT(showTable()));
    connect(p_action, SIGNAL(triggered()), p_tableWin, SLOT(raise()));
    connect(p_action, SIGNAL(triggered()), p_tableWin, SLOT(syncColumns()));
    p_tableWin->installEventFilter(this);

    p_tableWin->addToTable(false, "Feature\nName", "Feature Name");
    p_tableWin->addToTable(false, "Feature\nType", "Feature Type");
    p_tableWin->addToTable(true,
                           "Start\nLatitude:Start\nLongitude:End\nLatitude:End\nLongitude",
                           "Ground Range", -1, Qt::Horizontal, "Start Latitude/Longitude to End Latitude/Longitude");
    p_tableWin->addToTable(false, "Start\nSample:Start\nLine:End\nSample:End\nLine",
                           "Pixel Range", -1, Qt::Horizontal, "Start Sample/Line to End Sample/Line");
    p_tableWin->addToTable(true, "Kilometer\nDistance", "Kilometer Distance");
    p_tableWin->addToTable(false, "Meter\nDistance", "Meter Distance");
    p_tableWin->addToTable(false, "Pixel\nDistance", "Pixel Distance");
    p_tableWin->addToTable(false, "Degree\nAngle", "Degree Angle");
    p_tableWin->addToTable(false, "Radian\nAngle", "Radian Angle");
    p_tableWin->addToTable(false, "Kilometer\nArea", "Kilometer Area");
    p_tableWin->addToTable(false, "Meter\nArea", "Meter Area");
    p_tableWin->addToTable(false, "Pixel\nArea", "Pixel Area");
    p_tableWin->addToTable(false, "Segments Sum\nkm", "Segments Sum", -1, Qt::Horizontal, "Sum of Segment lengths in kilometers");
    p_tableWin->addToTable(false, "Segment Number", "Segment Number", -1, Qt::Horizontal, "Segment number of a segmented line");
    p_tableWin->addToTable(false, "Path", "Path");
    p_tableWin->addToTable(false, "Filename", "Filename");
    p_tableWin->addToTable(false, "Notes", "Notes");

    // Setup 10 blank rows in the table
    for(int r = 0; r < 4; r++) {
      p_tableWin->table()->insertRow(r);
      for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        p_tableWin->table()->setItem(r, c, item);
      }
    }

    p_tableWin->setStatusMessage("Click, Drag, and Release to Measure a Line");
  }


  /**
   * Add the measure tool action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MeasureTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir() + "/measure.png"));
    action->setToolTip("Measure (M)");
    action->setShortcut(Qt::Key_M);

    QString text  =
      "<b>Function:</b>  Measure features in active viewport \
      <p><b>Shortcut:</b> M</p> ";
    action->setWhatsThis(text);

    return action;
  }


  /**
   * Creates the widget (button) that goes on the tool bar
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *MeasureTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);
    QToolButton *measureButton = new QToolButton(hbox);
    measureButton->setText("Table");
    measureButton->setToolTip("Record Measurement Data in Table");
    QString text =
      "<b>Function:</b> This button will bring up a table that will record the \
     starting and ending points of the line, along with the distance between \
     the two points on the image.  To measure the distance between two points, \
      click on the first point and releasing the mouse at the second point. \
      <p><b>Shortcut:</b>  CTRL+M</p>";
    measureButton->setWhatsThis(text);
    measureButton->setShortcut(Qt::CTRL + Qt::Key_M);
    connect(measureButton, SIGNAL(clicked()), p_tableWin, SLOT(showTable()));
    connect(measureButton, SIGNAL(clicked()), p_tableWin, SLOT(syncColumns()));
    connect(measureButton, SIGNAL(clicked()), p_tableWin, SLOT(raise()));
    measureButton->setEnabled(true);

    p_rubberBand = new RubberBandComboBox(
      RubberBandComboBox::Angle |
      RubberBandComboBox::Circle |
      RubberBandComboBox::Ellipse |
      RubberBandComboBox::Line |
      RubberBandComboBox::Rectangle |
      RubberBandComboBox::RotatedRectangle |
      RubberBandComboBox::Polygon |
      RubberBandComboBox::SegmentedLine, // options
      RubberBandComboBox::Line // default
    );

    p_distLineEdit = new QLineEdit(hbox);
    p_distLineEdit->setText("");
    p_distLineEdit->setMaxLength(12);
    p_distLineEdit->setToolTip("Line Length");
    QString text2 = "<b>Function: </b> Shows the length of the line drawn on \
                     the image.";
    p_distLineEdit->setWhatsThis(text2);
    p_distLineEdit->setReadOnly(true);

    p_showAllSegments = new QCheckBox(hbox);
    p_showAllSegments->setText("Show All Segments");

    p_unitsComboBox = new QComboBox(hbox);
    p_unitsComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    updateUnitsCombo();
    connect(p_unitsComboBox, SIGNAL(activated(int)), this, SLOT(updateDistEdit()));
    connect(RubberBandTool::getInstance(), SIGNAL(modeChanged()), this, SLOT(updateUnitsCombo()));

    miComboUnit = -1;

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(p_rubberBand);
    layout->addWidget(p_distLineEdit);
    layout->addWidget(p_unitsComboBox);
    layout->addWidget(measureButton);
    layout->addWidget(p_showAllSegments);
    layout->addStretch(1);
    hbox->setLayout(layout);
    return hbox;
  }


  /**
   * Updates the units combo box.
   *
   */
  void MeasureTool::updateUnitsCombo(void) {
    // get the previous index if not initializing
    if(miComboUnit >= 0) {
      miComboUnit = p_unitsComboBox->currentIndex();
    }

    p_unitsComboBox->clear();
    p_showAllSegments->setEnabled(false);

    if(RubberBandTool::getMode() == RubberBandTool::Line ||
        RubberBandTool::getMode() == RubberBandTool::SegmentedLine) {

      if(RubberBandTool::getMode() == RubberBandTool::SegmentedLine) {
        p_showAllSegments->setEnabled(true);
      }

      p_unitsComboBox->addItem("km");
      p_unitsComboBox->addItem("m");
      p_unitsComboBox->addItem("pixels");
      if(miComboUnit < 0 || miComboUnit > 2) {   // default && error checking
        miComboUnit = 2;
      }
    }
    else if(RubberBandTool::getMode() == RubberBandTool::Angle) {
      p_unitsComboBox->addItem("degrees");
      p_unitsComboBox->addItem("radians");
      if(miComboUnit > 1 || miComboUnit < 0) {   // default && error checking
        miComboUnit = 0;
      }
    }
    else {
      p_unitsComboBox->addItem("km^2");
      p_unitsComboBox->addItem("m^2");
      p_unitsComboBox->addItem("pix^2");
      if(miComboUnit < 0 || miComboUnit > 2) {   // default && error checking
        miComboUnit = 2;
      }
    }

    p_unitsComboBox->setCurrentIndex(miComboUnit);
  }


  /**
   * Adds the measure action to the given menu.
   *
   *
   * @param menu
   */
  void MeasureTool::addTo(QMenu *menu) {
    menu->addAction(p_action);
  }


  /**
   * Updates the Measure specifications.
   *
   */
  void MeasureTool::updateMeasure() {
    MdiCubeViewport *cvp = cubeViewport();
    MdiCubeViewport *d;
    p_numLinked = 0;

    if(cvp == NULL) {
      p_tableWin->clearRow(p_tableWin->currentRow());
      return;
    }

    updateDist(cvp, p_tableWin->currentRow());

    p_tableWin->table()->selectRow(p_tableWin->currentRow());

    if(cvp->isLinked()) {
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        d = (*(cubeViewportList()))[i];

        if(d->isLinked() && d != cvp) {
          p_numLinked++;
          updateDist(d, p_tableWin->currentRow() + p_numLinked);
        }
      }
    }
  }


  /**
   * Called when the rubberBanding by the user is finished.
   *
   */
  void MeasureTool::rubberBandComplete() {
    updateMeasure();

    if(RubberBandTool::getMode() != RubberBandTool::Angle && p_unitsComboBox->currentIndex() != 2) {
      if(p_cvp->camera() == NULL && p_cvp->projection() == NULL) {
        QMessageBox::information((QWidget *)parent(), "Error",
                                 "File must have a Camera Model or Projection to measure in km or m");
        return;
      }
    }

    if(!p_tableWin->table()->isVisible()) return;
    if(p_tableWin->table()->item(p_tableWin->currentRow(), StartLineIndex)->text() == "N/A" &&
        p_tableWin->table()->item(p_tableWin->currentRow(), AngleDegIndex)->text() == "N/A" &&
        p_tableWin->table()->item(p_tableWin->currentRow(), AreaPixIndex)->text() == "N/A") return;

    if(!p_cvp->isLinked())p_tableWin->setCurrentRow(p_tableWin->currentRow() + 1);
    if(p_cvp->isLinked()) {
      p_tableWin->setCurrentRow(p_tableWin->currentRow() + p_numLinked + 1);
    }
    p_tableWin->setCurrentIndex(p_tableWin->currentIndex() + 1);
    while(p_tableWin->currentRow() >= p_tableWin->table()->rowCount()) {
      int row = p_tableWin->table()->rowCount();
      p_tableWin->table()->insertRow(row);
      for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        p_tableWin->table()->setItem(row, c, item);
      }
    }

    QApplication::sendPostedEvents(p_tableWin->table(), 0);
    p_tableWin->table()->scrollToItem(p_tableWin->table()->item(p_tableWin->currentRow(), 0), QAbstractItemView::PositionAtBottom);
  }


  /**
   * Mouse leave event.
   *
   */
  void MeasureTool::mouseLeave() {
    //p_tableWin->clearRow(p_tableWin->currentRow());
  }


  /**
   * Enables/resets the rubberband tool.
   *
   */
  void MeasureTool::enableRubberBandTool() {
    if(p_rubberBand) {
      p_rubberBand->reset();
    }
  }


  /**
   * This method updates the row in the table window with the
   * current measure information.
   *
   *
   * @param row
   */
  void MeasureTool::updateRow(int row) {
    if(row + 1 > p_tableWin->table()->rowCount()) {
      p_tableWin->table()->insertRow(row);
      for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        p_tableWin->table()->setItem(row, c, item);
        if(c == 0) p_tableWin->table()->scrollToItem(item);
      }
    }

    // Blank out the row to remove stuff left over from previous cvps
    for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
      p_tableWin->table()->item(row, c)->setText("");
    }

    // Write all the new info to the current row
    if(p_startLat != Isis::Null && p_startLon != Isis::Null) {
      p_tableWin->table()->item(row, StartLatIndex)->setText(QString::number(p_startLat));
      p_tableWin->table()->item(row, StartLonIndex)->setText(QString::number(p_startLon));
    }
    else {
      p_tableWin->table()->item(row, StartLatIndex)->setText("N/A");
      p_tableWin->table()->item(row, StartLonIndex)->setText("N/A");
    }

    if(p_endLat != Isis::Null && p_endLon != Isis::Null) {
      p_tableWin->table()->item(row, EndLatIndex)->setText(QString::number(p_endLat));
      p_tableWin->table()->item(row, EndLonIndex)->setText(QString::number(p_endLon));
      p_tableWin->table()->item(row, DistanceKmIndex)->setText(QString::number(p_kmDist));
      p_tableWin->table()->item(row, DistanceMIndex)->setText(QString::number(p_mDist));
    }
    else {
      p_tableWin->table()->item(row, EndLatIndex)->setText("N/A");
      p_tableWin->table()->item(row, EndLonIndex)->setText("N/A");
      p_tableWin->table()->item(row, DistanceKmIndex)->setText("N/A");
      p_tableWin->table()->item(row, DistanceMIndex)->setText("N/A");
    }

    if(p_degAngle != Isis::Null && p_radAngle != Isis::Null) {
      p_tableWin->table()->item(row, AngleDegIndex)->setText(QString::number(p_degAngle));
      p_tableWin->table()->item(row, AngleRadIndex)->setText(QString::number(p_radAngle));
    }
    else {
      p_tableWin->table()->item(row, AngleDegIndex)->setText("N/A");
      p_tableWin->table()->item(row, AngleRadIndex)->setText("N/A");
    }

    if(p_startSamp != Isis::Null && p_startLine != Isis::Null) {
      p_tableWin->table()->item(row, StartSampIndex)->setText(QString::number(p_startSamp));
      p_tableWin->table()->item(row, StartLineIndex)->setText(QString::number(p_startLine));
    }
    else {
      p_tableWin->table()->item(row, StartSampIndex)->setText("N/A");
      p_tableWin->table()->item(row, StartLineIndex)->setText("N/A");
    }

    if(p_endSamp != Isis::Null && p_endLine != Isis::Null) {
      p_tableWin->table()->item(row, EndSampIndex)->setText(QString::number(p_endSamp));
      p_tableWin->table()->item(row, EndLineIndex)->setText(QString::number(p_endLine));
      p_tableWin->table()->item(row, DistancePixIndex)->setText(QString::number(p_pixDist));
    }
    else {
      p_tableWin->table()->item(row, EndSampIndex)->setText("N/A");
      p_tableWin->table()->item(row, EndLineIndex)->setText("N/A");
      p_tableWin->table()->item(row, DistancePixIndex)->setText("N/A");
    }

    if(p_pixArea != Isis::Null) {
      p_tableWin->table()->item(row, AreaPixIndex)->setText(QString::number(p_pixArea));
    }
    else {
      p_tableWin->table()->item(row, AreaPixIndex)->setText("N/A");
    }

    if(p_mArea != Isis::Null) {
      p_tableWin->table()->item(row, AreaKmIndex)->setText(QString::number(p_kmArea));
      p_tableWin->table()->item(row, AreaMIndex)->setText(QString::number(p_mArea));
    }
    else {
      p_tableWin->table()->item(row, AreaKmIndex)->setText("N/A");
      p_tableWin->table()->item(row, AreaMIndex)->setText("N/A");
    }

    p_tableWin->table()->item(row, PathIndex)->setText(p_path.c_str());
    p_tableWin->table()->item(row, FilenameIndex)->setText(p_fname.c_str());

  }



  /**
   * This method is called instead of updateRows if the
   * 'Show All Segment' checkbox is checked.
   *
   *
   * @param row
   */
  void MeasureTool::updateRows(int row) {
    if(p_distanceSegments.size() < 2) {
      updateRow(row);
      return;
    }

    int requiredRows = p_distanceSegments.size() + row + 2;
    int rowDiff = (int)(requiredRows - p_tableWin->table()->rowCount());

    //Make sure we have all the necessary rows and items in each table cell.
    if(requiredRows > p_tableWin->table()->rowCount()) {
      for(int r = 0; r < rowDiff; r++) {
        p_tableWin->table()->insertRow(row + r);
        for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
          QTableWidgetItem *item = new QTableWidgetItem("");
          p_tableWin->table()->setItem(row + r, c, item);
          if(c == 0) p_tableWin->table()->scrollToItem(item);
        }
      }
    }

    if(RubberBandTool::getMode() == RubberBandTool::SegmentedLine && p_distanceSegments.size() > 0) {
      double distanceSum = 0;
      for(int i = 0; i < p_distanceSegments.size(); i++) {
        //write a new row for each segment...
        p_tableWin->table()->item(row + i, StartLatIndex)->setText(QString::number(p_startLatSegments[i]));
        p_tableWin->table()->item(row + i, StartLonIndex)->setText(QString::number(p_startLonSegments[i]));

        p_tableWin->table()->item(row + i, EndLatIndex)->setText(QString::number(p_endLatSegments[i]));
        p_tableWin->table()->item(row + i, EndLonIndex)->setText(QString::number(p_endLonSegments[i]));

        p_tableWin->table()->item(row + i, StartSampIndex)->setText(QString::number(p_startSampSegments[i]));
        p_tableWin->table()->item(row + i, StartLineIndex)->setText(QString::number(p_startLineSegments[i]));

        p_tableWin->table()->item(row + i, EndSampIndex)->setText(QString::number(p_endSampSegments[i]));
        p_tableWin->table()->item(row + i, EndLineIndex)->setText(QString::number(p_endLineSegments[i]));

        p_tableWin->table()->item(row + i, DistancePixIndex)->setText(QString::number(p_pixDistSegments[i]));

        p_tableWin->table()->item(row + i, DistanceKmIndex)->setText(QString::number(p_distanceSegments[i]));
        p_tableWin->table()->item(row + i, DistanceMIndex)->setText(QString::number(p_distanceSegments[i] * 1000));

        p_tableWin->table()->item(row + i, PathIndex)->setText(p_path.c_str());
        p_tableWin->table()->item(row + i, FilenameIndex)->setText(p_fname.c_str());

        distanceSum += p_distanceSegments[i];

        p_tableWin->table()->item(row + i, SegmentsSumIndex)->setText(QString::number(distanceSum));
        p_tableWin->table()->item(row + i, SegmentNumberIndex)->setText(QString::number(i + 1));
      }

      //update the current row
      p_tableWin->setCurrentRow(row + p_distanceSegments.size() - 1);
      p_distanceSegments.clear();
      p_pixDistSegments.clear();
      p_startSampSegments.clear();
      p_endSampSegments.clear();
      p_startLineSegments.clear();
      p_endLineSegments.clear();
      p_startLatSegments.clear();
      p_endLatSegments.clear();
      p_startLonSegments.clear();
      p_endLonSegments.clear();

    }
  }

  /**
   * Initialize Class data
   *
   * @author sprasad (10/23/2009)
   */
  void MeasureTool::initData(void) {
    // Initialize the class data
    p_startSamp = Isis::Null;
    p_endSamp   = Isis::Null;
    p_startLine = Isis::Null;
    p_endLine   = Isis::Null;
    p_kmDist    = Isis::Null;
    p_mDist     = Isis::Null;
    p_pixDist   = Isis::Null;
    p_startLon  = Isis::Null;
    p_startLat  = Isis::Null;
    p_endLon    = Isis::Null;
    p_endLat    = Isis::Null;
    p_radAngle  = Isis::Null;
    p_degAngle  = Isis::Null;
    p_pixArea   = Isis::Null;
    p_kmArea    = Isis::Null;
    p_mArea     = Isis::Null;
  }

  /**
   * This method updates the distance variables
   *
   *
   * @param cvp - Pointer to CubeViewPort
   * @param row - row index
   *
   * @returns void
   */
  void MeasureTool::updateDist(MdiCubeViewport *cvp, int row) {
    // Initialize class data
    initData();

    // Write out col 8 (the file name)
    Isis::Filename fname = Isis::Filename(cvp->cube()->Filename()).Expanded();
    p_path  = fname.Path();
    p_fname = fname.Name();

    // reset the distnace gui
    p_distLineEdit->setText("");

    if(RubberBandTool::getMode() == RubberBandTool::Line ||
        RubberBandTool::getMode() == RubberBandTool::SegmentedLine) {

      p_pixDist = 0;
      p_mDist   = 0;
      p_kmDist  = 0;

      // Flag to indicate whether distance was calculated
      bool bDistance = false;
      double radius = 0;
      double mDist  = 0;

      for(int startIndex = 0; startIndex < RubberBandTool::getVertices().size() - 1; startIndex++) {
        QPoint start = RubberBandTool::getVertices()[startIndex];
        QPoint end   = RubberBandTool::getVertices()[startIndex+1];

        //  Convert rubber band line to cube coordinates
        cvp->viewportToCube(start.x(), start.y(), p_startSamp, p_startLine);
        cvp->viewportToCube(end.x(), end.y(), p_endSamp, p_endLine);

        // Don't write anything if we are outside the cube
        if((p_startSamp < 0.5) || (p_endSamp < 0.5) ||
            (p_startLine < 0.5) || (p_endLine < 0.5) ||
            (p_startSamp > cvp->cubeSamples() + 0.5) ||
            (p_endSamp > cvp->cubeSamples() + 0.5) ||
            (p_startLine > cvp->cubeLines() + 0.5) ||
            (p_endLine > cvp->cubeLines() + 0.5)) {
          p_mDist   = Isis::Null;
          p_kmDist  = Isis::Null;
          p_pixDist = 0;
          return;
        }

        // Check if the image is projected (Projected Images also have camera except for mosaics)
        if(cvp->projection() != NULL) {
          if(cvp->projection()->SetWorld(p_startSamp, p_startLine)) {
            // If our projection is sky, the lat & lons are switched
            if(cvp->projection()->IsSky()) {
              p_startLat = cvp->projection()->UniversalLongitude();
              p_startLon = cvp->projection()->UniversalLatitude();
            }
            else {
              p_startLat = cvp->projection()->UniversalLatitude();
              p_startLon = cvp->projection()->UniversalLongitude();
            }

            if(cvp->projection()->SetWorld(p_endSamp, p_endLine)) {
              // If our projection is sky, the lat & lons are switched
              if(cvp->projection()->IsSky()) {
                p_endLat = cvp->projection()->UniversalLongitude();
                p_endLon = cvp->projection()->UniversalLatitude();
              }
              else {
                p_endLat = cvp->projection()->UniversalLatitude();
                p_endLon = cvp->projection()->UniversalLongitude();
              }
            }
            // Calculate and write out the distance between the two points
            radius = cvp->projection()->LocalRadius();

            // distance is calculated
            bDistance = true;
          }
        }
        // Do we have a camera model?
        else if(cvp->camera() != NULL) {
          if(cvp->camera()->SetImage(p_startSamp, p_startLine)) {

            // Write columns 2-3 (Start lat/lon)
            p_startLat = cvp->camera()->UniversalLatitude();
            p_startLon = cvp->camera()->UniversalLongitude();

            if(cvp->camera()->SetImage(p_endSamp, p_endLine)) {
              // Write columns 4-5 (End lat/lon)
              p_endLat = cvp->camera()->UniversalLatitude();
              p_endLon = cvp->camera()->UniversalLongitude();

              radius = cvp->camera()->LocalRadius();

              // distance is calculated
              bDistance = true;
            }
          }
        }

        // Calculate the pixel difference
        double lineDif = p_startLine - p_endLine;
        double sampDif = p_startSamp - p_endSamp;
        double pixDist = sqrt(lineDif * lineDif + sampDif * sampDif);
        p_pixDist =  pixDist;

        if(bDistance) {
          mDist  = Isis::Camera::Distance(p_startLat, p_startLon, p_endLat, p_endLon, radius);
          p_mDist  += mDist;
          p_kmDist  = p_mDist / 1000.0;

          if(RubberBandTool::getMode() == RubberBandTool::SegmentedLine) {
            if(pixDist > 16 / cvp->scale() && p_distanceSegments.size() < 75) {
              p_distanceSegments.append(mDist / 1000.0);
              p_pixDistSegments.append(pixDist);
              p_startSampSegments.append(p_startSamp);
              p_endSampSegments.append(p_endSamp);
              p_startLineSegments.append(p_startLine);
              p_endLineSegments.append(p_endLine);
              p_startLatSegments.append(p_startLat);
              p_endLatSegments.append(p_endLat);
              p_startLonSegments.append(p_startLon);
              p_endLonSegments.append(p_endLon);
            }
          }
        }
        bDistance = false;
      }
      // Distance was not calculated
      if(!p_mDist) {
        p_mDist   = Isis::Null;
        p_kmDist  = Isis::Null;
      }
    }
    else if(RubberBandTool::getMode() == RubberBandTool::Angle) {
      p_radAngle = RubberBandTool::getAngle();
      p_degAngle = p_radAngle * 180.0 / Isis::PI;
    }
    else {
      geos::geom::Geometry *polygon = RubberBandTool::geometry();
      if(polygon != NULL) {
        // pix area = screenpix^2 / scale^2
        p_pixArea = polygon->getArea() / pow(cvp->scale(), 2);
        geos::geom::Point *center = polygon->getCentroid();
        double line, sample;
        cvp->viewportToCube((int)center->getX(), (int)center->getY(), sample, line);

        if(cvp->camera() != NULL) {
          cvp->camera()->SetImage(sample, line);
          // pix^2 * (m/pix)^2 = m^2
          p_mArea = p_pixArea * pow(cvp->camera()->PixelResolution(), 2);
          // m^2 * (km/m)^2 = km^2
          p_kmArea = p_mArea * pow(1 / 1000.0, 2);
        }

        if(cvp->projection() != NULL) {
          cvp->projection()->SetWorld(sample, line);
          // pix^2 * (m/pix)^2 = m^2
          p_mArea = p_pixArea * pow(cvp->projection()->Resolution(), 2);
          // m^2 * (km/m)^2 = km^2
          p_kmArea = p_mArea * pow(1 / 1000.0, 2);
        }
      }
    }

    updateDistEdit();

    if(p_showAllSegments->isChecked()) {
      updateRows(row);
    }
    else {
      updateRow(row);
    }
  }


  //! Change the value in the distance edit to match the units
  void MeasureTool::updateDistEdit() {
    if(RubberBandTool::getMode() == RubberBandTool::Line ||
        RubberBandTool::getMode() == RubberBandTool::SegmentedLine) {
      if(p_unitsComboBox->currentIndex() == 0) {
        if(p_kmDist == Isis::Null) {
          p_distLineEdit->setText("N/A");
        }
        else {
          p_distLineEdit->setText(QString::number(p_kmDist));
        }
      }
      else if(p_unitsComboBox->currentIndex() == 1) {
        if(p_mDist == Isis::Null) {
          p_distLineEdit->setText("N/A");
        }
        else {
          p_distLineEdit->setText(QString::number(p_mDist));
        }
      }
      else {
        p_distLineEdit->setText(QString::number(p_pixDist));
      }
    }
    else if(RubberBandTool::getMode() == RubberBandTool::Angle) {
      if(p_unitsComboBox->currentIndex() == 0) {
        p_distLineEdit->setText(QString::number(p_degAngle));
      }
      else {
        p_distLineEdit->setText(QString::number(p_radAngle));
      }
    }
    else {
      if(p_unitsComboBox->currentIndex() == 0) {
        if(p_kmArea == Isis::Null) {
          p_distLineEdit->setText("N/A");
        }
        else {
          p_distLineEdit->setText(QString::number(p_kmArea));
        }
      }
      else if(p_unitsComboBox->currentIndex() == 1) {
        if(p_mArea == Isis::Null) {
          p_distLineEdit->setText("N/A");
        }
        else {
          p_distLineEdit->setText(QString::number(p_mArea));
        }
      }
      else {
        if(p_pixArea != Isis::Null) {
          p_distLineEdit->setText(QString::number(p_pixArea));
        }
        else {
          p_distLineEdit->setText("N/A");
        }
      }
    }
  }


  /**
   * Removes the connection on the given cube viewport.
   *
   *
   * @param cvp
   */
  void MeasureTool::removeConnections(MdiCubeViewport *cvp) {
    //  cvp->repaint();
    cvp->update();
  }


  /**
   * Updates the measure tool.
   *
   */
  void MeasureTool::updateTool() {
    p_distLineEdit->clear();
  }

}

