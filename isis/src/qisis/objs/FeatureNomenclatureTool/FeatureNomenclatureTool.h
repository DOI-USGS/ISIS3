#ifndef FeatureNomenclatureTool_h
#define FeatureNomenclatureTool_h

#include "Tool.h"

#include <QPointer>

#include "FeatureNomenclature.h"

class QCheckBox;
class QComboBox;
class QDialog;
class QLabel;
class QLineEdit;
class QMenu;
class QProgressBar;
class QPushButton;

template <typename A, typename B> class QMap;
template <typename A, typename B> class QPair;


namespace Isis {
  class iString;
  class MdiCubeViewport;

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
   */
  class FeatureNomenclatureTool : public Tool {
      Q_OBJECT

    public:
      FeatureNomenclatureTool(QWidget *parent);
      ~FeatureNomenclatureTool();

      void addTo(QMenu *menu);
      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

      bool defaultEnabled() const;
      QColor fontColor() const;
      int fontSize() const;
      bool showVectors() const;

      void setDefaultEnabled(bool defaultEnabled);
      void setFontColor(QColor color);
      void setFontSize(int newFontSize);
      void setShowVectors(bool show);

    protected:
      QString menuName() const;

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
       */
      class FeaturePosition {
        public:
          FeaturePosition();
          FeaturePosition(MdiCubeViewport *, FeatureNomenclature::Feature);
          FeaturePosition(const FeaturePosition &other);
          ~FeaturePosition();

          bool isValid() const;

          QPair<double, double> center() const;
          QList< QPair<double, double> > edges() const;
          FeatureNomenclature::Feature &feature();
          const FeatureNomenclature::Feature &feature() const;

          void swap(FeaturePosition &other);
          FeaturePosition &operator=(const FeaturePosition &rhs);

        private:
          //! The cube line position of the feature center, Null if !isValid()
          double m_centerLine;
          //! The cube sample position of the feature center, Null if !isValid()
          double m_centerSample;

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
       */
      class ViewportFeatureDisplay {
        public:
          ViewportFeatureDisplay();
          ViewportFeatureDisplay(FeatureNomenclatureTool *tool,
              MdiCubeViewport *sourceViewport,
              QList<FeatureNomenclature::Feature> features);
          ViewportFeatureDisplay(const ViewportFeatureDisplay &other);
          ~ViewportFeatureDisplay();

          void centerFeature(FeatureNomenclature::Feature);
          QList<FeatureNomenclature::Feature> features();
          MdiCubeViewport *sourceViewport() const;
          void paint(QPainter *painter, bool showVectors) const;

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
      //! Display vectors to feature extents?
      bool m_showVectors;
  };
}

#endif
