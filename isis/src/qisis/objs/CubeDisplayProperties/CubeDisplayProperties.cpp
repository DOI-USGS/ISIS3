#include "CubeDisplayProperties.h"

#include <QAction>
#include <QBitArray>
#include <QBuffer>
#include <QColorDialog>
#include <QInputDialog>
#include <QMutexLocker>

#include "Cube.h"
#include "Filename.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "Table.h"
#include "UniversalGroundMap.h"

using geos::geom::MultiPolygon;

namespace Isis {
  /**
   * CubeDisplayProperties constructor. This sets default values and constructs
   *   the Cube *. You cannot have much more than 1K of these without calling
   *   closeCube().
   *
   *
   * @param cubeFilename The filename (fully expanded) of the cube file.
   * @param parent Qt parent object (this is destroyed when parent is destroyed)
   */
  CubeDisplayProperties::CubeDisplayProperties(QString filename,
      QMutex *cameraMutex, QObject *parent) : QObject(parent) {
    m_propertyUsed = new QBitArray;
    m_propertyValues = new QMap<int, QVariant>;

    m_filename = filename;

    // set all of the defaults to prevent unwanted change signals from
    //   being emitted later.
    setShowFill(true);
    setShowOutline(true);
    setShowDNs(false);
    setShowLabel(false);
    setSelected(false);

    setValue(Color, QVariant::fromValue(randomColor()));

    m_cube = NULL;
    m_gMap = NULL;
    m_footprint = NULL;

    cube();
    footprint(cameraMutex);

    m_incidenceAngle = Null;
    m_resolution = Null;
    m_emissionAngle = Null;

    try {
      Table table("CameraStatistics", m_filename.toStdString());
      //Table table("CameraStatistics", m_filename.Name());
      for (int i = 0; i < table.Records(); i++) {
        for (int j = 0; j < table[i].Fields(); j++) {
          QString label;

          if (table[i][j].IsText()) {
            label = QString::fromStdString((std::string)table[i][j]);
            label.truncate(10);
          }

          // Get the average resolution for this mosaic item.
          if (table[i][j].IsText() && label.compare("Resolution") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                m_resolution = (double)table[i][j+3];
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average emission angle for this mosaic item.
          if (table[i][j].IsText() && label.compare("EmissionAn") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                m_emissionAngle = (double)table[i][j+3];
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average incidence angle for this mosaic item.
          if (table[i][j].IsText() && label.compare("IncidenceA") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                m_incidenceAngle = (double)table[i][j+3];
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

        } // end for table[i].Fields
      } // end for table.Records
    }
    catch(IException &e) {
      IException error(
          e,
          IException::Io,
          "Please run camstats with the attach option. "
          "Camera statistics will be unavailable for [" +
              m_filename.toStdString() + "]", _FILEINFO_);

      error.print();
    }
  }


  /**
   * This deletes the Cube * and frees other allocated memory. The
   *   destroyed() signal will be called from here.
   */
  CubeDisplayProperties::~CubeDisplayProperties() {
    closeCube();

    if(m_gMap) {
      delete m_gMap;
      m_gMap = NULL;
    }
  }


  void CubeDisplayProperties::fromPvl(const PvlObject &pvl) {
    QByteArray hexValues(pvl["Values"][0].c_str());
    QDataStream valuesStream(QByteArray::fromHex(hexValues));
    valuesStream >> *m_propertyValues;
  }


  /**
   * Call this with every property you support, otherwise they will not
   *   communicate properly between widgets.
   *
   * @param prop The property you are adding support for
   */
  void CubeDisplayProperties::addSupport(Property prop) {
    if(m_propertyUsed->size() <= prop)
      m_propertyUsed->resize((int)(prop + 1));

    if(!m_propertyUsed->testBit(prop)) {
      m_propertyUsed->setBit(prop);
      emit supportAdded(prop);
    }
  }


  /**
   * Returns true if all of the given displays support the property
   *
   * @param prop The property we're testing for support for
   * @param displays The displays we're doing the test on
   */
  bool CubeDisplayProperties::allSupport(Property prop,
      QList<CubeDisplayProperties *> displays) {
    if(displays.empty())
      return false;

    CubeDisplayProperties *display;
    foreach(display, displays) {
      if(!display->supports(prop))
        return false;
    }

    return true;
  }


  /**
   * Support may come later, please make sure you are connected to the
   *   supportAdded signal.
   *
   * @returns True if the property has support, false otherwise
   */
  bool CubeDisplayProperties::supports(Property prop) {
    if(m_propertyUsed->size() <= prop)
      return false;

    return m_propertyUsed->testBit(prop);
  }


  /**
   * Get a property's associated data.
   *
   * @param prop The property
   */
  QVariant CubeDisplayProperties::getValue(Property prop) const {
    return (*m_propertyValues)[prop];
  }


  /**
   * Get the Cube * associated with this display property. This will allocate
   *   the Cube * if one is not already present.
   */
  Cube *CubeDisplayProperties::cube() {
    if(!m_cube) {
      m_cube = new Cube;
      m_cube->open(m_filename.toStdString());
    }

    return m_cube;
  }


  /**
   * Get the UniversalGroundMap * associated with this display property.
   *   This will allocate the pointer if one is not already present.
   */
  UniversalGroundMap *CubeDisplayProperties::groundMap() {
    if(m_gMap == NULL) {
      Pvl lab(m_filename.toStdString());
      m_gMap = new UniversalGroundMap(lab);
    }

    return m_gMap;
  }


  /**
   * Returns the display name
   */
  QString CubeDisplayProperties::displayName() const {
    return Filename(m_filename.toStdString()).fileName();
  }


  /**
   * Cleans up the Cube *. You want to call this once you're sure you are done
   *   with the Cube because the OS will limit how many of these we have open.
   */
  void CubeDisplayProperties::closeCube() {
    if(m_cube) {
      delete m_cube;
      m_cube = NULL;
    }
  }


  /**
   * Cleans up the Cube *. You want to call this once you're sure you are done
   *   with the Cube because the OS will limit how many of these we have open.
   */
  geos::geom::MultiPolygon *CubeDisplayProperties::footprint(QMutex *lock) {
    if(!m_footprint) {
      try {
        ImagePolygon poly;
        cube()->read(poly);
        m_footprint = PolygonTools::MakeMultiPolygon(poly.Polys()->clone());
      }
      catch (IException &e) {
        try {
          createManualFootprint(lock);
        }
        catch(IException &e) {
          iString msg = "Could not read the footprint from cube [" +
              displayName() + "]. Please make "
              "sure footprintinit has been run";
          throw IException(e, IException::Io, msg, _FILEINFO_);
        }
      }
    }

    return m_footprint;
  }


  /**
   * Convert to Pvl for project files. This stores all of the data associated
   *   with all of the properties (but not what is supported). This also stores
   *   the cube filename.
   */
  PvlObject CubeDisplayProperties::toPvl() const {
    PvlObject output("CubeProperties");
    output += PvlKeyword("Filename", m_filename);

    QBuffer dataBuffer;
    dataBuffer.open(QIODevice::ReadWrite);

    QDataStream propsStream(&dataBuffer);
    propsStream << *m_propertyValues;
    dataBuffer.seek(0);

    output += PvlKeyword("Values", QString(dataBuffer.data().toHex()));

    return output;
  }


  /**
   * Gets a list of pre-connected actions that have to do with display,
   *   such as color, alpha, outline, fill, etc.
   */
  QList<QAction *> CubeDisplayProperties::getSupportedDisplayActions(
      QList<CubeDisplayProperties *> cubeDisplays) {
    QList<QAction *> actions;

    if(allSupport(Color, cubeDisplays)) {

      QAction *alphaAction = new QAction("Change Transparency",
                                         cubeDisplays[0]);

      alphaAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(alphaAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(askAlpha()));

      actions.append(alphaAction);

      QAction *colorAction = new QAction("Change Color", cubeDisplays[0]);

      colorAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(colorAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(askNewColor()));

      actions.append(colorAction);

      QAction *ranColorAction = new QAction("Randomize Color", cubeDisplays[0]);

      ranColorAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(ranColorAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(showRandomColor()));

      actions.append(ranColorAction);
    }


    if(allSupport(ShowLabel, cubeDisplays)) {
      QAction *labelVisibleAction;
      if(!cubeDisplays[0]->getValue(ShowLabel).toBool())
        labelVisibleAction = new QAction("Show Label", cubeDisplays[0]);
      else
        labelVisibleAction = new QAction("Hide Label", cubeDisplays[0]);

      labelVisibleAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(labelVisibleAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(toggleShowLabel()));

      actions.append(labelVisibleAction);
    }


    if(allSupport(ShowFill, cubeDisplays)) {
      QAction *fillAction;
      if(!cubeDisplays[0]->getValue(ShowFill).toBool())
        fillAction = new QAction("Show Filled", cubeDisplays[0]);
      else
        fillAction = new QAction("Show Unfilled", cubeDisplays[0]);

      fillAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(fillAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(toggleShowFill()));
      actions.append(fillAction);
    }


    if(allSupport(ShowDNs, cubeDisplays)) {
      QAction *cubeDataAction;
      if(!cubeDisplays[0]->getValue(ShowDNs).toBool())
        cubeDataAction = new QAction("Show Cube Data", cubeDisplays[0]);
      else
        cubeDataAction = new QAction("Hide Cube Data", cubeDisplays[0]);

      cubeDataAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(cubeDataAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(toggleShowDNs()));
      actions.append(cubeDataAction);
    }


    if(allSupport(ShowOutline, cubeDisplays)) {
      QAction *outlineAction;
      if(!cubeDisplays[0]->getValue(ShowOutline).toBool())
        outlineAction = new QAction("Show Outline", cubeDisplays[0]);
      else
        outlineAction = new QAction("Hide Outline", cubeDisplays[0]);

      outlineAction->setData( QVariant::fromValue(cubeDisplays) );
      connect(outlineAction, SIGNAL(triggered()),
              cubeDisplays[0], SLOT(toggleShowOutline()));
      actions.append(outlineAction);
    }

    return actions;
  }


  /**
   * Gets a list of pre-connected actions that have to do with Z-Ordering,
   *   such as Bring to Front, Send to Back, etc.
   */
  QList<QAction *> CubeDisplayProperties::getSupportedZOrderActions(
      QList<CubeDisplayProperties *> cubeDisplays) {
    QList<QAction *> actions;

    if(allSupport(ZOrdering, cubeDisplays)) {;
      QAction *moveTop = new QAction("Bring to Front", cubeDisplays[0]);
      QAction *moveUp = new QAction("Bring Forward", cubeDisplays[0]);
      QAction *moveBottom = new QAction("Send to Back", cubeDisplays[0]);
      QAction *moveDown = new QAction("Send Backward", cubeDisplays[0]);

      CubeDisplayProperties *display;
      foreach(display, cubeDisplays) {
        connect(moveTop, SIGNAL(triggered()),
                display, SIGNAL(moveToTop()));
        connect(moveUp, SIGNAL(triggered()),
                display, SIGNAL(moveUpOne()));
        connect(moveBottom, SIGNAL(triggered()),
                display, SIGNAL(moveToBottom()));
        connect(moveDown, SIGNAL(triggered()),
                display, SIGNAL(moveDownOne()));
      }

      actions.append(moveTop);
      actions.append(moveUp);
      actions.append(moveBottom);
      actions.append(moveDown);
    }

    return actions;
  }


  /**
   * Gets a list of pre-connected actions that have to do with zooming,
   *   i.e. Zoom Fit.
   */
  QList<QAction *> CubeDisplayProperties::getSupportedZoomActions(
      QList<CubeDisplayProperties *> cubeDisplays) {
    QList<QAction *> actions;

    if(cubeDisplays.size() == 1 && allSupport(Zooming, cubeDisplays)) {
      QAction *zoomFit = new QAction("Zoom Fit", cubeDisplays[0]);
      connect(zoomFit, SIGNAL(triggered()),
              cubeDisplays[0], SIGNAL(zoomFit()));
      actions.append(zoomFit);
    }

    return actions;
  }


  /**
   * Creates and returns  a random color for the intial color of
   * the footprint polygon.
   */
  QColor CubeDisplayProperties::randomColor() {
    // Gives a random number between 0 and 255
    int red = 0;
    int green = 0;
    int blue = 0;

    // Generate dark
    while(red + green + blue < 300) {
      red   = rand() % 256;
      green = rand() % 256;
      blue  = rand() % 256;
    }

    return QColor(red, green, blue, 60);
  }


  /**
   * Change the color associated with this cube.
   */
  void CubeDisplayProperties::setColor(QColor newColor) {
    setValue(Color, QVariant::fromValue(newColor));
  }


  /**
   * Change the selected state associated with this cube.
   */
  void CubeDisplayProperties::setSelected(bool newValue) {
    setValue(Selected, newValue);
  }


  /**
   * Change the visibility of DNs associated with this cube.
   */
  void CubeDisplayProperties::setShowDNs(bool newValue) {
    setValue(ShowDNs, newValue);
  }


  /**
   * Change the visibility of the fill area associated with this cube.
   */
  void CubeDisplayProperties::setShowFill(bool newValue) {
    setValue(ShowFill, newValue);
  }


  /**
   * Change the visibility of the display name associated with this cube.
   */
  void CubeDisplayProperties::setShowLabel(bool newValue) {
    setValue(ShowLabel, newValue);
  }


  /**
   * Change the visibility of the outline associated with this cube.
   */
  void CubeDisplayProperties::setShowOutline(bool newValue) {
    setValue(ShowOutline, newValue);
  }


  /**
   * Prompt the user for a new alpha value. This should only be connected to
   *   by an action with a list of displays as its data. If the user selects
   *   a new alpha then the cube displays are updated.
   */
  void CubeDisplayProperties::askAlpha() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    bool ok = false;
    int alpha = QInputDialog::getInt(NULL, "Transparency Value",
        "Set the cube's transparency\n"
        "Values are 0 (invisible) to 255 (solid)",
        getValue(Color).value<QColor>().alpha(), 0, 255, 1, &ok);

    if(ok) {
      CubeDisplayProperties *display;
      foreach(display, displays) {
        QColor displayColor = display->getValue(Color).value<QColor>();
        displayColor.setAlpha(alpha);
        display->setColor(displayColor);
      }
    }
  }


  /**
   * Prompt the user for a new color. This should only be connected to
   *   by an action with a list of displays as its data. If the user selects
   *   a new color then the cube displays are updated.
   */
  void CubeDisplayProperties::askNewColor() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    QColor newColor = QColorDialog::getColor(
        getValue(Color).value<QColor>(), NULL,
        "Cube Display Color",
        QColorDialog::ShowAlphaChannel);

    if(newColor.isValid()) {
      CubeDisplayProperties *display;
      foreach(display, displays) {
        display->setColor(newColor);
      }
    }
  }


  void CubeDisplayProperties::showRandomColor() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    CubeDisplayProperties *display;
    foreach(display, displays) {
      QColor ranColor = randomColor();
      display->setColor(ranColor);
    }
  }


