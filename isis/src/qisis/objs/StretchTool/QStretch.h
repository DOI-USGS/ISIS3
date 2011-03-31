#ifndef QStretch_h
#define QStretch_h

#include <QGroupBox>
#include <QTableWidget>

#include "Stretch.h"

namespace Qisis {
  /**
   * @brief Abstract class for complex stretch objects
   *
   * This class serves as the parent class to complex stretch objects that
   * are used by the AdvancedStretchTool. It must define parameters to
   * manipulate its stretch pairs that will be used by the AdvancedStretchTool's
   * 'Advanced' dialog.
   *
   * @ingroup Visualization Tools
   *
   * @author 2009-05-01 Noah hilt
   *
   *
   */
  class StretchTool;

  class QStretch : public QObject {
      Q_OBJECT

    public:
      /**
       * QStretch Constructor. StretchTool can not be null.
       *
       * Initializes the stretch and sets its special pixel values
       * to their defaults.
       *
       * @param stretchTool
       * @param name
       */
      QStretch(const StretchTool *stretchTool, QString name)
        : p_stretchTool(stretchTool),
          p_name(name),
          p_min(0.0),
          p_max(255.0),
          p_parametersBox(NULL) {
        p_stretch.SetNull(0.0);
        p_stretch.SetLis(0.0);
        p_stretch.SetLrs(0.0);
        p_stretch.SetHis(255.0);
        p_stretch.SetHrs(255.0);
        p_stretch.SetMinimum(0.0);
        p_stretch.SetMaximum(255.0);
      };


      /**
       * Abstract method to clone a QStretch in order to retain its parameters and
       * stretch pairs. This method must be overridden.
       *
       *
       * @return QStretch*
       */
      virtual QStretch *clone() = 0;

      /**
       * Abstract method to connect the AdvancedStretchTool's table of stretch pairs
       * in order to modify it if necessary. The default implementation is to do
       * nothing.
       *
       * @param widget
       */
      virtual void connectTable(QTableWidget *widget) {};

      /**
       * Abstract method to disconnect the AdvancedStretchTool's table of stretch
       * pairs. The default implementation is to do nothing.
       *
       * @param widget
       */
      virtual void disconnectTable(QTableWidget *widget) {};

      /**
       * Abstract method to return a QGroupBox of this QStretch's parameters. This
       * method must be overridden.
       *
       *
       * @return QGroupBox*
       */
      virtual QGroupBox *getParameters() = 0;

      /**
       * This method returns the name of this QStretch.
       *
       *
       * @return QString
       */
      QString name() {
        return p_name;
      }

      /**
       *
       * This method returns the stretch.
       *
       * @return Isis::Stretch
       */
      Isis::Stretch stretch() {
        return p_stretch;
      }

      /**
       * Abstract method to set the minimum and maximum values that stretch input
       * pairs can be set to.
       *
       * @param min
       * @param max
       */
      virtual void setMinMax(double min, double max) {
        p_min = min;
        p_max = max;
      }

    signals:
      /**
       * Signal to emit when this QStretch has been updated.
       */
      void update();

    protected:
      const StretchTool *p_stretchTool; //!< The StretchTool that created this QStretch
      QString p_name; //!< The name of this QStretch
      double p_min; //!< The minimum value a stretch input pair can be set to
      double p_max; //!< The maximum value a stretch input pair can be set to
      QGroupBox *p_parametersBox; //!< The QGroupBox that holds this QStretch's parameters
      Isis::Stretch p_stretch; //!< The stretch that this QStretch modifies
  };
};

#endif
