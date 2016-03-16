ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.apps
endif

# Since opencv include path and libs are not in isismake.os
# (so all apps don't link against the large opencv libraries)
# we need to make sure those are added here:
ALLLIBS += $(OPENCVLIBS)

