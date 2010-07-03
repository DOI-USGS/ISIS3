APPNAME = maplab

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/maplabInput3.cub $(OUTPUT)/maplabOutput31.cub
	catlab from=$(OUTPUT)/maplabOutput31.cub >& $(OUTPUT)/tempLab.txt;
	head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/origLabel3.txt;
	$(APPNAME) from= $(OUTPUT)/maplabOutput31.cub \
	  map = $(INPUT)/mapFile.map \
	  x =  -3000.0 \
	  y =  201.0 \
	  line = 23.0 \
	  sample = 85.0 ;
	  catlab from=$(OUTPUT)/maplabOutput31.cub >& $(OUTPUT)/tempLab.txt;
	  head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth31.txt;
	cp $(INPUT)/maplabInput3.cub $(OUTPUT)/maplabOutput32.cub
	  $(APPNAME) from= $(OUTPUT)/maplabOutput32.cub \
	  map = $(INPUT)/mapFile.map \
	  c = latlon \
	  lat = 5.75 \
	  lon = 42.0 \
	  line = 23.0 \
	  sample = 85.0;
	  catlab from=$(OUTPUT)/maplabOutput32.cub >& $(OUTPUT)/tempLab.txt;
	  head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth32.txt;
	  rm $(OUTPUT)/tempLab.txt
