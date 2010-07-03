#ifndef Qisis_MosaicTrackTool_h
#define Qisis_MosaicTrackTool_h

#include "MosaicTool.h"

class QLabel;
class QStatusBar;

namespace Qisis {
  class MosaicTrackTool : public Qisis::MosaicTool {
    Q_OBJECT

    public:
      MosaicTrackTool (QStatusBar *parent);
      void updateLabels(QPointF p);

    public slots:
      virtual void mouseMove(QPoint p);
      virtual void mouseLeave();
     
    private:
      
      void clearLabels();
      QStatusBar *p_sbar;   //!< Status bar
      QLabel *p_latLabel;  //!< Latitude label
      QLabel *p_lonLabel;  //!< Longitude label
      MosaicWidget *p_parentWidget;
  };
};

#endif

