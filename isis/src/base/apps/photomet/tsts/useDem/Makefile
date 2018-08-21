APPNAME = photomet

angleSourceDemUsedemFalse.cub.TOLERANCE = 0.001
angleSourceDemUsedemTrue.cub.TOLERANCE = 0.001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/cropped.cub \
	to=$(OUTPUT)/angleSourceDemUsedemFalse.cub \
	maxemission=87.0 \
	anglesource=dem \
	phtname=minnaert \
	k=0.5 \
	normname=albedo \
	incref=0 \
	incmat=80 \
	thresh=30 \
	albedo=1.0 > /dev/null;
	$(APPNAME) from=$(INPUT)/cropped.cub \
	to=$(OUTPUT)/angleSourceEllipsoidUsedemFalse.cub \
	maxemission=87.0 \
	phtname=minnaert \
	k=0.5 \
	normname=albedo \
	incref=0 \
	incmat=80 \
	thresh=30 \
	albedo=1.0 > /dev/null;
	$(APPNAME) from=$(INPUT)/cropped.cub \
	to=$(OUTPUT)/angleSourceEllipsoidUsedemTrue.cub \
	maxemission=87.0 \
	usedem=yes \
	phtname=minnaert \
	k=0.5 \
	normname=albedo \
	incref=0 \
	incmat=80 \
	thresh=30 \
	albedo=1.0 > /dev/null;
	$(APPNAME) from=$(INPUT)/cropped.cub \
	to=$(OUTPUT)/angleSourceDemUsedemTrue.cub \
	maxemission=87.0 \
	anglesource=dem \
	usedem=yes \
	phtname=minnaert \
	k=0.5 \
	normname=albedo \
	incref=0 \
	incmat=80 \
	thresh=30 \
	albedo=1.0 > /dev/null;
	
