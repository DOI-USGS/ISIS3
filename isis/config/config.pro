#########################################################################
#  This qmake file establishes some specific parameters for ISIS build
#  information.  Currently, it is Mac specific but can be easily adapted
#  for Linux systems.
#########################################################################
TEMPLATE = app
CONFIG -= app_bundle

TARGET = $(ISISROOT)/make/isis.conf
QMAKE_CLEAN += $${TARGET}

ISIS_ARCH = $$QMAKE_HOST.arch
isis_config.target = isis_conf
isis_config.commands = @$(DEL_FILE) $${TARGET};

linux-g++-32 {
#   message(Linux_i368)
}

linux-g++-64 {
#   message(Linux_x86_64)
}

macx {
  MAC_ARCH = -arch $$ISIS_ARCH
  ISIS_MACOSX_TARGET = $$QMAKE_MACOSX_DEPLOYMENT_TARGET
  contains (ISIS_MACOSX_TARGET, 10.4) {
    ISIS_MACOSX_TARGET = 10.5
  }
  else {
    MAC_XARCH = -Xarch_$$ISIS_ARCH
  }
 
  MAC_OS_MIN = -mmacosx-version-min=$$ISIS_MACOSX_TARGET
  QTDEFINES =  $(DEFINES)
  ISIS_CXXFLAGS = $$MAC_ARCH $$MAC_XARCH $$MAC_OS_MIN $$QTDEFINES

  isis_config.commands += echo "MAC_ARCH=$${MAC_ARCH}" > $$TARGET; \
                          echo "MAC_XARCH=$${MAC_XARCH}" >> $$TARGET; \
                          echo "MAC_OS_MIN=$${MAC_OS_MIN}" >> $$TARGET; \
                          echo "QTDEFINES=$${QTDEFINES}" >> $$TARGET; \
                          echo "ISIS_ARCH=$$ISIS_ARCH" >> $$TARGET; \
                          echo "ISIS_MACOSX_TARGET=$$ISIS_MACOSX_TARGET" >> $$TARGET; \
                          echo "ISIS_CXXFLAGS=$$ISIS_CXXFLAGS" >> $$TARGET; \
                          echo "ISIS_LFLAGS=$(LFLAGS)" >> $$TARGET
}

QMAKE_EXTRA_TARGETS  += isis_config

