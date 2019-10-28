$(function(){
	// var nav_list = $('.nav_list'); 
    $('.nav_list').bind('mouseover',function(e){
		var index = $(this).index();
		var one = $(this).find('.one');
		var current = 'current';
		one.attr('id',current);
		var list_one = $(this).find('.list_one');
		list_one.css('display','block');
		
		// console.log(index);
	});
	$('.nav_list').bind('mouseout',function(e){
		var index = $(this).index();
		var one = $(this).find('.one');
		one.attr('id','');
		var list_one = $(this).find('.list_one');
		list_one.css('display','none');
		
	});
	
	$('.sub').bind('mouseover',function(e){
		var list_two = $(this).find('.list_two');
		list_two.css('display','block');
	});
	$('.sub').bind('mouseout',function(e){
		var list_two = $(this).find('.list_two');
		list_two.css('display','none');
	});
	
});