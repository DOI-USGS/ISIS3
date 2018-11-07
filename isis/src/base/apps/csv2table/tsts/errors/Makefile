APPNAME = csv2table

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/isisTruth.cub $(OUTPUT)/isisTruth.cub;

	if [ `$(APPNAME) csv=$(INPUT)/not_a_file.csv \
	     tablename="TestTable" \
	     to=$(OUTPUT)/isisTruth.cub 2> $(OUTPUT)/errors.txt` ]; \
	then \
	  true; \
	fi;

	if [ `$(APPNAME) csv=$(INPUT)/empty.csv \
	      tablename="TestTable" \
	      to=$(OUTPUT)/isisTruth.cub 2>> $(OUTPUT)/errors.txt` ]; \
	then \
	  true; \
	fi;

	if [ `$(APPNAME) csv=$(INPUT)/test.csv \
	      label=$(INPUT)/not_a_file.pvl \
	      tablename="TestTable" \
	      to=$(OUTPUT)/isisTruth.cub 2>> $(OUTPUT)/errors.txt` ]; \
	then \
	  true; \
	fi;

	cat $(OUTPUT)/errors.txt | sed 's+\[.*input/+[+' \
	    > $(OUTPUT)/clean_errors.txt;

	rm $(OUTPUT)/isisTruth.cub $(OUTPUT)/errors.txt;
