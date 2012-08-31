#ifndef MosaicSceneWidgetTester_H
#define MosaicSceneWidgetTester_H

#include <QtTestGui>

namespace Isis {
  /**
   * @author 2012-??-?? Steven Lambright
   * 
   * @internal
   */
  class MosaicSceneWidgetTester : public QObject {
    Q_OBJECT

    private slots:
      void testBasicFunctionality();
      void testSynchronization();

  };
}

#endif
