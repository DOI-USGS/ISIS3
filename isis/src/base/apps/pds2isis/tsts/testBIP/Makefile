APPNAME = pds2isis

include $(ISISROOT)/make/isismake.tsts

# The output of this test will not look like a usable image
# because the band storage type was changed from BIL to BIP
# for this test.

commands:
	$(APPNAME) from=$(INPUT)/BIPtestData.LBL \
	to=$(OUTPUT)/testBIPtruth.cub+32bit > /dev/null;
	catlab from=$(OUTPUT)/testBIPtruth.cub to=$(OUTPUT)/testBIPtruth.pvl > /dev/null;
