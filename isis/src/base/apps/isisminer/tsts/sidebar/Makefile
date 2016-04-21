# Test Sidebar Strategy
# First isisminer run tests normal behavior
# Second tests debug conditionals
# the sed commands remove timestamps from the debug output
# Third tests exceptions

APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) config=$(INPUT)/sidebar_test.conf \
	parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null; \
	$(APPNAME) config=$(INPUT)/sidebar_debug.conf \
	parameters="inputdir:$(INPUT)" > $(OUTPUT)/temp.txt; \
	$(CAT) $(OUTPUT)/temp.txt \
	| $(SED) 's/[0-9][0-9]:[0-9][0-9]:[0-9][0-9]\.[0-9][0-9][0-9]/00:00:00.000/g' \
	| $(SED) 's/):.[0-9].*/): 0/g' \
	> $(OUTPUT)/sidebar_debug.txt; \
	$(RM) $(OUTPUT)/temp.txt > /dev/null; \
	if [ `$(APPNAME) config=$(INPUT)/sidebar_exception.conf \
	  2> $(OUTPUT)/sidebar_exception.txt > /dev/null` ]; \
	  then true; \
	fi;
