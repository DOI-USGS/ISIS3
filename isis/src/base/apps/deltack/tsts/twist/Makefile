APPNAME = deltack
#This file tests the twist (2-point) option

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT)
	$(APPNAME) from=$(OUTPUT)/I08526015RDR_band9.cub \
	maxits=25 \
	tol=0.4194172 \
	twist=true \
	samp1=58.5 \
	line1=11603. \
	lat1=-6.3533854373815 \
	lon1=354.86255739295 \
	samp2=99.25 \
	line2=8044.75 \
	lat2=-0.3270664209085 \
	lon2=355.70986410595 > /dev/null;
	$(RM) print.prt < /dev/null;
