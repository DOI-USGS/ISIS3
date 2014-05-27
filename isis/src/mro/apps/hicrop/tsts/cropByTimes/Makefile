# This test will test cropping cubes using user-entered start/stop times.
APPNAME = hicrop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped.cub \
	source=time \
        start=264289110.0 \
        stop=264289111.0 >& $(OUTPUT)/resultsLog.txt;
	catlab from=$(OUTPUT)/cropped.cub to=$(OUTPUT)/croppedLabel.pvl > /dev/null;
