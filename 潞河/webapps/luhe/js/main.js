var addr = "http://192.168.0.99/luhe/";
var content_len  = "30%";

function onTabChange(index){
	if ( 0 == index ) {
		$("#imgMine").attr('src','images/mine.png'); 
		$("#imgGroup").attr('src','images/group_blur.png'); 
		
		$("#iframeMine").css('display','block'); 
		$("#iframeGroup").css('display','none'); 
	} else {
		$("#imgMine").attr('src','images/mine_blur.png'); 
		$("#imgGroup").attr('src','images/group.png'); 
		
		$("#iframeMine").css('display','none'); 
		$("#iframeGroup").css('display','block'); 
	}
}

function getUrlParam(name) {
 var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)"); //构造一个含有目标参数的正则表达式对象
 var r = window.location.search.substr(1).match(reg);    //匹配目标参数
 if (r != null) return unescape(r[2]); return null;      //返回参数值
}

function onMainLoad() {
	//uid = getUrlParam('uid');
	$("#iframeMine").attr('src','station.html'); 
	$("#iframeGroup").attr('src','group.html'); 
	
	$("#divMine").click(function(){
	  onTabChange(0);
	});
	
	$("#divGroup").click(function(){
	  onTabChange(1);
	});
}

// “我的”页面加载
function onMineLoad() {
	console.log("mine load")
	//uid = getUrlParam('uid');
	//alert("content:"+uid);
	getStationList();
}

function getStationList() {
	console.log("getStationList ......");
	$.get(addr+"manage?type=station_list",function(data,status){
		if ( "success" == status && 0 == data.error ) {
			console.log("get station list success");
			console.log(data);
			$("#divStationList").css('display','block');	
			if ( data.stationlist.length == 0 ) {
				$("#divNone").css('display','block');
			} else {
				$("#divNone").css('display','none');
				for (var i = 0;i < data.stationlist.length;i++ ){
					$("#divNone").before('<div class="row1"></div>');
					var item = $("#divNone").prev();
					var divName = $('<div class="listitem" style="width:' + content_len + ';line-height:21px;" >' + data.stationlist[i].name + '</div>');
					item.append(divName);
					var divIp = $('<div class="listitem" style="line-height:21px;" >' + data.stationlist[i].ip + '</div>');
					item.append(divIp);
				}
			}			
		} else {
			console.log("get station list fail");
		}			
	});
	
}
