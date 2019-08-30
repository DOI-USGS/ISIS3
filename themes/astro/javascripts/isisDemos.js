// global strings don't change
var addToFile = "destripe.cub"; // m_AddtoFileParam
var filtFromFile = "input.cub"; // m_FiltFromFileParam
var LPFtoFile = "lpf.cub"; // LPFToFileParam
var HPFtoFile = "hpf.cub"; // HPFToFileParam

/* need to create the stage before we pass it to the console */
var canvasSeams, contextSeams, stageSeams, rSeamsConsole, area1, area2, area3, area4, mosaic, original, area1b, area2b, area3b, area4b, originalb, area1o, area2o, area3o, area4o, grey, greyb, area1Base, area2Base, area3Base, area4Base, result, resultb, mosaicib, mosaici, mosaicb;

var img, imgi, stage, context, canvas, destripeConsole, base, hi, lo, fin,imgb,imgh,imgl,imgf,bmp_hi,bmp_low,bmp_bg;

var isisConsole = function(options) {
    // initialize
    this.options = (options) ? options : {};
    this.boxes = (this.options.boxes) ? this.options.boxes : [];
    this.activeBox = (this.boxes.length) ? this.boxes[0].activate() : false;
    this.slider = (this.options.slider) ? true : false;
    this.showDnMultiplier = (this.options.dnMultiplier) ? true : false;

    // create display
    // display bottom contains mouseover pixel information
    this.container = document.getElementById(this.options.target);
    this.displayBottom = document.createElement('div');
    this.displayBottom.setAttribute('class', 'console bottom');

    // right display contains options to change the boxes and readouts
    this.displayRight = document.createElement('div');
    this.displayRight.setAttribute('class', 'console right');
    this.displayRight.style.height = this.container.offsetHeight-1 + "px";
    this.subpixels = (this.options.subpixels) ? this.options.subpixels : false;

    // static images
    this.image = (this.options.staticImage) ? this.options.staticImage : false;

    // layered images
    if( this.options.hasOwnProperty('challenge') ) {
	this.canvas = this.options.canvas || null;
	this.ctx = this.options.context || null;
	this.stage = this.options.stage || null;
	this.challenge = this.options.challenge || null;
	this.showOriginalId = this.options.target + '-showOriginal';
	if( this.canvas != null && this.ctx != null && this.stage != null && this.challenge != null ) {
	    this.turnOnFilters = true;
	}
    }

    // build the console

    // show the right console ( if the options are set to true )
    if(this.options.showRightConsole) {
	this.container.appendChild(this.displayRight);
    }
    // append the bottom of the console
    this.container.appendChild(this.displayBottom);

    // initialize slider element
    if ( this.slider && this.options.showRightConsole ) {
	this.sliderId = this.options.target + '-slider';
	this.sliderValId = this.options.target + '-sliderVal';
	this.initSlider('change');
    }

    // extra style fixes
    if(this.options.bottomConsoleTop) { this.displayBottom.style.top = this.options.bottomConsoleTop; }

    // pixel information - mouse position
    this.mousePos = document.createElement('span');
    this.mousePos.setAttribute('class', 'text');
    this.displayBottom.appendChild(this.mousePos);
    this.mousePos.innerHTML = "Pixel at: 0, 0";

    // pixel information - DN at mouse position
    var storedDnContainer = document.createElement('span');
    storedDnContainer.setAttribute('class', 'text');
    this.storedDn = document.createElement('input');
    if(this.boxes.length > 0 || this.image) { 
	storedDnContainer.innerHTML = "Stored DN: "; 
	this.storedDn.value = 255;

    } else { 
	storedDnContainer.innerHTML = "RGB: "; 
	this.storedDn.setAttribute('class', 'special-width');
	this.storedDn.value = "255, 255, 255";
    };
    
    storedDnContainer.appendChild(this.storedDn);
    this.displayBottom.appendChild(storedDnContainer);
    this.storedDn.readOnly = true;

    // true DN
    if( this.showDnMultiplier && this.options.showRightConsole ) {
	this.dnMultiplierId = this.options.target + '-dnMultiplier';
	this.dnMultiplierValId = this.options.target + '-dnMultiplierVal';
	this.dnBaseId = this.options.target + '-dnBase';
	this.dnBaseValId = this.options.target + '-dnBaseVal';

	var trueDnContainer = document.createElement('span');
	trueDnContainer.setAttribute('class', 'text');
	var trueDn = document.createElement('input');
	trueDnContainer.innerHTML = "True DN: ";
	trueDn.value = 255;
	trueDn.setAttribute('id', 'trueDn');
	trueDnContainer.appendChild(trueDn);
	this.trueDn = trueDn;
	this.displayBottom.appendChild(trueDnContainer);
	this.initSlider('base');
	this.initSlider('multiplier');
	
	this.trueDn.readOnly = true;
    }

    if (this.options.target == 'isis-cube') {
	this.initCubeDemoButtons();
    }

    // reset button
    this.reset = document.createElement('button');
    this.displayRight.appendChild(this.reset);
    if( this.boxes.length ) {
	if( this.boxes[0].type == 'special' ) {
	    if( this.boxes[0].colorized ) {
		this.reset.appendChild(document.createTextNode('Decolorize'));
	    } else {
		this.reset.appendChild(document.createTextNode('Colorize'));
	    }
	    this.addSpecialPixelsLegend();
	} else {
	    this.reset.appendChild(document.createTextNode('RESET'));
	}
    } else {
	this.reset.appendChild(document.createTextNode('RESET'));
    }
    
    

    var myConsole = this;

    // handle static images
    if( myConsole.image ) {
	var i = myConsole.image;
	
	i.canvas.addEventListener( 'mousemove', function(e) {
		var x = i.getMousePos(e).x, y = i.getMousePos(e).y;
		myConsole.mousePos.innerHTML = 'Pixel at: ';
		myConsole.mousePos.innerHTML += x + ", " + y;
		var lx = e.layerX;
		var ly = e.layerY;
		var pixel = i.ctx.getImageData(lx, ly, 1, 1);
		var data = pixel.data;
		var dn = data[0];
		myConsole.storedDn.value = dn;
		if ( myConsole.trueDn.value ) {
		    var base = $("#" + myConsole.dnBaseId).slider('option', 'value');
		    var multiplier = $("#" + myConsole.dnMultiplierId).slider('option', 'value');
		    
		    myConsole.trueDn.value = (parseInt(myConsole.storedDn.value * parseFloat(multiplier)) + parseInt(base));

		}
	    });
    }
    // isis filter challenges
    if( myConsole.turnOnFilters ) {
	// identifiers for input/output	
	myConsole.initInputIds();
	myConsole.addChallengeControls();
	var showOriginal = document.getElementById(myConsole.showOriginalId);
	var addImages = document.getElementById(myConsole.resultsId);
	var isisOut = document.getElementById(myConsole.isisOutputId);

	if( myConsole.challenge == 'destripe' ) {
	    
	    if(myConsole.options.overlaySupport) {		
		myConsole.addFilterListener('hpf');
		myConsole.addFilterListener('lpf');	    

		addImages.addEventListener('click', function() {			 
			showOriginal.checked = false;
			var lpf = myConsole.stage.getChildByName('lpf');
			var hpf = myConsole.stage.getChildByName('hpf');
			var base = myConsole.stage.getChildByName('base');
			lpf.set({'visible':true});
			hpf.set({'visible':true});
			base.set({'visible':true});
			myConsole.ctx.globalCompositeOperation = 'overlay';      
			myConsole.stage.updateContext(myConsole.ctx);
			
			lpf.updateCache();
			hpf.updateCache();
			myConsole.stage.update();
			
			infoText = "Finished image addition\n\n";
			infoText += "ISIS Command Line:\n";
			infoText += " algebra OPERATOR=ADD FROM=" + LPFtoFile + "\n FROM2=" + HPFtoFile + " TO=" + addToFile + "\n" ;
			isisOut.value = infoText ;

		    });	
		var lpf_v, hpf_v, base_v;
		showOriginal.addEventListener('click', function() {
			var lpf = myConsole.stage.getChildByName('lpf');
			var hpf = myConsole.stage.getChildByName('hpf');
			var base = myConsole.stage.getChildByName('base');
			if(showOriginal.checked) {
			    // the set up before we switch it
			    lpf_v = lpf.visible;
			    hpf_v = hpf.visible;
			    base_v = base.visible;
			    
			    base.visible = true;
			    lpf.visible = false;
			    hpf.visible = false;
			} else {
			    base.visible = base_v;
			    hpf.visible = hpf_v;
			    lpf.visible = lpf_v;
			}
			lpf.updateCache();
			hpf.updateCache();
			myConsole.stage.update();
			
		    });
	    }
	    else {
		var lowpassButton = document.getElementById(myConsole.runLowPassId);
		var hipassButton = document.getElementById(myConsole.runHiPassId);
		var resultsButton = document.getElementById(myConsole.resultsId);
		var showOriginal = document.getElementById(myConsole.showOriginalId);
		var infoText = "";
		var isisOut = document.getElementById(myConsole.isisOutputId);
		
		lowpassButton.addEventListener('click', function() {
			showOriginal.checked = false;
			var l = myConsole.stage.getChildByName('lpf');
			var h = myConsole.stage.getChildByName('hpf');
			var f = myConsole.stage.getChildByName('final');
			var rows = document.getElementById(myConsole.loBoxcarRowsId);
			var cols = document.getElementById(myConsole.loBoxcarColsId);
		       
			l.set({'visible':true});
			h.set({'visible':false});
			f.set({'visible':false});

			rows.value = "53";
			cols.value = "251";
			myConsole.stage.update();
			
			infoText = "Finished Low Pass Filter\n\n";
			infoText += "ISIS Command Line:\n";
			infoText += " lowpass FROM=" + filtFromFile + " TO=" + LPFtoFile +
			    " FILT=LPF\n LINE=" + rows.value + " SAMP=" + cols.value +
			    " BAND=1\n" ;
			isisOut.value = infoText;
			
		    });
		hipassButton.addEventListener('click', function() {
			showOriginal.checked = false;
			var l = myConsole.stage.getChildByName('lpf');
			var h = myConsole.stage.getChildByName('hpf');
			var f = myConsole.stage.getChildByName('final');
			var rows = document.getElementById(myConsole.hiBoxcarRowsId);
			var cols = document.getElementById(myConsole.hiBoxcarColsId);
		       
			l.set({'visible':false});
			h.set({'visible':true});
			f.set({'visible':false});
			myConsole.stage.update();
			
			rows.value = "1";
			cols.value = "91";

			infoText = "Finished High Pass Filter\n\n";
			infoText += "ISIS Command Line:\n";
			infoText += " highpass FROM=" + filtFromFile + " TO=" + HPFtoFile + " FILT=HPF\n" +
			    " LINE=" + rows.value + " SAMP=" + cols.value + " BAND=1\n" ;
			isisOut.value = infoText;

		    });
		resultsButton.addEventListener('click', function() {
			showOriginal.checked = false;
			var l = myConsole.stage.getChildByName('lpf');
			var h = myConsole.stage.getChildByName('hpf');
			var f = myConsole.stage.getChildByName('final');
			var hrows = document.getElementById(myConsole.hiBoxcarRowsId).value;
			var hcols = document.getElementById(myConsole.hiBoxcarColsId).value;
			var lrows = document.getElementById(myConsole.loBoxcarRowsId).value;
			var lcols = document.getElementById(myConsole.loBoxcarColsId).value;

			if ( hrows != '' & hcols != '' & lrows != '' & lcols != '') {

			    l.set({'visible':false});
			    h.set({'visible':false});
			    f.set({'visible':true});
			}
			myConsole.stage.update();
			infoText = "Finished image addition\n\n";
			infoText += "ISIS Command Line:\n";
			infoText += " algebra OPERATOR=ADD FROM=" + LPFtoFile + "\n FROM2=" + HPFtoFile + " TO=" + addToFile + "\n" ;
			isisOut.value = infoText ;

			
		    });
		var l_v, h_v, f_v, b_v;
		showOriginal.addEventListener('click', function() {			
			var l = myConsole.stage.getChildByName('lpf');
			var h = myConsole.stage.getChildByName('hpf');
			var f = myConsole.stage.getChildByName('final');
			var b = myConsole.stage.getChildByName('base');
			if(showOriginal.checked) {
			    l_v = l.visible;
			    h_v = h.visible;
			    f_v = f.visible;
			    b_v = b.visible;
			    b.visible = true;
			    h.visible = false;
			    l.visible = false;
			    f.visible = false;
			} else {
			    base.visible = b_v;
			    h.visible = h_v;
			    l.visible = l_v;
			    f.visible = f_v;
			}
			myConsole.stage.update();
			
		    });

	    }
	    
	} else if (myConsole.challenge == 'seam-removal') {
	    myConsole.addRadioButtonListener();
	    if (myConsole.options.overlaySupport) {
		myConsole.addFilterListener('hpf-area');
		myConsole.addFilterListener('lpf-mosaic');
		
		addImages.addEventListener('click', function() {
			showOriginal.checked = false;
			document.getElementById(myConsole.areaChooseId).children[6].children[0].checked=true;
			var layers = myConsole.stage.children;
			var mosaic = myConsole.stage.getChildByName('mosaic'); // visible
			var mosaic_i = myConsole.stage.getChildByName('mosaic-inv'); // visible
			var grey = myConsole.stage.getChildByName('grey'); // invisible
			var base = myConsole.stage.getChildByName('base'); // visible
			for (var i=1; i<=4; i++) {
			    myConsole.stage.getChildByName('area' + i).set({'visible':false}).updateCache();
			    myConsole.stage.getChildByName('area' + i + '-base').set({'visible':false});
			}
			base.set({'visible':true});
			mosaic.set({'visible':true});
			mosaic_i.set({'visible':true});
			grey.set({'visible':false});
			myConsole.ctx.globalCompositeOperation = 'overlay';      
			myConsole.stage.updateContext(myConsole.ctx);
			for(var i; i<layers.length; i++){
			    if(layers[i].cacheID>0) {
				layers[i].updateCache();
			    }
			}
			
			myConsole.stage.update();
			
			infoText = "Showing seam removal results";
			isisOut.value = infoText;

		    });
		var original_visibility = [];
		var original_text = "";
		showOriginal.addEventListener('click', function() {
			var layers = myConsole.stage.children;
			var infoText = "";
			var isisOut = document.getElementById(myConsole.isisOutputId);
			if(showOriginal.checked) {
			    for(var i = 0; i < layers.length; i++) {
				var layer = layers[i];
				original_visibility[i] = layer.visible;
				original_text = isisOut.value;
				// get rid of all filters
				if (layer.name == 'base') {
				    layer.set({'visible':true});
				} else {
				    layer.set({'visible':false});
				}
				if (layer.cacheID != 0) {
				//layer.filters = [];
				    layer.updateCache();
				}
			    }
			    infoText = "Showing original mosaic with seams";
			} else {
			    for(var i = 0; i < layers.length; i++) {
				var layer = layers[i];
				var set_back = original_visibility[i];
				// get rid of all filters
				layer.visible = set_back;
				if (layer.cacheID != 0) {
				//layer.filters = [];
				    layer.updateCache();
				}
			    }
			    infoText = original_text;
			    
			    
			}
			isisOut.value = infoText;
			myConsole.stage.update();
			
		    });
		
	    } else {
		var lowpassButton = document.getElementById(myConsole.runLowPassId);
		var hipassButton = document.getElementById(myConsole.runHiPassId);
		var resultsButton = document.getElementById(myConsole.resultsId);
		var showOriginal = document.getElementById(myConsole.showOriginalId);
		var infoText = "";
		var isisOut = document.getElementById(myConsole.isisOutputId);
		var hiBoxcarRows = document.getElementById(myConsole.hiBoxcarRowsId);
		var hiBoxcarCols = document.getElementById(myConsole.hiBoxcarColsId);
		var loBoxcarRows = document.getElementById(myConsole.loBoxcarRowsId);
		var loBoxcarCols = document.getElementById(myConsole.loBoxcarColsId);

		lowpassButton.addEventListener('click', function() {
			showOriginal.checked = false;
			var stage = myConsole.stage;
			var layers = stage.children;
			for(var i = 0; i < layers.length; i++) {
			    layer = layers[i];
			    if (layer.name != 'mosaic') {
				layer.set({'visible':false});
			    } else {
				layer.set({'visible':true});
			    }
			}
			loBoxcarRows.value = '30';
			loBoxcarCols.value = '30';
			document.getElementById(myConsole.areaChooseId).children[0].children[0].checked=true;
			isisOut.value = "Showing low pass filter mosaic.";
			stage.update();
		    });
		hipassButton.addEventListener('click', function() {
			showOriginal.checked = false;
			var stage = myConsole.stage;
			var layers = stage.children;
			for(var i = 0; i < layers.length; i++) {
			    layer = layers[i];
			    if (layer.name != 'mosaic-inv') {
				layer.set({'visible':false});
			    } else {
				layer.set({'visible':true});
			    }
			}		
			hiBoxcarRows.value = '50';
			hiBoxcarCols.value = '10';
			document.getElementById(myConsole.areaChooseId).children[5].children[0].checked=true;
			isisOut.value = "Showing high pass filter mosaic.";
			stage.update();
		    });
		resultsButton.addEventListener('click', function() {
			if(hiBoxcarRows.value != '' && loBoxcarRows.value != '') {
			    showOriginal.checked = false;
			    var stage = myConsole.stage;
			    var layers = stage.children;
			    for(var i = 0; i < layers.length; i++) {
				layer = layers[i];
				if (layer.name != 'result') {
				    layer.set({'visible':false});
				} else {
				    layer.set({'visible':true});
				}
			    }		
			    document.getElementById(myConsole.areaChooseId).children[6].children[0].checked=true;
			    isisOut.value = "Showing seam removal results";
			    stage.update();
			} else {
			    isisOut.value = "You must perform a high pass filter and a low pass filter first.";
			}
		    });
		var original_visibility = [];
		var original_text = "";
		showOriginal.addEventListener('click', function() {
			var layers = myConsole.stage.children;
			var infoText = "";
			var isisOut = document.getElementById(myConsole.isisOutputId);
			if(showOriginal.checked) {
			    for(var i = 0; i < layers.length; i++) {
				var layer = layers[i];
				original_visibility[i] = layer.visible;
				original_text = isisOut.value;
				// get rid of all filters
				if (layer.name == 'base') {
				    layer.set({'visible':true});
				} else {
				    layer.set({'visible':false});
				}
				if (layer.cacheID != 0) {
				//layer.filters = [];
				    layer.updateCache();
				}
			    }
			    infoText = "Showing original mosaic with seams";
			} else {
			    for(var i = 0; i < layers.length; i++) {
				var layer = layers[i];
				var set_back = original_visibility[i];
				// get rid of all filters
				layer.visible = set_back;
			    }
			    infoText = original_text;
			    
			}
			isisOut.value = infoText;
			myConsole.stage.update();
		    });
		
	    }
	    
	}

	// mouse position
	myConsole.stage.enableMouseOver(10);
	myConsole.stage.on("stagemousemove", function(e) {
		// mouse position
		myConsole.mousePos.innerHTML = "Pixel at: ";
		myConsole.mousePos.innerHTML += parseInt(e.stageX) +", "+  parseInt(e.stageY);
		var pixel = myConsole.ctx.getImageData(e.stageX, e.stageY, 1,1);
		var data = pixel.data;
		myConsole.storedDn.value = data[0] + ", " + data[1] + ", " + data[2];
	    });

    }


    // handle boxes
    this.addMouseListener();
    this.addClickListener();
    this.addResetListener();

};


