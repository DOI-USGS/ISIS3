# This tests the errors that can occur when running jigsaw.
#   TEST A: "Must either solve for camera pointing or spacecraft position."
#   UNTESTED, not sure how to force exception during bundle adjust
#     TEST B: "Unable to bundle adjust network"
#   TEST C: "Input Target parameters file missing main Target object."
#   TEST D: "Must solve for at least one target body option."
#
# 2016-08-22 Ian Humphrey - Original version. Fixes #4269.
# 2016-10-13 Ian Humphrey - Removed TEST E: "Input SC_PARAMETERS file missing SensorParameters
#                object," as USEPVL and SC_PARAMETERS have been turned off. References #4292,
#                #4293."
APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
# TEST A: CAMSOLVE=None and SPSOLVE=None
	echo -e "Error Test A:" > $(OUTPUT)/error.txt;
	if [[ `$(APPNAME) \
	  fromlist=$(INPUT)/empty.lis \
	  cnet=$(INPUT)/empty.net \
	  onet=$(OUTPUT)/out.net \
	  camsolve=None \
	  spsolve=None \
	  2>> $(OUTPUT)/error.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
# TEST B: exception occurs when performing bundle adjust
#         TODO: This still needs to be tested.
# TEST C: SOLVETARGETBODY=yes and TBPARAMETERS file missing Target object
	echo -e "Error Test C:" >> $(OUTPUT)/error.txt;
	if [[ `$(APPNAME) \
	  fromlist=$(INPUT)/empty.lis \
	  cnet=$(INPUT)/empty.net \
	  onet=$(OUTPUT)/out.net \
	  solvetargetbody=yes \
	  tbparameters=$(INPUT)/invalidTargetParameters.pvl \
	  2>> $(OUTPUT)/error.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
# TEST D: SOLVETARGETBODY=yes and # of parameters to solve is 0
	echo -e "Error Test D:" >> $(OUTPUT)/error.txt;
	if [[ `$(APPNAME) \
	  fromlist=$(INPUT)/empty.lis \
	  cnet=$(INPUT)/empty.net \
	  onet=$(OUTPUT)/out.net \
	  solvetargetbody=yes \
	  tbparameters=$(INPUT)/zeroTargetParameters.pvl \
	  2>> $(OUTPUT)/error.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
