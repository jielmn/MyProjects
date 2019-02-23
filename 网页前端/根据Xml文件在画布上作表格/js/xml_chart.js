function  Rect(l,t,r,b) {
	this.left = l;
	this.top = t;
	this.right = r;
	this.bottom = b;
}

function  Pen(color,size) {
	this.color = color;
	this.size = size;
}

function  SplitPen( splitNum, pen ) {
	this.splitNum = splitNum;
	this.pen = pen;
}

function Thread ( id, size, color ) {
	this.id = id;
	this.pen = new Pen(color, size);
}

function  Font ( name, size, bold, italic, id ) {
	this.name = name;
	this.size = size;
	this.bold = bold;
	this.italic = italic;
	this.id = id;
}

function  createNewFont( emt ) {
	var attrs = emt.attributes;
	var attr = null
	var name="宋体";
	var size=15;
	var bold=false;
	var italic=false;
	var id = 0;
	var tmp;
	
	attr = attrs.getNamedItem("id");
	if ( attr ) {
		id  = parseInt(attr.value);
	}
	
	attr = attrs.getNamedItem("bold");
	if ( attr ) {
		tmp = attr.value.toLowerCase();
		bold  = ( tmp == "true" ? true : false );
	}
	
	attr = attrs.getNamedItem("italic");
	if ( attr ) {
		tmp = attr.value.toLowerCase();
		italic  = ( tmp == "true" ? true : false );
	}
	
	attr = attrs.getNamedItem("height");
	if ( attr ) {
		size  = parseInt(attr.value);
	}
	
	attr = attrs.getNamedItem("facename");
	if ( attr ) {
		name  = attr.value;
	}
	
	return new Font( name, size, bold, italic, id );
}

function  createNewThread ( emt ) {
	var attrs = emt.attributes;
	var attr = null
	var id = 0;
	var size = 0;
	var color = "#000000";
	
	attr = attrs.getNamedItem("id");
	if ( attr ) {
		id  = parseInt(attr.value);
	}
	
	attr = attrs.getNamedItem("width");
	if ( attr ) {
		size  = parseInt(attr.value);
	}
	
	attr = attrs.getNamedItem("color");
	if ( attr ) {
		color  = attr.value;
	}
	
	return new Thread( id, size, color );
}

function  XmlChartUI( ) {
	this.rect = new Rect(0,0,0,0);
	this.padding = new Rect(0,0,0,0);
	this.width = 0;
	this.height = 0;
	this.parent = null;
	this.layoutType = 0;
	this.children = new Array();
	this.preSibling = null;
	this.nextSibling = null;
	this.strText="";
	this.name="";
	
	this.borderPen = new Array(4);
	for ( var i = 0; i < 4; i++ ) {
		this.borderPen[i] = new Pen( "#000000", 1 );
	}
	
	this.HSplitLines = new Array();
	this.VSplitLines = new Array();
	
	this.textColor = "#000000";
	this.align = 0;
	this.valign = 0;
}

function  getAttr( emt, attrName ) {
	var attrs = emt.attributes;	
	var attr  = attrs.getNamedItem(attrName);
	if ( attr ) {
		return attr.value;
	} else {
		return "";
	}
}

function  getIntAttr( emt, attrName ) {
	var attrs = emt.attributes;	
	var attr  = attrs.getNamedItem(attrName);
	if ( attr ) {
		return parseInt(attr.value);
	} else {
		return 0;
	}
}

/* num='5',pen='6' */
function  parseSplitPen ( ui, str, bVertical, threads ) {
	
	var arr = str.split(",", 2);
	if ( arr.length != 2 ) 
		return;
	
	var i = 0;
	var index = 0;
	var num = 0;
	var threadIndex = 0;
	
	for ( i = 0; i < arr.length; i++ ) {
		if ( arr[i].indexOf("num") >= 0 ) {
			index = arr[i].indexOf("'");			
			if ( index < 0 )
				return;
			num = parseInt( arr[i].slice(index+1) );
		} else if ( arr[i].indexOf("pen") >= 0 ) {			
			index = arr[i].indexOf("'");
			if ( index < 0 )
				return;
			threadIndex = parseInt( arr[i].slice(index+1) );
		}
	}	
	
	if ( num <= 0 )
		return;
	
	
	var pen = null;
	for ( i = 0; i < threads.length; i++ ) {
		if ( threads[i].id == threadIndex ){
			pen = threads[i].pen;
			break;
		}
	}
	if ( i >= threads.length ) {
		return;
	}
	
	var x;
	if ( bVertical ) {
		x = ui.VSplitLines;		
	} else {
		x = ui.HSplitLines;		
	}
	
	x.push(new SplitPen(num, pen));
}

