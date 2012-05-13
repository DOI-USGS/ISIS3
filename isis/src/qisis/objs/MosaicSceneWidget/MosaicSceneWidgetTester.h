#ifndef MosaicSceneWidgetTester_H
#define MosaicSceneWidgetTester_H

#include <QtTestGui>

namespace Isis {
  class MosaicSceneWidgetTester : public QObject {
    Q_OBJECT

    private slots:
      void testBasicFunctionality();
      void testSynchronization();

  };
}

#endif
