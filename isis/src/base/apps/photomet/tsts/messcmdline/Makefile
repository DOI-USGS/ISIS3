APPNAME = photomet

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/EN0213240738M.lev1.cub \
	to=$(OUTPUT)/EN0213240738M.lev1_phot.cub \
	maxemission=85.0 maxincidence=89.0 phtname=hapkehen \
	theta=6.021327012 wh=0.283104821 hg1=0.246493283 \
	hg2=0.559208947 hh=0.075 b0=2.3 zerob0standard=false \
	normname=albedo incref=30.0 incmat=0.0 thresh=10e30 albedo=1.0 \
	> /dev/null;
