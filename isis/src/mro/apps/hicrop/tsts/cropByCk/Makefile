# This test will test cropping cubes using the user-entered CK file.
# The first run will use a cropped ck file that does not cover the entire image.
# The second run will use an original ck file that spans a larger time interval
# than the image.
#
# This test also covers using the the latest versions of the sclk and lsk files. 
# These files are not expected to change the cube output. It is possible that 
# when the highest versions change, it may alter the values for the StartTime, 
# StopTime, SpacecraftClockStartCount, and SpacecraftClockStopCount in the 
# output labels.
APPNAME = hicrop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	to=$(OUTPUT)/cropped1.cub \
	source=ck >& $(OUTPUT)/resultsLog1.txt;
	catlab from=$(OUTPUT)/cropped1.cub to=$(OUTPUT)/croppedLabel1.pvl;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519.bc \
	to=$(OUTPUT)/cropped2.cub \
	source=ck >& $(OUTPUT)/resultsLog2.txt;
	catlab from=$(OUTPUT)/cropped2.cub to=$(OUTPUT)/croppedLabel2.pvl;
