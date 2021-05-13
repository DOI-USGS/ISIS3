#ifndef CubeViewport_h
#define CubeViewport_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


// parent of this class
#include <QAbstractScrollArea>

class QPaintEvent;

namespace Isis {
  class Brick;
  class Camera;
  class Cube;
  class CubeDataThread;
  class Projection;
  class Pvl;
  class PvlKeyword;
  class CubeStretch;
  class Stretch;
  class Tool;
  class UniversalGroundMap;

  class ViewportBuffer;

  /**
   * @brief Widget to display Isis cubes for qt apps
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Jeff Anderson
   *
   * @internal
   *  @history 2007-01-30  Tracie Sucharski,  Add information
   *                           message if spice not found.
   *  @history 2007-02-07  Tracie Sucharski,  Remove error message, decided it
   *                           was more of a hassle to click ok when displaying
   *                           many images without spice.
   *  @history 2007-03-20  Tracie Sucharski,  Add fitScaleMinDimension,
   *                           fitScaleWidth and fitScaleHeight methods.  Change
   *                           cursor to wait cursor in paintPixmap.
   *  @history 2007-04-13  Tracie Sucharski, Remove fitScaleMinDimension, turns
   *                           out it was not needed.
   *  @history 2007-09-11  Steven Lambright, Added double click signal
   *  @history 2007-12-11  Steven Lambright, Added 1x1xn cube auto stretch support
   *  @history 2008-06-19  Noah Hilt, Added a close event for saving and discarding
   *                           changes and a method to set the cube.
   *  @history 2008-12-04  Jeannie Walldren, Fixed a bug in computeStretch() method
   *                           for comparing precision difference on 32 bit Linux
   *                           system. Added try/catch in showCube() to set
   *                           p_cubeShown = false if this fails.
   *  @history 2009-03-09  Steven Lambright - Changed the way we do floating point
   *                           math in autoStretch to work better in order to allow
   *                           more cubes to open.
   *  @history 2009-03-27 Noah Hilt/Steven Lambright - Removed old rubber band
   *                          methods and variables. Added new ViewportBuffer to
   *                          read and store visible dn values in the viewport.
   *  @history 2009-10-23 Steven Lambright - Camera::SetBand is now called when
   *                          switching the band being shown.
   *  @history 2010-04-08 Steven Lambright and Eric Hyer -
   *                          Now supports ViewportBuffer using threaded cube I/O
   *  @history 2010-04-30 Steven Lambright - Bug fixes and better
   *                          support for threaded cube I/O
   *  @history 2010-05-20 Steven Lambright - Added memory of global
   *                          stretches
   *  @history 2010-05-24 Eric Hyer - Fixed bug where QPainter construction was
   *                          being attempted with a potentially null pixmap
   *  @history 2010-06-26 Eric Hyer - Moved MDI specific code to new child class
   *                          called MdiCubeViewport.  Fixed many include mistakes.
   *  @history 2010-07-12 Jeannie Walldren - Changed setScale() method's maximum
   *                          value from hard-coded 16 to the max of the viewport
   *                          width and height.  Added setScale() minimum value
   *                          of 1/min(samps,lines).  Added exceptions to
   *                          ViewportBuffer to help track errors.
   *  @history 2010-11-08 Eric Hyer -  Added better mouseMove signal
   *  @history 2010-12-01 Steven Lambright - The initial scale now uses fitScale
   *                          so that it is consistent with the zoom tool.
   *  @history 2011-03-30 Sharmila Prasad - Set the frame shadow and style to remove
   *                          border around the image
   *  @history 2011-03-31 Sharmila Prasad - Added band info to "whatsthis"
   *                          API to store the whatsthis info in a PVL format
   *  @history 2011-04-25 Steven Lambright - Fixed "center" and added more safety
   *                          checks.
   *  @history 2012-03-22 Steven Lambright and Jai Rideout - Fixed bug where
   *                          screenPixelsChanged was not correctly emitted
   *                          on resize.
   *  @history 2012-05-29 Steven Lambright - Changed destructor to clean up the cube when
   *                          necessary.
   *  @history 2012-06-28 Steven Lambright - Stretching gray no longer removes stretch
   *                          special pixel values from the RGB stretches. References #684.
   *  @history 2012-07-27 Tracie Sucharski - Added viewportClosed signal so that tools
   *                          can respond to the user closing a viewport rather than when the
   *                          application exits.
   *  @history 2013-11-04 Janet Barrett - Added the p_comboCount and p_comboIndex variables to
   *                          store the current state of the band bin combo box. Also added
   *                          comboCount(), comboIndex(), setComboCount(), and setComboIndex()
   *                          accessors and setters to allow the BandTool class to retain the
   *                          settings for each cube viewport. Fixes #1612.
   *  @history 2017-05-10 Ian Humphrey - Modified showEvent() so that when the cube viewport is
   *                          initially shown, the image is centered properly in the viewport.
   *                          Modified center(double, double) to no longer increment the
   *                          cubeToContents x,y by 1. This was causing some images to be
   *                          off-centered by 1 pixel (the bottom and right side of viewport would
   *                          be a thin strip of non-pixel space). Fixes #4756.
   *  @history 2017-08-11 Adam Goins - Added the ability to ctrl + c to copy the filename
   *                          of the current cube into the system's clipboard.
   *                          Fixes #5098.
   *  @history 2018-07-31 Kaitlyn Lee - Added setTrackingCube() and trackingCube() so that a
   *                          tracking cube is stored when needed and we do not have to open it in
   *                          AdvancedTrackTool every time the cursor is moved.
   *  @history 2020-06-09 Kristin Berry - Updated paintPixmap() to move getStretch out of inner
   *                          for loops. This provides a significant speed increase for qisis
   *                          applications with cube viewports.
   */
  class CubeViewport : public QAbstractScrollArea {
      Q_OBJECT

