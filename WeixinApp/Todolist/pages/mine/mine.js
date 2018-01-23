//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    todolist_items: [
    ],

    history_todolist_items: [
    ],

    windowWidth:375,
    windowHeight: 667,

    todo_item_text_width:240,
    todo_item_width:0,

    self:false,    
    personid:null,
    personInfo:null,

    login:false,           // 是否登录
    open_id: null,         // 用户id
    userInfo: null,        // 用户信息

    infoFailResult:false,         // 是否已经获取过info的失败结果
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

    // 设置userInfo成功的回调
    app.userInfoReadyCallback = res => {
      console.log("callback get userInfo")
      wx.hideLoading()

      this.setData({
        userInfo: res.userInfo
      })

      // 修改personInfo      
      this.CheckPersonId();

      // 注册自己
      this.Register();
    }  

    // 获取userInfo失败的回调 
    app.userInfoFailCallback = function() {
      console.log("get info failed result")
      that.data.infoFailResult = true;
      wx.hideLoading()
      wx.navigateTo({ url: '../error/error' })
    }

    // 获取openid成功的回调
    app.openidReadyCallback = openid => {
      this.setData({
        openid: openid
      })

      // 检查是否自己
      this.CheckPersonId();  

      // 注册自己
      this.Register();    
    }  

    // 获取openid失败的回调
    app.openidFailCallback = function () {

    }

    // 获取传递的参数personid
    var personid = options.personid
    this.setData({ personid: personid })

    // 检查是否自己
    // 修改personInfo
    this.CheckPersonId();

    // 注册用户
    this.Register();

    // 计算已经过去多少时间
    //this.CalculateElapsedTime(this.data.items);
    //this.setData({items:this.data.items})
    //var arr = [{ id: 1, value: 'aaa' }, { id: 2, value: 'bbb' }, { id: 3, value: 'ccc' }, ]
    //this.deleteFromArray( arr, 'id', 3 )
    //console.log("test....")
    //console.log(arr)
  },

  onShow: function (){
    var that = this;
    console.log("mine page onshowing......")

    // 如果没有获取到用户信息，开启loading条
    if (!that.data.userInfo) {      
      wx.showLoading({
        title: '获取userInfo中',
      })

      // 如果得到获取的结果，则跳转
      if (this.data.infoFailResult){
        setTimeout(function () {
          wx.hideLoading()
          wx.navigateTo({ url: '../error/error' })
        }, 1000)
      }
    }

    // 如果从additem页面返回
    if (app.globalData.additem) {
      var item = {
        id: app.globalData.itemid,
        value: app.globalData.content,
        start_time: Date.now(),
        start_time_txt:'0分',
      };
      
      this.data.todolist_items.push(item)
      this.setData({ todolist_items: this.data.todolist_items })
      
      app.globalData.additem = false
      app.globalData.content=null
      app.globalData.itemid = null
    }
  },

  deleteFromArray: function(arr, key,val) {  
    for (var i = 0; i < arr.length; i++) {
      if (arr[i][key] == val) {
        arr.splice(i, 1);
        break;
      }
    }
  },

  findFromArray: function( arr, key, val ) {
    for (var i = 0; i < arr.length; i++) {
      if (arr[i][key] == val) {
        return arr[i]
      }
    }
    return null
  },

  OnDelete: function(e) {
    //console.log("delete....")
    //console.log(e.currentTarget.dataset.id)
    var that = this;
    var id = e.currentTarget.dataset.id
    var del_type = e.currentTarget.dataset.type

    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=delete&id=' + id,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("delete success!")
          wx.showToast({
            title: '删除成功'
          })

          if (del_type) {
            that.deleteFromArray(that.data.history_todolist_items, 'id', id)
            that.setData({ history_todolist_items: that.data.history_todolist_items })
          } else {
            that.deleteFromArray(that.data.todolist_items, 'id', id)
            that.setData({ todolist_items: that.data.todolist_items })        
          }          
        } else {
          console.log("delete result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to delete")
        console.log(res);
      },
      complete() {
        
      },
    })
  },

  OnDetail:function(e) {
    //console.log("on detail")

    var that = this;
    var id = e.currentTarget.dataset.id
    var del_type = e.currentTarget.dataset.type

    var item

    if ( del_type ) {
      item = this.findFromArray(that.data.history_todolist_items, 'id', id);
    } else {
      item = this.findFromArray(that.data.todolist_items, 'id', id);
    }
    
    if ( !item ) {
      return;
    }

    //console.log("test....")
    //console.log(item);

    if ( del_type ) {
      wx.navigateTo({
        url: '../showItem/showItem?id=' + id + '&content=' + item.value + '&start_time=' + item.start_time + '&is_complete=true',
      })
    } else {
      wx.navigateTo({
        url: '../showItem/showItem?id=' + id + '&content=' + item.value + '&start_time=' + item.start_time,
      })
    }
    
  },

  OnCheckbox: function (e) {
    //console.log("check")
    var that = this;
    var id = e.currentTarget.dataset.id
    var del_type = e.currentTarget.dataset.type

    var item;

    if ( del_type ) {
      item = this.findFromArray(that.data.history_todolist_items, 'id', id);
    } else {
      item = this.findFromArray(that.data.todolist_items, 'id', id);
    }    

    if (!item) {
      return;
    }

    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=finish&id=' + id,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("finish success!")

          /*wx.showToast({
            title: '清单状态修改OK'
          })*/

          if (del_type) {
            that.deleteFromArray(that.data.history_todolist_items, 'id', id)
            that.setData({ history_todolist_items: that.data.history_todolist_items })

            that.data.todolist_items.unshift(item)
            that.setData({ todolist_items: that.data.todolist_items })

          } else 
          {
            that.deleteFromArray(that.data.todolist_items, 'id', id)
            that.setData({ todolist_items: that.data.todolist_items })

            item.end_time=Date.now()
            item.end_time_txt='0分'
            that.data.history_todolist_items.unshift(item)
            that.setData({ history_todolist_items: that.data.history_todolist_items })
          }          
        } else {
          console.log("finish result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to finish")
        console.log(res);
      },
      complete() {

      },
    })

  },

  OnIgnore:function() {
    
  },

  // 注册当前用户
  Register: function() {
    var that  = this;

    // 还没有拿到openid
    if (!this.data.openid) {
      return;
    }

    // 还没有拿到userInfo
    if (!this.data.userInfo) {
      return;
    }

    // 如果已经注册
    if (this.data.login){
      return;
    }

    wx.showLoading({
      title: '登录中',
    })

    // 开始注册
    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=register&open_id=' + this.data.openid+'&name=' + this.data.userInfo.nickName+'&avatarUrl='+this.data.userInfo.avatarUrl + '&abc= 你好',
      method: 'GET',
      success: (res) => {
        if ( res.data.error != null && res.data.error == 0 ) {
          that.setData({ login: true })            
          app.globalData.login = true;
          if (app.loginReadyCallback ) {
            app.loginReadyCallback()
          }
          console.log("login success!") 
          that.GetTodoList();
        } else {
          console.log("register result:")
          console.log(res);
        }        
      },
      fail() {
        console.log("failed to register")
        wx.showModal({
          title: '登录失败',
          content: '登录失败，请稍后再试',
          showCancel:false,
        })
      },
      complete() {
        wx.hideLoading()
      },
    })

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
  },

  toAddItem:function() {
    wx.navigateTo({ url: '../item/item' })
  },

  canHideLoading:function() {
    if ( !this.data.login ) {
      return false;
    }

    if (!this.data.open_id ){
      return false;
    }

    if ( !this.data.userInfo ) {
      return false;
    }
    return true;
  },

  GetTodoList:function() {
    var that = this;

    // 如果没有登录
    if ( !this.data.login ) {
      return;
    }

    // 如果没有获取到openid
    if ( !this.data.openid ) {
      return;
    }

    wx.showLoading({
      title: '获取正在进行的清单',
    })

    // 开始注册
    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=todolist&open_id=' + this.data.openid,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          
          console.log("get todolist success!")
          //console.log(res);

          var items = res.data.todolist;
          that.CalculateElapsedTime(items);
          //console.log(items)
          that.setData({ todolist_items: items })

          that.GetHistoryTodoList();
        } else {
          console.log("get todolist result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get todolist")
      },
      complete() {
        //wx.hideLoading()
      },
    })
  },

  GetHistoryTodoList:function(start_index) {
    var that = this;

    // 如果没有登录
    if (!this.data.login) {
      return;
    }

    // 如果没有获取到openid
    if (!this.data.openid) {
      return;
    }

    if ( !start_index ) {
      start_index = 0
    }

    wx.showLoading({
      title: '获取历史清单',
    })


    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=history&open_id=' + this.data.openid + '&start_index=' + start_index,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("get history success!")

          var items = res.data.todolist;
          that.CalculateElapsedTime(items);

          for (var i in items) {
            var item      = items[i]
            var item_find = that.findFromArray(that.data.history_todolist_items, 'id', item.id);
            if ( !item_find ) {
              that.data.history_todolist_items.push(item)
            }
          }

          that.setData({ history_todolist_items: that.data.history_todolist_items })
        } else {
          console.log("get history result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get history")
      },
      complete() {
        wx.hideLoading()
      },
    })
  },

  loadMoreHistory:function() {
    console.log('load more history')
    var start_index = this.data.history_todolist_items.length
    this.GetHistoryTodoList(start_index);
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

      var end_time = items[i].end_time || 0;
      if (end_time && end_time > 0 && end_time <= now) {
        items[i].end_time_txt = this.FormatTime(now - end_time);
      } else {
        items[i].end_time_txt = '0分'
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
