#ifndef SpecialPixelTool_h
#define SpecialPixelTool_h


// This should be the only include in this file!
#include "Tool.h"

// FIXME: remove this include
#include <QColor>


class QDialog;
class QSettings;
class QToolButton;

namespace Isis {
  class Workspace;
}

namespace Qisis {
  /**
   * @brief Sets the colors for the special pixel values
   *
   * @ingroup Visualization Tools
   *
   * @author  Jeff Anderson -  ???
   *
   * @internal
   *   @history 2008-06-09 Noah Hilt - Added settings to read and
   *            write default colors for special pixel values.
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *            CubeViewport.  Also fixed some include issues.
   */
  class SpecialPixelTool : public Qisis::Tool {
      Q_OBJECT

    public:
      SpecialPixelTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addToPermanent(QToolBar *perm);

    protected:
      //! Returns the menu name.
      QString menuName() const {
        return "&Options";
      };
      void updateTool();
      void setColor(QToolButton *button);
      void readSettings();
      void writeSettings();

    signals:
      //! Emitted when the default colors are reset.
      void setDefaultColors();

    private slots:
      void apply();
      void setNullColor();
      void setLisColor();
      void setLrsColor();
      void setLdsColor();
      void setHisColor();
      void setHrsColor();
      void setHdsColor();
      void setBgColor();
      void defaultBW();
      void defaultColor();

    private:
      QAction *p_action; //!< Special Pixel Tool's action
      QWidget *p_parent; //!< Parent widget
      QWidget *p_spWindow;//!< Window widget
      QDialog *p_dialog; //!< Dialog box
      QColor p_nullDefault;//!< Color chosen for null pixels
      QColor p_lisDefault; //!< Color chosen for low instrument saturation pixels
      QColor p_lrsDefault; //!< Color chosen for Low representation saturation pixels
      QColor p_ldsDefault; //!< Color chosen for pixels
      QColor p_hisDefault; //!< Color chosen for high instrument saturation pixels
      QColor p_hrsDefault; //!< Color chosen for high representation saturation pixels
      QColor p_hdsDefault; //!< Color chosen for pixels
      QColor p_bgDefault; //!< Color chosen for pixels
      QToolButton *p_nullColor;//!< Null Button
      QToolButton *p_lisColor; //!< Low instrument saturation Button
      QToolButton *p_lrsColor; //!< Low representation saturation Button
      QToolButton *p_ldsColor; //!< Low Button
      QToolButton *p_hisColor; //!< High instrument saturation Button
      QToolButton *p_hrsColor; //!< High representation saturation Button
      QToolButton *p_hdsColor; //!< High Button
      QToolButton *p_bgColor; //!< Background color Button
      QSettings *p_settings; //!<< Settings
      bool p_color; //!< Color Mode?


  };
};

#endif

