//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    todolist_items: [
      { id: 1, value: 'abc的很长长长长长长长长长长长长长长长长长长长长长长长长长长长的清单',personid:'111',nickName:'abc' },
      { id: 2, value: 'abc的清单2', personid: '111', nickName: 'abc' },
      { id: 3, value: 'hello的清单1', personid: '111', nickName: 'hello' },
      { id: 4, value: 'world的清单1', personid: '111', nickName: 'world' },
      { id: 5, value: 'world的清单2', personid: '111', nickName: 'world' },
      { id: 6, value: 'abc的清单3', personid: '111', nickName: 'abc' },
      { id: 7, value: '某某某某的清单1', personid: '111', nickName: '某某某某' },
      { id: 8, value: '某某某某的清单2', personid: '111', nickName: '某某某某' }
    ],

    history_todolist_items: [
      { id: 100, value: '完成的很长长长长长长长长长长长长长长长长长长长长长长长长长长长长长的清单' },
      { id: 101, value: '完成的清单2' },
      { id: 102, value: '完成的清单3' },
      { id: 103, value: '完成的清单4' },
      { id: 104, value: '完成的清单5' },
      { id: 105, value: '完成的清单6' },
    ],

    array: ['全部','某某1', '某某2', '某某3', '某某4', '某某5', '某某6', '某某7', '某某8', '某某9', '某某10',
            '某某11', '某某12', '某某13', '某某14', '某某15', '某某16', '某某17', '某某18', '某某19', '某某20',
            '某某21', '某某22', '某某23', '某某24', '某某25', '某某26', '某某27', '某某28', '某某29', '某某30',
            '某某31', '某某32', '某某33', '某某34', '某某35', '某某36', '某某37', '某某38', '某某39', '某某40',
            ],
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