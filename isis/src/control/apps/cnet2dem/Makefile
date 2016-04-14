ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.apps
endif

# Special library for this app
 ALLLIBS += $(NNLIB)
