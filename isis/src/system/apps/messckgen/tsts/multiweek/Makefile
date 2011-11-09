APPNAME = messckgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/kernels.0110.db to=$(OUTPUT)/kernels.0111.pvl \
	pivot=$(INPUT)/pivot_kernels.0111.db \
	atthist=$(INPUT)/atthist_kernels.0111.db \
	> /dev/null;
