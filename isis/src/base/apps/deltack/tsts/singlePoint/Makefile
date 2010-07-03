APPNAME = deltack
#This file tests the single point (don't solve for twist) option

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT)
	$(APPNAME) from=$(OUTPUT)/I20042001RDR_driftcosi.cub \
	maxits=20 \
	tol=0.5 \
	twist=false \
	samp1=21. \
	line1=1177. \
	lat1=32.53 \
	lon1=79.72 > /dev/null;
	$(RM) print.prt < /dev/null;
