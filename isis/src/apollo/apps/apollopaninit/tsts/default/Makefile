APPNAME = apollopaninit

apolloPanImageOutput.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/apolloPanImage.cub $(OUTPUT)/apolloPanImageOutput.cub> /dev/null;
	$(APPNAME) from = $(OUTPUT)/apolloPanImageOutput.cub \
		Mission = "APOLLO15" \
		Microns = 50 \
		GMT = "1971-08-04 16:28:25.829" \
		LON_NADIR = 5.4338492 \
		LAT_NADIR = 18.2524543 \
		CRAFT_ALTITUDE = 116.9061584 \
		LON_INT = 6.2666336 \
		LAT_INT = 17.953633 \
		VEL_AZM = 292.6602745 \
		VEL_HORIZ = 1.6212918 \
		VEL_RADIAL = -0.0056892 > /dev/null;
	catlab from=$(OUTPUT)/apolloPanImageOutput.cub to=$(OUTPUT)/label.pvl > /dev/null;
	tabledump from=$(OUTPUT)/apolloPanImageOutput.cub to=$(OUTPUT)/BodyRotationTable.csv name=BodyRotation > /dev/null;
	tabledump from=$(OUTPUT)/apolloPanImageOutput.cub to=$(OUTPUT)/SunPositionTable.csv name=SunPosition > /dev/null;
	tabledump from=$(OUTPUT)/apolloPanImageOutput.cub to=$(OUTPUT)/InstrumentPositionTable.csv name=InstrumentPosition > /dev/null;
	tabledump from=$(OUTPUT)/apolloPanImageOutput.cub to=$(OUTPUT)/InstrumentPointingTable.csv name=InstrumentPointing > /dev/null;
	tabledump from=$(OUTPUT)/apolloPanImageOutput.cub to=$(OUTPUT)/FiducialMeasurementTable.csv name="Fiducial Measurement" > /dev/null;
	rm $(OUTPUT)/apolloPanImageOutput.cub > /dev/null;
