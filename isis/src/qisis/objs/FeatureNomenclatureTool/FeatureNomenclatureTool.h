#ifndef FeatureNomenclatureTool_h
#define FeatureNomenclatureTool_h

#include "Tool.h"

#include <QPointer>
#include <QProgressDialog>
#include "FeatureNomenclature.h"

class QCheckBox;
class QComboBox;
class QDialog;
class QLabel;
class QLineEdit;
class QMenu;
class QProgressBar;
class QPushButton;
class QString;

template <typename A, typename B> class QMap;
template <typename A, typename B> struct QPair;

namespace Isis {
  class MdiCubeViewport;
  class UniversalGroundMap;

  /**
   * @brief Display nomenclature on MDI Cube Viewports
   *
   * This tool is designed to paint named features onto the viewports' displays.
   *   The nomenclature and it's positioning comes from the FeatureNomenclature
   *   class. Options such as auto-enabling from program start, showing vectors,
   *   font configurations, showing detailed feature information and linking
   *   back to the nomenclature website are built-in.
   *
   * @ingroup Visualization Tools
   *
   * @author 2012-03-22 Steven Lambright and Jai Rideout
   *
   * @internal
   *   @history 2012-06-12 Steven Lambright and Kimberly Oyama - Implemented multiple extent types
   *                           (Box, 4 Arrows, 8 Arrows). Added ability to hide unapproved or
   *                           dropped features. Added appropriate mutators and accessors
   *                           (showApprovedOnly(), vectorType(), setShowApprovedOnly(),
   *                           setVectorType()). Added an enumerator for the vector types.
   *                           Fixes #852. Fixes #892.
   *   @history 2012-06-26 Kimberly Oyama - Minor refactoring in the findMissingNomenclature()
   *                           method. References #958.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972
   *   @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References Mantis tickets 
   *                           #775 and #1114.
   *   @history 2016-06-21 Kris Becker - Properly forward declare QPair as struct not class
   * 
   */
  class FeatureNomenclatureTool : public Tool {
      Q_OBJECT

    public:

      //!Enumeration of extent vector typess
      enum VectorType {
        /**
         * When using this vector (extent) type, no extents will be drawn.
         */
        None,
        /**
         * When using this vector (extent) type, 4 arrows will be drawn out from the text of
         *   the feature. If an arrow doesn't extend past the text then it will not be drawn.
         *   The extents should look something like this:
         *              ___
         *               ^
         *               |
         *               |
         *               |
         *               |
         *    |<---- Feature Name ------>|
         *               |
         *               |
         *               v
         *              ___
         */
        Arrows4,
        /**
         * When using this vector (extent) type, 8 arrows will be drawn out from the text of
         *   the feature. If an arrow doesn't extend past the text then it will not be drawn.
         *   The extents would look similar to the 4 arrow version shown above. There would be
         *   8 arrows extending to the marked points:
         *                 *

         *          *             *
         *
         * 
         *    *      Feature Name      *
         *           
         *           
         *          *             *
         * 
         *                 *
         */
        Arrows8,
        /**
         * When using this vector (extent) type, 4 arrows will be drawn out from the text of
         *   the feature. If the arrow doesn't extend past the text then it will not be drawn.
         *   The extents should look something like this:
         *     _____________
         *    |Feature Name |
         *    |_____________|           
         */
        Box       
      };
      
      FeatureNomenclatureTool(QWidget *parent);
      ~FeatureNomenclatureTool();

      void addTo(QMenu *menu);
      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

      bool defaultEnabled() const;
      QColor fontColor() const;
      int fontSize() const;
      bool showApprovedOnly() const;
      VectorType vectorType() const;

      void setDefaultEnabled(bool defaultEnabled);
      void setFontColor(QColor color);
      void setFontSize(int newFontSize);
      void setShowApprovedOnly(bool approvedOnly);
      void setVectorType(VectorType show);

      QString menuName() const;

    protected:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);
      void updateTool();

    private slots:
      void centerOnSelectedFeature();
      void configure();
      void featureSelected();
      void featuresIdentified(FeatureNomenclature *);
      void findNomenclatureStateChanged(int);
      void nomenclaturePositionsOutdated();
      void onToolActivated();
      void showDisclaimer();

    private:
      // This is an inner class defined below.
      class ViewportFeatureDisplay;