function  parseSplit ( emt, ui, threads ) {
	if ( threads.length == 0 )
		return;
	
	var vsplitStr = getAttr( emt, "VSplit" );
	var hsplitStr = getAttr( emt, "HSplit" );
	
	if ( vsplitStr.length > 0 )
		parseSplitPen ( ui, vsplitStr, true, threads );
	
	if ( hsplitStr.length > 0 )	
		parseSplitPen ( ui, hsplitStr, false, threads );
}

function parseBorder( emt, ui, threads ) {
	if ( threads.length == 0 )
		return;
	
	var border = getAttr( emt, "border" );
	if ( border.length == 0 )
		return;
	
	var arr = border.split(",",4);
	var i = 0;
	var index = 0;
	var j = 0;
	for ( i = 0; i < arr.length; i++ ) {
		index = parseInt( arr[i] );
		for ( j = 0; j < threads.length; j++ ) {
			if ( threads[j].id == index ) {
				//找到
				ui.borderPen[i] = threads[j].pen;
				break;
			}
		}
	}
}

function parsePadding( emt, ui ) {
	var padding = getAttr( emt, "padding" );
	if ( padding.length == 0 )
		return;
	
	var arr = padding.split(",",4);
	var arr1 = new Array(0,0,0,0);
	for ( var i = 0; i < arr.length; i++ ) {
		arr1[i] = parseInt(arr[i]);
	}
	
	ui.padding = new Rect( arr1[0],arr1[1],arr1[2],arr1[3] );
}


function LoadXmlChart( xmlFile, canvasId ) {
	var canvas = document.getElementById(canvasId);
	if ( canvas == null ) {
		console.log("canvas not found!");
		return;
	}
			
	$.ajax({
		type: "get",
		url:  xmlFile,              //这里通过设置url属性来获取xml
		dataType:"xml",
		timeout:10000,              //设定超时
		cache:false,                //禁用缓存
		success:function(xml){      //这里是解析xml			
			var root  = xml.documentElement;
			var attrs = root.attributes;
			var width = 0;
			var height = 0;
			var attr = null;
			var findLayout = false;
			var child = null;
			var fonts = new Array();
			var threads = new Array();
			var findFirstLayout = false;
			
			attr = attrs.getNamedItem("width");
			if ( attr ) {
				width  = parseInt(attr.value);
			}
			
			attr = attrs.getNamedItem("height");
			if ( attr ) {
				height  = parseInt(attr.value);
			}
			
			console.log("width:"+width+", height="+height);
			
			
			child = root.firstChild;
			while ( child ) {
				if ( 1 != child.nodeType ) {
					child = child.nextSibling;
					continue;
				}
				//console.log( child );
				//console.log( child.nodeName );
				
				if ( child.nodeName == "Font" ) {
					var font = createNewFont ( child );
					var i = 0;
					for ( i = 0; i < fonts.length; i++ ) {
						if ( fonts[i].id == font.id ) {
							break;
						} 
					}
					if ( i >= fonts.length ) {
						fonts.push( font );
					}
				} else if ( child.nodeName == "Thread" ) {
					var thread = createNewThread ( child );
					var i = 0;
					for ( i = 0; i < threads.length; i++ ) {
						if ( threads[i].id == thread.id ) {
							break;
						} 
					}
					if ( i >= threads.length ) {
						threads.push( thread );
					}
				} else {
					var bVertical = false;
					if ( child.nodeName == "VerticalLayout" ) {
						if (findFirstLayout) {
							child = child.nextSibling;
							continue;
						}
						findFirstLayout = true;
						bVertical = true;
					}
					else if ( child.nodeName == "HorizontalLayout" ) {
						if (findFirstLayout) {
							child = child.nextSibling;
							continue;
						}
						findFirstLayout = true;
						bVertical = false;
					}
					else {
						child = child.nextSibling;
						continue;
					}
					
					// chart UI
					var chartUI = new XmlChartUI();
					chartUI.name = getAttr(child, "name");
					chartUI.rect = new Rect(0,0,getIntAttr(child,"width"),getIntAttr(child,"height"));
					chartUI.layoutType = ( bVertical ? 1 : 0 );
					parseSplit( child, chartUI, threads );
					parseBorder( child, chartUI, threads );
					parsePadding( child, chartUI );
					
					console.log(chartUI);
					
				}
				child = child.nextSibling;								
			}
			
			console.log( fonts );
			console.log( threads );
			/*
			var cxt=canvas.getContext("2d");
			cxt.moveTo(10,10);
			cxt.lineTo(150,50);
			cxt.lineTo(10,50);
			cxt.stroke();
			*/
			
			var ctx=canvas.getContext("2d");
			ctx.font="40px Arial";
			ctx.fillText("Hello World",10,50);			
			ctx.font="20px Arial";
			ctx.fillText("Hello World",100,100);
		},
		error:function() {
			console.log( "failed to load \"" + xmlFile + "\" file" );
		}
	});
}