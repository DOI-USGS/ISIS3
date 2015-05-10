ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.apps
endif

#----------------------------------------------------------------------------
# Use to see values of variables
#  Example: make print-HOST_OS
#  Will print the make variable HOST_OS
#----------------------------------------------------------------------------
print-%  :
	@echo '$* = $($*)'
