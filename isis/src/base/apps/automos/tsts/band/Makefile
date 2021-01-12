APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  mosaic=$(OUTPUT)/automosTruth1.cub \
	  priority=band \
	  type=keyword \
	  keyname=filternumber \
	  keyvalue=5 \
	  criteria=greater \
	  grange=auto \
	  track=true \
	  matchbandbin=true \
	  highsaturation=true \
	  lowsaturation=true \
	  null=false > /dev/null;
