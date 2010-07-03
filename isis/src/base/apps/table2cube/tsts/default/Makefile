APPNAME = table2cube

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/AEB_000001_0000_RED0_1.cub \
	 to=$(OUTPUT)/table.truth.cub tablename="HiRISE Calibration Ancillary" \
	 field=DarkPixels \
	> /dev/null;
