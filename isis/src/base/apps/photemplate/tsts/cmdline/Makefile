APPNAME = photemplate

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/mars_mdim2.1_blu_hapken_albat_h2.pvl \
	topvl=$(OUTPUT)/hapken_hapkeatm2.pvl phtname=hapkehen theta=30.0 \
	wh=0.52 hg1=0.213 hg2=1.0 hh=0.0 b0=0.0 zerob0standard=false \
	atmname=hapkeatm2 nulneg=no tau=.3 tauref=0 hga=.68 wha=.213 \
	hnorm=.003 > /dev/null;

	$(APPNAME) topvl=$(OUTPUT)/temp.pvl phtname=hapkehen zerob0standard=readfrompvl \
	wh=.25 hh=.075 b0=2.3 theta=7.72 hg1=.25 hg2=.58 > /dev/null;