      void centerOnFeature(MdiCubeViewport *vp, FeatureNomenclature::Feature);
      void featuresForViewportFound(MdiCubeViewport *vp);
      void findMissingNomenclature();
      void findMissingNomenclature(MdiCubeViewport *vp);
      void rebuildFeaturesCombo();
      void removeFeatureDisplay(MdiCubeViewport *vp);
      void showFeatureDetails(FeatureNomenclature::Feature);
      void showFeatureWebsite(FeatureNomenclature::Feature);
      void toolStateChanged();
      void viewportDone(MdiCubeViewport *vp);
      ViewportFeatureDisplay *viewportFeatureDisplay(MdiCubeViewport *vp);
      const ViewportFeatureDisplay *
          viewportFeatureDisplay(MdiCubeViewport *vp) const;
      bool viewportFeaturesFound(MdiCubeViewport *vp) const;
      QList<MdiCubeViewport *> viewportsWithFoundNomenclature();

      void readSettings();
      void writeSettings();

    private:
      /**
       * @brief A named feature's position in a cube.
       *
       * This class encapsulates the sample,line position and extents of a
       *   named feature given an image (in this case, a viewport, because that
       *   holds the universal ground map).
       *
       * @author 2012-03-22 Steven Lambright and Jai Rideout
       *
       * @internal
       *   @history 2012-06-06 Steven Lambright and Kimberly Oyama - Added support for multiple
       *                           extent/vector types (FeaturePosition(), applyExtentType()). 
       *                           Fixes #852. Fixes #892.
       *                           
       */
      class FeaturePosition {
        public:
          FeaturePosition();
          FeaturePosition(MdiCubeViewport *, FeatureNomenclature::Feature, VectorType vectorType);
          FeaturePosition(const FeaturePosition &other);
          ~FeaturePosition();

          bool isValid() const;

          QPair<double, double> center() const;
          QList< QPair<double, double> > edges() const;
          FeatureNomenclature::Feature &feature();
          const FeatureNomenclature::Feature &feature() const;
          void applyExtentType(VectorType vectorType);
          
          void swap(FeaturePosition &other);
          FeaturePosition &operator=(const FeaturePosition &rhs);

        private:
          //! The cube line position of the feature center, Null if !isValid()
          double m_centerLine;
          //! The cube sample position of the feature center, Null if !isValid()
          double m_centerSample;

          //!The map used to determine the sample, line pair from a lat, lon pair.
          UniversalGroundMap *m_gmap;

          /**
           * The pair is cube sample, line (first and second) respectively. This
           *   holds the edge points/extents of the feature.
           */
          QList< QPair<double, double> > *m_featureEdgeLineSamples;
          //! The feature for which we're encapsulating a viewport position
          FeatureNomenclature::Feature m_feature;
      };


      /**
       * @brief A named feature's position in a viewport.
       *
       * This class encapsulates the screen x,y boxes, extents, and edge points
       *   of a named feature in a viewport.
       *
       * @author 2012-03-22 Steven Lambright and Jai Rideout
       *
       * @internal
       * 
       */
      class FeatureDisplayPosition {
        public:
          FeatureDisplayPosition();
          FeatureDisplayPosition(QRect textRect, QRect fullDisplayRect,
              QList<QPoint> edgePoints);
          FeatureDisplayPosition(const FeatureDisplayPosition &other);
          ~FeatureDisplayPosition();

          QRect textArea() const;
          QRect displayArea() const;
          QList<QPoint> edgePoints() const;

          void swap(FeatureDisplayPosition &other);
          FeatureDisplayPosition &operator=(const FeatureDisplayPosition &rhs);
        private:

        private:
          //! The viewport screen pixel rect which the text will consume
          QRect *m_textRect;
          //! The viewport screen pixel rect which the entire display will use
          QRect *m_fullDisplayRect;
          //! The viewport screen pixel points which the edges are at
          QList<QPoint> *m_edgePoints;
      };