    public:
      /**
       * Constructor for the CubeViewport
       *
       * @param cube The cube to load into a CubeViewport
       * @param cubeData The Cube Data Thread
       * @param parent The parent widget to this Viewport
       *
       */
      CubeViewport(Cube *cube, CubeDataThread * cdt = 0, QWidget *parent = 0);

      /**
       * Deconstructor for the Cubeviewport
       */
      virtual ~CubeViewport();


      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class BandInfo {
        public:
          BandInfo();
          //! @param other The other BandInfo
          BandInfo(const BandInfo &other);
          //! Deconstructor
          ~BandInfo();

          /**
           * The BandInfo for the Cube
           *
           * @param other The other BandInfo
           * @return The BandInfo
           */
          const BandInfo &operator=(BandInfo other);

          //! @return The Stretch
          CubeStretch getStretch() const;
          //! @param newStretch The new Stretch value
          void setStretch(const Stretch &newStretch);
          //! The band
          int band;
        private:
          //! The Stretch
          CubeStretch *stretch;
      };

      //! @param cube The cube to set the CubeViewport window to
      void setCube(Cube *cube);
      //! @return The number of samples in the cube
      int cubeSamples() const;
      //! @return The number of lines in the cube
      int cubeLines() const;
      //! @return The number of bands in the cube
      int cubeBands() const;

      //! @return Is the viewport shown in 3-band color
      bool isColor() const {
        return p_color;
      };

      //! @return Is the viewport shown in gray / b&w
      bool isGray() const {
        return !p_color;
      };

      //! @return the gray band currently viewed
      int grayBand() const {
        return p_gray.band;
      };

      //! @return the red band currently viewed
      int redBand() const {
        return p_red.band;
      };

      //! @return the green band currently viewed
      int greenBand() const {
        return p_green.band;
      };

      //! @return the blue band currently viewed
      int blueBand() const {
        return p_blue.band;
      };

      //! @return the scale
      double scale() const {
        return p_scale;
      };

      //! @return if the cube is visible
      bool cubeShown() const {
        return p_cubeShown;
      };

      //! @return the BandBin combo box count
      int comboCount() const {
        return p_comboCount;
      };

      //! @return the BandBin combo box index
      int comboIndex() const {
        return p_comboIndex;
      }

