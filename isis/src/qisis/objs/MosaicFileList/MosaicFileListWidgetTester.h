#ifndef MosaicFileListWidgetTester_H
#define MosaicFileListWidgetTester_H

#include <QtTestGui>

namespace Isis {
  class MosaicFileListWidgetTester : public QObject {
    Q_OBJECT

    private slots:
      void initTestCase();
      void testSimpleInstantiation();
  };
}

#endif
