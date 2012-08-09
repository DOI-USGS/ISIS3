#ifndef MosaicGridToolConfigDialog_h
#define MosaicGridToolConfigDialog_h

#include <QDialog>
#include <QPointer>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;

namespace Isis {
  class MosaicGridTool;

  /**
   * @brief Configure user's settings for the grid tool
   *
   * This dialog enables the user to configure the given grid tool. You can show or hid the
   *     grid, draw an auto grid that is based on the open cubes or the user selected grid
   *     extents, and change the parameters of a custom drawn grid. It allows the user to
   *     select the source of the longitude and latitude ranges (from the map, from the open
   *     cubes, or manually entered). The widgets are enabled/disabled depending on the state
   *     of the tool. There is also an option to 'auto apply' the grid settings, which allows
   *     the user to see live updates of the grid as the parameters are changed. If the tool
   *     and dialog take too long to update the grid, the density will be reduced to increase
   *     the speed with which the grid is drawn. References #604.
   *
   * @author 2012-07-25 Kimberly Oyama and Steven Lambright
   *
   * @internal
   */
  class MosaicGridToolConfigDialog : public QDialog {
      Q_OBJECT

    public:
      MosaicGridToolConfigDialog(MosaicGridTool *tool,
                                   QWidget *parent);
      ~MosaicGridToolConfigDialog();

    public slots:
      void applySettings();
      void applySettings(bool shouldReadSettings);
      void readSettings();
      void refreshWidgetStates();

      void onBaseLatSliderChanged();
      void onBaseLonSliderChanged();
      void onLatIncSliderChanged();
      void onLonIncSliderChanged();
      void onExtentTypeChanged();
      void onMinLatExtentSliderChanged();
      void onMaxLatExtentSliderChanged();
      void onMinLonExtentSliderChanged();
      void onMaxLonExtentSliderChanged();

    private:
      Q_DISABLE_COPY(MosaicGridToolConfigDialog); 
      void refreshWidgetStates(bool canAutoApply);
      
      MosaicGridTool *m_tool; //! The tool we're configuring

      QPointer<QCheckBox> m_showGridCheckBox; //!< True to display grid.
      QPointer<QCheckBox> m_autoGridCheckBox; //!< True if grid properties come form open cubes.

      QPointer<QLabel>    m_baseLatLabel; //!< Label for the base latitude
      QPointer<QLineEdit> m_baseLatLineEdit; //!< Input for base latitude
      QPointer<QSlider>   m_baseLatSlider; //!< Input for base latitude
      QPointer<QLabel>    m_baseLatTypeLabel; //!<  Label for the baselat type (degrees/radians)

      QPointer<QLabel>    m_baseLonLabel; //!<  Label for the base longitude
      QPointer<QLineEdit> m_baseLonLineEdit; //!< Input for base longitude
      QPointer<QSlider>   m_baseLonSlider; //!< Input for base longitude
      QPointer<QLabel>    m_baseLonTypeLabel; //!<  Label for the baselon type (degrees/radians)
      
      QPointer<QLabel>    m_latIncLabel; //!< Label for the latitude increment
      QPointer<QLineEdit> m_latIncLineEdit; //!< Input for latitude increment
      QPointer<QSlider>   m_latIncSlider; //!< Input for latitude increment
      QPointer<QLabel>    m_latIncTypeLabel; //!< Label for the increment type (degrees/radians)
      
      QPointer<QLabel>    m_lonIncLabel; //!< Label for the longitude increment
      QPointer<QLineEdit> m_lonIncLineEdit; //!< Input for longitude increment
      QPointer<QSlider>   m_lonIncSlider; //!< Input for longitude increment
      QPointer<QLabel>    m_lonIncTypeLabel; //!< Label for the increment type (degrees/radians)

      QPointer<QLabel>    m_latExtentLabel; //!< Label for the latitude range
       //!< Selection for the latitude extent source (Map, Cube, Manual)
      QPointer<QComboBox> m_latExtentCombo;
      QPointer<QLabel>    m_latExtentTypeLabel;
      
      QPointer<QLabel>    m_minLatExtentLabel; //!< Label for the minimum latitude
      QPointer<QLineEdit> m_minLatExtentLineEdit; //!< Input for the minimum latitude
      QPointer<QSlider>   m_minLatExtentSlider; //!< Input for the minimum latitude
      QPointer<QLabel>    m_minLatExtentTypeLabel;
      
      QPointer<QLabel>    m_maxLatExtentLabel; //!< Label for the maximum latitude
      QPointer<QLineEdit> m_maxLatExtentLineEdit; //!< Input for the maximum latitude
      QPointer<QSlider>   m_maxLatExtentSlider; //!< Input for the maximum latitude
      QPointer<QLabel>    m_maxLatExtentTypeLabel;

      QPointer<QLabel>    m_lonExtentLabel; //!< Label for the longitude range
       //!< Selection for the longitude extent source (Map, Cube, Manual)
      QPointer<QComboBox> m_lonExtentCombo;
      QPointer<QLabel>    m_lonDomainLabel; //!< Label for the longitude domain
      
      QPointer<QLabel>    m_minLonExtentLabel; //!< Label for the minimum longitude
      QPointer<QLineEdit> m_minLonExtentLineEdit; //!< Input for the minimum longitude
      QPointer<QSlider>   m_minLonExtentSlider; //!< Input for the minimumlongitude
      QPointer<QLabel>    m_minLonExtentTypeLabel;
      
      QPointer<QLabel>    m_maxLonExtentLabel; //!< Label for the maximum longitude
      QPointer<QLineEdit> m_maxLonExtentLineEdit; //!< Input for the maximum longitude
      QPointer<QSlider>   m_maxLonExtentSlider; //!< Input for the maximum longitude
      QPointer<QLabel>    m_maxLonExtentTypeLabel;

      QPointer<QLabel>    m_densityLabel; //!< Label for the grid density
      QPointer<QLineEdit> m_densityEdit; //!< Input for grid density

      QPointer<QCheckBox> m_autoApplyCheckBox; //!< True to applySettings on state change
  };
}

#endif
