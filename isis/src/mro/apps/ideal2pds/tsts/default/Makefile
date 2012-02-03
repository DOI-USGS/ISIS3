APPNAME = ideal2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_007556_2010_RED4.balance.cropped.jitter.cub \
	to=$(OUTPUT)/truth.cub \
	pds_metadata_file=$(OUTPUT)/truth.txt > /dev/null;
