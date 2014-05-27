APPNAME = maplab

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/maplabInput3.cub $(OUTPUT)/maplabOutput31.cub
	$(APPNAME) from= $(OUTPUT)/maplabOutput31.cub \
	  map = $(INPUT)/mapFile.map \
	  x =  -3000.0 \
	  y =  201.0 \
	  line = 23.0 \
	  sample = 85.0 ;
	catlab from=$(OUTPUT)/maplabOutput31.cub to=$(OUTPUT)/tempLab.txt > /dev/null;
	head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth31.pvl;
	rm $(OUTPUT)/tempLab.txt;
	cp $(INPUT)/maplabInput3.cub $(OUTPUT)/maplabOutput32.cub
	  $(APPNAME) from= $(OUTPUT)/maplabOutput32.cub \
	  map = $(INPUT)/mapFile.map \
	  c = latlon \
	  lat = 5.75 \
	  lon = 42.0 \
	  line = 23.0 \
	  sample = 85.0;
	catlab from=$(OUTPUT)/maplabOutput32.cub to=$(OUTPUT)/tempLab.txt > /dev/null;
	head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth32.pvl;
	rm $(OUTPUT)/tempLab.txt;
