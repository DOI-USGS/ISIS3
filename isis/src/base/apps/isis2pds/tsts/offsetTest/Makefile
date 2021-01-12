APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ldem_4.cub \
	to=$(OUTPUT)/ldem_4_isis2pds.img > /dev/null;
	catlab from=$(OUTPUT)/ldem_4_isis2pds.img to=$(OUTPUT)/ldem_4truth.pvl > /dev/null;
	$(RM) $(OUTPUT)/ldem_4_isis2pds.img > /dev/null;

