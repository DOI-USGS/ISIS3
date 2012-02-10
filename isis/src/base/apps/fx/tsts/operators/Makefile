APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	f2=$(INPUT)/peaks.cub+2 \
	to=$(OUTPUT)/max2.cub \
	equation="max(90, max(f1,f2))" > /dev/null;
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	f2=$(INPUT)/peaks.cub+2 \
	to=$(OUTPUT)/min2.cub \
	equation="min(40, min(f1,f2))" > /dev/null;
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	to=$(OUTPUT)/neg.cub \
	equation="neg(f1)" > /dev/null;
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	to=$(OUTPUT)/degs.cub \
	equation="degs(pi)" > /dev/null;
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	to=$(OUTPUT)/rads.cub \
	equation="rads(360.0)" > /dev/null;