isisConsole.prototype.addNewBoxesToConsole = function(boxes) {
  this.boxes = boxes;
  this.addMouseListener();
  this.addClickListener();
  this.addResetListener();
};

isisConsole.prototype.addRadioButtonListener = function() {
    var chooser = document.getElementById(this.areaChooseId);
    var c = this;
    chooser.addEventListener('click', function(e) {
	    // discard the label
	    var layersToHide = [];
	    var layersToShow = [];
	    var infoText = "";
	    var isisOut = document.getElementById(c.isisOutputId);
	    var layers = c.stage.children;
	    var grey = c.stage.getChildByName('grey');
	    var mosaic = c.stage.getChildByName('mosaic');
	    var hmosaic = c.stage.getChildByName('mosaic-inv');
	    var base = c.stage.getChildByName('base');
	    var showOriginal = document.getElementById(c.showOriginalId);
	    showOriginal.checked=false;
	    
	    if(e.target.value) {
		var value = e.target.value; 
		if(value.indexOf('area') != -1) { var areaToShow = value.charAt(4); }
		if(c.options.overlaySupport) {
		    switch(value) {
		    case 'area1':
		    case 'area2':
		    case 'area3':
		    case 'area4':
			if((document.getElementById(c.hiBoxcarRowsId).value != '' 
			    && document.getElementById(c.hiBoxcarColsId).value != '') 
			   || c.ctx.globalCompositeOperation == 'overlay' ) {
			    
			    layersToHide = [mosaic, hmosaic, base];
			    layersToShow = [grey];
			    for(var i = 1; i<=4; i++) {
				if(i != areaToShow) {
				    layersToHide.push(c.stage.getChildByName('area' + i));
				    layersToHide.push(c.stage.getChildByName('area' + i + '-base'));
			    } else {
				    layersToShow.push(c.stage.getChildByName('area' + i));
				    layersToShow.push(c.stage.getChildByName('area' + i + '-base'));
				}
			    }
			    c.ctx.globalCompositeOperation = 'overlay';
			    c.stage.updateContext(c.ctx);
			    infoText = "Display area #" + areaToShow;
			} else {
			    infoText = "You must run a high pass filter first";
			}
  		        break;
		    case 'mosaic-inv':
			if((document.getElementById(c.hiBoxcarRowsId).value != '' 
			    && document.getElementById(c.hiBoxcarColsId).value != '') 
			   || c.ctx.globalCompositeOperation == 'overlay' ) {
			    
			    layersToHide = [];
			    layersToShow = [base, hmosaic];
			    for(var i = 0; i<layers.length; i++) {
				layer = layers[i];
				if(layer.name != 'mosaic-inv' && layer.name != 'base') {
				    layersToHide.push(layer);
				}
			    }
			    c.ctx.globalCompositeOperation = 'overlay';
			    c.stage.updateContext(c.ctx);
			    infoText = "Showing high pass filter of the mosaic";
			} else {
			    infoText = "You must run a high pass filter first";
			}
			break;
		    case 'mosaic':
			layersToHide = [];
			layersToShow = [mosaic, base];
			for(var i = 0; i<layers.length; i++) {
			    layer = layers[i];
			if(layer.name != 'mosaic' && layer.name != 'base') {
			    layersToHide.push(layer);
			}
			}
			c.ctx.globalCompositeOperation = 'source-over';
			c.stage.updateContext(c.ctx);
			infoText = "Showing low pass filter of the mosaic";
			break;
		    case 'seam-removal-results':
			layersToHide = [];
			layersToShow = [mosaic, base, hmosaic];
			for(var i = 0; i<layers.length; i++) {
			    layer = layers[i];
			    if(layer.name != 'mosaic' && layer.name != 'base' && layer.name != 'mosaic-inv') {
				layersToHide.push(layer);
			    }
			}
			c.ctx.globalCompositeOperation = 'overlay';
			c.stage.updateContext(c.ctx);
			infoText = "Showing seam removal results";
			break;
		    }
		} else {
		    switch(value) {
		    case 'area1':
		    case 'area2':
		    case 'area3':
		    case 'area4':
			if(document.getElementById(c.hiBoxcarRowsId).value != '' 
			    && document.getElementById(c.hiBoxcarColsId).value != '') {
			    var layers = c.stage.children;
			    layersToHide = [];
			    layersToShow = [];
			    for (var i=0; i<layers.length; i++) {
				var layer = layers[i];
				if (layer.name != 'area'+areaToShow) {
				    layersToHide.push(layer);
				} else {
				    layersToShow.push(layer);
				}
			    }
			    infoText = "Display area #" + areaToShow;
			} else {
			    infoText = "You must run a high pass filter first";
			}
		        break;
		    case 'mosaic-inv':
			if(document.getElementById(c.hiBoxcarRowsId).value != '' 
			    && document.getElementById(c.hiBoxcarColsId).value != '') {
			    var layers = c.stage.children;
			    layersToHide = [];
			    layersToShow = [];
			    for (var i=0; i<layers.length; i++) {
				var layer = layers[i];
				if (layer.name != 'mosaic-inv') {
				    layersToHide.push(layer);
				} else {
				    layersToShow.push(layer);
				}
			    }
			    infoText = "Showing high pass filter of the mosaic.";
			} else {
			    infoText = "You must run a high pass filter first";
			}
			break;
		    case 'mosaic':
			if(document.getElementById(c.loBoxcarRowsId).value != '' 
			    && document.getElementById(c.loBoxcarColsId).value != '') {
			    var layers = c.stage.children;
			    layersToHide = [];
			    layersToShow = [];
			    for (var i=0; i<layers.length; i++) {
				var layer = layers[i];
				if (layer.name != 'mosaic') {
				    layersToHide.push(layer);
				} else {
				    layersToShow.push(layer);
				}
			    }
			    infoText = "Showing low pass filter of the mosaic.";
			} else {
			    infoText = "You must run a low pass filter first.";
			}
			break;
		    case 'seam-removal-results':
			if(document.getElementById(c.loBoxcarRowsId).value != '' 
			   && document.getElementById(c.loBoxcarColsId).value != '' 
			   && document.getElementById(c.hiBoxcarRowsId).value != '' 
			   && document.getElementById(c.hiBoxcarColsId).value != '') {
			    
			    var layers = c.stage.children;
			    layersToHide = [];
			    layersToShow = [];
			    for (var i=0; i<layers.length; i++) {
				var layer = layers[i];
				if (layer.name != 'result') {
				    layersToHide.push(layer);
				} else {
				    layersToShow.push(layer);
				}
			    }
			    infoText = "Showing seam removal results.";
			} else {
			    infoText = "You must run a low pass filter and a high pass filter first.";
			}
			break;
			
		    }
			   
		}
				
		for (var j=0; j<layersToHide.length; j++) {
		    var layer = layersToHide[j];
		    layer.set({'visible':false});
		    if(layer.cacheID != 0) {
			layer.updateCache();
		    }
		}
		for (var k=0; k<layersToShow.length; k++) {
		    var layer = layersToShow[k];
		    layer.set({'visible':true});
		    if(layer.cacheID != 0) {
			layer.updateCache();
		    }
		}
		isisOut.value = infoText; 
		
		c.stage.update();
	    }
	});

}

