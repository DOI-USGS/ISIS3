#include "SpiceClientStarter.h"

#include "SpiceClient.h"


namespace Isis {
  /**
   * This starts the POST request to the server prepared by the
   * SpiceClient. This is necessary because a signal must call
   * sendRequest() in SpiceClient.
   *
   * @param client
   */
  SpiceClientStarter::SpiceClientStarter(SpiceClient &client) {
    connect(this, SIGNAL(startIt()), &client, SLOT(sendRequest()));
  }


  /**
   * Destroys a SpiceClientStarter
   */
  SpiceClientStarter::~SpiceClientStarter() {
  }


  /**
   * Call this to actually start the SpiceClient POST request.
   */
  void SpiceClientStarter::start() {
    emit startIt();
  }
}

