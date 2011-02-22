APPNAME = hicubenorm

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ATL_20050526T063349_IR10_0.cub \
	stats= $(OUTPUT)/truth \
	new_version= true statsource= table \
	fromstats= $(INPUT)/cubenorm.stats \
	highpass_mode = highpass_divide \
	filter = 5 \
	pausecrop = true > /dev/null;
