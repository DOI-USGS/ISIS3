#ifndef MosaicFileListWidgetTester_H
#define MosaicFileListWidgetTester_H

#include <QtTestGui>

namespace Isis {
  /**
   * @author 2012-??-?? Steven Lambright
   *
   * @internal
   */
  class MosaicFileListWidgetTester : public QObject {
    Q_OBJECT

    private slots:
      void initTestCase();
      void testBasicFunctionality();
      void testSynchronization();

  };
}

#endif
