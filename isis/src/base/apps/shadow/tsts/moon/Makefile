APPNAME = shadow

include $(ISISROOT)/make/isismake.tsts

A15.lowprec.subtracted.cub.TOLERANCE = 0.00001
A15.lowprec2.subtracted.cub.TOLERANCE = 0.00001
A15.noshadow.subtracted.cub.TOLERANCE = 0.00001
# Added tolerance for OS X 10.11   
A15.accurate.shadow.subtracted.cub.TOLERANCE = 0.00001

commands: A15 M1104295651RE

A15:
	$(APPNAME) from=$(INPUT)/A15.crop2.dem.cub to=$(OUTPUT)/A15.accurate.shadow.cub \
	  match=$(INPUT)/M1104295651RE.lev1.cub preset=accurate | grep -v shadow | \
	  grep -v Processed > $(OUTPUT)/A15.accurate.shadow.log.pvl;
	fx f1=$(OUTPUT)/A15.accurate.shadow.cub f2=$(INPUT)/A15.accurate.shadow.cub \
	  equation='f1-f2' to=$(OUTPUT)/A15.accurate.shadow.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/A15.accurate.shadow.cub; \
	$(APPNAME) from=$(INPUT)/A15.crop2.dem.cub to=$(OUTPUT)/A15.lowprec2.cub \
	  sunpositionsource=TIME TIME="2012-10-08T00:33:06.000" preset=custom precision=10 \
	  cacheinterpolatedvalues=true | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/A15.lowprec2.log.pvl; \
	fx f1=$(OUTPUT)/A15.lowprec2.cub f2=$(INPUT)/A15.lowprec2.cub \
	  equation='f1-f2' to=$(OUTPUT)/A15.lowprec2.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/A15.lowprec2.cub; \
	$(APPNAME) from=$(INPUT)/A15.crop2.dem.cub to=$(OUTPUT)/A15.lowprec.cub \
	  sunpositionsource=TIME TIME="2012-10-08T00:33:06.000" preset=custom precision=10 \
	  cacheinterpolatedvalues=true \
	  spk='($$lro/kernels/spk/fdf29r_2012275_2012306_v01.bsp, $$lro/kernels/tspk/de421.bsp, $$lro/kernels/tspk/moon_pa_de421_1900-2050.bpc)' \
	  pck='($$base/kernels/pck/pck00009.tpc, $$lro/kernels/pck/moon_080317.tf, $$lro/kernels/pck/moon_assoc_me.tf)' \
	  | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/A15.lowprec.log.pvl;
	fx f1=$(OUTPUT)/A15.lowprec.cub f2=$(INPUT)/A15.lowprec.cub \
	  equation='f1-f2' to=$(OUTPUT)/A15.lowprec.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/A15.lowprec.cub; \
	$(APPNAME) from=$(INPUT)/A15.crop2.dem.cub to=$(OUTPUT)/A15.shadow.cub \
	  match=$(INPUT)/M1104295651RE.lev1.cub | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/A15.shadow.log.pvl;
	fx f1=$(OUTPUT)/A15.shadow.cub f2=$(INPUT)/A15.shadow.cub \
	  equation='f1-f2' to=$(OUTPUT)/A15.shadow.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/A15.shadow.cub; \
	$(APPNAME) from=$(INPUT)/A15.crop.dem.cub to=$(OUTPUT)/A15.noshadow.cub \
	  match=$(INPUT)/M129269398RE.lev1.cub preset=noshadow | grep -v 'shadow:' | \
	  grep -v Processed > $(OUTPUT)/A15.noshadow.log.pvl;
	fx f1=$(OUTPUT)/A15.noshadow.cub f2=$(INPUT)/A15.noshadow.cub \
	  equation='f1-f2' to=$(OUTPUT)/A15.noshadow.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/A15.noshadow.cub;

# This image ends up full of noise because the DEM is way too low res to get anything useful out of
#   it.
M1104295651RE:
	$(APPNAME) from=$(INPUT)/M1104295651RE.crop.dem.cub \
	  to=$(OUTPUT)/M1104295651RE.accurate.shadow.cub \
	  match=$(INPUT)/M1104295651RE.lev1.cub preset=accurate | \
	  grep -v 'shadow:' | grep -v Processed > $(OUTPUT)/M1104295651RE.accurate.shadow.log.pvl;
	fx f1=$(OUTPUT)/M1104295651RE.accurate.shadow.cub f2=$(INPUT)/M1104295651RE.accurate.shadow.cub \
	  equation='f1-f2' to=$(OUTPUT)/M1104295651RE.accurate.shadow.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/M1104295651RE.accurate.shadow.cub;
	$(APPNAME) from=$(INPUT)/M1104295651RE.crop.dem.cub \
	  to=$(OUTPUT)/M1104295651RE.shadow.cub \
	  match=$(INPUT)/M1104295651RE.lev1.cub | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/M1104295651RE.shadow.log.pvl;
	fx f1=$(OUTPUT)/M1104295651RE.shadow.cub f2=$(INPUT)/M1104295651RE.shadow.cub \
	  equation='f1-f2' to=$(OUTPUT)/M1104295651RE.shadow.subtracted.cub > /dev/null; \
	$(RM) $(OUTPUT)/M1104295651RE.shadow.cub;
