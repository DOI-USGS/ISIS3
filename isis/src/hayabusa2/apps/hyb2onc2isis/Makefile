ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.apps
endif

# opencv includes path and libs that are not in
# isismake.os, but we don't want all apps to 
# link against the large opencv libraries
# so they are added here:
#
ALLLIBS += $(OPENCVLIBS)
