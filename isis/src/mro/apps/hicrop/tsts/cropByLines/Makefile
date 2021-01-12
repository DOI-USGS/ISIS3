# This test will test cropping cubes using user-entered line values.
# The first run tests user values within the input cube.
# The second run tests end line value greater than the input cube line count.
APPNAME = hicrop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped1.cub \
	source=linevalues \
        line=10 \
        nlines=30 >& $(OUTPUT)/resultsLog1.txt;
	catlab from=$(OUTPUT)/cropped1.cub to=$(OUTPUT)/croppedLabel1.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped2.cub \
	source=linevalues \
        line=79990 \
        nlines=30 >& $(OUTPUT)/resultsLog2.txt;
	catlab from=$(OUTPUT)/cropped2.cub to=$(OUTPUT)/croppedLabel2.pvl > /dev/null;
