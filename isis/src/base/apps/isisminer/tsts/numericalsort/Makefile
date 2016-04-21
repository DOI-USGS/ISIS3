# Test the NumericalSortStrategy
# First isisminer run tests normal functionality of NumericalSort
# Second isisminer run tests exceptions in NumericalSort
APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) config=$(INPUT)/numericalsort_test.conf \
	parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null;
	if [ `$(APPNAME) config=$(INPUT)/numericalsort_error.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" \
	  2> $(OUTPUT)/numericalsort_error.txt > /dev/null` ]; \
	  then true; \
	fi;

