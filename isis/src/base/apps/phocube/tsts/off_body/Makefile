# Tests RA and declination bands to make sure values are displayed even if not on the planet.
APPNAME = phocube

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EW0131773041G.cal.cub \
	  to=$(OUTPUT)/radec_bands.cub \
	  pixelresolution=true \
	  radec=true > /dev/null; 
	catlab from= $(OUTPUT)/radec_bands.cub \
	  to=$(OUTPUT)/output.pvl > /dev/null;
