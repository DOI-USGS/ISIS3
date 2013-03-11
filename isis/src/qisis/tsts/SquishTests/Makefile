include $(ISISROOT)/make/isismake.tsts

.ONESHELL:

SQUISHBIN=/home/slambright/Squish/bin

commands: configsquish
	if [ -f "$$HOME/.squish/ver1/server.ini" ]; then
	  $(RM) "$$HOME/.squish/ver1/server.ini"
	fi
	$(SQUISHBIN)/squishserver >& /dev/null & 
	$(SQUISHBIN)/squishserver --config addAppPath '$(ISISROOT)/bin' >& /dev/null
	$(SQUISHBIN)/squishrunner --config setCursorAnimation off >& /dev/null
	cd $(OUTPUT) >& /dev/null
	ABSOUTPUT=`pwd`
	cd - >& /dev/null
	DELETEINPUT=false
	if [ ! -d input ]; then
	  ln -s "$(INPUT)"
	  DELETEINPUT=true
	fi
	for SUITE in `ls -1 suites/`; do
	  if [ -d "suites/$$SUITE" ] && [ -f "suites/$$SUITE/suite.conf" ]; then
	    touch $$ABSOUTPUT/$$SUITE.xml
	    $(SQUISHBIN)/squishrunner --testsuite "suites/$$SUITE" \
	      --reportgen xml2.1,$$ABSOUTPUT/$$SUITE.xml 2>&1 >& /dev/null
	    if [ -f "$$ABSOUTPUT/$$SUITE.xml" ]; then
	      $(CAT) $$ABSOUTPUT/$$SUITE.xml | sed 's/time="[^"]*"/time="..."/g' | \
	        sed 's/file="[^"]*"/file="..."/g' > $$ABSOUTPUT/$$SUITE.tmp.xml
	      $(MV) $$ABSOUTPUT/$$SUITE.tmp.xml $$ABSOUTPUT/$$SUITE.xml
	      $(MV) $$ABSOUTPUT/$$SUITE.xml $$ABSOUTPUT/$$SUITE.xml.txt
	    else
	      echo "File $$ABSOUTPUT/$$SUITE.xml not created by squishrunner"
	    fi
	  fi
	done
	$(SQUISHBIN)/squishserver --stop >& /dev/null
	if [ -L input ]; then
	  $(RM) input
	fi

configsquish:
	if [ ! -d input ]; then \
	  ln -s "$(INPUT)";     \
	fi                      
	$(SQUISHBIN)/squishserver >& /dev/null & 
	$(SQUISHBIN)/squishserver --config addAppPath '$(ISISROOT)/bin' >& /dev/null
	$(SQUISHBIN)/squishrunner --config setCursorAnimation off >& /dev/null
	$(SQUISHBIN)/squishserver --stop >& /dev/null
	$(LS) $$PWD/input/Ground/Extracted_AllOverlaps/I*.cub > input/Ground/Extracted_AllOverlaps.lis
	$(LS) $$PWD/input/Ground/Extracted_AllOverlaps/f*.cub >> input/Ground/Extracted_AllOverlaps.lis
	$(LS) $$PWD/input/m01018/N1597182735_2_tr.cub > input/m01018/tr.lis
	$(LS) $$PWD/input/m01018/N1597182896_2_tr.cub >> input/m01018/tr.lis
	$(LS) $$PWD/input/m01018/N1597183061_2_tr.cub >> input/m01018/tr.lis
	$(LS) $$PWD/input/m01018/N1597183216_2_tr.cub >> input/m01018/tr.lis
	$(LS) $$PWD/input/m01018/N1602275390_1_tr.cub >> input/m01018/tr.lis
	$(LS) $$PWD/input/m01018/N1604169204_2_tr.cub >> input/m01018/tr.lis
	$(CAT) input/m01018/tr.lis | grep -v 'N1602275390_1_tr.cub' > input/m01018/tr_no390.lis
	$(LS) $$PWD/input/Ground/Mini/*.cub > input/Ground/Mini.lis

suite_%: configsquish
	$(SQUISHBIN)/squishserver >& /dev/null & 
	
	if [ ! -d output ]; then
	  $(MKDIR) output;
	fi
	
	cd $(OUTPUT) >& /dev/null
	ABSOUTPUT=`pwd`
	cd - >& /dev/null
	
	if [ -d "suites/$@" ] && [ -f "suites/$@/suite.conf" ]; then
	  touch $$ABSOUTPUT/$@.xml
	  $(SQUISHBIN)/squishrunner --testsuite "suites/$@" \
	    --reportgen xml2.1,$$ABSOUTPUT/$@.xml 2>&1 >& /dev/null
	  if [ -f "$$ABSOUTPUT/$@.xml" ]; then
	    $(CAT) $$ABSOUTPUT/$@.xml | sed 's/time="[^"]*"/time="..."/g' | \
	      sed 's/file="[^"]*"/file="..."/g' > $$ABSOUTPUT/$@.tmp.xml
	    $(MV) $$ABSOUTPUT/$@.tmp.xml $$ABSOUTPUT/$@.xml
	    $(MV) $$ABSOUTPUT/$@.xml $$ABSOUTPUT/$@.xml.txt
	    grep -Hn -A 1 '<result type="FAIL" time="...">' $(OUTPUT)/$@.xml.txt 2>>/dev/null
	    grep -Hn -A 2 '<message .*>' $(OUTPUT)/$@.xml.txt 2>>/dev/null
	  else
	    echo "File $$ABSOUTPUT/$@.xml not created by squishrunner"
	  fi
	fi
	$(SQUISHBIN)/squishserver --stop >& /dev/null
	if [ -L input ]; then
	  $(RM) input
	fi

