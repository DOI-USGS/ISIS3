APPNAME = apollopaninit

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/apolloPanImage.cub $(OUTPUT)/ > /dev/null;
	$(APPNAME) from = $(OUTPUT)/apolloPanImage.cub \
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
	catlab from=$(OUTPUT)/apolloPanImage.cub to=$(OUTPUT)/label.pvl > /dev/null;
	$(RM) $(OUTPUT)/apolloPanImage.cub > /dev/null;
