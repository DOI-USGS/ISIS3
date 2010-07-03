APPNAME = maplab

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/maplabInput2.cub $(OUTPUT)/maplabOutput21.cub
	catlab from=$(OUTPUT)/maplabOutput21.cub >& $(OUTPUT)/tempLab.txt;
	head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/origLabel2.txt;
	$(APPNAME) from= $(OUTPUT)/maplabOutput21.cub \
	  map = $(INPUT)/mapFile.map \
	  x =  -3000.0 \
	  y =  201.0 \
	  line = 23.0 \
	  sample = 85.0 ;
	  catlab from=$(OUTPUT)/maplabOutput21.cub >& $(OUTPUT)/tempLab.txt;
	  head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth21.txt;
	cp $(INPUT)/maplabInput2.cub $(OUTPUT)/maplabOutput22.cub
	  $(APPNAME) from= $(OUTPUT)/maplabOutput22.cub \
	  map = $(INPUT)/mapFile.map \
	  c = latlon \
	  lat = 5.75 \
	  lon = 42.0 \
	  line = 23.0 \
	  sample = 85.0;
	  catlab from=$(OUTPUT)/maplabOutput22.cub >& $(OUTPUT)/tempLab.txt;
	  head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth22.txt;
	  rm $(OUTPUT)/tempLab.txt
