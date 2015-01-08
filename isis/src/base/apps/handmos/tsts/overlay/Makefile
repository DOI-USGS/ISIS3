APPNAME = handmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/tychogrid.cub $(OUTPUT);
	$(APPNAME) from=$(INPUT)/20kmgrid3.cub \
	  mosaic=$(OUTPUT)/tychogrid.cub \
	  priority=ontop \
	  outsample=1001 \
	  outline=1001 \
	  outband=1 \
	  create=no > /dev/null;
