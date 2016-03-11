APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/Sub4-AS15-M-0584_msk_cropped.cub > $(OUTPUT)/apollo_nadir.lis;

	$(APPNAME) algorithm="sift/sift" \
	           match=$(INPUT)/Sub4-AS15-M-0583_msk_cropped.cub \
	           fromlist=$(OUTPUT)/apollo_nadir.lis \
	           onet=$(OUTPUT)/apolloAS15_1583_none.net \
	           epitolerance=1.0 \
	           ratio=0.65 \
	           hmgtolerance=1.0 \
	           filter=sobel \
	           maxpoints=5000 \
	           networkid="ApolloAS15_0583_none" \
	           pointid="AS15_0583_sobel_????" \
	           description="Test Apollo 15 images - No Filter" \
	           debug=false \
	           >& /dev/null; 

	$(APPNAME) algorithm="sift/sift" \
	           match=$(INPUT)/Sub4-AS15-M-0583_msk_cropped.cub \
	           fromlist=$(OUTPUT)/apollo_nadir.lis \
	           onet=$(OUTPUT)/apolloAS15_1583_sobel.net \
	           filter=sobel maxpoints=5000 \
	           epitolerance=1.0 \
	           ratio=0.65 \
	           hmgtolerance=1.0 \
	           networkid="ApolloAS15_0583_sobel" \
	           pointid="AS15_0583_sobel_????" \
	           description="Test Apollo 15 images - Sobel Filter" \
	           debug=false \
	           >& /dev/null; 

	$(APPNAME) algorithm="sift/sift" \
	           match=$(INPUT)/Sub4-AS15-M-0583_msk_cropped.cub \
	           fromlist=$(OUTPUT)/apollo_nadir.lis \
	           onet=$(OUTPUT)/apolloAS15_1583_scharr.net \
	           epitolerance=1.0 \
	           ratio=0.65 \
	           hmgtolerance=1.0 \
	           filter=scharr maxpoints=5000 \
	           networkid="ApolloAS15_0583_scharr" \
	           pointid="AS15_0583_scharr_????" \
	           description="Test Apollo 15 images - Scharr Filter" \
	           debug=false \
	           >& /dev/null; 

	$(RM) $(OUTPUT)/apollo_nadir.lis; 