      /**
       * Calle dhwen the contents of the cube changes
       *
       * @param rect The QRect
       */
      void cubeContentsChanged(QRect rect);

      //! @return The fitScale of the Viewport
      double fitScale() const;
      //! @return The width of the Viewport
      double fitScaleWidth() const;
      //! @return The height of the Viewport/
      double fitScaleHeight() const;

      /**
       * Turns a viewport into a cube
       *
       * @param x
       * @param y
       * @param sample
       * @param line
       */
      void viewportToCube(int x, int y,
                          double &sample, double &line) const;

       /**
       * Turns a cube into a viewport
       *
       * @param x
       * @param y
       * @param sample
       * @param line
       */
      void cubeToViewport(double sample, double line,
                          int &x, int &y) const;
       /**
       * Turns contents to a cube
       *
       * @param x
       * @param y
       * @param sample
       * @param line
       */
      void contentsToCube(int x, int y,
                          double &sample, double &line) const;
       /**
       * Turns a cube into contents
       *
       * @param x
       * @param y
       * @param sample
       * @param line
       */
      void cubeToContents(double sample, double line,
                          int &x, int &y) const;

      /**
       * Gets the red pixel
       *
       * @param sample The sample
       * @param line The line
       *
       * @return The redPixel value
       */
      double redPixel(int sample, int line);

      /**
       * Gets the green pixel
       *
       * @param sample The sample
       * @param line The line
       *
       * @return The greenPixel value
       */
      double greenPixel(int sample, int line);

      /**
       * Gets the blue pixel
       *
       * @param sample The sample
       * @param line The line
       *
       * @return The bluePixel value
       */
      double bluePixel(int sample, int line);

      /**
       * Gets the gray pixel
       *
       * @param sample The sample
       * @param line The line
       *
       * @return The grayPixel value
       */
      double grayPixel(int sample, int line);
      //! @return The gray Stretch
      CubeStretch grayStretch() const;
      //! @return The red Stretch
      CubeStretch redStretch() const;
      //! @return The green Stretch
      CubeStretch greenStretch() const;
      //! @return The blue Strech
      CubeStretch blueStretch() const;

      //! @return The cube associated with viewport
      Cube *cube() const {
        return p_cube;
      };

      //! @return The projection associated with cube (NULL implies none)
      Projection *projection() const {
        return p_projection;
      };

      //! @return The camera associated with the cube (NULL implies none)
      Camera *camera() const {
        return p_camera;
      };

      //! @return the universal ground map associated with the cube (NULL implies none)
      UniversalGroundMap *universalGroundMap() const {
        return p_groundMap;
      };

      //! @return The tracking cube associated with p_cube (if it has one)
      Cube *trackingCube() const {
        return p_trackingCube;
      };

      void moveCursor(int x, int y);
      bool cursorInside() const;
      QPoint cursorPosition() const;
      void setCursorPosition(int x, int y);
      void setCaption();

      /**
       * Sets the background color
       *
       * @param color
       */
      void setBackground(QColor color) {
        p_bgColor = color;
      }

      /**
       * Sets the band bin combo box count
       *
       * @param count
       */
      void setComboCount(int count) {
        p_comboCount = count;
      }

      /**
       * Sets the band bin combo box index
       *
       * @param index
       */
      void setComboIndex(int index) {
        p_comboIndex = index;
      }

      /**
       * Returns the pixmap
       *
       *
       * @return QPixmap
       */
      QPixmap pixmap() {
        return p_pixmap;
      }

      /**
       * Returns the gray viewport buffer (Will be NULL if in RGB mode.)
       *
       *
       * @return ViewportBuffer*
       */
      ViewportBuffer *grayBuffer() {
        return p_grayBuffer;
      }

      /**
       * Returns the red viewport buffer (Will be NULL if in Gray mode.)
       *
       *
       * @return ViewportBuffer*
       */
      ViewportBuffer *redBuffer() {
        return p_redBuffer;
      }

