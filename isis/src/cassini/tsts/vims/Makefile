APP1NAME = vims2isis
APP2NAME = spiceinit
APP3NAME = campt
APP4NAME = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/CM_1477461989_1.cub \
	  VIS= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  IR= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub > /dev/null

	$(APP2NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=32 LINE=7 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=55 LINE=33 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=32 LINE=58 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=4 LINE=33 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  to= $(OUTPUT)/CM_1477461989_1.cub--finalOutput.pvl SAMPLE=32 LINE=33 ALLOWOUTSIDE=no \
	  > /dev/null

	echo -e "\nOutput of getsn..." >> $(OUTPUT)/CM_1477461989_1.cub--getsnOutput.txt && \
	  $(APP4NAME) from= $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub \
	  >> $(OUTPUT)/CM_1477461989_1.cub--getsnOutput.txt

	$(RM) $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-vis.cub > /dev/null
	$(RM) $(OUTPUT)/CM_1477461989_1.cub--vims2isis-spiceinit-ir.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null