isisConsole.prototype.addFilterListener = function(filterType) {
    var runButton, blurRows, blurCols, blurFilter, layerToBlur, layerToHide, infoText;
    var myConsole = this;
    var isisOut = document.getElementById(this.isisOutputId);
   
    if(filterType == 'hpf' || filterType == 'hpf-area') {runButton = document.getElementById(myConsole.runHiPassId);}
    else {runButton = document.getElementById(myConsole.runLowPassId);}
    if(myConsole.challenge=='seam-removal' ){ var layersToHide=[]; var layersToBlur = []; var layersToShow=[]; }
    runButton.addEventListener('click', function() {	    
	    // for some reason, getChildByName only works inside addEventListeners
	    // I think it is a 'this' keyword problem
	    var base = myConsole.stage.getChildByName("base");
	    switch(filterType) {
	    case 'hpf':	
		blurRows = document.getElementById(myConsole.hiBoxcarRowsId).value;
		blurCols = document.getElementById(myConsole.hiBoxcarColsId).value;
		layerToBlur = myConsole.stage.getChildByName('hpf');
		layerToHide = myConsole.stage.getChildByName('lpf');
		base.set({'visible':true});	
		myConsole.ctx.globalCompositeOperation = 'overlay';      
		myConsole.stage.updateContext(myConsole.ctx);

		infoText = "Finished High Pass Filter\n\n";
		infoText += "ISIS Command Line:\n";
		infoText += " highpass FROM=" + filtFromFile + " TO=" + HPFtoFile + " FILT=HPF\n" +
		    " LINE=" + blurRows + " SAMP=" + blurCols + " BAND=1\n" ;
		break;
	    case 'lpf':
		// the button we're adding a click listener to
		blurRows = document.getElementById(myConsole.loBoxcarRowsId).value;
		blurCols = document.getElementById(myConsole.loBoxcarColsId).value;
		layerToBlur = myConsole.stage.getChildByName('lpf');
		layerToHide = myConsole.stage.getChildByName('hpf');
		base.set({'visible':false});


		infoText = "Finished Low Pass Filter\n\n";
		infoText += "ISIS Command Line:\n";
		infoText += " lowpass FROM=" + filtFromFile + " TO=" + LPFtoFile +
		    " FILT=LPF\n LINE=" + blurRows + " SAMP=" + blurCols +
		    " BAND=1\n" ;

		break;
	    case 'hpf-area':
		blurRows = document.getElementById(myConsole.hiBoxcarRowsId).value;
		blurCols = document.getElementById(myConsole.hiBoxcarColsId).value;
		// set everything but base, grey and blur to visible
		var mosaic = myConsole.stage.getChildByName('mosaic');
		var mosaic_i = myConsole.stage.getChildByName('mosaic-inv');
	       	var base = myConsole.stage.getChildByName('base');
		var grey = myConsole.stage.getChildByName('grey');
		var areaB = myConsole.stage.getChildByName('area1-base');
		var area = myConsole.stage.getChildByName('area1');
		layersToShow = [grey, area, areaB];
		layersToHide = [mosaic, mosaic_i, base];
		layersToBlur = [mosaic_i, area];
		for(var i=1; i<=4; i++){
		    layersToBlur.push(myConsole.stage.getChildByName('area' + i));
		    if(i != 1) {
			layersToHide.push(myConsole.stage.getChildByName('area' + i));
			layersToHide.push(myConsole.stage.getChildByName('area' + i + '-base'));
		    }		
		}

		infoText = "Showing high pass filter of area #1";
		myConsole.ctx.globalCompositeOperation = 'overlay';      
		myConsole.stage.updateContext(myConsole.ctx);
		document.getElementById(myConsole.areaChooseId).children[1].children[0].checked=true;
		break;
	    case 'lpf-mosaic':
		blurRows = document.getElementById(myConsole.loBoxcarRowsId).value;
		blurCols = document.getElementById(myConsole.loBoxcarColsId).value;
		var mosaic = myConsole.stage.getChildByName('mosaic');
		var mosaic_i = myConsole.stage.getChildByName('mosaic-inv');
		var base = myConsole.stage.getChildByName('base');
		layersToBlur = [mosaic];
		layersToShow = [mosaic, base];
		layersToHide = [];
		for (var i=0; i < myConsole.stage.children.length; i++) {
		    var layer = myConsole.stage.children[i];
		    if(layer.name != 'mosaic' && layer.name != 'base') {	
			layersToHide.push(layer);
		    }
		}
		infoText = "Showing low pass filter of the mosaic";
		myConsole.ctx.globalCompositeOperation = 'source-over';      
		myConsole.stage.updateContext(myConsole.ctx);
		document.getElementById(myConsole.areaChooseId).children[0].children[0].checked=true;
		break;
	    }
	    
	    var showOriginal = document.getElementById(myConsole.showOriginalId);
	    showOriginal.checked = false;
	    blurFilter = new createjs.BlurFilter(blurCols, blurRows, 1);
	    if (myConsole.challenge == 'seam-removal') {
		
		// do stuff
		for (var j = 0; j<layersToBlur.length; j++) {
		    var layer = layersToBlur[j];
		    layer.filters = [blurFilter];
		}
		for (var k = 0; k<layersToHide.length; k++) {
		    var layer = layersToHide[k];
		    layer.set({'visible':false});
		}
		for (var l = 0; l<layersToShow.length; l++) {
		    var layer = layersToShow[l];
		    layer.set({'visible':true});
		 
		}
		for (var m = 0; m<myConsole.stage.children.length; m++) {
		    var layer = myConsole.stage.children[m];
		    if (layer.cacheID != 0) {
			
			layer.updateCache();
		    }
		}
	    } else {
		layerToHide.set({'visible':false});
		layerToBlur.set({'visible':true});
	   	layerToBlur.filters = [blurFilter];
	   	layerToBlur.updateCache();
		layerToHide.updateCache();
	    }
	    myConsole.stage.update();
	    isisOut.value = infoText;

	});

}


