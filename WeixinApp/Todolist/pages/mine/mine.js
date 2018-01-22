//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    todolist_items: [
    ],

    history_todolist_items: [
      /*{ id: 100, value: '完成的很长长长长长长长长长长长长长长长长长长长长长长长长长长长长长的清单' },
      { id: 101, value: '完成的清单2' },
      { id: 102, value: '完成的清单3' },
      { id: 103, value: '完成的清单4' },
      { id: 104, value: '完成的清单5' },
      { id: 105, value: '完成的清单6' },*/
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

    wx.request({
      url: 'http://118.25.26.186:8080/todolist/main?type=delete&id=' + id,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("delete success!")
          wx.showToast({
            title: '删除成功'
          })
          that.deleteFromArray(that.data.todolist_items, 'id', id)
          that.setData({ todolist_items: that.data.todolist_items })        
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

  OnDetail:function() {
    //console.log("on detail")

    var that = this;
    var id = e.currentTarget.dataset.id

    var item = findFromArray(that.data.todolist_items,'id',id);
    if ( !item ) {
      return;
    }

    
  },

  OnCheckbox: function () {
    console.log("check")
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
      url: 'http://118.25.26.186:8080/todolist/main?type=register&open_id=' + this.data.openid+'&name=' + this.data.userInfo.nickName+'&avatarUrl='+this.data.userInfo.avatarUrl + '&abc= 你好',
      method: 'GET',
      success: (res) => {
        if ( res.data.error != null && res.data.error == 0 ) {
          that.setData({ login: true })            
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
      url: 'http://118.25.26.186:8080/todolist/main?type=todolist&open_id=' + this.data.openid,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          
          console.log("get todolist success!")
          //console.log(res);

          var items = res.data.todolist;
          that.CalculateElapsedTime(items);
          //console.log(items)
          that.setData({ todolist_items: items })

        } else {
          console.log("get todolist result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get todolist")
      },
      complete() {
        wx.hideLoading()
      },
    })
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
