QtWebAppLib is a library to develop server-side web applications in C++.
It requires the Qt SDK version 4.7.0 or newer.

There are three demo applications that demonstrate how to use the library.

Demo1 shows how to use the library by including the source code into your
project. This does not depend on the shared library.

Demo2 shows how to link against the shared library.
Build the project QtWebApp to generate the shared library.

Demo3 shows how to use the qtservice component to start the application
as a Windows Service or Unix daemon.

    On Windows:
    Run "Demo3 -i" once to install the  Windows service. After that you
    will see a related entry in the system control service manager.
    Run Demo3 without command-line option to start the service.
    Run "Demo3 -t" to terminate the service.

    On Unix:
    Run Demo3 without command-line option to start the program in background as a daemon.
    Run "Demo3 -t" to terminate the daemon.

    On all operating system, -e executes it as a regular console
    application, or use -h to get help.

I recommend to include the library by source as shown in Demo1 and 3.

The API documentation on http://stefanfrings.de/qtwebapp/api/index.html has been
generated with Doxygen.

Please read also the other documents in the QtWebApp/doc folder.

If you need WebSockets then check this fork of my project: https://github.com/samiavasil/QtWebApp

Stefan Frings
http://stefanfrings.de

