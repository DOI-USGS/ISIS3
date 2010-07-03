APPNAME  = mapmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosTruth2.cub \
	  create=true \
	  track=true \
	  match=false \
	  minlat=10.765 \
	  maxlat=34.445 \
	  minlon=219.720 \
	  maxlon=236.190 \
	  highsaturation=true \
	  lowsaturation=true \
	  null=true > /dev/null;

	  map2map from=$(INPUT)/ab102402.lev2.cub \
	  map=$(INPUT)/ab102401.lev2.cub \
	  to=$(OUTPUT)/map.cub \
	  defaultrange=map \
	  inter=near > /dev/null;

	  $(APPNAME) from=$(OUTPUT)/map.cub \
	  mosaic=$(OUTPUT)/mapmosTruth2.cub match=false  > /dev/null;

	  
