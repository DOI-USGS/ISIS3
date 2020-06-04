# This tests FASTGEOM, FASTGEOMPOINTS, GEOMTYPE, and GEOMSOURCE params
APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=surf/surf \
	           onet=$(OUTPUT)/geomsourceFrom.net \
	           networkid="NETTYPE image, GEOMSOURCE from" \
	           geomsource=from;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=surf/surf \
	           onet=$(OUTPUT)/geomsourceMatch.net \
	           networkid="NETTYPE image, GEOMSOURCE match" \
	           geomsource=match;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=brisk/brisk \
	           fastgeom=yes \
	           geomtype=camera \
	           onet=$(OUTPUT)/fastgeomCamera.net;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=brisk/brisk \
	           fastgeom=yes \
	           geomtype=crop \
	           onet=$(OUTPUT)/fastgeomCrop.net;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=brisk/brisk \
	           fastgeom=yes \
	           fastgeompoints=200 \
	           geomtype=crop \
	           onet=$(OUTPUT)/fastgeomCrop200Points.net;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=brisk/brisk \
	           fastgeom=yes \
	           geomtype=map \
	           onet=$(OUTPUT)/fastgeomMap.net;

