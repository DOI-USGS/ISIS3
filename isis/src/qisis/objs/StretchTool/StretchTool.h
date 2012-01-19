#ifndef StretchTool_h
#define StretchTool_h

// This is the only include allowed in this file!
#include "Tool.h"

class QComboBox;
class QPushButton;
class QLineEdit;
class QRect;
class QToolButton;

/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/06/30 03:45:09 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

namespace Isis {
  class AdvancedStretchDialog;
  class Cube;
  class CubeViewport;
  class Histogram;
  class Statistics;
  class Stretch;
  class ViewportBuffer;

  /**
   * @brief Stretch image edit tool
   *
   * This tool is part of the Qisis namespace and allows interactive editing
   * of displayed images.
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *  @history 2008-05-23 Noah Hilt - Added RubberBandTool
   *  @history 2010-03-18 Sharmila Prasad - Exception handling and emit warning
   *                          signal to the Viewportwindow to display warning
   *                          status on the status bar(Track Tool)
   *  @history 2010-04-08 Steven Lambright - Made to work with threaded
   *                           viewportbuffer and also removed map to viewport
   *                           stretches due to synchronization problems.
   *  @history 2010-04-08 Steven Lambright - Fixed access of viewport data for
   *                          statistics
   *  @history 2010-05-20 Steven Lambright - Re-enabled the advanced stretch...
   *                          without a viewport map!
   *  @history 2010-06-26 Eric Hyer
   *                          - Now uses MdiCubeViewport
   *                          - Fixed all include abuses
   *  @history 2010-08-30 Eric Hyer
   *                          - Added RGB support for stretchRegional
   *                          - stretchRegional and rubberBandComplete now use
   *                            the new private method stretchRect in order to
   *                            avoid duplicate code.
   *  @history 2010-11-22 Eric Hyer - new SIGNAL called stretchChipViewport
   *                          exists for sending stretches made in CubeViewports
   *                          to any ChipViewports that want to get stretched.
   *  @history 2010-12-01 Steven Lambright - Improved stretch copy abilities
   *  @history 2010-12-02 Eric Hyer - changed where stretchChipViewport is
   *                          emitted.
   *  @history 2011-03-22 Sharmila Prasad - Add option to stretch All Bands
   *  @history 2011-11-04 Steven Lambright - Improved advanced stretch to allow
   *                          all stretches to be changed despite combo box.
   *                          References #567.
   *  @history 2012-01-18 Steven Lambright - Fixed a bug that caused the stretch
   *                          pair line edits to not update properly in grey
   *                          mode when the color mode band selection was set to
   *                          all. Also, fixed layout of stretch options to
   *                          correctly squeeze widgets left. Fixes #640.
   */
  class StretchTool : public Tool {
      Q_OBJECT

    public:
      StretchTool(QWidget *parent);
      ~StretchTool();
      void addTo(QMenu *menu);

      /**
       * Enum to store the bands.
       */
      enum StretchBand {
        Gray,  //!< Gray Band
        Red,   //!< Red Band
        Green, //!< Green Band
        Blue,  //!< Blue Band
        All    //!< All Bands
      };

      static Stretch stretchBuffer(ViewportBuffer *buffer, QRect rect);
      static Stretch stretchBand(CubeViewport *cvp, StretchBand band);
      
      static Statistics statsFromCube(Cube *cube, int band);
      static Statistics statsFromBuffer(ViewportBuffer *buffer, QRect rect);
      static Histogram histFromCube(Cube *cube, int band,
                                          double min, double max);
      static Histogram histFromBuffer(ViewportBuffer *buffer);
      static Histogram histFromBuffer(ViewportBuffer *buffer, QRect rect,
                                            double min, double max);
      void updateAdvStretchDialogforAll(void);
      

    signals:
      /**
       * when a viewport is stretched, send the stretch and the viewport
       * associated with it to any ChipViewport's that might be listening
       */
      void stretchChipViewport(Stretch *, CubeViewport *);
      
      /**
       * Shows a warning. This sends a signal (meant for when an
       * exception occurs) to display the error using the warning
       * object
       *
       * @param pStr
       * @param pExStr
       */
      void warningSignal(std::string   &pStr, const std::string pExStr);
      
    public slots:
      void stretchGlobal();
      void stretchGlobal(CubeViewport *);
      void stretchGlobalAllBands();
      void stretchGlobalAllViewports();
      void stretchRegional();
      void stretchRegional(CubeViewport *);

    protected:
      /**
       * This let's Tool know which Menu the actions this class has
       * should be added to. Removal of this results in "Ctrl+R" and "Ctrl+G"
       * not working.
       *
       * @return Name of the Menu
       */
      QString menuName() const {
        return "&View";
      };
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *parent);
      void updateTool();
      void stretchRequested(MdiCubeViewport *cvp, int bandId);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);
      void enableRubberBandTool();
      void screenPixelsChanged();
      void updateHistograms();
      void rubberBandComplete();
      void setCubeViewport(CubeViewport *);

    private slots:
      void stretchChanged();
      void advancedStretchChanged();
      void changeStretch();
      void showAdvancedDialog();
      void setStretchAcrossBands();
      void setStretchAllViewports();
      void stretchBandChanged(int);

    private:
      void stretchRect(CubeViewport *cvp, QRect rect);
      
    private:
      AdvancedStretchDialog *p_advancedStretch; //!< The advanced dialog

      QToolButton *p_copyButton;         //!< Copy Button
      QToolButton *p_globalButton;       //!< Global Button
      QToolButton *p_stretchRegionalButton; //!< Regional Stretch Button
      QPushButton *p_flashButton; //!< Button to press for global stretch

      QAction *p_stretchGlobal;          //!< Global stretch action
      QAction *p_stretchRegional;        //!< Regional stretch action
      QAction *p_stretchManual;          //!< Manual stretch action
      QAction *p_copyBands;              //!< Copy band stretch action

      QComboBox *p_stretchBandComboBox;  //!< Stretch combo box

      QLineEdit *p_stretchMinEdit;       //!< Min. line edit
      QLineEdit *p_stretchMaxEdit;       //!< Max. line edit

      StretchBand p_stretchBand;         //!< Current stretch band

      //! Stretches before global button pressed
      Stretch *p_preGlobalStretches;
      
      Stretch *p_chipViewportStretch; //!< ChipViewport's stretch
  };
};

#endif

