APPNAME = phocube

# 1.52587890625e-05
phocubeTruth.cub.TOLERANCE = .0000153

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/ab102401.cropped.cub \
	  to=$(OUTPUT)/all_phocube_bands.cub \
	  dn=true \
	  phase=true \
	  emission=true \
	  incidence=true \
	  localemission=true \
	  localincidence=true \
	  latitude=true \
	  longitude=true \
	  pixelresolution=true \
	  lineresolution=true \
	  sampleresolution=true \
	  detectorresolution=true \
	  northazimuth=true \
	  sunazimuth=true \
	  spacecraftazimuth=true \
	  offnadirangle=true \
	  subspacecraftgroundazimuth=true \
	  subsolargroundazimuth=true \
	  morphology=true \
	  albedo=true \
	  radec=true \
	  bodyfixed=true> /dev/null;  