isisConsole.prototype.addClickListener = function() {
  var boxes = this.boxes;
  var c = this;
    $.each(boxes, function( i,v ) {
	    var b = boxes[i];
	    b.canvas.addEventListener( 'click', function(e) {
			       c.toggleActive(b);
			       // change the value of the slider to match the dn of the box
			       $("#" + c.sliderValId).val('DN: ' + b.selection.dn);
			       $("#" + c.sliderId).slider('value', b.selection.dn.toString());
			     });

	    });
};


isisConsole.prototype.addSpecialPixelsLegend = function() {
    var nul = document.createElement('div');
    var lrs = document.createElement('div');
    var lis = document.createElement('div');
    var hrs = document.createElement('div');
    var his = document.createElement('div');
    
    nul.setAttribute('class', 'legend');
    lrs.setAttribute('class', 'legend');
    lis.setAttribute('class', 'legend');
    hrs.setAttribute('class', 'legend');
    his.setAttribute('class', 'legend');

    nul.style.backgroundColor = '#bf0000';
    lrs.style.backgroundColor = '#3f3fff';
    lis.style.backgroundColor = '#007f00';
    hrs.style.backgroundColor = '#7fffff';
    his.style.backgroundColor = '#ffff7f';
    
    nul.innerHTML = "NUL";
    lrs.innerHTML = "LRS";
    lis.innerHTML = "LIS";
    hrs.innerHTML = "HRS";
    his.innerHTML = "HIS";
    
    this.displayRight.appendChild(nul);
    this.displayRight.appendChild(lrs);
    this.displayRight.appendChild(lis);
    this.displayRight.appendChild(hrs);
    this.displayRight.appendChild(his);
}

isisConsole.prototype.addMouseListener = function() {

  var boxes = this.boxes;
  var c = this;
  $.each(boxes, function( i,v ) {
	    var b = boxes[i];
	    
	    b.canvas.addEventListener( 'mousemove', function(e) {
		    // print the mouse position
		    var x = b.relMousePos(e, c.subpixels).x, y = b.relMousePos(e, c.subpixels).y;
		    c.mousePos.innerHTML = 'Pixel at: ';
		    c.mousePos.innerHTML +=  x + ", " + y;
		    // get the DN at the mouse position
		    var mx = b.getMousePos(e).x;
		    var my = b.getMousePos(e).y;
		    $.each( b.pixels, function(i,v) {
			    if (b.pixels[i].contains(mx, my)) {
				var dn = b.pixels[i].dn;
				c.storedDn.value = (dn.toString().length <= 3) ? dn : "";
			    }
		    });
	    });
  });
};


// reset boxes associated with this console
isisConsole.prototype.addResetListener = function() {

   var c = this;
   if(c.boxes.length || c.image) {
       this.reset.addEventListener( 'click', function(e) {
	       $.each(c.boxes, function ( i,v ) {
		       var b = c.boxes[i];
		       if (b.type == 'special') {
			   b.colorized = !b.colorized;
			   if(b.colorized) {
			       c.reset.innerHTML = "Decolorize";
			   } else {
			       c.reset.innerHTML = "Colorize";
			   }
		       }
		       b.init();
		   });
	       if ( c.showDnMultiplier ) {
		   // set 2 sliders to base
		   $("#" + c.dnMultiplierValId).val('Multiplier: 1.0');
		   $("#" + c.dnBaseValId).val('Base: 0');
		   $("#" + c.dnMultiplierId).slider('value', 1.0);
		   $("#" + c.dnBaseId).slider('value', 0);
	       }
	       if ( c.slider ) {
		   // set the dn slider to base
		   $("#" + c.sliderValId).val('DN: ' + 255);
		   $("#" + c.sliderId).slider('value', 255);
	       }
	   });
   } else if (c.turnOnFilters) {
       if(c.challenge == 'destripe') {
	   c.reset.addEventListener( 'click', function(e) {
		   document.getElementById(c.isisOutputId).value = "";
		   document.getElementById(c.showOriginalId).checked = false;
		   document.getElementById(c.hiBoxcarRowsId).value = "";
		   document.getElementById(c.hiBoxcarColsId).value = "";
		   document.getElementById(c.loBoxcarRowsId).value = "";
		   document.getElementById(c.loBoxcarColsId).value = "";
			  
		   var lpf  = c.stage.getChildByName('lpf');
		   var hpf = c.stage.getChildByName('hpf');
		   if(c.options.overlaySupport) { 
		       lpf.filters = [];
		       hpf.filters = [];
		       hpf.set({'visible':false});
		       lpf.updateCache();
		       hpf.updateCache();
		       c.ctx.globalCompositeOperation = 'source-over';
		       c.stage.updateContext(c.ctx);
		       c.stage.update();
		   } 
		   else {
		       var base = c.stage.getChildByName('base');
		       var fin = c.stage.getChildByName('final');
		       lpf.set({'visible':false});
		       hpf.set({'visible':false});
		       base.set({'visible':true});
		       fin.set({'visible':false});
		       c.stage.update();
		   }

	       });
       } else if (c.challenge == 'seam-removal'){
	   c.reset.addEventListener( 'click', function(e) {
		   document.getElementById(c.isisOutputId).value = "";
		   document.getElementById(c.showOriginalId).checked = false;
		   document.getElementById(c.hiBoxcarRowsId).value = "";
		   document.getElementById(c.hiBoxcarColsId).value = "";
		   document.getElementById(c.loBoxcarRowsId).value = "";
		   document.getElementById(c.loBoxcarColsId).value = "";
		   
		   var layers = c.stage.children;
		   if(c.options.overlaySupport) { 
		       for(var i = 0; i < layers.length; i++) {
			   var layer = layers[i];
			   // get rid of all filters
			   if (layer.name == 'mosaic-inv') {
			       layer.set({'visible':false});
			   } 
			   else {
			       layer.set({'visible':true});
			   }
			   if (layer.cacheID != 0) {
			       layer.filters = [];
			       layer.updateCache();
			   }
		       }
		       c.ctx.globalCompositeOperation = 'source-over';
		       c.stage.updateContext(c.ctx);
		       c.stage.update();
		   }
		   else {
		       for (var i = 0; i < layers.length; i++) {
			   var layer = layers[i];
			   if (layer.name != "base") {
			       layer.set({'visible':false});
			   } else {
			       layer.set({'visible':true});
			   }
		       }
		       var message = "Please upgrade to a modern browser, such as Firefox or Chrome, in order to enable input.";
		       document.getElementById(c.isisOutputId).value = message;
		       c.stage.update();
		   }
	       });
      

       }
   }
}


isisConsole.prototype.toggleActive = function(b) {
    var boxes = this.boxes;
    $.each( boxes, function(i,v) {
	    matchbox = boxes[i];
	    if( !(b == matchbox) ) {
		matchbox.deactivate();
	    }
     });
    this.activeBox = b.activate();
}

