//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    items: [
      { id: 1, value: '11111111111111111111111111111111111111111111111111111111111111111111'},
      { id: 2, value: '222', start_time:1515720680262 },
      { id: 3, value: '333', start_time: 1515720680269 },
      { id: 4, value: '44444' },
      { id: 5, value: '555555' },
      { id: 6, value: '66666' },
      { id: 7, value: '测试清单7' },
      { id: 8, value: '测试清单8' },
      { id: 9, value: '测试清单9' },
      { id: 10, value: '测试清单10' },
      { id: 11, value: '测试清单11' }
    ],

    items1: [
      { id: 100, value: '美国' },
      { id: 101, value: '中国' },
      { id: 102, value: '巴西' },
      { id: 103, value: '日本' },
      { id: 104, value: '英国' },
      { id: 105, value: '法国' },
    ],

    windowWidth:375,
    windowHeight: 667,

    todo_item_text_width:240,
    todo_item_width:0,
  },
  
  onLoad: function () {
    this.setData({ windowWidth: wx.getStorageSync('windowWidth') || 320 } )
    this.setData({ windowHeight: wx.getStorageSync('windowHeight') || 568  })

    var todo_item_text_width = this.data.windowWidth -5 - 50 ;
    this.setData({ todo_item_text_width: todo_item_text_width })

    var todo_item_width = this.data.windowWidth + 60;
    this.setData({ todo_item_width: todo_item_width })
    //this.CalculateElapsedTime(this.data.items);
    //this.setData({items:this.data.items})
  },

  OnDelete: function() {
    console.log("delete.")
  },

  OnCheckbox: function () {
    console.log("check")
  },

  OnIgnore:function() {

  },

  CalculateElapsedTime:function( items ) {
    var now = Date.now()
    for (var i in items ) {
      var start_time = items[i].start_time || 0;
      if ( start_time > 0 && start_time <= now ) {
        items[i].start_time_txt = this.FormatTime(now-start_time);
      } else {
        items[i].start_time_txt = '0分'
      }
    }
  },

  FormatTime : function (elapsed) {
    if (elapsed < 0 ) {
      return ''
    }

    elapsed = parseInt(elapsed / 1000);

    // 小于一天
    if ( elapsed < 3600 * 24 ) {
      var hour = parseInt( elapsed / 3600 );
      var minute = parseInt( ( elapsed % 3600 ) / 60 );
      if ( hour > 0 ) {
        return hour + '小时' + minute + '分'
      }      
      return minute + '分'
    }

    var day = parseInt(elapsed / (3600 * 24))
    return day + '天'
  }

})
