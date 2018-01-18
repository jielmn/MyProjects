//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    todolist_items: [
      { id: 1, value: '很长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长长的清单'},
      { id: 2, value: '清单2'},
      { id: 3, value: '清单3'},
      { id: 4, value: '清单4' },
      { id: 5, value: '清单5' },
      { id: 6, value: '清单6' },
      { id: 7, value: '测试清单7' },
      { id: 8, value: '测试清单8' },
      { id: 9, value: '测试清单9' },
      { id: 10, value: '测试清单10' },
      { id: 11, value: '测试清单11' }
    ],

    history_todolist_items: [
      { id: 100, value: '完成的很长长长长长长长长长长长长长长长长长长长长长长长长长长长长长的清单' },
      { id: 101, value: '完成的清单2' },
      { id: 102, value: '完成的清单3' },
      { id: 103, value: '完成的清单4' },
      { id: 104, value: '完成的清单5' },
      { id: 105, value: '完成的清单6' },
    ],

    windowWidth:375,
    windowHeight: 667,

    todo_item_text_width:240,
    todo_item_width:0,

    self:false,

    personid:null,
    personInfo:null,
  },
  
  onLoad: function (options) {
    console.log("mine page onloading......")
    
    var that = this;

    this.setData({ windowWidth:  app.globalData.windowWidth || 320 })
    this.setData({ windowHeight: app.globalData.windowHeight || 568 })

    var todo_item_text_width = this.data.windowWidth -5 - 50 ;
    this.setData({ todo_item_text_width: todo_item_text_width })

    var todo_item_width = this.data.windowWidth + 60;
    this.setData({ todo_item_width: todo_item_width })

    this.setData({ userInfo: app.globalData.userInfo})
    this.setData({ openid:   app.globalData.openid })

    app.userInfoReadyCallback = res => {
      this.setData({
        userInfo: res.userInfo
      })
      wx.hideLoading()
      this.CheckPersonId();
    }  

    app.openidReadyCallback = openid => {
      this.setData({
        openid: openid
      })

      // 检查是否自己
      this.CheckPersonId();      
    }  

    var personid = options.personid
    this.setData({ personid: personid })

    // 检查是否自己
    this.CheckPersonId();

    // 计算已经过去多少时间
    //this.CalculateElapsedTime(this.data.items);
    //this.setData({items:this.data.items})
  },

  onShow:function(){
    var that = this;
    console.log("mine page onshowing......")
    // 如果没有获取到用户信息，开启loading条
    if (!that.data.userInfo) {      
      wx.showLoading({
        title: '加载中',
      })
      
      setTimeout(function () {
        wx.hideLoading()
        if (!that.data.userInfo) {
          wx.navigateTo({ url: '../error/error' })
        }
      }, 2000)
    }
  },

  OnDelete: function() {
    console.log("delete.")
  },

  OnCheckbox: function () {
    console.log("check")
  },

  OnIgnore:function() {
    
  },

  // 检查person id是否是当前的登录用户
  CheckPersonId : function() {
    if ( this.data.personid == null ) {
      this.setData({ self: true })
      this.setData({ personInfo: this.data.userInfo })
    } else {
      // 还没有拿到openid
      if ( this.data.openid == null ) {
        return;
      }      
      if (this.data.personid == this.data.openid ) {
        this.setData({ self: true })
        this.setData({ personInfo: this.data.userInfo })
      }
    }

    if ( this.data.self == false ) {
      this.setData({ todo_item_text_width: this.data.windowWidth - 20 })
      this.setData({ todo_item_width: this.data.windowWidth })
    }

    /*
    this.setData({ self: false })
    this.setData({ todo_item_text_width: this.data.windowWidth - 20 })
    this.setData({ todo_item_width: this.data.windowWidth })
    */
  },



  /*
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
  */

})
