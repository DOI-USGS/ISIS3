APPNAME = cnethist

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.pvl $(OUTPUT) > /dev/null;
	$(LS)  $(OUTPUT)/*jigged.pvl > $(OUTPUT)/net.lis;
	$(LS)  $(OUTPUT)/*Chen.pvl >> $(OUTPUT)/net.lis;
	#this last network has no residual data and was added to test the error message
	$(LS)  $(OUTPUT)/*autoseed*.pvl >> $(OUTPUT)/net.lis;
	$(APPNAME) clist=$(OUTPUT)/net.lis \
	to=hist.txt \
	bin_width = 0.1 >& $(OUTPUT)/temp.txt;
	cat $(OUTPUT)/temp.txt | grep -v "Processed" | grep -v "Reading Control Points" \
		| grep -v "Adding Control Points to Network"  | grep -v "Program" > $(OUTPUT)/error.pvl;
	cat hist.txt  \
	  | sed 's/\/[^,]*\/\([^,\/]*\.net\)/\1/g' \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  | sed s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  > $(OUTPUT)/hist.txt
	# The above command uses sed to do the following (in order):
	# 2. remove net filename paths
	# 3. remove digits beyond the fifth decimal place of decimal numbers
	# 4. remove date and time
	$(RM) $(OUTPUT)/*jigged.pvl > /dev/null;
	$(RM) $(OUTPUT)/*Chen.pvl > /dev/null;
	$(RM) $(OUTPUT)/*autoseed*.pvl > /dev/null;
	$(RM) $(OUTPUT)/*.lis > /dev/null;
	$(RM) $(OUTPUT)/temp.txt > /dev/null;
	$(RM) hist.txt > /dev/null;
