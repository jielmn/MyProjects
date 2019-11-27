var pageIndex = 0;
var sliderIndex = 0;

function OnIndexLoad() {
	pageIndex = 0;
	$('#divNavs').children().eq(pageIndex).css('background-color','#e5e5e5');
	var el = $('#divNavsFoot').children().eq(pageIndex).find('a');
	el.css('border-bottom', '2px solid #bf3d5f');
	el.css('padding', '0px 0px 6px 0px');
}

function OnProductLoad() {
	pageIndex = 1;
	$('#divNavs').children().eq(pageIndex).css('background-color','#e5e5e5');
	var el = $('#divNavsFoot').children().eq(pageIndex).find('a');
	el.css('border-bottom', '2px solid #bf3d5f');
	el.css('padding', '0px 0px 6px 0px');
	
	$('#suits').children().mouseover(function(){
		var index=$('#suits').children().index(this);
		showImg(index);
	})
	
	$('#suits').children().mouseleave(function(){
		var index=$('#suits').children().index(this);
		hideImg(index);
	})
}

function showImg(index){			
	$("#show").children().eq(0).children().eq(0).attr('src','images/suit' + (index + 1) + '.jpg');	
	var lst = $("#show").children().eq(1).children().eq(0);
	lst.children().remove();
	var item;
	
	switch( index ){
		case 0:
		item=$("<li></li>").text("测温传感器使用高弹性泡棉作为赋型材料，并配合3M医用水凝硅双面胶用于辅助固定于测温位置。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("不锈钢感温头由凸起泡棉包裹，使得探头与腋下等位置皮肤紧密贴合的同时，也能给用户更好的触感体验。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("环形天线可延伸到胸前或手臂开阔位置，无需被试者大幅度移动身体即可用读卡器进行扫描。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("绿色电子温度计的传感器精度可达到0.01℃，蓝色测温传感器精度为0.1℃，可以满足不同用户的测温需求。").attr('class', 'itemShow');
		lst.append(item);
		break;
		
		case 1:
		item=$("<li></li>").text("测温传感器使用高弹性泡棉作为赋型材料，并配合3M医用水凝硅双面胶用于辅助固定于测温位置。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("不锈钢感温头由凸起泡棉包裹，使得探头与腋下等位置皮肤紧密贴合的同时，也能给用户更好的触感体验。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("环形天线可延伸到儿童（2-12岁）胸前或手臂开阔位置，无需被试者大幅度移动身体即可用读卡器进行扫描。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("橙色电子温度计的传感器精度可达到0.01℃，黄色测温传感器精度为0.1℃，可以满足不同用户的测温需求。").attr('class', 'itemShow');
		lst.append(item);
		break;
		
		case 2:
		item=$("<li></li>").text("测温传感器使用高韧性硅胶和高弹性泡棉作为赋型材料。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("采用常规不锈钢短棒状的感温头，适合腔体内测温需求。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("环形天线具有极佳的柔顺型，既满足功能使用，又能在腔内测温的时为用户带来舒适的触感。").attr('class', 'itemShow');
		lst.append(item);
		break;
		
		case 3:
		item=$("<li></li>").text("测温传感器使用医疗级无纺布和高弹性泡棉作为赋型材料。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("采用微型树脂封装感温头，其质量轻、测温干扰小，适合肢体末梢和各皮温测测量。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("无纺布能使得感温头周围形成有效的封闭环境，较小环境温度影响。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("环形天线具有良好的柔顺型，既满足功能使用，又安全舒适。").attr('class', 'itemShow');
		lst.append(item);
		break;
		
		case 4:
		item=$("<li></li>").text("读卡器的天线通过同轴电缆引出，并与测温传感器环形天线灵活扣合，时刻形成有效的耦合可进行长时间体温监控。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("兼容所有易温®️测温传感器。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("可设定连续测温采样间隔时间、温度报警阈值。").attr('class', 'itemShow');
		lst.append(item);
		item=$("<li></li>").text("可同步输出温度数据至多参数监护仪。").attr('class', 'itemShow');
		lst.append(item);
		break;
	}
	
	$('#suits').children().eq(index).css('background-color','#CE587A');
	$("#show").show();
} 
		
function hideImg(index) {
	$("#show").hide();
	$('#suits').children().eq(index).css('background-color','#ccc');
}

function OnSolutionLoad() {
	pageIndex = 2;
	$('#divNavs').children().eq(pageIndex).css('background-color','#e5e5e5');
	var el = $('#divNavsFoot').children().eq(pageIndex).find('a');
	el.css('border-bottom', '2px solid #bf3d5f');
	el.css('padding', '0px 0px 6px 0px');
	$("#btns").children().click( function(){
		var index=$('#btns').children().index(this);
		sliderImg(index);
	});
	sliderImg(sliderIndex);
	
	setInterval( function () {
		sliderIndex++;
		if ( sliderIndex > 2 ){
			sliderIndex = 0;
		}
		sliderImg(sliderIndex);
	}, 5000 );
}

function sliderImg(i){	
	$("#slider").attr('src', 'images/slider'+(i+1)+'.png');
	
	$("#btns").children().css('background-color','');
	$("#btns").children().eq(i).css('background-color','#bbb');
	sliderIndex = i;
} 

function OnAboutLoad() {
	pageIndex = 3;
	$('#divNavs').children().eq(pageIndex).css('background-color','#e5e5e5');
	var el = $('#divNavsFoot').children().eq(pageIndex).find('a');
	el.css('border-bottom', '2px solid #bf3d5f');
	el.css('padding', '0px 0px 6px 0px');
}