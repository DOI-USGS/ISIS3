APPNAME = tgocassis2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/MY36_015782_024_0_PAN.xml \
	           to=$(OUTPUT)/MY36_015782_024_0_PAN.cub > /dev/null;

	catlab from=$(OUTPUT)/MY36_015782_024_0_PAN.cub \
	       to=$(OUTPUT)/MY36_015782_024_0_PAN.lbl > /dev/null;
