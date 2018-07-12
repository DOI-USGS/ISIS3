ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.objs
endif

# Since opencv include path and libs are not in isismake.os
# we need to make sure those are added here:
ALLLIBS += $(OPENCVLIBS)