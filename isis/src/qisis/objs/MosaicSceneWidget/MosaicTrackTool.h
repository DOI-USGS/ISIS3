#ifndef MosaicTrackTool_h
#define MosaicTrackTool_h

#include "MosaicTool.h"

// required since it's in a slot
#include <QPointF>

class QLabel;
class QStatusBar;

namespace Isis {
  class MosaicTrackTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicTrackTool(MosaicSceneWidget *, QStatusBar *);
      virtual ~MosaicTrackTool();
      void updateLabels(QPointF p);

    public slots:
      virtual void mouseMove(QPointF p);
      virtual void mouseLeave();

    private slots:
      void labelDestroyed(QObject *);

    private:
      virtual QAction *getPrimaryAction();

      void clearLabels();
      QStatusBar *p_sbar;   //!< Status bar
      QLabel *p_latLabel;  //!< Latitude label
      QLabel *p_lonLabel;  //!< Longitude label
      QLabel *p_xLabel;  //!< Latitude label
      QLabel *p_yLabel;  //!< Longitude label
  };
};

#endif

