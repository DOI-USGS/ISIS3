# history:
# 2017-12-21 Kristin Berry - Updated for new test data and significant updates to tgocassisi2isis
# 2017-11-20 Kristin Bery - Updated with new test data released Nov 2017. 

APPNAME = tgocassis2isis

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-PAN-00000-B1.xml \
	           to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-PAN-00000-B1.cub > /dev/null;

	catlab from=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-PAN-00000-B1.cub \
	       to=$(OUTPUT)/labels.pvl > /dev/null;
