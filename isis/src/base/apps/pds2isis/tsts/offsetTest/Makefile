APPNAME = pds2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ldem_4.lbl \
	to=$(OUTPUT)/ldem_4.cub > /dev/null;
	catlab from=$(OUTPUT)/ldem_4.cub to=$(OUTPUT)/ldem_4truth.pvl > /dev/null;

