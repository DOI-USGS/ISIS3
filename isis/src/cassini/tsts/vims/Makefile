IMPORT = vims2isis
SPICEINIT = spiceinit shape=ellipsoid
CAMPT = campt
GETSN = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(IMPORT) from= $(INPUT)/CM_1477461989_1.cub \
	  VIS= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  IR= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub > /dev/null

	$(SPICEINIT) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  > /dev/null

	$(CAMPT) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=32 LINE=7 ALLOWOUTSIDE=no \
	  > /dev/null

	$(CAMPT) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=55 LINE=33 ALLOWOUTSIDE=no \
	  > /dev/null

	$(CAMPT) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=32 LINE=58 ALLOWOUTSIDE=no \
	  > /dev/null

	$(CAMPT) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=4 LINE=33 ALLOWOUTSIDE=no \
	  > /dev/null

	$(CAMPT) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=32 LINE=33 ALLOWOUTSIDE=no \
	  > /dev/null

	cat $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{9\}\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/temp.txt

	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl

	$(ECHO) "Output of getsn..." >> $(OUTPUT)/CM_1477461989_1.cub--getsnOutput.txt

	$(GETSN) from=$(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  >> $(OUTPUT)/CM_1477461989_1.cub--getsnOutput.txt

	$(RM) $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub > /dev/null
	$(RM) $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null
