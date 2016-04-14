APP1NAME = clem2isis
APP2NAME = spiceinit
APP3NAME = campt
APP4NAME = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/lna3819k.045 \
	  to= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub > /dev/null

	$(APP2NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
	  to= $(OUTPUT)/lna3819k.045--finalOutput.pvl SAMPLE=2 LINE=2 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
	  to= $(OUTPUT)/lna3819k.045--finalOutput.pvl SAMPLE=254 LINE=2 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
	  to= $(OUTPUT)/lna3819k.045--finalOutput.pvl SAMPLE=254 LINE=254 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
	  to= $(OUTPUT)/lna3819k.045--finalOutput.pvl SAMPLE=2 LINE=254 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
	  to= $(OUTPUT)/lna3819k.045--finalOutput.pvl SAMPLE=128 LINE=128 ALLOWOUTSIDE=no \
	  > /dev/null

	echo -e "\nOutput of getsn..." > $(OUTPUT)/lna3819k.045--getsnOutput.txt && \
          $(APP4NAME) from= $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub \
          >> $(OUTPUT)/lna3819k.045--getsnOutput.txt

	$(RM) $(OUTPUT)/lna3819k.045--clem2isis-spiceinit.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null
