APPNAME = hicubenorm

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/PSP_001331_2260_RED5_0.cub \
	to= $(OUTPUT)/PSP_001331_2260_RED5_0.avgTrue.cub \
	normalizer = average \
	mode = subtract \
	filter = 5 \
	preserve = false \
	pausecrop = true > /dev/null;
