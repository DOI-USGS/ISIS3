APPNAME = maplab

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/maplabInput2.cub $(OUTPUT)/maplabOutput21.cub
	$(APPNAME) from= $(OUTPUT)/maplabOutput21.cub \
	  map = $(INPUT)/mapFile.map \
	  x =  -3000.0 \
	  y =  201.0 \
	  line = 23.0 \
	  sample = 85.0;
	catlab from=$(OUTPUT)/maplabOutput21.cub to=$(OUTPUT)/tempLab.txt > /dev/null;
	head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth21.pvl;
	rm $(OUTPUT)/tempLab.txt;
	cp $(INPUT)/maplabInput2.cub $(OUTPUT)/maplabOutput22.cub
	  $(APPNAME) from= $(OUTPUT)/maplabOutput22.cub \
	  map = $(INPUT)/mapFile.map \
	  c = latlon \
	  lat = 5.75 \
	  lon = 42.0 \
	  line = 23.0 \
	  sample = 85.0;
	catlab from=$(OUTPUT)/maplabOutput22.cub to=$(OUTPUT)/tempLab.txt > /dev/null;
	head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth22.pvl;
	rm $(OUTPUT)/tempLab.txt;
