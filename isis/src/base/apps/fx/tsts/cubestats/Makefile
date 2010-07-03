APPNAME = fx

include $(ISISROOT)/make/isismake.tsts
commands:
	$(APPNAME) f1=$(INPUT)/in1.cub \
	f2=$(INPUT)/in2.cub \
	to=$(OUTPUT)/result.cub \
	equation="cubemax(f1)-cubemin(f2)+linemin(f1)-linemin(f2)" \
	  > /dev/null; \
	$(APPNAME) f1=$(INPUT)/in1.cub \
	f2=$(INPUT)/in2.cub \
	to=$(OUTPUT)/result.cub \
	equation="max(f1,f2)+min(f1,f2)" > /dev/null;
