APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/C1716334271_1.irBand248.cub \
	           to=$(OUTPUT)/C1716334271_1.irBand248.polar.cub \
	           map=$(ISIS3DATA)/base/templates/maps/polarstereographic.map \
	           pixres=ppd \
	           res=20 \
	           > /dev/null;
