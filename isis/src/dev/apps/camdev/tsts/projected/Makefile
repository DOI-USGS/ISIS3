APPNAME = camdev

# 1.52587890625e-05
camdev_projected_bands.cub.TOLERANCE = .0000153

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/camdevProjectedInput.cub \
	  to=$(OUTPUT)/camdev_projected_bands.cub \
	  source=projection \
	  dn=yes \
	  radec=yes \
	  positiveeast360longitude=yes \
	  pixelresolution=yes \
	  spacecraftposition=yes \
	  distortedfocalplane=yes \
	  undistortedfocalplane=yes \
	  ephemeristime=yes \
	  utc=yes > /dev/null;
	  $(RM) print.prt > /dev/null;