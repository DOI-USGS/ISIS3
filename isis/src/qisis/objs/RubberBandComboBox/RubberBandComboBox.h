#ifndef RubberBandComboBox_h
#define RubberBandComboBox_h

#include <QComboBox>

// FIXME: Remove this include!
#include "RubberBandTool.h"

namespace Qisis {
  /**
  * @brief Combo box for choosing a rubber band type
  *
  * @ingroup Visualization Tools
  *
  * @author 2007-09-11 Steven Lambright
  *
  * @internal
  *   @history 2008-09-26 Steven Lambright Added Segmented line, reordered banding
  *            options at Stuart's Request
  *   @history 2008-10-01 Steven Lambright Fixed bug with segmented line, if
  *            polygon was enabled then segmented line was
  *   @history 2010-06-26 Eric Hyer - Fixed one include issue
  *   @history 2010-11-08 Eric Hyer - Updated documentation for drawing lines
  */

  class RubberBandComboBox : public QComboBox {
      Q_OBJECT

    public:
      RubberBandComboBox(unsigned int bandingOptions, unsigned int defaultOption, bool showIndicatorColors = false);

      /**
       * Enum to store rubber band shapes.
       */
      enum RubberBandOptions {
        Circle           = 1,   //!< Circle
        Ellipse          = 2,   //!< Ellipse
        Rectangle        = 4,   //!< Rectangle
        RotatedRectangle = 8,   //!< RotatedRectangle
        Polygon          = 16,  //!< Polygon
        Line             = 32,  //!< Line
        SegmentedLine    = 64,  //!< Segmented Line
        Angle            = 128  //!< Angle
      };

      //! Returns the icon directory.
      QString toolIconDir() const {
        return RubberBandTool::getInstance()->toolIconDir();
      }
      void reset();

    protected slots:
      void selectionChanged(int index);

    private:
      //! This is used to figure out which option should be defaulted
      unsigned int getDefault(unsigned int defaultOption, unsigned int bandingOptions);

      //! Enables the angle shape
      void showAngle()             {
        RubberBandTool::enable(RubberBandTool::Angle, p_showIndicatorColors);
      };
      //! Enables the circle shape
      void showCircle()            {
        RubberBandTool::enable(RubberBandTool::Circle, p_showIndicatorColors);
      };
      //! Enables the ellipse shape
      void showEllipse()           {
        RubberBandTool::enable(RubberBandTool::Ellipse, p_showIndicatorColors);
      };
      //! Enables the line
      void showLine()              {
        RubberBandTool::enable(RubberBandTool::Line, p_showIndicatorColors);
      };
      //! Enables the rectangle shape
      void showRectangle()         {
        RubberBandTool::enable(RubberBandTool::Rectangle, p_showIndicatorColors);
      };
      //! Enables the rotated rectangle shape
      void showRotatedRectangle()  {
        RubberBandTool::enable(RubberBandTool::RotatedRectangle, p_showIndicatorColors);
      };
      //! Enables the polygon shape
      void showPolygon()           {
        RubberBandTool::enable(RubberBandTool::Polygon, p_showIndicatorColors);
      };
      //! Enables the segmented line shape
      void showSegmentedLine()     {
        RubberBandTool::enable(RubberBandTool::SegmentedLine, p_showIndicatorColors);
      };

      QStringList p_bandingOptionStrings; //!< List of rubberband options
      bool p_showIndicatorColors; //!< Show colors?
  };
};

#endif