  /**
   * Change the visibility of DNs. This should only be connected to
   *   by an action with a list of displays as its data. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  void CubeDisplayProperties::toggleShowDNs() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowDNs).toBool();
    value = !value;

    CubeDisplayProperties *display;
    foreach(display, displays) {
      display->setShowDNs(value);
    }
  }


  /**
   * Change the visibility of the fill area. This should only be connected to
   *   by an action with a list of displays as its data. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  void CubeDisplayProperties::toggleShowFill() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowFill).toBool();
    value = !value;

    CubeDisplayProperties *display;
    foreach(display, displays) {
      display->setShowFill(value);
    }
  }


  /**
   * Change the visibility of the display name. This should only be connected to
   *   by an action with a list of displays as its data. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  void CubeDisplayProperties::toggleShowLabel() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowLabel).toBool();
    value = !value;

    CubeDisplayProperties *display;
    foreach(display, displays) {
      display->setShowLabel(value);
    }
  }


  /**
   * Change the visibility of the outline. This should only be connected to
   *   by an action with a list of displays as its data. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  void CubeDisplayProperties::toggleShowOutline() {
    QList<CubeDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowOutline).toBool();
    value = !value;

    CubeDisplayProperties *display;
    foreach(display, displays) {
      display->setShowOutline(value);
    }
  }


  /**
   * This is the generic mutator for properties. Given a value, this will
   *   change it and emit propertyChanged if its different and supported.
   */
  void CubeDisplayProperties::setValue(Property prop, QVariant value) {
    if((*m_propertyValues)[prop] != value) {
      (*m_propertyValues)[prop] = value;

      if(supports(prop)) {
        emit propertyChanged(this);
      }
    }
  }


