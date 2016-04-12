APP1NAME = hi2isis
APP2NAME = spiceinit
APP3NAME = campt
APP4NAME = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) $(TSTARGS) from= $(INPUT)/PSP_010186_1785_RED3_1.IMG \
	  to= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub > /dev/null

	$(APP2NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  > /dev/null

	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  to= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--finalOutput.pvl SAMPLE=1 LINE=1 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  to= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--finalOutput.pvl SAMPLE=256 LINE=1 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  to= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--finalOutput.pvl SAMPLE=256 LINE=5000 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  to= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--finalOutput.pvl SAMPLE=1 LINE=5000 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  to= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--finalOutput.pvl SAMPLE=128 LINE=2500 ALLOWOUTSIDE=no \
	  > /dev/null

	echo -e "\nOutput of getsn..." > $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--getsnOutput.txt && \
	  $(APP4NAME) $(TSTARGS) from= $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub \
	  >> $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--getsnOutput.txt

	$(RM) $(OUTPUT)/PSP_010186_1785_RED3_1.IMG--hi2isis-spiceinit.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null
