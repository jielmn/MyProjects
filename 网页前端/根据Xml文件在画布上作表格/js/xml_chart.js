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
	this.size = size;
	this.color = color;
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
		this.borderPen[i] = new Pen( 0, 1 );
	}
	
	this.HSplitLines = new Array();
	this.VSplitLines = new Array();
	
	this.textColor = 0;
	this.align = 0;
	this.valign = 0;
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
					
					var chartUI = new XmlChartUI();
					
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