# history - Added to test the instrument option. 
APPNAME = voy2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/c2061155.imq \
        to=$(OUTPUT)/c2061155.cub instrument=nac > /dev/null;
        
	catlab from=$(OUTPUT)/c2061155.cub \
        to=$(OUTPUT)/c2061155.pvl > /dev/null;

	> /dev/null;
