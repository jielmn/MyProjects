//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    todolist_items: [    
    ],

    history_todolist_items: [
    ],

    array: ['全部'],
    index: 0,

    windowWidth: 375,
    windowHeight: 667,

    todo_item_text_width: 240,
    todo_item_width: 0,
  } ,

  onLoad: function (options) {

    this.setData({ windowWidth: app.globalData.windowWidth || 320 })
    this.setData({ windowHeight: app.globalData.windowHeight || 568 })

    var todo_item_text_width = this.data.windowWidth - 55;
    this.setData({ todo_item_text_width: todo_item_text_width })

    var todo_item_width = this.data.windowWidth ;
    this.setData({ todo_item_width: todo_item_width })

    
  } 

})