      /**
       * @brief The feature display on a single viewport
       *
       * This class encapsulates everything to do with displaying found features
       *   on a single viewport.
       *
       * @author 2012-03-22 Steven Lambright and Jai Rideout
       *
       * @internal
       *   @history 2012-06-06 Steven Lambright and Kimberly Oyama - Added support for multiple
       *                           extent/vector types (ViewportFeatureDisplay(), applyExtentType(),
       *                           features()). Fixes #852. Fixes #892.
       */
      class ViewportFeatureDisplay {
        public:
          ViewportFeatureDisplay();
          ViewportFeatureDisplay(FeatureNomenclatureTool *tool,
              MdiCubeViewport *sourceViewport,
              QList<FeatureNomenclature::Feature> features,
              VectorType vectorType);
          ViewportFeatureDisplay(const ViewportFeatureDisplay &other);
          ~ViewportFeatureDisplay();

          void applyExtentType(VectorType vectorType);
          void centerFeature(FeatureNomenclature::Feature);
          QList<FeatureNomenclature::Feature> features();
          QList<FeaturePosition> featurePositions();
          MdiCubeViewport *sourceViewport() const;
          void paint(QPainter *painter, bool showVectors,
                     VectorType vectorType, bool approvedOnly)const;

          void handleMouseClicked(FeatureNomenclatureTool *tool, QPoint p,
                                  Qt::MouseButton s);
          void handleViewChanged(FeatureNomenclatureTool *tool);

          void swap(ViewportFeatureDisplay &other);
          ViewportFeatureDisplay &operator=(
              const ViewportFeatureDisplay &rhs);

        private:
          QPair<QPointF, QPointF> viewportCubeRange() const;

        private:
          /**
           * The viewport this display is working with; we paint onto this
           *   viewport and react to events on this viewport.
           */
          MdiCubeViewport *m_sourceViewport;
          //! The features on the image in m_sourceViewport
          QList<FeaturePosition> *m_features;
          //! The visible features on the image in m_sourceViewport
          QList<FeatureDisplayPosition> *m_featureScreenAreas;
          /**
           * A check to make sure the cube viewport is in the correct state for
           *   painting. Sometimes (mac laptop ssh'd to linux) paints happen
           *   before handleViewChanged().
           */
          QPair<QPointF, QPointF> *m_viewportCubeRange;
      };

    private:
      //! This is the 'Show Nomenclature' toggleable action in the options menu.
      QPointer<QAction> m_action;

      //! This is the 'Name Features' check box when this tool is active
      QPointer<QCheckBox> m_findNomenclatureCheckBox;
      /**
       * This combo box lists all of the found features and their viewports.
       *   The data stored in this combo box is of format:
       *
       *   Map<QString, QVariant>:
       *     "Viewport" -> MdiCubeViewport*
       *     "Feature"  -> FeatureNomenclature::Feature
       *     "Target"   -> QString(target name, all upper case)
       */
      QPointer<QComboBox> m_foundFeaturesCombo;
      //! This is the 'Center' button in this tool's tool bar
      QPointer<QPushButton> m_nomenclatureCenterBtn;
      //! This is the 'Tool Options' button in this tool's tool bar
      QPointer<QPushButton> m_nomenclatureOptionsBtn;
      //! This is the 'Disclaimer' button in this tool's tool bar
      QPointer<QPushButton> m_disclaimerBtn;
      /**
       * This is a busy indicator that is visible when queries are out to the
       *   nomenclature database.
       */
      QPointer<QProgressBar> m_queryingProgress;

      //! The nomenclature that has been identified, one for each viewport
      QList<ViewportFeatureDisplay> * m_foundNomenclature;
      /**
       * The nomenclature being queried currently, one for each viewport that
       *   has no found nomenclature.
       */
      QMap< MdiCubeViewport *,
            FeatureNomenclature *> * m_nomenclatureSearchers;

      /**
       * Do we find and display nomenclature? This corresponds to the
       *   'Name Features' check box and the 'Show Nomenclature' action in the
       *   options menu.
       */
      bool m_nomenclatureEnabled;

      //! The (HTML) contents of the disclaimer to show the user.
      QString m_disclaimerText;

      //! The font size to use when naming features
      int m_fontSize;
      //! The color to use when drawing on the viewport
      QColor *m_fontColor;
      //! Do we turn ourselves on immediately?
      bool m_defaultEnabled;
      //! Have we ever shown the user our disclaimer?
      bool m_disclaimedAlready;
      //! How we need to draw extents (if at all)
      VectorType m_extentType;
      //! Only show IAU approved features
      bool m_showApprovedOnly;
  };
}

#endif
