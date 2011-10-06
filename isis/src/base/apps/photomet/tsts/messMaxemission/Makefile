APPNAME = photomet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0031257484M.iof.cub \
	to=$(OUTPUT)/EN0031257484M.phot.cub \
	frompvl=$(INPUT)/pvlfile.txt maxemission=75 \
	normname=albedo phtname=hapkehen incref=0.0 \
	incmat=0.0 thresh=30.0 albedo=1.0 wh=0.5 \
	hh=0.0 > /dev/null;
