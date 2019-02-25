function  Rect(l,t,r,b) {
	this.left = l;
	this.top = t;
	this.right = r;
	this.bottom = b;
	this.setRect = function(l,t,r,b){
		this.left = l;
		this.top = t;
		this.right = r;
		this.bottom = b;
	}
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

function MIN( a, b ) {
	if ( a <= b )
		return a;
	else
		return b;
}

function  XmlChartUI( ) {
	this.rect = new Rect(0,0,0,0);
	this.padding = new Rect(0,0,0,0);
	this.width = -1;
	this.height = -1;
	this.parent = null;
	this.layoutType = 0;
	this.children = new Array();
	this.preSibling = null;
	this.nextSibling = null;
	this.text="";
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
	this.font = new Font("宋体", 15, false, false, -1);
	this.bFloat = false;
	this.position = new Rect( 0, 0, 0, 0);
	
	this.addChild = function ( child ) {
		if ( child == null ) {
			return;
		}
		child.parent = this;
		
		var nCount = this.children.length;
		var preSibling = null;
		if (nCount > 0) {
			preSibling = this.children[nCount - 1];
		}
		child.preSibling = preSibling;
		child.nextSibling = null;

		if (preSibling) {
			preSibling.nextSibling = child;
		}
		this.children.push(child);

		this.recacluteLayout();	
	}
	
	this.recacluteLayout = function () {
		var  nWidth  = this.width;
		var  nHeight = this.height;
		var  nAccu         = 0;
		var  nAutoChildCnt = 0;
		var  nAve   = 0;                   // 自动计算的平均值
		var  nMod   = 0;                   // 自动计算的余数
		var  nIndex = 0;                   // 用于判断是否最后一个
		var  nLeft  = 0;
		var  nTop = 0;
		var  iLeft = 0;
		var  i = 0;
		var  child = null;
		
		var tmpPadding = new Rect( this.padding.left, this.padding.top, this.padding.right, this.padding.bottom );
		if (tmpPadding.left > nWidth) {
			tmpPadding.left = nWidth;
			tmpPadding.right = 0;
		}
		else if (tmpPadding.left + tmpPadding.right > nWidth) {
			tmpPadding.right = nWidth - tmpPadding.left;
		}

		if (tmpPadding.top > nHeight ) {
			tmpPadding.top = nHeight;
			tmpPadding.bottom = 0;
		}
		else if (tmpPadding.top + tmpPadding.bottom > nHeight) {
			tmpPadding.bottom = nHeight - tmpPadding.top;
		}

		if ( this.layoutType == 1 ) {
			// 计算垂直方向，自动子UI的高度
			for ( i = 0; i < this.children.length; i++ ) {
				child = this.children[i];				
				if (!child.bFloat) {
					// 固定大小的
					if (child.height != -1) {
						nAccu += child.height;
					}
					// 自动计算大小的
					else {
						nAutoChildCnt++;
					}				
				}
			}

			// 如果自动计算大小的存在
			if (nAutoChildCnt > 0) {
				nLeft = nHeight - nAccu - tmpPadding.top - tmpPadding.bottom;
				if (nLeft > 0) {
					nAve = (nLeft) / nAutoChildCnt;
					nMod = (nLeft) % nAutoChildCnt;
				}
			}
			
			nLeft = nHeight - tmpPadding.top - tmpPadding.bottom;
			nTop = tmpPadding.top;
			
			for ( i = 0, nIndex = 0; i < this.children.length; i++, nIndex++ ){
				child = this.children[i];
				// 如果是浮动的
				if ( child.bFloat ) {
					if (child.position.left > nWidth) {
						child.rect.left = nWidth;
					}

					if ( child.position.right > nWidth ) {
						child.rect.right = nWidth;
					}

					if (child.position.top > nHeight) {
						child.rect.top = nHeight;
					}

					if ( child.position.bottom > nHeight ) {
						child.rect.bottom = nHeight;
					}
					child.recacluteLayout();
				}
				// 如果是固定的高度
				else if (child.height != -1) {
					// 剩余的高度不足以支持固定高度
					if (child.height > nLeft) {
						// 如果还是固定宽度
						if (child.width != -1) {
							child.rect.setRect(tmpPadding.left, nTop, MIN(child.width, nWidth - tmpPadding.right), nTop + nLeft );
						}
						else {
							child.rect.setRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + nLeft );
						}
						nTop += nLeft;
						nLeft = 0;
					}
					else {
						// 如果还是固定宽度
						if (child.width != -1) {
							child.rect.setRect(tmpPadding.left, nTop, MIN(child.width, nWidth - tmpPadding.right), nTop + child.height);
						}
						else {
							child.rect.setRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + child.height);
						}
						nTop += child.height;
						nLeft -= child.height;
					}
					child.recacluteLayout();
				}
				// 如果是自动计算
				else {
					// 如果是固定宽度
					if (child.width != -1) {
						// 如果是最后一个
						if (nIndex == nAutoChildCnt - 1) {
							child.rect.setRect(tmpPadding.left, nTop, MIN(child.width, nWidth - tmpPadding.right), nTop + nAve + nMod );
							nTop += nAve + nMod;
							nLeft -= nAve + nMod;
						}
						else {
							child.rect.setRect(tmpPadding.left, nTop, MIN(child.width, nWidth - tmpPadding.right), nTop + nAve);
							nTop += nAve;
							nLeft -= nAve;
						}		
					}
					else {
						if (nIndex == nAutoChildCnt - 1) {
							child.rect.setRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + nAve + nMod);
							nTop += nAve + nMod;
							nLeft -= nAve + nMod;
						}
						else {
							child.rect.setRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + nAve);
							nTop += nAve;
							nLeft -= nAve;
						}
					}
					child.recacluteLayout();
				}
			}
		}
		else if ( this.layoutType == 0 ) {
			// 计算垂直方向，自动子UI的高度
			for ( i = 0; i < this.children.length; i++ ) {
				child = this.children[i];
				if (!child.bFloat) {
					// 固定大小的
					if (child.width != -1) {
						nAccu += child.width;
					}
					// 自动计算大小的
					else {
						nAutoChildCnt++;
					}
				}
			}

			// 如果自动计算大小的存在
			if (nAutoChildCnt > 0) {
				nLeft = nWidth - nAccu - tmpPadding.left - tmpPadding.right;
				if (nLeft > 0) {
					nAve = (nLeft) / nAutoChildCnt;
					nMod = (nLeft) % nAutoChildCnt;
				}
			}

			nLeft = nWidth - tmpPadding.left - tmpPadding.right;
			iLeft = tmpPadding.left;

			for ( i = 0, nIndex = 0; i < this.children.length; i++, nIndex++ ) {
				child = this.children[i];
				// 如果是浮动的
				if (child.bFloat) {
					if (child.position.left > nWidth) {
						child.rect.left = nWidth;
					}

					if (child.position.right > nWidth) {
						child.rect.right = nWidth;
					}

					if (child.position.top > nHeight) {
						child.rect.top = nHeight;
					}

					if (child.position.bottom > nHeight) {
						child.rect.bottom = nHeight;
					}
					child.recacluteLayout();
				}
				// 如果是固定的宽度
				else if (child.width != -1) {
					// 剩余的宽度不足以支持固定宽度
					if (child.width > nLeft) {
						// 如果还是固定高度
						if (child.height != -1) {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + nLeft, MIN(nHeight - tmpPadding.bottom, child.height));
						}
						else {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + nLeft, nHeight - tmpPadding.bottom );
						}
						iLeft += nLeft;
						nLeft = 0;
					}
					else {
						// 如果还是固定高度
						if (child.height != -1) {
							child.rect.setRect( iLeft, tmpPadding.top, iLeft + child.width, MIN(nHeight - tmpPadding.bottom, child.height));
						}
						else {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + child.width, nHeight - tmpPadding.bottom );
						}
						iLeft += child.width;
						nLeft -= child.width;
					}
					child.recacluteLayout();
				}
				// 如果是自动计算
				else {
					// 如果是固定宽度
					if (child.height != -1) {
						// 如果是最后一个
						if (nIndex == nAutoChildCnt - 1) {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + nAve + nMod, MIN(nHeight - tmpPadding.bottom, child.height));
							iLeft += nAve + nMod;
							nLeft -= nAve + nMod;
						}
						else {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + nAve, MIN(nHeight - tmpPadding.bottom, child.height));
							iLeft += nAve;
							nLeft -= nAve;
						}
					}
					else {
						if (nIndex == nAutoChildCnt - 1) {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + nAve + nMod, nHeight - tmpPadding.bottom );
							iLeft += nAve + nMod;
							nLeft -= nAve + nMod;
						}
						else {
							child.rect.setRect(iLeft, tmpPadding.top, iLeft + nAve, nHeight - tmpPadding.bottom);
							iLeft += nAve;
							nLeft -= nAve;
						}
					}
					child.recacluteLayout();
				}
			}
		}
	}
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

