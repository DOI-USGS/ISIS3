APPNAME = demwrap

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/ulcn2005_lpo.cub \
	  top=2 bottom=1 left=3 right=4 TO=$(OUTPUT)/ulcn2005_lpo.cub \
	  > /dev/null;
	catlab FROM=$(OUTPUT)/ulcn2005_lpo.cub > $(OUTPUT)/ulcn2005_lpo.pvl
