#include "SocketThread.h"
#include "iException.h"
#include "Application.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <sys/wait.h>
#include <fcntl.h>

#include <QObject>
#include <QThread>

namespace Qisis {

  /**
   * Constructor for the SocketThread
   * 
   * 
   * @param parent 
   */
  SocketThread::SocketThread(QObject *parent): QThread(parent){
  }

  //! Destroys the SocketThread object
  SocketThread::~SocketThread(){
       // wait();
  }

  //! Starts the socket thread
  void SocketThread::run() {
    std::string p_socketFile = "/tmp/isis_qview_" + Isis::Application::UserName(); 
    struct sockaddr_un p_socketName;
    p_socketName.sun_family = AF_UNIX;
    strcpy(p_socketName.sun_path,p_socketFile.c_str());
    int p_socket;

    // Create a socket
    if ((p_socket = socket(PF_UNIX,SOCK_STREAM,0)) < 0) {
      std::string msg = "Unable to create socket";
      std::cerr << msg << std::endl;
      return;
    }

    //const long unsigned int timeout = 1;
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    if(setsockopt(p_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) != 0) {
      std::string msg = "Unable set socket timeout";
      std::cerr << msg << std::endl;
      return;
    }

    // Bind the file to the socket
    int status =  bind(p_socket,(struct sockaddr *)&p_socketName, sizeof(p_socketName));
    if (status < 0) {
      std::string msg = "Unable to bind to socket [" + p_socketFile + "]";
      std::cerr << msg << std::endl;
      return;
    }

    // Set up to listen to the socket
    if (listen(p_socket,5) < 0) {
      std::string msg = "Unable to listen to socket [" + p_socketFile + "]";
      std::cerr << msg << std::endl;
      return;
    }

    p_done = false;
    
    while(!p_done) {  
      // Accept Socket
      socklen_t len;
      int childSocket = accept(p_socket, (struct sockaddr *)&p_socketName, &len);
      if (childSocket < 0) {
        continue; // probably timed out, we cant do anything about this anyways
      }

      // Receive Data
      int bytes;
      char buf[1024*1024];
      if ((bytes = recv(childSocket,&buf,1024*1024,0)) < 0) {
        std::string msg = "Unable to read from socket [" + p_socketFile + "]";
        std::cerr << msg << std::endl;
        return;
      }

      // Push everything onto our string buffer
      Isis::iString buffer;
      for (int i=0; i<bytes; i++) buffer += buf[i];
      while (buffer.size() > 0) {
        Isis::iString token = buffer.Token(" ");
        if (token == "raise") {
          emit focusApp();
        }
        else emit newImage(token.c_str());
      }

    };
  }
}
