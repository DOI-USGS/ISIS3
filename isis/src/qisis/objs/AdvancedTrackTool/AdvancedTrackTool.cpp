#include "AdvancedTrackTool.h"

#include <QApplication>
#include <QMenu>

#include "Camera.h"
#include "Distance.h"
#include "iTime.h"
#include "MdiCubeViewport.h"
#include "Projection.h"
#include "SerialNumber.h"
#include "SpecialPixel.h"
#include "TableMainWindow.h"

using namespace Isis;

namespace Qisis {

  // For mosaic tracking
#define FLOAT_MIN         -16777215
#define TABLE_MOSAIC_SRC  "InputImages"

  /**
   * Constructs an AdvancedTrackTool object
   *
   * @param parent
   */
  AdvancedTrackTool::AdvancedTrackTool(QWidget *parent) : Tool(parent) {
    p_tableWin = new Qisis::TableMainWindow("Advanced Tracking", parent);
    p_tableWin->setTrackListItems(true);
    connect(p_tableWin, SIGNAL(fileLoaded()), this, SLOT(updateID()));

    p_action = new QAction(parent);
    p_action->setText("Tracking ...");
    p_action->setIcon(QPixmap(toolIconDir() + "/goto.png"));
    p_action->setShortcut(Qt::CTRL + Qt::Key_T);
    p_action->setWhatsThis("<b>Function: </b> Opens the Advanced Tracking Tool \
                           window. This window will track sample/line positions,\
                           lat/lon positions, and many other pieces of \
                           information.  All of the data in the window can be \
                           saved to a text file. <p><b>Shortcut: </b> Ctrl+T</p>");
    connect(p_action, SIGNAL(triggered()), p_tableWin, SLOT(showTable()));
    activate(true);
    connect(p_action, SIGNAL(triggered()), p_tableWin, SLOT(raise()));
    connect(p_action, SIGNAL(triggered()), p_tableWin, SLOT(syncColumns()));
    p_tableWin->installEventFilter(this);

    p_tableWin->addToTable(false, "Id", "Id");
    p_tableWin->addToTable(true, "Sample:Line", "Sample:Line", -1, Qt::Horizontal, "Sample and Line");
    p_tableWin->addToTable(false, "Band", "Band");
    p_tableWin->addToTable(true, "Pixel", "Pixel");
    p_tableWin->addToTable(true, "Planetocentric Latitude", "Planetocentric Lat");
    p_tableWin->addToTable(false, "Planetographic Latitude", "Planetographic Lat");
    p_tableWin->addToTable(true, "360 Positive East Longitude", "360 East Longitude");
    p_tableWin->addToTable(false, "360 Positive West Longitude", "360 West Longitude");
    p_tableWin->addToTable(true, "180 Positive East Longitude", "180 East Longitude");
    p_tableWin->addToTable(false, "180 Positive West Longitude", "180 West Longitude");
    p_tableWin->addToTable(false, "Projected X:Projected Y", "Projected X:Projected Y", -1, Qt::Horizontal, "X and Y values for a projected image");
    p_tableWin->addToTable(false, "Local Radius", "Radius");
    p_tableWin->addToTable(false, "Point X:Point Y:Point Z", "XYZ", -1, Qt::Horizontal, "The X, Y, and Z of surface intersection in body-fixed coordinates");
    p_tableWin->addToTable(false, "Right Ascension:Declination", "Ra:Dec", -1, Qt::Horizontal, "Right Ascension and Declination");
    p_tableWin->addToTable(false, "Resolution", "Resolution");
    p_tableWin->addToTable(false, "Phase", "Phase");
    p_tableWin->addToTable(false, "Incidence", "Incidence");
    p_tableWin->addToTable(false, "Emission", "Emission");
    p_tableWin->addToTable(false, "North Azimuth", "North Azimuth");
    p_tableWin->addToTable(false, "Sun Azimuth", "Sun Azimuth");
    p_tableWin->addToTable(false, "Solar Longitude", "Solar Longitude");
    p_tableWin->addToTable(false, "Spacecraft X:Spacecraft Y:Spacecraft Z", "Spacecraft Position", -1, Qt::Horizontal, "The X, Y, and Z of the spacecraft position");
    p_tableWin->addToTable(false, "Spacecraft Azimuth", "Spacecraft Azimuth");
    p_tableWin->addToTable(false, "Slant Distance", "Slant Distance");
    p_tableWin->addToTable(false, "Ephemeris Time", "Ephemeris iTime");
    p_tableWin->addToTable(false, "Local Solar Time", "Local Solar iTime");
    p_tableWin->addToTable(false, "UTC", "UTC", -1, Qt::Horizontal, "Internal time in UTC format");
    p_tableWin->addToTable(false, "Path", "Path");
    p_tableWin->addToTable(false, "Filename", "Filename");
    p_tableWin->addToTable(false, "Serial Number", "Serial Number");
    p_tableWin->addToTable(false, "Track Mosaic Index", "Track Mosaic Index");
    p_tableWin->addToTable(false, "Track Mosaic FileName", "Track Mosaic FileName");
    p_tableWin->addToTable(false, "Track Mosaic Serial Number", "Track Mosaic Serial Number");
    p_tableWin->addToTable(false, "Notes", "Notes");
    //This variable will keep track of how many times
    // the user has issued the 'record' command.
    p_id = 0;

    // Setup 10 blank rows in the table
    for(int r = 0; r < 10; r++) {
      p_tableWin->table()->insertRow(r);
      for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        p_tableWin->table()->setItem(r, c, item);
      }
    }

