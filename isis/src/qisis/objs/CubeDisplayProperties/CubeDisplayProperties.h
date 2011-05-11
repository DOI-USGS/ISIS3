#ifndef CubeDisplayProperties_H
#define CubeDisplayProperties_H

#include <QObject>

// This is required since we're adding to QVariant
#include <QMetaType>

// This is required since QColor is in a slot
#include <QColor>

class QAction;
class QBitArray;

namespace geos {
  namespace geom {
    class MultiPolygon;
  }
}

namespace Isis {
  class Cube;
  class PvlObject;
  class UniversalGroundMap;

  /**
   * @brief This is the GUI communication mechanism for cubes
   *
   * This class is the connector between various GUI interfaces for cubes.
   *   We use this to communicate shared properties that various widgets need
   *   to know/should react to in a generic way.
   *
   * This is how this class is supposed to "connect" widgets:
   *
   *  widgetA         widgetB           widgetC
   *     |               |                 |
   *     ------CubeDisplayProperties -------
   *
   * When a user selects a cube in widgetA, widgetB and widgetC now have a
   *   chance to also select the same cube. This applies to all shared
   *   properties. Some of the properties are actions - such as zoomFit. This
   *   also allows a widget with no zooming (such as a list) to have an option
   *   to zoom (if any of the widgets support it*) and have that option work.
   *   There is no state associated with zoomFit - it's an action connected
   *   to a signal.
   *
   * The proper way to detect a cube going away is to connect to the
   *   destroyed signal (from the parent QObject). Once that is emitted you
   *   cannot call any methods on this object.
   *
   * @author 2011-05-05 Steven Lambright
   *
   * @internal
   *   @history 2011-05-11 Steven Lambright - Added accessors for data that is
   *                       complicated to get or expensive (i.e. Camera
   *                       statistics and the footprint).
   */
  class CubeDisplayProperties : public QObject {
      Q_OBJECT
    public:
      /**
       * This is a list of properties and actions that are possible.
       */
      enum Property {
        //! The color of the cube, default randomized (QColor)
        Color,
        //! The selection state of this cube (bool)
        Selected,
        //! True if the cube should show DN values if possible (bool)
        ShowDNs,
        //! True if the cube should show a fill area if possible (bool)
        ShowFill,
        //! True if the cube should show its display name (bool)
        ShowLabel,
        //! True if the cube should be outlined (bool)
        ShowOutline,
        //! Data ignored. Tells if the cube supports the zoomFit action
        Zooming,
        //! Data ignored. Tells if the cube supports the "move*" actions
        ZOrdering
      };


      CubeDisplayProperties(QString filename, QObject *parent = NULL);
      CubeDisplayProperties(const PvlObject &pvl, QObject *parent = NULL);
      virtual ~CubeDisplayProperties();

      void addSupport(Property prop);
      static bool allSupport(Property prop, QList<CubeDisplayProperties *>);
      bool supports(Property prop);

      QVariant getValue(Property prop) const;

      Cube *cube();
      UniversalGroundMap *groundMap();

      QString displayName() const;

      // Use this only if you actually need the file name
      QString fileName() const {
        return m_filename;
      }

      void closeCube();

      double incidenceAngle() const {
        return m_incidenceAngle;
      }

      double emissionAngle() const {
        return m_emissionAngle;
      }

      double resolution() const {
        return m_resolution;
      }

      geos::geom::MultiPolygon *footprint();

      PvlObject toPvl() const;

      static QList<QAction *> getSupportedDisplayActions(
          QList<CubeDisplayProperties *> cubeDisplays);

      static QList<QAction *> getSupportedZoomActions(
          QList<CubeDisplayProperties *> cubeDisplays);

      static QList<QAction *> getSupportedZOrderActions(
          QList<CubeDisplayProperties *> cubeDisplays);

      static QColor randomColor();

    signals:
      void propertyChanged(CubeDisplayProperties *);
      void supportAdded(Property);

      //! Z Order up one
      void moveUpOne();
      //! Z Order to top
      void moveToTop();

      //! Z Order down one
      void moveDownOne();
      //! Z Order to bottom
      void moveToBottom();

      //! Fit in window
      void zoomFit();

    public slots:
      void setColor(QColor newColor);
      void setShowDNs(bool);
      void setShowFill(bool);
      void setShowLabel(bool);
      void setShowOutline(bool);
      void setSelected(bool);

    private slots:
      void askAlpha();
      void askNewColor();
      void showRandomColor();
      void toggleShowDNs();
      void toggleShowFill();
      void toggleShowLabel();
      void toggleShowOutline();

    protected:

    private:
      CubeDisplayProperties(const CubeDisplayProperties &);
      CubeDisplayProperties &operator=(const CubeDisplayProperties &);

      void setValue(Property prop, QVariant value);
      static QList<Isis::CubeDisplayProperties *> senderToData(QObject *sender);

      void createManualFootprint();

      /**
       * This indicated whether any widgets with this CubeDisplayProperties
       *   is using a particulat property. This helps others who can set
       *   but not display know whether they should give the option to set.
       */
      QBitArray *m_propertyUsed;

      /**
       * This is a map from Property to value -- the reason I use an int is
       *   so Qt knows how to serialize this QMap into binary data
       */
      QMap<int, QVariant> *m_propertyValues;

      /**
       * This is the filename of the input cube
       */
      QString m_filename;

      /**
       * This is an optionally allocated version of the cube; typically present
       *   only during the loading phase.
       */
      Cube *m_cube;

      UniversalGroundMap *m_gMap;
      geos::geom::MultiPolygon *m_footprint;
      double m_incidenceAngle;
      double m_resolution;
      double m_emissionAngle;
  };
}

Q_DECLARE_METATYPE(QList<Isis::CubeDisplayProperties *>);

#endif

