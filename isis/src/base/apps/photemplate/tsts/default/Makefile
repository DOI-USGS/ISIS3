APPNAME = photemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) topvl=$(OUTPUT)/default.pvl phtname=lambert \
	atmname=isotropic2 tau=.28 tauref=0.0 wha=.95 hnorm=.003 \
	nulneg=yes > /dev/null;