    // Create the action for recording points
    QAction *recordAction = new QAction(parent);
    recordAction->setShortcut(Qt::Key_R);
    parent->addAction(recordAction);
    connect(recordAction, SIGNAL(activated()), this, SLOT(record()));
    p_tableWin->setStatusMessage("To record press the R key");

  }

  /**
   * An event filter that calls methods on certain events.
   *
   * @param o
   * @param e
   *
   * @return bool
   */
  bool AdvancedTrackTool::eventFilter(QObject *o, QEvent *e) {
    if(e->type() == QEvent::Show) {
      activate(true);
    }
    else if(e->type() == QEvent::Hide) {
      activate(false);
    }
    return Tool::eventFilter(o, e);
  }


  /**
   * This method adds the action to bring up the track tool to the menu.
   *
   * @param menu
   */
  void AdvancedTrackTool::addTo(QMenu *menu) {
    menu->addAction(p_action);
  }

  /**
   * This method adds the action to bring up the track tool to the permanent tool
   * bar.
   *
   * @param perm
   */
  void AdvancedTrackTool::addToPermanent(QToolBar *perm) {
    perm->addAction(p_action);
  }

  /**
   * This method is called when the mouse has moved across the viewport and
   * updates the row accordingly.
   *
   * @param p
   */
  void AdvancedTrackTool::mouseMove(QPoint p) {
    updateRow(p);
  }

  /**
   * This method is called when the mouse leaved the viewport and clears any rows
   * accordingly.
   *
   */
  void AdvancedTrackTool::mouseLeave() {

    if(cubeViewport()->isLinked()) {
      for(int i = 0; i < p_numRows; i++) {
        p_tableWin->clearRow(i + p_tableWin->currentRow());
      }
    }
    else {
      p_tableWin->clearRow(p_tableWin->currentRow());
    }

  }

  /**
   * This method updates the row with data from the point given.
   *
   * @param p
   */
  void AdvancedTrackTool::updateRow(QPoint p) {
    MdiCubeViewport *cvp = cubeViewport();
    if(cvp == NULL) {
      p_tableWin->clearRow(p_tableWin->currentRow());
      return;
    }

    if(!cubeViewport()->isLinked()) {
      updateRow(cvp, p, p_tableWin->currentRow());
      p_numRows = 1;
    }
    else {
      p_numRows = 0;
      for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
        MdiCubeViewport *d = (*(cubeViewportList()))[i];
        if(d->isLinked()) {
          updateRow(d, p, p_tableWin->currentRow() + p_numRows);
          p_numRows++;
        }
      }
    }
  }

  /**
   * This method updates the row given with data from the viewport cvp at point p.
   *
   * @param cvp CubeViewPort that contains p
   * @param p   QPoint from which the row will be updated
   * @param row Row to be updated
   */
  void AdvancedTrackTool::updateRow(MdiCubeViewport *cvp, QPoint p, int row) {
    // Get the sample line position to report
    double sample, line;
    cvp->viewportToCube(p.x(), p.y(), sample, line);
    int isample = int (sample + 0.5);
    int iline = int (line + 0.5);

    /*if there are linked cvp's then we want to highlight (select)
    the row of the active cvp.*/
    if(cvp->isLinked()) {

      if(cvp == cubeViewport()) {
        p_tableWin->table()->selectRow(row);
      }

    }


    // Do we need more rows?
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

    // Don't write anything if we are outside the cube
    if(sample < 0.5) return;
    if(line < 0.5) return;
    if(sample > cvp->cubeSamples() + 0.5) return;
    if(line > cvp->cubeLines() + 0.5) return;

    // Write cols 0-2 (id, sample, line)
    p_tableWin->table()->item(row, ID)->setText(QString::number(p_id));
    p_tableWin->table()->item(row, SAMPLE)->setText(QString::number(sample));
    p_tableWin->table()->item(row, LINE)->setText(QString::number(line));

    // Write col 3 (band)
    if(cvp->isGray()) {
      p_tableWin->table()->item(row, BAND)->setText(QString::number(cvp->grayBand()));
    }
    else {
      p_tableWin->table()->item(row, BAND)->setText(QString::number(cvp->redBand()));
    }

    // Write out the path, filename, and serial number
    Isis::Filename fname = Isis::Filename(cvp->cube()->Filename()).Expanded();
    std::string fnamePath = fname.Path();
    std::string fnameName = fname.Name();
    p_tableWin->table()->item(row, PATH)->setText(fnamePath.c_str());
    p_tableWin->table()->item(row, FILENAME)->setText(fnameName.c_str());
    //p_tableWin->table()->item(row,34)->setText(Isis::SerialNumber::Compose(*cvp->cube()).c_str());

    // If we are outside of the image then we are done
    if((sample < 0.5) || (line < 0.5) ||
        (sample > cvp->cubeSamples() + 0.5) ||
        (line > cvp->cubeLines() + 0.5)) {
      return;
    }

    // Otherwise write out col 4 (Pixel value)
    if(cvp->isGray()) {
      std::string grayPixel = Isis::PixelToString(cvp->grayPixel(isample, iline));
      QString p = grayPixel.c_str();
      p_tableWin->table()->item(row, PIXEL)->setText(p);
    }
    else {
      std::string redPixel = Isis::PixelToString(cvp->redPixel(isample, iline));
      QString p = redPixel.c_str();
      p_tableWin->table()->item(row, PIXEL)->setText(p);
    }

    // Do we have a camera model?
    if(cvp->camera() != NULL) {
      if(cvp->camera()->SetImage(sample, line)) {
        // Write columns 5,7,9 (ocentric lat/lon, and radius)
        double lat = cvp->camera()->UniversalLatitude();
        double lon = cvp->camera()->UniversalLongitude();

        double radius = cvp->camera()->LocalRadius();
        p_tableWin->table()->item(row, PLANETOCENTRIC_LAT)->setText(QString::number(lat, 'f', 15));
        p_tableWin->table()->item(row, EAST_LON_360)->setText(QString::number(lon, 'f', 15));
        p_tableWin->table()->item(row, RADIUS)->setText(QString::number(radius, 'f', 15));

        /* 180 Positive East Lon. */
        p_tableWin->table()->item(row, EAST_LON_180)->setText(QString::number(Isis::Projection::To180Domain(lon), 'f', 15));

        // Write out the planetographic and positive west values
        lon = -lon;
        while(lon < 0.0) lon += 360.0;
        double radii[3];
        cvp->camera()->Radii(radii);
        lat = Isis::Projection::ToPlanetographic(lat, radii[0], radii[2]);
        p_tableWin->table()->item(row, PLANETOGRAPHIC_LAT)->setText(QString::number(lat, 'f', 15));
        p_tableWin->table()->item(row, WEST_LON_360)->setText(QString::number(lon, 'f', 15));

        /*180 Positive West Lon.  */
        p_tableWin->table()->item(row, WEST_LON_180)->setText(QString::number(Isis::Projection::To180Domain(lon), 'f', 15));

        // Next write out columns 10-12 (the x/y/z position of the lat/lon)
        double pos[3];
        cvp->camera()->Coordinate(pos);
        p_tableWin->table()->item(row, POINT_X)->setText(QString::number(pos[0]));
        p_tableWin->table()->item(row, POINT_Y)->setText(QString::number(pos[1]));
        p_tableWin->table()->item(row, POINT_Z)->setText(QString::number(pos[2]));

        // Write out columns 15 (resolution)
        double res = cvp->camera()->PixelResolution();
        p_tableWin->table()->item(row, RESOLUTION)->setText(QString::number(res));

        // Write out columns 16-18 (phase, incidence, emission)
        double phase = cvp->camera()->PhaseAngle();
        double incidence = cvp->camera()->IncidenceAngle();
        double emission = cvp->camera()->EmissionAngle();
        p_tableWin->table()->item(row, PHASE)->setText(QString::number(phase));
        p_tableWin->table()->item(row, INCIDENCE)->setText(QString::number(incidence));
        p_tableWin->table()->item(row, EMISSION)->setText(QString::number(emission));

        // Write out columns 19-21 (north azimuth, sun azimuth, solar longitude)
        double northAzi = cvp->camera()->NorthAzimuth();
        double sunAzi   = cvp->camera()->SunAzimuth();
        double solarLon = cvp->camera()->SolarLongitude();
        p_tableWin->table()->item(row, NORTH_AZIMUTH)->setText(QString::number(northAzi));
        p_tableWin->table()->item(row, SUN_AZIMUTH)->setText(QString::number(sunAzi));
        p_tableWin->table()->item(row, SOLAR_LON)->setText(QString::number(solarLon));

        // Write out columns 25-29 (spacecraft azimuth, slant distance,
        // et, local solar time, and UTC)
        double spacecraftAzi = cvp->camera()->SpacecraftAzimuth();
        double slantDistance = cvp->camera()->SlantDistance();
        double lst = cvp->camera()->LocalSolarTime();
        p_tableWin->table()->item(row, SPACECRAFT_AZIMUTH)->setText(QString::number(spacecraftAzi));
        p_tableWin->table()->item(row, SLANT)->setText(QString::number(slantDistance));
        p_tableWin->table()->item(row, SOLAR_TIME)->setText(QString::number(lst));
      }

      // Always write out columns 13-14 (ra/dec);
      double ra = cvp->camera()->RightAscension();
      double dec = cvp->camera()->Declination();
      p_tableWin->table()->item(row, RIGHT_ASCENSION)->setText(QString::number(ra));
      p_tableWin->table()->item(row, DECLINATION)->setText(QString::number(dec));

      // Always write out columns 27/29 et and utc
      double et = cvp->camera()->EphemerisTime();
      Isis::iTime time(et);
      p_tableWin->table()->item(row, EPHEMERIS_TIME)->setText(QString::number(et, 'f', 15));
      std::string time_utc = time.UTC();
      p_tableWin->table()->item(row, UTC)->setText(time_utc.c_str());

      // Always out columns 22-24 (spacecraft position)
      double pos[3];
      cvp->camera()->InstrumentPosition(pos);
      p_tableWin->table()->item(row, SPACECRAFT_X)->setText(QString::number(pos[0]));
      p_tableWin->table()->item(row, SPACECRAFT_Y)->setText(QString::number(pos[1]));
      p_tableWin->table()->item(row, SPACECRAFT_Z)->setText(QString::number(pos[2]));
    }

    else if(cvp->projection() != NULL) {
      if(cvp->projection()->SetWorld(sample, line)) {
        double lat = cvp->projection()->UniversalLatitude();
        double lon = cvp->projection()->UniversalLongitude();

        double glat = cvp->projection()->ToPlanetographic(lat);
        double wlon = -lon;
        while(wlon < 0.0) wlon += 360.0;
        if(cvp->projection()->IsSky()) {
          lon = cvp->projection()->Longitude();
          p_tableWin->table()->item(row, RIGHT_ASCENSION)->setText(QString::number(lon, 'f', 15));
          p_tableWin->table()->item(row, DECLINATION)->setText(QString::number(lat, 'f', 15));
        }
        else {
          double radius = cvp->projection()->LocalRadius();
          p_tableWin->table()->item(row, PLANETOCENTRIC_LAT)->setText(QString::number(lat, 'f', 15));
          p_tableWin->table()->item(row, PLANETOGRAPHIC_LAT)->setText(QString::number(glat, 'f', 15));
          p_tableWin->table()->item(row, EAST_LON_360)->setText(QString::number(lon, 'f', 15));
          p_tableWin->table()->item(row, EAST_LON_180)->setText(QString::number(Isis::Projection::To180Domain(lon), 'f', 15));
          p_tableWin->table()->item(row, WEST_LON_360)->setText(QString::number(wlon, 'f', 15));
          p_tableWin->table()->item(row, WEST_LON_180)->setText(QString::number(Isis::Projection::To180Domain(wlon), 'f', 15));
          p_tableWin->table()->item(row, RADIUS)->setText(QString::number(radius, 'f', 15));
        }
      }
    }

    //If there is a projection add the Projected X and Y coords to the table
    if(cvp->projection() != NULL) {
      if(cvp->projection()->SetWorld(sample, line)) {
        double projX = cvp->projection()->XCoord();
        double projY = cvp->projection()->YCoord();
        p_tableWin->table()->item(row, PROJECTED_X)->setText(QString::number(projX, 'f', 15));
        p_tableWin->table()->item(row, PROJECTED_Y)->setText(QString::number(projY, 'f', 15));
      }
    }

    // Track the Mosaic Origin -  Index (Zero based) and FileName
    int iMosaicOrigin = -1;
    std::string sSrcFileName = "";
    std::string sSrcSerialNum = "";
    TrackMosaicOrigin(cvp, iline, isample, iMosaicOrigin, sSrcFileName, sSrcSerialNum);
    p_tableWin->table()->item(row, TRACK_MOSAIC_INDEX)->setText(QString::number(iMosaicOrigin));
    p_tableWin->table()->item(row, TRACK_MOSAIC_FILENAME)->setText(QString(sSrcFileName.c_str()));
    p_tableWin->table()->item(row, TRACK_MOSAIC_SERIAL_NUM)->setText(QString(sSrcSerialNum.c_str()));
  }


  /**
   * TrackMosaicOrigin - Given the pointer to Cube and line and
   * sample index, finds the origin of the mosaic if the TRACKING
   * band and Mosaic Origin Table  exists.
   *
   * @author sprasad (11/16/2009)
   *
   * @param cvp           - Points to the CubeViewPort
   * @param piLine        - Line Index
   * @param piSample      - Sample Index
   * @param piOrigin      - Contains the Src Index (zero based)
   * @param psSrcFileName - Contains the Src Filename
   * @param psSrcSerialNum- Contains the Src Serial Number
   *
   * @return void
   */
  void AdvancedTrackTool::TrackMosaicOrigin(MdiCubeViewport *cvp, int piLine,
      int piSample, int &piOrigin, std::string &psSrcFileName,
      std::string &psSrcSerialNum) {
    Cube *cCube = cvp->cube();
    int iTrackBand = -1;

    if(cCube->HasTable(TABLE_MOSAIC_SRC)) {
      Pvl *cPvl = cCube->Label();
      PvlObject cObjIsisCube = cPvl->FindObject("IsisCube");
      PvlGroup cGrpBandBin = cObjIsisCube.FindGroup("BandBin");
      for(int i = 0; i < cGrpBandBin.Keywords(); i++) {
        PvlKeyword &cKeyTrackBand = cGrpBandBin[i];
        for(int j = 0; j < cKeyTrackBand.Size(); j++) {
          if(cKeyTrackBand[j] == "TRACKING") {
            iTrackBand = j;
            break;
          }
        }
      }

      if(iTrackBand > 0 && iTrackBand <= cCube->Bands()) {
        Isis::Portal cOrgPortal(cCube->Samples(), 1, cCube->PixelType());
        cOrgPortal.SetPosition(piSample, piLine, iTrackBand + 1); // 1 based
        cCube->Read(cOrgPortal);

        piOrigin = (int)cOrgPortal[0];
        switch(SizeOf(cCube->PixelType())) {
          case 1:
            piOrigin -= VALID_MIN1;
            break;

          case 2:
            piOrigin -= VALID_MIN2;
            break;

          case 4:
            piOrigin -= FLOAT_MIN;
            break;
        }

        // Get the input file name and serial number
        Table cFileTable(TABLE_MOSAIC_SRC);
        cCube->Read(cFileTable);
        int iRecs =   cFileTable.Records();
        if(piOrigin >= 0 && piOrigin < iRecs) {
          psSrcFileName = std::string(cFileTable[piOrigin][0]);
          psSrcSerialNum = std::string(cFileTable[piOrigin][1]);
        }
      }
    }
  }

  /**
   * This method records data to the current row.
   *
   */
  void AdvancedTrackTool::record() {
    if(p_tableWin->table()->isHidden()) return;
    if(p_tableWin->table()->item(p_tableWin->currentRow(), 0)->text() == "") return;

    int row = 0;
    p_tableWin->setCurrentRow(p_tableWin->currentRow() + p_numRows);
    p_tableWin->setCurrentIndex(p_tableWin->currentIndex() + p_numRows);
    while(p_tableWin->currentRow() >= p_tableWin->table()->rowCount()) {
      row = p_tableWin->table()->rowCount();
      p_tableWin->table()->insertRow(row);
      for(int c = 0; c < p_tableWin->table()->columnCount(); c++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        p_tableWin->table()->setItem(row, c, item);
      }
    }

    QApplication::sendPostedEvents(p_tableWin->table(), 0);
    p_tableWin->table()->scrollToItem(p_tableWin->table()->item(p_tableWin->currentRow(), 0), QAbstractItemView::PositionAtBottom);

    //Keep track of number times user presses 'R' (record command)
    p_id = p_tableWin->table()->item(p_tableWin->currentRow() - 1, 0)->text().toInt() + 1;
  }


  /**
   * This slot updates the row with data from the point given and
   * records data to the current row.
   *
   * @param p   QPoint from which the row(s) will be updated and
   *            recorded.
   * @return void
   * @author Jeannie Walldren
   *
   * @internal
   *  @history 2010-03-08 - Jeannie Walldren - This slot was
   *           added to be connected to the FindTool recordPoint()
   *           signal in qview.
   *  @history 2010-05-07 - Eric Hyer - Now shows the table as well
   */
  void AdvancedTrackTool::record(QPoint p) {
    updateRow(p);
    record();
    p_tableWin->showTable();
  }

  /**
   * This method updates the record ID.
   *
   */
  void AdvancedTrackTool::updateID() {
    //Check if the current row is 0
    if(p_tableWin->currentRow() == 0)
      p_id = 0;
    else
      p_id = p_tableWin->table()->item(p_tableWin->currentRow() - 1, ID)->text().toInt() + 1;
  }
}
