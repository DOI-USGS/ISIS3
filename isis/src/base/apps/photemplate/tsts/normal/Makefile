APPNAME = photemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) pvl=$(OUTPUT)/albedo.pvl normalization=albedo > /dev/null;
	$(APPNAME) pvl=$(OUTPUT)/mixed.pvl normalization=mixed > /dev/null;
	$(APPNAME) pvl=$(OUTPUT)/moonalbedo.pvl normalization=moonalbedo > /dev/null;
