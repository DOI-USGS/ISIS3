ifeq ($(ISISROOT), $(BLANK))
.SILENT:
error:
	echo "Please set ISISROOT";
else
	include $(ISISROOT)/make/isismake.objs
endif

#ISISCPPFLAGS += $(MAC_ARCH_FLAGS)