APPNAME = photemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) pvl=$(OUTPUT)/anisotropic.pvl normalization=atmalbedo > /dev/null;
	$(APPNAME) pvl=$(OUTPUT)/hapkeatm.pvl normalization=atmalbedo \
	atmospheric=hapkeatm1 > /dev/null;
	$(APPNAME) pvl=$(OUTPUT)/isotropic.pvl normalization=atmalbedo \
	atmospheric=isotropic1 > /dev/null;