function getAlignValue(value) {
	if ( value == null ) {
		return 0;
	} else if ( value == "center" ) {
		return 1;
	} else if ( value == "right" ) {
		return 2;
	}
	return 0;
}

function parseText( emt, ui, fonts ) {
	ui.text = getAttr( emt, "text" );
	ui.align = getAlignValue( getAttr( emt, "align" ) );
	ui.valign = getAlignValue( getAttr( emt, "valign" ) );
	
	var font = getAttr( emt, "font" );
	if ( font.length == 0 ) {
		return;
	}
	
	var fontIndex = parseInt ( font );
	var i = 0;
	for ( i = 0; i < fonts.length; i++ ) {
		if ( fonts[i].id == fontIndex ) {
			ui.font = fonts[i];
			break;
		}
	}
}

function parseFixed( emt, ui ) {
	var attr;
	attr = getAttr( emt, "width" );
	if ( attr ) {
		ui.width = parseInt( attr );
	}
	
	attr = getAttr( emt, "height" );
	if ( attr ) {
		ui.height = parseInt( attr );
	}
}

function parseChartFile( emt, ui, threads, fonts ) {	
	var child = emt.firstChild;
	while ( child ) {
		var nodeName = child.nodeName;
		if ( nodeName == "Item" ) {
			var childUI = new XmlChartUI();
			ui.addChild( childUI );			
			parseText( child, childUI, fonts );
			parseFixed( child, childUI );			
			parseBorder( child, childUI, threads );			
			childUI.name = getAttr( child, "name" );
		} else {
			var bVertical = false;
			if ( nodeName == "VerticalLayout" ) {
				bVertical = true;
			} else if ( nodeName == "HorizontalLayout" ) {
				bVertical = false;
			} else {
				child = child.nextSibling;
				continue;
			}

			var childUI = new XmlChartUI();
			ui.addChild( childUI );			
			
			if ( bVertical )
				childUI.layoutType = 1;
			else
				childUI.layoutType = 0;


			parseFixed( child, childUI );
			parseSplit( child, childUI, threads );
			parseBorder( child, childUI, threads );
			parsePadding( child, childUI );
			childUI.name = getAttr( child, "name" );

			parseChartFile(child, childUI, threads, fonts);
		}
		child = child.nextSibling;
	}
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
					parseChartFile( child, chartUI, threads, fonts );
				}
				child = child.nextSibling;								
			}
			
			chartUI.recacluteLayout();
			
			console.log(chartUI);
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