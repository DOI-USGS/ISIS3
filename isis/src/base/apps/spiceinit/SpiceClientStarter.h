#ifndef SpiceClientStarter_H
#define SpiceClientStarter_H

#include <QObject>

namespace Isis {
  class SpiceClient;

  class SpiceClientStarter : public QObject {
      Q_OBJECT

    public:
      SpiceClientStarter(SpiceClient &client);
      ~SpiceClientStarter();

      void start();

    signals:
      //! Signal sent to SpiceClient telling it to start
      void startIt();
  };
}

#endif