      /**
       * Returns the green viewport buffer (Will be NULL if in Gray mode.)
       *
       *
       * @return ViewportBuffer*
       */
      ViewportBuffer *greenBuffer() {
        return p_greenBuffer;
      }

      /**
       * Returns the blue viewport buffer (Will be NULL if in Gray mode.)
       *
       *
       * @return ViewportBuffer*
       */
      ViewportBuffer *blueBuffer() {
        return p_blueBuffer;
      }

      void bufferUpdated(QRect rect);

      /**
       * This is called by internal viewport buffers when a stretch
       * action should be performed. The buffer passes itself as the
       * argument.
       *
       * @param buffer
       */
      virtual void restretch(ViewportBuffer *buffer) = 0;
      void paintPixmap();

      /**
       * Resets all remembered stretches
       */
      void forgetStretches();

      /**
       * Sets a stretch for all bands
       *
       * @param stretch
       */
      void setAllBandStretches(Stretch stretch);


      /**
       * @returns this CubeViewport's CubeDataThread
       */
      CubeDataThread *cubeDataThread() {
        return p_cubeData;
      }

      /**
       * @returns the CubeViewport's cube id
       */
      int cubeID() {
        return p_cubeId;
      }


      /**
       * Get All WhatsThis info - viewport, cube, area in PVL format
       *
       * @param pWhatsThisPvl - Pvl for all whatsthis info
       */
      void getAllWhatsThisInfo(Pvl & pWhatsThisPvl);

      /**
       * Get Band Filter name from the Isis cube label
       *
       * @param pFilterNameKey - FilterName keyword containing the
       *              corresponding keyword from the Isis Cube label
       */
      void getBandFilterName(PvlKeyword & pFilterNameKey);

      /**
       * Get Cube area corresponding to the viewport's dimension
       *
       * @param pdStartSample - Cube Start Sample
       * @param pdEndSample   - Cube End Sample
       * @param pdStartLine   - Cube Start Line
       * @param pdEndLine     - Cube End Line
       */
      void getCubeArea(double & pdStartSample, double & pdEndSample,
                                     double & pdStartLine, double & pdEndLine);

      bool confirmClose();

      void setTrackingCube();

    signals:
      void viewportUpdated();//!< Emitted when viewport updated.
      void viewportClosed(CubeViewport *);//!< Emitted when viewport is closed.
      void mouseEnter();//!< Emitted when the mouse enters the viewport
      void mouseMove(QPoint);//!< Emitted when the mouse moves.
      void mouseMove(QPoint, Qt::MouseButton);//!< Emitted when the mouse moves.
      void mouseLeave();//!< Emitted when the mouse leaves the viewport.
      void mouseButtonPress(QPoint, Qt::MouseButton);//!< Emitted when mouse button pressed
      void mouseButtonRelease(QPoint, Qt::MouseButton);//!< Emitted when mouse button released
      void mouseDoubleClick(QPoint);//!< Emitted when double click happens
      void windowTitleChanged();//!< Emitted when window title changes
      void scaleChanged(); //!< Emitted when zoom factor changed just before the repaint event
      void saveChanges(CubeViewport *); //!< Emitted when changes should be saved
      void discardChanges(CubeViewport *); //!< Emitted when changes should be discarded
      void screenPixelsChanged(); //!< Emitted when cube pixels that should be on the screen change

      /**
       * Emitted with current progress (0 to 100) when working
       */
      void progressChanged(int);
      /**
       * Emitted when the current progress is complete (100)
       */
      void progressComplete();

      /**
       * Emitted when a brick is no longer needed, should only be sent
       * to cube data thread
       */
      void doneWithData(int, const Isis::Brick *);


    public slots:
      QSize sizeHint() const;
      void setScale(double scale);
      void setScale(double scale, double sample, double line);
      void setScale(double scale, int x, int y);
      void center(int x, int y);
      void center(double sample, double line);

      virtual void viewRGB(int redBand, int greenBand, int blueBand);
      virtual void viewGray(int band);

      void stretchGray(const QString &string);
      void stretchRed(const QString &string);
      void stretchGreen(const QString &string);
      void stretchBlue(const QString &string);

