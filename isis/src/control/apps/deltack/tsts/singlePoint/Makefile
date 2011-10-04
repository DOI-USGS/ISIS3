APPNAME = deltack
#This file tests the single point (don't solve for twist) option

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT)
	$(APPNAME) from=$(OUTPUT)/vik.cub \
	twist=false \
	maxit=10 \
	samp1=731.875 \
	line1=669.375 \
	lat1=-26.439008370134 \
	lon1= 352.19745020451 > /dev/null;
	cat bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g' \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  | sed 's/\-0.00000/ 0.00000/' \
	  | sed 's/\-0.00000/ 0.00000/' \
	  > $(OUTPUT)/bundleout.txt
	# The above command uses sed to do the following (in order):
	# 1. remove cube filename paths
	# 2. remove digits beyond the fifth decimal place of decimal numbers
	# 3. remove date and time
	cat residuals.csv | sed 's/\/[^,]*\/\([^,\/]*\.cub\)/\1/g'\
	  > $(OUTPUT)/residuals.csv
	$(RM) bundleout.txt > /dev/null;
	$(RM) residuals.csv  > /dev/null;
	$(RM) print.prt < /dev/null;
