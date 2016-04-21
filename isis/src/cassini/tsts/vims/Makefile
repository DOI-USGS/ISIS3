# 2014-04-09 Tracie Sucharski - Only the vis was being tested.  Since the vis is not used, I 
#               changed it to ir.  Added more tests to test the vims camera model on images
#               containing the pole.  Test vims camera model using cam2map to project larger areas
#               to insure no extra pixels appear in the projected output.    

IMPORT = vims2isis
SPICEINIT = spiceinit shape=ellipsoid
CAMPT = campt
CAM2MAP = cam2map
GETSN = getsn

C1514302573_1.irBand71.sinu.cub.TOLERANCE = 0.0000001 
 
include $(ISISROOT)/make/isismake.tsts

commands:
	$(IMPORT) $(TSTARGS) from= $(INPUT)/CM_1477461989_1.cub \
          VIS= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          IR= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub > /dev/null

#  Test IR
	$(SPICEINIT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
	  PCK=$(ISIS3DATA)/cassini/kernels/pck/cpck24Jul2006.tpc \
	  > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl SAMPLE=35 LINE=8 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl SAMPLE=55 LINE=33 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl SAMPLE=32 LINE=58 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl SAMPLE=4 LINE=33 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl SAMPLE=32 LINE=33 ALLOWOUTSIDE=no \
          > /dev/null

	cat $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl | \
          sed 's/\([0-9]*\.[0-9]\{9\}\)\([0-9]*\)/\1/g' \
          > $(OUTPUT)/temp.txt

	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/CM_1477461989_1.cub--finalOutput-ir.pvl

#  Test Vis
	$(SPICEINIT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          PCK=$(ISIS3DATA)/cassini/kernels/pck/cpck24Jul2006.tpc \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl SAMPLE=35 LINE=8 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl SAMPLE=55 LINE=33 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl SAMPLE=32 LINE=58 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl SAMPLE=4 LINE=33 ALLOWOUTSIDE=no \
          > /dev/null

	$(CAMPT) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
          to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl SAMPLE=32 LINE=33 ALLOWOUTSIDE=no \
          > /dev/null

	cat $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl | \
          sed 's/\([0-9]*\.[0-9]\{9\}\)\([0-9]*\)/\1/g' \
          > $(OUTPUT)/temp.txt

	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/CM_1477461989_1.cub--finalOutput-vis.pvl



	$(ECHO) "Output of getsn..." >> $(OUTPUT)/CM_1477461989_1.cub--getsnOutput.txt

	$(GETSN) $(TSTARGS) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub \
          >> $(OUTPUT)/CM_1477461989_1.cub--getsnOutput.txt

	$(RM) $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub > /dev/null
	$(RM) $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null

#  Test south pole
	$(CAMPT) $(TSTARGS) from=$(INPUT)/C1596102183_1.irBand71.cub type=ground lat=-90 lon=0 \
          to=$(OUTPUT)/C1596102183_1.ir.pvl > /dev/null

#  Make sure there are no extra pixels in projected cube.  Global projections were getting ghost
#  images because the vims camera model finds solution for any lat/lon and the range checking 
#  was not working correctly.
	$(CAM2MAP) $(TSTARGS) from=$(INPUT)/C1514302573_1.irBand71.cub to=$(OUTPUT)/C1514302573_1.irBand71.sinu.cub \
          map='$$base/templates/maps/sinusoidal.map' \
          pixres=ppd resolution=2 defaultrange=map minlat=-90 maxlat=90 minlon=0 maxlon=360 \
          trim=yes > /dev/null

