APPNAME = clemnirclean

lne4885r.cal.box1_100000.cub.TOLERANCE = 0.0000001
lne4885r.cal.box2_100000.cub.TOLERANCE = 0.0000001
lne4885r.cal.box3_100000.cub.TOLERANCE = 0.0000001
lne4885r.cal.box4_100000.cub.TOLERANCE = 0.0000001
lne4885r.truth.cub.TOLERANCE = 0.0000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/lne4885r.cal.cub \
	TO=$(OUTPUT)/lne4885r.truth.cub remove=no\
	> /dev/null;