isisConsole.prototype.initInputIds = function() {
    this.showOriginalId = this.options.target + '-showOriginalCheck';
    this.resultsId = this.options.target + '-results';
    this.hiBoxcarRowsId = this.options.target + '-hiBoxcarRows';
    this.hiBoxcarColsId = this.options.target + '-hiBoxcarCols';
    this.loBoxcarRowsId = this.options.target + '-loBoxcarRows';
    this.loBoxcarColsId = this.options.target + '-loBoxcarCols';
    this.runLowPassId = this.options.target + '-runLowPass';
    this.runHiPassId = this.options.target + '-runHiPass';
    this.isisOutputId = this.options.target + '-isisOut';
    if (this.challenge == 'seam-removal') {
	this.areaChooseId = this.options.target + '-chooseArea'
	this.showAreaName = this.options.target + '-show';
    }
}

isisConsole.prototype.addChallengeControls = function() {
    var loBoxcar = document.createElement('div');
    var hiBoxcar = document.createElement('div');
    var boxcarLabels = document.createElement('div');
    loBoxcar.setAttribute('class', 'boxcarContainer');
    hiBoxcar.setAttribute('class', 'boxcarContainer');
    boxcarLabels.setAttribute('class', 'boxcarContainer');
    
    var colLabel = document.createElement('label');
    colLabel.appendChild(document.createTextNode('Columns'));
    var rowLabel = document.createElement('label');
    rowLabel.appendChild(document.createTextNode('Rows'));
    
    
    var loBoxcarRows = document.createElement('input');
    loBoxcarRows.setAttribute('type', 'text');
    loBoxcarRows.setAttribute('id', this.loBoxcarRowsId);
    var hiBoxcarRows = document.createElement('input');
    hiBoxcarRows.setAttribute('type', 'text');
    hiBoxcarRows.setAttribute('id', this.hiBoxcarRowsId);
    var loBoxcarCols = document.createElement('input');
    loBoxcarCols.setAttribute('type', 'text');
    loBoxcarCols.setAttribute('id', this.loBoxcarColsId);
    var hiBoxcarCols = document.createElement('input');
    hiBoxcarCols.setAttribute('type', 'text');
    hiBoxcarCols.setAttribute('id', this.hiBoxcarColsId);
    var submitLPF = document.createElement('button');
    submitLPF.setAttribute('id', this.runLowPassId);
    submitLPF.appendChild(document.createTextNode('Low Pass'));
    var submitHPF = document.createElement('button');
    submitHPF.setAttribute('id', this.runHiPassId);
    submitHPF.appendChild(document.createTextNode('Hi Pass'));
    var isisOutput = document.createElement('textarea');
    isisOutput.setAttribute('id', this.isisOutputId);
    isisOutput.style.width = "90%";
    isisOutput.setAttribute('rows', 6);
    isisOutput.readOnly = true;
    
    var showOriginalContainer = document.createElement('div');
    var showOriginal = document.createElement('input');
    showOriginal.type = 'checkbox';
    showOriginal.id = this.showOriginalId;
    var showOriginalLabel = document.createElement('label');
    showOriginalLabel.innerHTML = "Show Original";
    showOriginalLabel.style.fontWeight = "bold";
    showOriginalContainer.appendChild(showOriginal);
    showOriginalContainer.appendChild(showOriginalLabel);
  
    
    loBoxcar.appendChild(loBoxcarRows);
    loBoxcar.appendChild(loBoxcarCols);
    loBoxcar.appendChild(submitLPF);

    hiBoxcar.appendChild(hiBoxcarRows);
    hiBoxcar.appendChild(hiBoxcarCols);
    hiBoxcar.appendChild(submitHPF);

    boxcarLabels.appendChild(rowLabel);
    boxcarLabels.appendChild(colLabel);

    var addImagesButton = document.createElement('button');
    addImagesButton.appendChild(document.createTextNode('Add Images'));
    addImagesButton.setAttribute('id', this.resultsId);


    this.displayRight.appendChild(showOriginalContainer);
    this.displayRight.appendChild(boxcarLabels);
    this.displayRight.appendChild(loBoxcar);
    this.displayRight.appendChild(hiBoxcar);
    this.displayRight.appendChild(addImagesButton);
    if(this.challenge == 'seam-removal') {
	var radioButtons = document.createElement('div');
	radioButtons.id = this.areaChooseId;

	var area1 = makeRadioButton(this.showAreaName, 'area1', 'HPF Area #1');
	var area2 = makeRadioButton(this.showAreaName, 'area2', 'HPF Area #2');
	var area3 = makeRadioButton(this.showAreaName, 'area3', 'HPF Area #3');
	var area4 = makeRadioButton(this.showAreaName, 'area4', 'HPF Area #4');
	var lpfm  = makeRadioButton(this.showAreaName, 'mosaic', 'LPF Mosaic');
	var hpfm  = makeRadioButton(this.showAreaName, 'mosaic-inv', 'HPF Mosaic');
	var res   = makeRadioButton(this.showAreaName, 'seam-removal-results', 'Results');
	
	radioButtons.appendChild(lpfm);
	radioButtons.appendChild(area1);
	radioButtons.appendChild(area2);
	radioButtons.appendChild(area3);
	radioButtons.appendChild(area4);
	radioButtons.appendChild(hpfm);
	radioButtons.appendChild(res);

	this.displayRight.appendChild(radioButtons);
    }
    this.displayRight.appendChild(isisOutput);

    if(!this.options.overlaySupport) {
	loBoxcarRows.readOnly = true;
	hiBoxcarRows.readOnly = true;
	loBoxcarCols.readOnly = true;
	hiBoxcarCols.readOnly = true;
	
	var message = "Please upgrade to a modern browser, such as Firefox or Chrome, in order to enable input.";
	isisOutput.value = message;
    }


}

isisConsole.prototype.initCubeDemoButtons = function() {
    var c = this;
    var container = document.createElement('div');
    container.setAttribute('class', 'range');
    var addBand = createButton('add-band','+band');
    var removeBand = createButton('remove-band', '-band');
    var addLine = createButton('add-line','+line');
    var removeLine = createButton('remove-line', '-line');
    var addSample = createButton('add-sample', '+sample');
    var removeSample = createButton('remove-sample', '-sample');
   
    container.appendChild(addBand);
    container.appendChild(removeBand);
    container.appendChild(addLine);
    container.appendChild(removeLine);
    container.appendChild(addSample);
    container.appendChild(removeSample);
    
    c.displayRight.appendChild(container);
}

function createButton(id, text) {
    var button = document.createElement('button');
    button.appendChild(document.createTextNode(text));
    button.setAttribute('id', id);
    
    return button;
     
}

isisConsole.prototype.initSlider = function(func) {
    var sliderContainer = document.createElement('div');
    sliderContainer.setAttribute('class', 'sliderContainer');
    var slider = document.createElement('div');
    var sliderVal = document.createElement('input');
    var myConsole = this;
    // slider styles
    slider.style.width = '70px';
    slider.style.height = '8px';
    slider.style.borderRadius = '0';
    slider.style.display = 'inline-block';
    slider.style.margin = '5px';
    sliderVal.readOnly = true;
    sliderVal.style.width = '100%';

    //put the slider on the display
    sliderContainer.appendChild(sliderVal);
    sliderContainer.appendChild(slider);

    myConsole.displayRight.appendChild(sliderContainer);

    var identifier, slideFunction, mini, maxi, initValue, step;
    switch(func) {
    case 'change':
	sliderVal.setAttribute('id', myConsole.sliderValId);
	slider.setAttribute('id', myConsole.sliderId);
	sliderVal.value = "DN: 255";
	slideFunction = function( event, ui ) {
	    $( "#" + myConsole.sliderValId ).val( "DN: " + ui.value );
	    myConsole.activeBox.changeSelected(ui.value);
	};
	identifier = myConsole.sliderId;
	initValue = 255;
	mini = 0;
	maxi = 255;
	step = 1;
	break;
    case 'base':
	sliderVal.setAttribute('id', myConsole.dnBaseValId);
	slider.setAttribute('id', myConsole.dnBaseId);
	sliderVal.value = "Base: 0";
	slideFunction = function( event, ui ) {
	    $( "#" + myConsole.dnBaseValId ).val( "Base: " + ui.value );
	};
	identifier = myConsole.dnBaseId;
	initValue = 0;
	mini = -500;
	maxi = 500;
	step = 1;
	break;
    case 'multiplier':
	sliderVal.setAttribute('id', myConsole.dnMultiplierValId);
	slider.setAttribute('id', myConsole.dnMultiplierId);
	sliderVal.value = "Multiplier: 1.0";
	slideFunction = function( event, ui ) {
	    $("#" + myConsole.dnMultiplierValId).val( "Multiplier: " + ui.value );
	};
	identifier = myConsole.dnMultiplierId;
	initValue = 1;
	mini = 1;
	maxi = 255;
	step = 0.1;
	break;
    }

    //initialize the slider
    $('#' + identifier).slider({
	        range: "min",
		value: initValue,
		slide: slideFunction,
		min: mini,
		max: maxi,
		step:step

     });

    // handle styles
    $("#" + identifier).find(".ui-slider-handle").css({
	    "height":".8em",
            "width":".8em",
            "top": "-.2em",
            "margin-left":"-.4em"
    });
}


/* most box code yoinked from https://github.com/simonsarris/Canvas-tutorials/blob/master/shapes.js#L186
 * author Emily Bartman
 *
 * DEPENDENCIES
 * - pixel.js
 * - isisConsole.js
 *
 * @param options - a hash of options for the box
 *                  target: id of the app container
 *
 */
