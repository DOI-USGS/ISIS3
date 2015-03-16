APPNAME = dsk2isis

itokawa.truth.cub.TOLERANCE = 0.00002

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/hay_a_amica_5_itokawashape_v1_0_512q.bds \
	  map=$(INPUT)/equi.map \
	  to=$(OUTPUT)/itokawa.truth.cub \
	  method=GRID \
	  > /dev/null;
