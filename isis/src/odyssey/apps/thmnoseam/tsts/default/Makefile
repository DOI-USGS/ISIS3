APPNAME = thmnoseam

V06944003RDR.noseam.even.cub.TOLERANCE = 0.000000001
V06944003RDR.noseam.odd.cub.TOLERANCE  = 0.000000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) INEVEN=$(INPUT)/V06944003RDR.even.cub \
	           INODD=$(INPUT)/V06944003RDR.odd.cub \
	           OUTEVEN=$(OUTPUT)/V06944003RDR.noseam.even.cub \
	           OUTODD=$(OUTPUT)/V06944003RDR.noseam.odd.cub > /dev/null;
