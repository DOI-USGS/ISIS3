APPNAME = maplab

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/maplabInput.cub $(OUTPUT)/maplabOutput11.cub
	catlab from=$(OUTPUT)/maplabOutput11.cub >& $(OUTPUT)/tempLab.txt;
	head -n 101 $(OUTPUT)/tempLab.txt > $(OUTPUT)/origLabel1.txt;
	$(APPNAME) from= $(OUTPUT)/maplabOutput11.cub \
	  map = $(INPUT)/mapFile.map \
	  x =  -395500.0 \
	  y =  2019500.0 \
	  line = 23.0 \
	  sample = 85.0;
	  catlab from=$(OUTPUT)/maplabOutput11.cub >& $(OUTPUT)/tempLab.txt;
	  head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth11.txt;
	cp $(INPUT)/maplabInput.cub $(OUTPUT)/maplabOutput12.cub
	$(APPNAME) from= $(OUTPUT)/maplabOutput12.cub \
	  map = $(INPUT)/mapFile.map \
	  coor = latlon \
	  lat = 34.070186510964 \
	  lon = 219.90185944016 \
	  line = 23.0 \
	  sample = 85.0 ;
	  catlab from=$(OUTPUT)/maplabOutput12.cub >& $(OUTPUT)/tempLab.txt;
	  head -n 121 $(OUTPUT)/tempLab.txt > $(OUTPUT)/maplabTruth12.txt;
	  rm $(OUTPUT)/tempLab.txt