  /**
   * This is for the slots that have a list of display properties as associated
   *   data. This gets that list out of the data.
   */
  QList<CubeDisplayProperties *> CubeDisplayProperties::senderToData(
      QObject *senderObj) {
    QList<CubeDisplayProperties *> data;

    if(senderObj) {
      QAction *caller = (QAction *)senderObj;
      QVariant callerData = caller->data();

      if(callerData.canConvert< QList<CubeDisplayProperties *> >() ) {
        data = callerData.value< QList<CubeDisplayProperties *> >();
      }
    }

    return data;
  }


  void CubeDisplayProperties::createManualFootprint(QMutex *cameraMutex) {
    QMutexLocker lock(cameraMutex);

    // We need to walk the polygon...
    ImagePolygon imgPoly;

    int sampleStepSize = cube()->getSampleCount() / 10;
    if(sampleStepSize <= 0) sampleStepSize = 1;

    int lineStepSize = cube()->getLineCount() / 10;
    if(lineStepSize <= 0) lineStepSize = 1;

    imgPoly.Create(*cube(), sampleStepSize, lineStepSize);

    m_footprint = PolygonTools::MakeMultiPolygon(imgPoly.Polys()->clone());

    IException e = IException(IException::User,
        "Warning: Polygon re-calculated "
        "for [" + displayName().toStdString() +
        "] which can be very slow", _FILEINFO_);
    e.print();
  }
}