var box = function(options) {

    this.options = options || {};
    var id = this.options.target;
    var div = document.getElementById(id);

    var canvas = document.createElement('canvas');
    canvas.height = this.options.height;
    canvas.width = this.options.width;
    canvas.style.top = this.options.top + 'px' || 0;
    canvas.style.left = this.options.left + 'px' || 0;
    canvas.style.zIndex = 0;
    div.appendChild(canvas);

    this.canvas = (this.options.canvas) ? this.options.canvas : canvas;
    this.origHeight = this.canvas.height;
    this.origWidth = this.canvas.width;
    this.pixelSize = this.options.pixelSize;
    this.type = this.options.type; // gradient, sample, line
    if (this.type == 'special') {
	this.colorized = false;
    }
    this.ctx = this.canvas.getContext('2d');
    this.id = this.options.id; // just an optional identifier for debugging

    // this is for lines, which may have been scrapped
    this.rows = this.options.rows || 4;
    this.columns = this.options.columns || 4;


    this.orientation = this.options.orientation || "top";


    this.pixels = [];
    this.active = false;

    this.visible = true; // future use

    /* Handle mouse position */
    var stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;
    if (document.defaultView && document.defaultView.getComputedStyle) {
       this.stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['paddingLeft'], 10)      || 0;
       this.stylePaddingTop  = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['paddingTop'], 10)       || 0;
       this.styleBorderLeft  = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['borderLeftWidth'], 10)  || 0;
       this.styleBorderTop   = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['borderTopWidth'], 10)   || 0;
    }

    var html = document.body.parentNode;
    this.htmlTop = html.offsetTop;
    this.htmlLeft = html.offsetLeft;

    this.selectionColor = '#CC0000';
    this.selectionWidth = 2;

    var myState = this;

    /* Events
     * mousedown: a pixel has been selected, set redraw to true to set a stroke on the pixel
     */

    this.canvas.addEventListener('mousedown', function(e) {
        var mouse = myState.getMousePos(e);
	var mx = mouse.x;
    	var my = mouse.y;
    	var pixels = myState.pixels;
    	var l = pixels.length;
    	for (var i = l-1; i >= 0; i--) {
      	    if (pixels[i].contains(mx, my)) {
               var mySel = pixels[i];
               myState.selection = mySel;
	       myState.redraw = true;
	       return;
      	    }
      	}
      	if (myState.selection) {
      	 myState.selection = null;
	 myState.redraw = true;
      	}
    }, true);


    /* Function to draw the original box */
    this.init();

    /* This function only does something if redraw is set to true */
    if(this.visible){setInterval( function() {myState.plotPixels()}, 30);}
}


box.prototype.init = function() {
    this.clear();
    this.selection = null;
    switch(this.type) {
    		 case "gradient":
        	      this.printGradient();
		      break;
                 case "lines":
		      this.printLines(this.rows, this.columns, this.orientation);
		      break;
                 case "special":
		      this.printSpecialPixels(this.colorized);
		      break;
    }
    this.redraw = true;
}

box.prototype.addLine = function () {
    if(this.rows<16) {
	this.rows += 1;
    }
    this.init();
}

box.prototype.addSample = function () {
    if(this.columns<16) {
	this.columns += 1;

    }
    this.init();
}

box.prototype.removeLine = function () {
    if(this.rows>4) {
	this.rows -= 1;
    }
    this.init();
}

box.prototype.removeSample = function () {
    if(this.columns>4) {
	this.columns -= 1;

    }
    this.init();
}

box.prototype.activate = function () {
    this.active = true;
    this.canvas.style.zIndex = 5;
    return this;
}

box.prototype.deactivate = function () {
    this.active = false;
    this.canvas.style.zIndex = 0;
    this.init();
}

box.prototype.getSelection = function() {
    if(this.selection) {
      return this.selection;
    } else {
      return false;
    }
}

box.prototype.setOffset = function() {
    this.canvas.style.top = 0;
    this.canvas.style.left = 0;
}

box.prototype.getOffset = function() {
    return {x: this.canvas.offsetLeft, y: this.canvas.offsetTop};
}

box.prototype.changeSelected = function(dn) {
    this.redraw = true;
    var s = this.getSelection();
    s.fill = "rgb("+dn+","+dn+","+dn+")";
}

box.prototype.getPixels = function() {
    return this.pixels;
}

box.prototype.addPixel = function(pixel) {
    this.pixels.push(pixel);
}

// method should only be used privately
box.prototype.getMousePos = function(event) {
    var element = this.canvas;
    var offsetX=0, offsetY=0;
    var mx,my;

    if (element.offsetParent !== undefined) {
       do {
       	  offsetX += element.offsetLeft;
          offsetY += element.offsetTop;
    	  } while ((element = element.offsetParent));
    }

    offsetX += this.stylePaddingLeft + this.styleBorderLeft + this.htmlLeft;
    offsetY += this.stylePaddingTop + this.styleBorderTop + this.htmlTop;

    mx = event.pageX - offsetX;
    my = event.pageY - offsetY;

    return {x: mx, y: my};
}

/* returns the grid mouse position, for printing to the console
 * @param event e A javascript event; bind this method with an event listener
 * @param bool subpixels TRUE will return a pair of floating points
 */
box.prototype.relMousePos = function(e, subpixels) {
    var show_subpixels = false || subpixels;
    pixelSize = this.pixelSize;

    mx = this.getMousePos(e).x;
    my = this.getMousePos(e).y;

    if (!show_subpixels) {
    	mx = Math.ceil(mx/pixelSize);
    	my = Math.ceil(my/pixelSize);
    } else {
      	mx = (mx/pixelSize + .5).toFixed(2);
	my = (my/pixelSize + .5).toFixed(2);
	if (mx < .5) {mx = .5;}
	if (my < .5) {my = .5;}
	if (mx > 8.5) {mx = 8.5;}
	if (my > 8.5) {my = 8.5;}
    }

    return {x: mx, y: my};
}

box.prototype.clear = function() {
  this.ctx.clearRect(0,0,this.canvas.width,this.canvas.height);
  this.pixels = [];
}

box.prototype.printGradient = function(orientation) {
    var pixelSize = this.pixelSize;
    var ctx = this.ctx;
    var diff = 255/((this.canvas.width/pixelSize + this.canvas.height/pixelSize) - 1);

    for( var i=0; i < this.canvas.width/pixelSize; i++) {
         for( var j=0; j < this.canvas.height/pixelSize; j++) {
      	     var fill = Math.floor( (255 - j*diff) - i*diff );
      	     var x = i*pixelSize;
      	     var y = j*pixelSize;
	     var p = new pixel(x, y, pixelSize, fill);
      	     this.addPixel(p);
    	  }
    }
}



box.prototype.printSpecialPixels = function(colorized) {
    var HIS = ["0,0","1,0","2,0","0,1","0,2","0,3"];
    var NUL = ["6,0","7,2","0,5","2,6"];
    var LRS = ["6,1","6,6","7,6","6,7","7,7"];
    var LIS = ["7,5","5,7"];
    var HRS = ["3,0","2,1","1,2"];

    var pixelSize = this.pixelSize;
    var ctx = this.ctx;
    var diff = 255/((this.canvas.width/pixelSize + this.canvas.height/pixelSize) - 1);

    for( var i=0; i < this.canvas.width/pixelSize; i++) {
         for( var j=0; j < this.canvas.height/pixelSize; j++) {
	     var fill;
	     var xy = i + "," + j;
	     if (colorized) { 
		 
		 if (HIS.indexOf(xy) > -1) { fill = ["255","255", "127"]; }
		 else if (NUL.indexOf(xy) > -1) { fill = ["191", "0", "0"] }
		 else if (LRS.indexOf(xy) > -1) { fill = ["63", "63", "255"] }
		 else if (LIS.indexOf(xy) > -1) { fill = ["0", "127", "0"] }
		 else if (HRS.indexOf(xy) > -1) { fill = ["127", "255", "255"] }
		 else { fill = Math.floor( (255 - j*diff) - i*diff ); }
 	     } else {
		 if (HIS.indexOf(xy) > -1 || HRS.indexOf(xy) > -1) { fill = "255"; }
		 else if (NUL.indexOf(xy) > -1 || LRS.indexOf(xy) > -1 || LIS.indexOf(xy) > -1) { fill = "0" }
		 else { fill = Math.floor( (255 - j*diff) - i*diff ); }
	     }
      	     var x = i*pixelSize;
      	     var y = j*pixelSize;
	     var p = new pixel(x, y, pixelSize, fill);
      	     this.addPixel(p);
    	  }
    }
}

box.prototype.printLines = function(r, c, orientation) {
    var ctx = this.ctx;
    var rows = ( r > 16 ) ? 16 : r;
    var columns = ( c > 16 ) ? 16 : c;
    var thisBox = this;

    // determine new pixel size;
    if ( rows > columns ) {
	var pixelSize = Math.ceil(thisBox.origHeight/rows);
    } else {
	var pixelSize = Math.ceil(thisBox.origWidth/columns);
    }
    this.pixelSize = pixelSize;

    this.canvas.height = rows*pixelSize;
    this.canvas.width = columns*pixelSize;


    for( var i=0; i < columns; i++ ) {
	for ( var j=0; j < rows; j++ ) {
	    switch(orientation) {
	    case 'top':
		var fill = Math.floor((15 + (16*j)));
		var x = i * pixelSize;
		var y = j * pixelSize;
	        var p = new pixel(x, y, pixelSize, fill);
		this.addPixel(p);
		break;
	    case 'left':
		var fill = Math.floor((15 + (16*i)));
	        var x = i * pixelSize;
		var y = j * pixelSize;
	        var p = new pixel(x, y, pixelSize, fill);
		this.addPixel(p);
		break;
	    case 'bottom':
		var fill = Math.floor((255 - (16*j)));
		var x = i * pixelSize;
		var y = j * pixelSize;
	        var p = new pixel(x, y, pixelSize, fill);
		this.addPixel(p);
		break;
	    case 'right':
		var fill = Math.floor((255 - (16*i)));
		var x = i * pixelSize;
		var y = j * pixelSize;
	        var p = new pixel(x, y, pixelSize, fill);
		this.addPixel(p);
		break;
	    }
	}
    }
}

// the method that draws the grid
box.prototype.plotPixels = function() {
    if( this.redraw ) {
        for( var i = 0; i < this.pixels.length; i++){
            var pixel = this.pixels[i];
	    pixel.plot(this.ctx);
    	}

    	if ( this.selection != null) {
       	  this.ctx.strokeStyle = this.selectionColor;
       	   this.ctx.lineWidth = this.selectionWidth;
       	   var mySel = this.selection;
      	  this.ctx.strokeRect(mySel.x+1,mySel.y+1,mySel.w-2,mySel.h-2);
    	}
    }
    this.redraw = false;
}

