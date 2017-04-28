APPNAME = messckgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/messenger_kernels.0004.db \
	to=$(OUTPUT)/messenger_kernels.0005.pvl \
	pivot=$(INPUT)/mdis_kernels.0002.db \
	atthist=$(INPUT)/messenger_mdis_att_kernels.0002.db \
	> /dev/null;
