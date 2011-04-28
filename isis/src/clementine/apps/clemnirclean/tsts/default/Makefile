APPNAME = clemnirclean

lne4885r.truth.cub.TOLERANCE = 0.0000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/lne4885r.cal.cub \
	TO=$(OUTPUT)/lne4885r.truth.cub remove=yes \
	> /dev/null;