var pixel = function(x, y, s, dn) {
    this.x = x || 0;
    this.y = y || 0;
    this.w = s || 25;
    this.h = s || 25;
    if (dn instanceof Array) {
	this.fill = 'rgb(' + dn[0] + ',' + dn[1] + ',' + dn[2] + ')';
	if(this.fill == 'rgb(255,255,127)') {
	    this.dn = 'HIS';
	} else if (this.fill == 'rgb(191,0,0)') {
	    this.dn = 'NUL';
	} else if (this.fill == 'rgb(63,63,255)') {
	    this.dn = 'LRS';
	} else if (this.fill == 'rgb(0,127,0)') {
	    this.dn = 'LIS';
	} else if (this.fill == 'rgb(127,255,255)') {
	    this.dn = 'HRS';
	}
    } else { 
	this.fill = 'rgb(' + dn + ',' + dn + ','  + dn + ')' || "rgb(0,0,0)";
	this.dn = dn;
    }

}

pixel.prototype.plot = function(ctx, selected) {
    ctx.fillStyle = this.fill;
    ctx.fillRect(this.x, this.y, this.w, this.h);  
}

pixel.prototype.contains = function(mx, my) {
  return  (this.x <= mx) && (this.x + this.w >= mx) &&
          (this.y <= my) && (this.y + this.h >= my);
}

var staticImage = function(options) {
    
    this.options = (options) ? options : {};
    var id = this.options.target;
    var div = document.getElementById(id);

    this.canvas = document.createElement('canvas');
    this.canvas.height = this.options.height;
    this.canvas.width = this.options.width;    
    this.ctx = this.canvas.getContext('2d');
    
    /* Handle mouse position */
    var stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;
    if (document.defaultView && document.defaultView.getComputedStyle) {
       this.stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['paddingLeft'], 10)      || 0;
       this.stylePaddingTop  = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['paddingTop'], 10)       || 0;
       this.styleBorderLeft  = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['borderLeftWidth'], 10)  || 0;
       this.styleBorderTop   = parseInt(document.defaultView.getComputedStyle(this.canvas, null)['borderTopWidth'], 10)   || 0;
    }
    
    var html = document.body.parentNode;
    this.htmlTop = html.offsetTop;
    this.htmlLeft = html.offsetLeft;


    var myLayer = this;
    var img = new Image();
    img.src = this.options.src;
    img.onload = function () {
	myLayer.ctx.drawImage(img, 0, 0);
	img.style.display = 'none';
    }
    div.appendChild(this.canvas);   
    
}

staticImage.prototype.getMousePos = function(event) {
    var element = this.canvas;
    var offsetX=0, offsetY=0;
    var mx,my;
 
    if (element.offsetParent !== undefined) {
       do {
       	  offsetX += element.offsetLeft;
          offsetY += element.offsetTop;
    	  } while ((element = element.offsetParent));
    }

    offsetX += this.stylePaddingLeft + this.styleBorderLeft + this.htmlLeft;
    offsetY += this.stylePaddingTop + this.styleBorderTop + this.htmlTop;    

    mx = event.pageX - offsetX;
    my = event.pageY - offsetY;  
    
    return {x: mx, y: my};
}

function makeRadioButton(name, value, text) {
    
    var label = document.createElement("label");
    var radio = document.createElement("input");
    radio.type = "radio";
    radio.name = name;
    radio.value = value;
    
    label.appendChild(radio);
    
    label.appendChild(document.createTextNode(text));
    return label;
}


function init() {
    img = new Image();
    img.src = 'themes/astro/images/stripe.jpg';
    
    // inverted image
    imgi = new Image();
    imgi.onload = handleImageLoad;
    imgi.src = 'themes/astro/images/stripe-inverted.jpg';
}

function initBasic() {
    imgb = new Image();
    imgb.src = 'themes/astro/images/stripe.jpg';
    imgh = new Image();
    imgh.src = 'themes/astro/images/Striping_Highpass.jpg';
    imgl = new Image();
    imgl.src = 'themes/astro/images/Striping_Lowpass.jpg';
    imgf = new Image();
    imgf.onload = handleBasicImageLoad;
    imgf.src = 'themes/astro/images/Striping_Final.jpg';

}

function handleImageLoad() {
    bmp_low = new createjs.Bitmap(img);
    bmp_hi = new createjs.Bitmap(imgi);
    bmp_bg = new createjs.Bitmap(img);

    bmp_low.cache(0, 0, img.width, img.height);
    bmp_hi.cache(0, 0, imgi.width, imgi.height); 
   
    bmp_bg.set({'name':'base'});
    bmp_low.set({'name':'lpf'});
    bmp_hi.set({'name':'hpf'});
    
    bmp_hi.visible = false;
    
    stage.addChild(bmp_bg);
    stage.addChild(bmp_low);
    stage.addChild(bmp_hi);
    
    stage.update();
}

function handleBasicImageLoad() {
    base = new createjs.Bitmap(imgb);
    hi = new createjs.Bitmap(imgh);
    low = new createjs.Bitmap(imgl);
    fin = new createjs.Bitmap(imgf);

    base.set({'name':'base'});
    low.set({'name':'lpf'});
    hi.set({'name':'hpf'});
    fin.set({'name':'final'});
    
    low.visible = false;
    hi.visible = false;
    fin.visible = false;
    
    stage.addChild(base);
    stage.addChild(low);
    stage.addChild(hi);
    stage.addChild(fin);
    
    stage.update();

}

function initSeams() {
    area1 = new Image();
    area2 = new Image();
    area3 = new Image();
    area4 = new Image();
    area1Base = new Image();
    area2Base = new Image();
    area3Base = new Image();
    area4Base = new Image();
    mosaic = new Image();
    mosaici = new Image();
    original = new Image();
    grey = new Image();
    
    area1.src = 'themes/astro/images/area1.png';
    area2.src = 'themes/astro/images/area2.png';
    area3.src = 'themes/astro/images/area3.png';
    area4.src = 'themes/astro/images/area4.png';
    mosaic.src = 'themes/astro/images/mos1234.jpg';
    area1Base.src = 'themes/astro/images/area1-base.png';
    area2Base.src = 'themes/astro/images/area2-base.png';
    area3Base.src = 'themes/astro/images/area3-base.png';
    area4Base.src = 'themes/astro/images/area4-base.png';
    mosaic.src = 'themes/astro/images/mos1234.jpg';
    mosaici.src = 'themes/astro/images/mos1234-inverted.jpg';
    original.src = 'themes/astro/images/mos1234.jpg';
    grey.src = 'themes/astro/images/grey.png';
    original.onload = handleSeamsImageLoad;

}

function initBasicSeams() {
    area1 = new Image();
    area2 = new Image();
    area3 = new Image();
    area4 = new Image();
    result = new Image();
    mosaic = new Image();
    mosaici = new Image();
    original = new Image();

    area1.src = 'images/area1-basic.png';
    area2.src = 'images/area2-basic.png';
    area3.src = 'images/area3-basic.png';
    area4.src = 'images/area4-basic.png';
    result.src = 'images/result.jpg';
    mosaic.src = 'images/lpf-mosaic.jpg';
    mosaici.src = 'images/hpf-mosaic.jpg';
    original.src = 'images/mos1234.jpg';

    original.onload = handleBasicSeamsImageLoad;
}

function handleSeamsImageLoad() {
    area1b = new createjs.Bitmap(area1); // blur
    area1o = new createjs.Bitmap(area1Base); // original
    area2b = new createjs.Bitmap(area2);
    area2o = new createjs.Bitmap(area2Base);
    area3b = new createjs.Bitmap(area3);
    area3o = new createjs.Bitmap(area3Base);
    area4b = new createjs.Bitmap(area4);
    area4o = new createjs.Bitmap(area4Base);
    mosaicb = new createjs.Bitmap(mosaic);
    mosaicib = new createjs.Bitmap(mosaici);
    originalb = new createjs.Bitmap(original);
    greyb = new createjs.Bitmap(grey);

    mosaicib.cache(0, 0, mosaici.width, mosaici.height);
    area1b.cache(0, 0, area1.width, area1.height);
    area2b.cache(0, 0, area2.width, area2.height);
    area3b.cache(0, 0, area3.width, area3.height);
    area4b.cache(0, 0, area4.width, area4.height);
    mosaicb.cache(0, 0, mosaic.width, mosaic.height);

    originalb.set({'name':'base'});
    area1b.set({'name':'area1'});
    area2b.set({'name':'area2'});
    area3b.set({'name':'area3'});
    area4b.set({'name':'area4'});
    area1o.set({'name':'area1-base'});
    area2o.set({'name':'area2-base'});
    area3o.set({'name':'area3-base'});
    area4o.set({'name':'area4-base'});
    mosaicb.set({'name':'mosaic'});
    mosaicib.set({'name':'mosaic-inv'});
    mosaicib.set({'visible':true});
    greyb.set({'name':'grey'});

    stageSeams.addChild(greyb);
    stageSeams.addChild(originalb);
    stageSeams.addChild(area1o);
    stageSeams.addChild(area1b);
    stageSeams.addChild(area2o);
    stageSeams.addChild(area2b);
    stageSeams.addChild(area3o);
    stageSeams.addChild(area3b);
    stageSeams.addChild(area4o);
    stageSeams.addChild(area4b);
    stageSeams.addChild(mosaicib);
    stageSeams.addChild(mosaicb);

    stageSeams.update();

}

function handleBasicSeamsImageLoad() {
    area1b = new createjs.Bitmap(area1); // blur
    area2b = new createjs.Bitmap(area2);
    area3b = new createjs.Bitmap(area3);
    area4b = new createjs.Bitmap(area4);
    mosaicb = new createjs.Bitmap(mosaic);
    mosaicib = new createjs.Bitmap(mosaici);
    originalb = new createjs.Bitmap(original);
    resultb = new createjs.Bitmap(result);

    originalb.set({'name':'base'});
    area1b.set({'name':'area1'});
    area2b.set({'name':'area2'});
    area3b.set({'name':'area3'});
    area4b.set({'name':'area4'});
    mosaicb.set({'name':'mosaic'});
    mosaicib.set({'name':'mosaic-inv'});
    resultb.set({'name':'result'});
    
    stageSeams.addChild(resultb);
    stageSeams.addChild(area1b);
    stageSeams.addChild(area2b);
    stageSeams.addChild(area3b);
    stageSeams.addChild(area4b);
    stageSeams.addChild(mosaicib);
    stageSeams.addChild(mosaicb);
    stageSeams.addChild(originalb);

    stageSeams.update();
}


