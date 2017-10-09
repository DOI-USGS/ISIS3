include $(ISISROOT)/make/isismake.os

.PHONY: libs license plugins

all:  install

install: libs license plugins

#libs: license
libs:
	@for library in $(THIRDPARTYLIBS); do                                \
	  $(INSTALL3P) $(INSTALL3POPTS) $$library $(ISISROOT)/3rdParty/lib/; \
	  echo $(CURTIMESTAMP) "  Installing [$$library]";                   \
	  if [ "$(HOST_ARCH)" == "Linux" ]; then                             \
	    for file in $$library; do                                        \
	      localFile=$(ISISROOT)/3rdParty/lib/`basename $$file`;          \
	      if [ ! -L "$$localFile" ]; then                                \
	        existingRpath=`$(PATCHELF) --print-rpath "$$localFile" 2>&- | \
	            cut -d '/' -f2-4`;                                       \
	        dollar='$$';                                                 \
	        newRpath=`echo "$${dollar}ORIGIN"`;                          \
	        if [ "$$existingRpath" == "usgs/pkgs/local" ]; then          \
	          echo $(CURTIMESTAMP) "    Patching [" `basename $$file` "]"; \
	          $(PATCHELF) --set-rpath "$$newRpath" $$localFile;          \
	        fi;                                                          \
	     fi;                                                             \
	   done;                                                             \
	  fi;                                                                \
        done;                                                                \
	for library in $(PATCHLIBS); do                                      \
	  $(INSTALL3P) $(INSTALL3POPTS) $$library $(ISISROOT)/3rdParty/lib/; \
	  echo $(CURTIMESTAMP) "  Installing [$$library]";                   \
	  if [ "$(HOST_ARCH)" == "Linux" ]; then                             \
	    for file in $$library; do                                        \
	      localFile=$(ISISROOT)/3rdParty/lib/`basename $$file`;          \
	      if [ ! -L "$$localFile" ]; then                                \
	        dollar='$$';                                                 \
	        newRpath=`echo "$${dollar}ORIGIN"`;                          \
	        echo $(CURTIMESTAMP) "    Patching [" `basename $$file` "]"; \
	        $(PATCHELF) --set-rpath "$$newRpath" $$localFile;            \
	     fi;                                                             \
	   done;                                                             \
	  fi;                                                                \
        done;                                                                \
	if [ "$(HOST_ARCH)" == "Darwin" ]; then                              \
	  chmod u+w $(ISISROOT)/3rdParty/lib/libcrypto.*.dylib;              \
	  chmod u+w $(ISISROOT)/3rdParty/lib/libssl.*.dylib;                 \
	  chmod u+w $(ISISROOT)/3rdParty/lib/Python.framework/Versions/3.5/Python;\
	  $(ISISROOT)/scripts/SetRunTimePath --libs                          \
	    --libmap=$(ISISROOT)/scripts/darwin_lib_paths.lis                \
	    --liblog=DarwinLibs.lis --update                                 \
	    --relocdir=$(ISISROOT)/3rdParty/lib:$(ISISROOT)/3rdParty         \
	    --errlog=DarwinErrors.lis                                        \
	    `find $(ISISROOT)/3rdParty/lib -name '*.dylib*' -type f`          \
	    > /dev/null;                                                     \
	  $(ISISROOT)/scripts/SetRunTimePath --libs                          \
	     --libmap=$(ISISROOT)/scripts/qt_paths.lis                       \
             --liblog=DarwinLibs.lis                                         \
	     --relocdir=$(ISISROOT)/3rdParty/lib:$ISISROOT/3rdParty          \
             --update                                                        \
	    `find $(ISISROOT)/3rdParty/lib -name '[Qq]*' -print              \
	    -mindepth 3 -maxdepth 4 -type f` > /dev/null;                    \
	  $(ISISROOT)/scripts/SetRunTimePath --bins                          \
	     --libmap=$(ISISROOT)/scripts/darwin_bin_paths.lis               \
             --liblog=DarwinLibs.lis                                         \
	     --relocdir=$(ISISROOT)/3rdParty/lib:$ISISROOT/3rdParty          \
             --update                                                        \
	    `find $(ISISROOT)/3rdParty/lib/Python.framework/Versions/3.5     \
	    -name 'Python' -print -type f` > /dev/null;                      \
	  chmod u-w $(ISISROOT)/3rdParty/lib/libcrypto.*.dylib;              \
	  chmod u-w $(ISISROOT)/3rdParty/lib/libssl.*.dylib;                 \
	  chmod u-w $(ISISROOT)/3rdParty/lib/Python.framework/Versions/3.5/Python;\
	  if [ -f "DarwinErrors.lis" ]; then                                 \
	    cat DarwinErrors.lis;                                            \
	  fi;                                                                \
	  $(RM) DarwinErrors.lis DarwinLibs.lis;                             \
	fi

license:
	$(RSYNC) -a $(THIRDPARTYLICPATH)                                     \
	  $(ISISROOT)/3rdParty/license/

plugins:
	@for plugs in $(THIRDPARTYPLUGINS); do                               \
	  echo $(CURTIMESTAMP) "  Installing [$$plugs...]";                  \
	  $(INSTALL3P) $(INSTALL3POPTS) $$plugs $(ISISROOT)/3rdParty/plugins/; \
	done;                                                                \
	if [ "$(HOST_ARCH)" == "Darwin" ]; then                              \
	  $(ISISROOT)/scripts/SetRunTimePath --bundles                       \
	    --libmap=$(ISISROOT)/scripts/qt_plugins_paths.lis                \
	    --liblog=DarwinLibs.lis --update                                 \
	    --relocdir=$(ISISROOT)/3rdParty/lib:$(ISISROOT)/3rdParty         \
	    --errlog=DarwinErrors.lis                                        \
            `find $(ISISROOT)/3rdParty/plugins -name '*.bundle' -type f`     \
            `find $(ISISROOT)/3rdParty/plugins -name '*.dylib' -type f`      \
            > /dev/null;                                                     \
	  if [ -f "DarwinErrors.lis" ]; then                                 \
	    cat DarwinErrors.lis;                                            \
	  fi;                                                                \
	  $(RM) DarwinErrors.lis DarwinLibs.lis;                             \
	fi


clean:
	rm -f lib/lib*.so* lib/lib*.dylib* lib/lib*.a
	cd lib && rm -rf *.framework
	$(RM) -rf license
	@for plugs in plugins/*; do \
	  if [ -d $$plugs -a $$plugs != "plugins/CVS" ]; \
	  then \
	    $(RM) -rf $$plugs; \
	  fi \
	done;

#----------------------------------------------------------------------------
# Use to see values of variables
#  Example: make print-HOST_OS
#  Will print the make variable HOST_OS
#----------------------------------------------------------------------------
print-%  :
	@echo '$* = $($*)'
