APPNAME = messckgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/kernels.0109.db to=$(OUTPUT)/kernels.0110.txt \
	pivot=$(INPUT)/pivot_kernels.0110.db \
	atthist=$(INPUT)/atthist_kernels.0110.db \
	> /dev/null;