var dynamicDestripeCanvas = document.createElement("canvas");
dynamicDestripeCanvas.id = "isis-destripe-canvas";
dynamicDestripeCanvas.width = 500;
dynamicDestripeCanvas.height = 500;
$('#isis-destripe').append(dynamicDestripeCanvas);

var dynamicSeamsCanvas = document.createElement("canvas");
dynamicSeamsCanvas.id = "isis-seams-canvas";
dynamicSeamsCanvas.width = 500;
dynamicSeamsCanvas.height = 500;
$('#isis-seams').append(dynamicSeamsCanvas);


$(document).ready( function() {


if (document.getElementById('isis-cube') !== null) {
  var cubeBoxes=[];
  var cubeLines=4;
  var cubeSamples=4;
  var cubeBands=1;
  var cubeConsole = '';

  function drawCubes() {
    var orientations, start_location;
    orientations = ["bottom","right","top","left"];
    start_location = [30,370];

    for ( var i = 0; i < cubeBands; i++ ) {
      var orientation = orientations[i%4];
	  b = new box({ 'target':'isis-cube',
			      'rows':cubeLines,
			      'columns':cubeSamples,
			      'orientation':orientation,
			      'type':'lines',
			      'height':200,
			      'width':200,
			      'pixelSize':50,
			      'top':(start_location[0]+(i*50)),
			      'left':(start_location[1]-(i*50))
		    });
		cubeBoxes.push(b);
    }
  }

  function clearCubes() {

    for ( var i = 0; i < cubeBands; i++ ) {
      cubeBoxes[i].deactivate();
      cubeBoxes[i].clear();
      cubeBoxes[i].canvas.parentElement.removeChild(cubeBoxes[i].canvas);
    }
    cubeBoxes = [];
  } 	


   drawCubes();
   cubeConsole = new isisConsole({ 'target':'isis-cube',
				   'showRightConsole':true,
				   'bottomConsoleTop':'600px',
				   'slider':true,
				   'boxes':cubeBoxes.reverse()
			  	   });
  				   cubeConsole.boxes = cubeBoxes;



    $("#add-line").click( function() {
      if (cubeLines < 16) {
	cubeLines++;
	$.each(cubeBoxes, function(i,b) {
		 b.addLine();
	       });
	}
    });

    $("#add-sample").click( function() {
      if (cubeSamples < 16) {
	cubeSamples++;
	$.each(cubeBoxes, function(i,b) {
		 b.addSample();
	       });
	}
      });

    $("#add-band").click( function() {
      if (cubeBands < 8) {
	clearCubes();
	cubeBands++;
	drawCubes();
	cubeConsole.addNewBoxesToConsole(cubeBoxes);
      }
    });

    $("#remove-line").click( function() {
      if (cubeLines > 4) {
	cubeLines--;
	$.each(cubeBoxes, function(i,b) {
		 b.removeLine();
	});
      }
    });

    $("#remove-sample").click( function() {
      if (cubeSamples > 4) {
	    cubeSamples--;
	    $.each(cubeBoxes, function(i,b) {
		    b.removeSample();
		});
	}
    });

    $("#remove-band").click( function() {
      if (cubeBands > 1) {
	clearCubes();
	cubeBands--;
	drawCubes();
	cubeConsole.addNewBoxesToConsole(cubeBoxes);
      }
    });


    $(cubeConsole.reset).click( function() {
				  clearCubes();
				  cubeLines=4;
				  cubeSamples=4;
				  cubeBands=1;
				  drawCubes();
				  cubeConsole.addNewBoxesToConsole(cubeBoxes);
      });
} // end isis cube


if (document.getElementById('isis-seams') !== null) {      
      canvasSeams = document.getElementById("isis-seams-canvas");
      contextSeams = canvasSeams.getContext("2d");
      stageSeams = new createjs.Stage(canvasSeams);
	
	// check to see if overlay is supported
       contextSeams.globalCompositeOperation = 'overlay';
	
	if(contextSeams.globalCompositeOperation == 'overlay') {
	    contextSeams.globalCompositeOperation = 'source-over';
	    $.when( initSeams() ).done( function() {
		    rSeamsConsole = new isisConsole({ 'target':'isis-seams',
						      'canvas':canvasSeams,
						      'context':contextSeams,
						      'stage':stageSeams,
						      'showRightConsole':true,
						      'challenge':'seam-removal',
						      'overlaySupport':true
			});
		});
	}
	else {
	    $.when( initBasicSeams() ).done( function() {
		    contextSeams.globalCompositeOperation = 'source-over';
		    rSeamsConsole = new isisConsole({ 'target':'isis-seams',
						      'canvas':canvasSeams,
						      'context':contextSeams,
						      'stage':stageSeams,
						      'showRightConsole':true,
						      'challenge':'seam-removal',
						      'overlaySupport':false
			});
		});
	}
}

if (document.getElementById('isis-destripe') !== null) {

	canvas = document.getElementById("isis-destripe-canvas");
	context = canvas.getContext("2d");
	
	stage = new createjs.Stage(canvas);

	console.log(stage);
	// check to see if overlay is supported
	context.globalCompositeOperation = 'overlay';     
	
	if(context.globalCompositeOperation == 'overlay') {
	    context.globalCompositeOperation = 'source-over';
	    $.when( init() ).done( function() {
		    destripeConsole = new isisConsole({ 'target': 'isis-destripe', 
							'canvas': canvas,
							'stage':stage,
							'context':context,
							'showRightConsole':true,
							'challenge':'destripe',
							'overlaySupport':true
			});
		});
	}
	else {
	    $.when( initBasic() ).done( function() {
		    destripeConsole = new isisConsole({ 'target': 'isis-destripe', 
							'canvas': canvas,
							'stage':stage,
							'context':context,
							'showRightConsole':true,
							'challenge':'destripe',
							'overlaySupport':false
			});
		});
	}	
}

if (document.getElementById('isis-pixels') !== null) {	
   var b = new box( { 'target':'isis-pixels', 
    	    	       'type': 'gradient', 
		       'pixelSize': 35,
		       'width':280,
		       'height':280,
		       'top':0,
		       'left':0
		       });

    var i = new isisConsole({ 'target':'isis-pixels',
			      'boxes':[b],
			      'subpixels':false,
			      'showRightConsole':true,
			      'slider':true
			      });    

}

if (document.getElementById('isis-subpixels') !== null) {	
    var b2 = new box( { 'target':'isis-subpixels', 
    	     	      	'type':'gradient',
			'pixelSize':35, 
			'width':280,
			'height':280});

    var i2 = new isisConsole({ 'target':'isis-subpixels',
                               'boxes':[b2],
			       'subpixels':true,
			       'showRightConsole':false,	
			       'slider':false
			       });
}

if (document.getElementById('isis-multiplier') !== null) {	
    var l = new staticImage({ 'target':'isis-multiplier',
			      'src':'themes/astro/images/elevation.jpg',
			      'height':256,
			      'width':256    
			    });

    var i3 = new isisConsole({ 'target':'isis-multiplier',
			       'staticImage':l,
			       'showRightConsole':true,
			       'dnMultiplier':true
			       });
}

if (document.getElementById('isis-special-pixels') !== null) {				       
    var b3 = new box({ 'target':'isis-special-pixels',
			'type':'special',
			'pixelSize':35,
			'width':280,
			'height':280});

    var i4 = new isisConsole({ 'target':'isis-special-pixels',
			       'boxes':[b3],
			       'showRightConsole':true,
			       'slider':false});


}

if (document.getElementById('isis-image-data-size') !== null) {

    var formHTML = '<div style="width:100%;"><form name="myForm"><table style="width:100%;background: #eee;margin: auto" ><tr><th colspan="2" ><p>Calculate image data size</p></th></tr><tr><td>Number of lines:<br/><input type="text" name="inLine" size="10"></td><td>Number of samples:<br/><input type="text" name="inSamp" size="10"></td></tr><tr><td colspan="2" align="center">Select bit type:<p><select name="inBit" size="1"><option>8</option><option>16</option><option>32</option></select></p></td></tr><tr><td colspan="2" align="center"><p>Select output units:<br/><select name="outType" size="1"><option value="b">Bytes </option><option value="k">Kilobyte </option><option value="m">Megabyte </option><option value="g">Gigabyte </option></select></p></td></tr><td colspan="2" align="center"><p><input type="button" value="calculate size" onclick="fsize();"></p></td><tr><td colspan="2" align="center"><br><span style="font-weight:bold;line-height:2em;float:left;">The data size is: </span><input style="float:left;width:100px;" type="text" name="answer" size="20"><br></td></tr></table></form></div>';
    document.getElementById('isis-image-data-size').innerHTML = formHTML;
    document.getElementById('isis-image-data-size').setAttribute("style","width:50%");
    $.getScript("themes/astro/javascripts/filesize.js");
}


if (document.getElementById("radii-demo") !== null) {
    $.getScript("themes/astro/javascripts/projections.js")
	.done(function(script, status) {
		initRadiiDemo();
	    });
}
	
if (document.getElementById("longitudes-demo") !== null) {
    $.getScript("themes/astro/javascripts/projections.js")
	.done(function(script, status) {
		initLongitudesDemo();
	    });
}

if (document.getElementById("planeto-demo") !== null) {
    $.getScript("themes/astro/javascripts/projections.js")
	.done(function(script, status) {
		initPlanetoDemo();
	    });
}

if (document.getElementById("ortho-demo") !== null) {
    $.getScript("themes/astro/javascripts/projections.js")
	.done(function(script, status) {
		initOrtho();
	    });
}

});