      void stretchGray(const Stretch &stretch);
      void stretchRed(const Stretch &stretch);
      void stretchGreen(const Stretch &stretch);
      void stretchBlue(const Stretch &stretch);

      void stretchKnownGlobal();

      void cubeChanged(bool changed);
      void showEvent(QShowEvent *);

      void scrollBy(int dx, int dy);

      void changeCursor(QCursor cursor);

      void onProgressTimer();
      void enableProgress();


    protected:
      void scrollContentsBy(int dx, int dy);
      virtual void resizeEvent(QResizeEvent *e);
      virtual bool eventFilter(QObject *o, QEvent *e);
      virtual void keyPressEvent(QKeyEvent *e);
      virtual void paintEvent(QPaintEvent *e);



    protected slots:
      virtual void cubeDataChanged(int cubeId, const Isis::Brick *);


    private:

      void paintPixmap(QRect rect);
      void shiftPixmap(int dx, int dy);

      void updateScrollBars(int x, int y);
      void paintPixmapRects();

      //void computeStretch(Brick *brick, int band,
      //                    int ssamp, int esamp,
      //                    int sline, int eline, int linerate,
      //                    Stretch &stretch);



    protected: // data
      QPixmap p_pixmap;//!< The qpixmap.

      //! Stretches for each previously stretched band
      QVector< Stretch * > * p_knownStretches;

      //! Global stretches for each stretched band
      QVector< Stretch * > * p_globalStretches;


    private: // data
      ViewportBuffer *p_grayBuffer;  //!< Viewport Buffer to manage gray band
      ViewportBuffer *p_redBuffer;  //!< Viewport Buffer to manage red band
      ViewportBuffer *p_greenBuffer;  //!< Viewport Buffer to manage green band
      ViewportBuffer *p_blueBuffer;  //!< Viewport Buffer to manage blue band

      QColor p_bgColor; //!< The color to paint the background of the viewport

      Cube *p_cube;  //!< The cube associated with the viewport.
      Camera *p_camera;  //!< The camera from the cube.
      Projection *p_projection;  //!< The projection from the cube.
      UniversalGroundMap *p_groundMap;  //!< The universal ground map from the cube.
      Cube *p_trackingCube; //<! The tracking cube associated with p_cube

      //! Activated to update progress bar
      QTimer *p_progressTimer;

      double p_scale;//!< The scale number.

      bool p_color;//!< Is the viewport in color?
      BandInfo p_gray;//!< Gray band info
      BandInfo p_red;//!< Red band info
      BandInfo p_green;//!< Green band info
      BandInfo p_blue;//!< Blue band info

      int p_comboCount;//!< Number of elements in band bin combo box
      int p_comboIndex;//!< Current element chosen from combo box

      Brick *p_redBrick;  //!< Bricks for every color.
      Brick *p_grnBrick;  //!< Bricks for every color.
      Brick *p_bluBrick;  //!< Bricks for every color.
      Brick *p_gryBrick;  //!< Bricks for every color.
      Brick *p_pntBrick;  //!< Bricks for every color.
      bool p_saveEnabled; //!< Has the cube changed?
      bool p_cubeShown;//!< Is the cube visible?
      QImage *p_image;  //!< The qimage.
      bool p_paintPixmap;//!< Paint the pixmap?
      bool p_updatingBuffers; //!< Changing RGB and need to not repaint pixmap?

      QString p_whatsThisText;//!< The text for What's this.
      QString p_cubeWhatsThisText;//!< The text for the cube's What's this.
      QString p_viewportWhatsThisText;//!< The text for the viewport's what's this.
      void updateWhatsThis();

      //! A list of rects that the viewport buffers have requested painted
      QList< QRect * > *p_pixmapPaintRects;

      CubeDataThread *p_cubeData;  //!< Does all the cube I/O
      int p_cubeId; //!< Cube ID given from cube data thread for I/O

      /**
       *  if true then this owns the CubeDataThread,
       *  and should thus delete it
       */
      bool p_thisOwnsCubeData;
  };
}

#endif
