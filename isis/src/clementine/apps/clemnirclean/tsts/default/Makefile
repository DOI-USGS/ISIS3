APPNAME = clemnirclean

lne4885r.truth.box1.cub.TOLERANCE = 0.0000001
lne4885r.truth.box2.cub.TOLERANCE = 0.0000001
lne4885r.truth.box3.cub.TOLERANCE = 0.0000001
lne4885r.truth.box4.cub.TOLERANCE = 0.0000001
lne4885r.truth.cub.TOLERANCE = 0.0000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/lne4885r.cal.cub \
	TO=$(OUTPUT)/lne4885r.truth.cub remove=no \
	> /dev/null;
	$(MV) ./lne4885r.truth.box1.cub $(OUTPUT)/;
	$(MV) ./lne4885r.truth.box2.cub $(OUTPUT)/;
	$(MV) ./lne4885r.truth.box3.cub $(OUTPUT)/;
	$(MV) ./lne4885r.truth.box4.cub $(OUTPUT)/;
