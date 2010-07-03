APPNAME  = mapmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosTruth1.cub \
	  create=true \
	  track=true \
	  priority=BAND \
	  type=KEYWORD \
	  keyname=FilterName \
	  keyvalue=RED \
	  criteria=LESSER \
	  minlat=10.765 \
	  maxlat=34.445 \
	  minlon=219.720 \
	  maxlon=236.190 \
	  highsaturation=false \
	  lowsaturation=false \
	  null=false > /dev/null;
