APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	f2=$(INPUT)/peaks.cub+2 \
	to=$(OUTPUT)/max2.cub \
	equation="max(150, max(f1,f2))" > /dev/null;
	$(APPNAME) f1=$(INPUT)/peaks.cub+1 \
	f2=$(INPUT)/peaks.cub+2 \
	to=$(OUTPUT)/min2.cub \
	equation="min(25, min(f1,f2))" > /dev/null;
