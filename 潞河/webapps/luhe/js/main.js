//var addr = "http://192.168.0.99/luhe/";
var addr = "";
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
	$("#iframeGroup").attr('src','patient.html'); 
	
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
	getStationList();
}

function getStationList() {
	console.log("getStationList ......");
	jQuery.ajax({
	type: "get",
	url: addr+"manage?",
	//data: "bid="+my_bid+"&name_cn="+name_cn+"&timeStamp=" + new Date().getTime(),
	data: {'type':'station_list'},
	dataType: 'json',
	error: function (err) { console.log("get station list failed!"); },
	success: function (data) {
		console.log(data);
		if ( 0 == data.error ) {
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
		}
		else {
			console.log("get station list failed!!!");
		}
	}});
}


// “数据”页面加载
function onPatientLoad() {
	console.log("patient load");
	getPatientList();
}

function getPatientList() {
	console.log("getPatientList ......");
	jQuery.ajax({
	type: "get",
	url: addr+"manage?",
	data: {'type':'patient_list'},
	dataType: 'json',
	error: function (err) { console.log("get patient list failed!"); },
	success: function (data) {
		console.log(data);
		if ( 0 == data.error ) {
			$("#divPatientList").css('display','block');	
			if ( data.patientlist.length == 0 ) {
				$("#divNone").css('display','block');
			} else {
				$("#divNone").css('display','none');
				
				for (var i = 0;i < data.patientlist.length;i++ ){
					$("#divNone").before('<div class="row2"></div>');
					var item = $("#divNone").prev();
					var divId   = $('<div class="listitem" style="width:20%;line-height:21px;" >' + data.patientlist[i].id + '</div>');
					var divName = $('<div class="listitem" style="width:20%;line-height:21px;" >' + data.patientlist[i].name + '</div>');
					var divSex = $('<div class="listitem" style="width:20%;line-height:21px;" >' + (data.patientlist[i].sex == '1' ? '男' : '女')  + '</div>');
					var divBed = $('<div class="listitem" style="width:20%;line-height:21px;" >' + data.patientlist[i].bed + '</div>');
					var divAge = $('<div class="listitem" style="line-height:21px;" >' + data.patientlist[i].age + '</div>');
					item.append(divId);
					item.append(divName);
					item.append(divSex);
					item.append(divBed);
					item.append(divAge);
				}
				
			}
		} else {
			console.log("get patient list failed!!!");
		}
	}});
}


function onClearPatients() {
	var msg = "您真的确定要清空病人信息吗？"; 
	if (confirm(msg)==true){ 
		console.log("clear patients ......");
		jQuery.ajax({
			type: "get",
			url: addr+"manage?",
			data: {'type':'clear_patient'},
			dataType: 'json',
			error: function (err) { console.log("clear patient failed!"); },
			success: function (data) {
				console.log(data);
				if ( 0 == data.error ) {
					$("#divPatientList").find(".row2").filter( function( index ) {
						// console.log(index);
						$( this ).remove();
					});
					$("#divNone").css('display','block');
				} else {
					console.log("clear patient failed!!!");
				}
		}});
	}
}

function getType(file){
	var filename=file;
	var index1=filename.lastIndexOf(".");
	var index2=filename.length;
	var type=filename.substring(index1,index2);
	return type;
}

function  onImportPatients() {
	//console.log( $("#fileUpload1")[0].value );
	var filename = $("#fileUpload1")[0].value;
	if ( filename == "" ) {
		alert("请选择文件");
		return;
	}
	
	var ext = getType(filename);
	if ( ext != ".xls" && ext != ".xlsx" ) {
		alert("请选择excel文件");
		return;
	}

	var fd = new FormData();
	//fd.append("paragram", 12345);
	//console.log($("#fileUpload1").get(0).files[0]);
	fd.append("upfile", $("#fileUpload1").get(0).files[0] );
	
	$.ajax({
		 url: addr+"manage",
		 type: "POST",
		 processData: false,
		 contentType: false,
		 data: fd,
		 success: function(data) {
			console.log(data);
		 }
	})

	
	/*
	jQuery.ajax({
		type: "post",
		url: addr+"manage",
		enctype: 'multipart/form-data',
		data: {
			file: filename
		},
		error: function (err) { console.log("upload excel file failed!"); },
		success: function (data) {
			console.log(data);
			if ( 0 == data.error ) {
				console.log("upload excel file success!");
			} else {
				console.log("upload excel file failed!!!");
			}
	}});
	*/
}