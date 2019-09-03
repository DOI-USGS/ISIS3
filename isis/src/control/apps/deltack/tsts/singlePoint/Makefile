#This file tests the single point (don't solve for twist) option
#
# 2014-07-23 JWB - Added semicolon to first command.
APPNAME = deltack

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT);
	$(APPNAME) from=$(OUTPUT)/vik.cub \
	twist=false \
	maxit=10 \
	samp1=731.875 \
	line1=669.375 \
	lat1=-26.439008370134 \
	lon1= 352.19745020451 > /dev/null;
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 2. remove digits beyond the fifth decimal place of decimal numbers
	# 3. remove date and time
	$(CAT) residuals.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  > $(OUTPUT)/residuals.csv
	$(RM) residuals.csv > /dev/null;
	$(RM) bundleout.txt > /dev/null;
