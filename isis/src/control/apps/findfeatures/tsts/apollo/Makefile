APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	ls -1 $(INPUT)/Sub4-AS15-M-058[4-5]_msk.cub > $(OUTPUT)/apollo_nadir.lis;
	$(APPNAME) algorithm="sift/sift" \
	           match=$(INPUT)/Sub4-AS15-M-0583_msk.cub \
	           fromlist=$(OUTPUT)/apollo_nadir.lis \
	           tolist=$(OUTPUT)/toList.txt \
	           tonotmatched=$(OUTPUT)/unmatched.txt \
	           maxpoints = 5000 \
	           epitolerance=1.0 \
	           ratio=0.65 \
	           hmgtolerance=3.0 \
	           onet=$(OUTPUT)/apolloAS15_1583.net \
	           networkid="ApolloAS15_0583" \
	           pointid="AS15_0583_????" \
	           description="Test Apollo 15 images" \
	           debug=false \
	           >& /dev/null; 

	# Need to trim filepaths from TOLIST output
	$(SED) 's|.*/\([^/]*\)|\1|' $(OUTPUT)/toList.txt \
	       >& $(OUTPUT)/cubes.txt;
	$(RM) $(OUTPUT)/toList.txt;
	$(RM) $(OUTPUT)/apollo_nadir.lis;
