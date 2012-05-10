APPNAME = cnethist

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.net $(OUTPUT) > /dev/null;
	$(LS)  $(OUTPUT)/*jigged.net > $(OUTPUT)/net.lis;
	$(LS)  $(OUTPUT)/*Chen.net >> $(OUTPUT)/net.lis;
	$(APPNAME) clist=$(OUTPUT)/net.lis \
	to=hist.txt \
	bin_width = 0.1 > /dev/null;
	cat hist.txt  \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/hist.txt
	# The above command uses sed to do the following (in order):
	# 2. remove net filename paths
	# 3. remove digits beyond the fifth decimal place of decimal numbers
	# 4. remove date and time
	$(RM) $(OUTPUT)/*.net > /dev/null;
	$(RM) $(OUTPUT)/*.lis > /dev/null;
	$(RM) hist.txt > /dev/null;
