APPNAME = fx

include $(ISISROOT)/make/isismake.tsts
commands:
	$(APPNAME) f1=$(INPUT)/in1.cub \
	f2=$(INPUT)/in2.cub \
	to=$(OUTPUT)/result1.cub \
	equation="cubemax(f1)-cubemin(f2)+linemin(f1)-linemin(f2)" \
	  > /dev/null; \
	$(APPNAME) f1=$(INPUT)/in1.cub \
	f2=$(INPUT)/in2.cub \
	to=$(OUTPUT)/result2.cub \
	equation="max(f1,f2)+min(f1,f2)" > /dev/null;
	$(APPNAME) f1=$(INPUT)/in1.cub \
	to=$(OUTPUT)/result3.cub \
	equation="cubeavg(f1)" > /dev/null;
	$(APPNAME) f1=$(INPUT)/in1.cub \
	to=$(OUTPUT)/result4.cub \
	equation="cubestd(f1)" > /dev/null;
