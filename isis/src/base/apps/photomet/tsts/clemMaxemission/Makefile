APPNAME = photomet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lub2303j.191.lev1.cub \
	to=$(OUTPUT)/lub2303j.191.lev1.phot.cub \
	frompvl=$(INPUT)/clem.pvl maxemission=4.0 \
	normname=moonalbedo phtname=lunarlambertmcewen \
	xmul=1.0 wl=1.0 xb1=-.0817 xb2=.0081 > /dev/null;
