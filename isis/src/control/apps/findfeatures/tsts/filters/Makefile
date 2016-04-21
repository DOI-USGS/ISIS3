APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=surf/surf \
	           onet=$(OUTPUT)/mess_sobel.net \
	           filter=sobel \
	           networkid="Messenger Sobel" \
	           pointid="Messenger_sobel_????" \
	           description="Test Messenger - Sobel Filter" \
	           >& /dev/null; 

	$(APPNAME) from=$(INPUT)/Sub4-AS15-M-0583_msk_cropped.cub \
	           match=$(INPUT)/Sub4-AS15-M-0584_msk_cropped.cub \
	           algorithm=sift/sift \
	           onet=$(OUTPUT)/app_scharr.net \
	           filter=scharr \
	           epitol=1.0 \
	           ratio=0.65 \
	           hmg=1.0 \
	           maxpoints=5000 \
	           debug=false \
	           networkid="Apollo Scharr" \
	           pointid="Apollo_scharr_????" \
	           description="Test Apollo - Scharr Filter" \
	           >& /dev/null; 

	# remove apollo network - not same across all platforms
	$(RM) $(OUTPUT)/app_scharr.net;
