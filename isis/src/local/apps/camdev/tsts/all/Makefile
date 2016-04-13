APPNAME = camdev

# 1.52587890625e-05
camdevTruth.cub.TOLERANCE = .0000153

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/camdevInput.cub \
	  to=$(OUTPUT)/all_camdev_bands.cub \
	  dn=yes \
	  radec=yes \
	  planetographiclatitude=yes \
	  positiveeast360longitude=yes \
	  positiveeast180longitude=yes \
	  positivewest360longitude=yes \
	  positivewest180longitude=yes \
	  bodyfixed=yes \
	  localradius=yes \
	  pixelresolution=yes \
	  lineresolution=yes \
	  sampleresolution=yes \
	  detectorresolution=yes \
	  spacecraftposition=yes \
	  spacecraftazimuth=yes \
	  slantdistance=yes \
	  targetcenterdistance=yes \
	  subspacecraftlatitude=yes \
	  subspacecraftlongitude=yes \
	  spacecraftaltitude=yes \
	  offnadirangle=yes \
	  subspacecraftgroundazimuth=yes \
	  sunposition=yes \
	  sunazimuth=yes \
	  solardistance=yes \
	  subsolarlatitude=yes \
	  subsolarlongitude=yes \
	  subsolargroundazimuth=yes \
	  phase=yes \
	  emission=yes \
	  incidence=yes \
	  localemission=yes \
	  localincidence=yes \
	  northazimuth=yes \
	  distortedfocalplane=yes \
	  undistortedfocalplane=yes \
	  ephemeristime=yes \
	  utc=yes \
	  localsolartime=yes \
	  solarlongitude=yes \
	  morphology=yes \
	  albedo=yes> /dev/null; 