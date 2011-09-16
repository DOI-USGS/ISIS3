APPNAME = photemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) topvl=$(OUTPUT)/anisotropic.pvl phtname=minnaert \
	k=1.0 atmname=anisotropic1 tau=.28 tauref=0.0 wha=.95 \
        bha=.85 hnorm=.003 > /dev/null;
	$(APPNAME) topvl=$(OUTPUT)/hapkeatm.pvl phtname=minnaert \
	k=1.0 atmname=hapkeatm1 tau=.28 tauref=0.0 wha=.95 \
	hga=.85 hnorm=.003 > /dev/null;
	$(APPNAME) topvl=$(OUTPUT)/isotropic.pvl phtname=minnaert \
	k=1.0 atmname=isotropic1 tau=.28 tauref=0.0 wha=.95 \
	hnorm=.003 > /dev/null;
