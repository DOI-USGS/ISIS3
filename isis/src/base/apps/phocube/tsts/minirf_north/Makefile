APPNAME = phocube

phocubeTruth.cub.TOLERANCE = .0000153

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/LSZ_04866_1CD_XKU_89N109_V1_lev1.cub+1 \
	  to=$(OUTPUT)/phocubeTruth.cub phase=no emission=no incidence=no \
	  latitude=no longitude=no subspacecraftgroundazimuth=yes > /dev/null;
