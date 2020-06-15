APPNAME = photomet

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/cropped.cub \
	to=$(OUTPUT)/angleSourceDemUsedemFalse.cub \
	maxemission=87.0 \
	anglesource=dem \
	phtname=minnaert \
	k=0.5 \
	normname=mixed \
	incref=0 \
	incmat=80 \
	thresh=30 \
	albedo=1.0 > $(OUTPUT)/test_mix_DEM.pvl;
	$(APPNAME) from=$(INPUT)/cropped.cub \
  to=$(OUTPUT)/angleSourceDemUsedemFalse.cub \
  maxemission=87.0 \
  anglesource=dem \
  phtname=minnaert \
  k=0.5 \
  normname=topo \
  incref=0 \
  thresh=30 \
  albedo=1.0 > $(OUTPUT)/test_topo_DEM.pvl;

	$(RM) $(OUTPUT)/angleSourceDemUsedemFalse.cub ;
