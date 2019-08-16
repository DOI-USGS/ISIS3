#This file tests the twist (2-point) option
#	tol=0.4194172
APPNAME = deltack

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT)
	$(APPNAME) from=$(OUTPUT)/I08526015RDR_band9.cub \
	maxits=25 \
	twist=true \
	samp1=336.30 \
	line1=11842.33 \
	lat1=-6.3533854373815 \
	lon1=354.86255739295 \
	samp2=366.79 \
	line2=8264.78 \
	lat2=-0.3270664209085 \
	lon2=355.70986410595 > /dev/null;
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 2. remove digits beyond the fifth decimal place of decimal numbers
	# 3. remove date and time
	$(CAT) residuals.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/residuals.csv
	$(RM) bundleout.txt > /dev/null;
	$(RM) residuals.csv  > /dev/null;
	$(RM) print.prt < /dev/null;
