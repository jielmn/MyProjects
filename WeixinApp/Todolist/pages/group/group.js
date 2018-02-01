//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    todolist_items: [    
    ],

    history_todolist_items: [
    ],

    users: [{ nick_name: '全部', user_id: null, avatarUrl:null}],
    index: 0,

    windowWidth: 375,
    windowHeight: 667,

    todo_item_text_width: 240,
    todo_item_width: 0,

    login:false,
  } ,

  onLoad: function (options) {
    var that = this;

    this.setData({ windowWidth: app.globalData.windowWidth || 320 })
    this.setData({ windowHeight: app.globalData.windowHeight || 568 })

    var todo_item_text_width = this.data.windowWidth - 55;
    this.setData({ todo_item_text_width: todo_item_text_width })

    var todo_item_width = this.data.windowWidth ;
    this.setData({ todo_item_width: todo_item_width })

    this.setData({ index: 0})
    this.setData({ users: this.data.users })

    // 是否在服务器登录 
    this.setData({login: app.globalData.login})

    app.loginReadyCallback = function () {
      console.log("login callback...")
      that.setData({ login: true })
      that.getUsers()
    }

    this.getUsers();
  },

  getUsers: function () {
    var that = this;

    if (!this.data.login) {
      return
    }

    wx.showLoading({
      title: '获取所有用户别名',
    })

    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=users',
      method: 'GET',
      success: (res) => {

        if (res.data.error != null && res.data.error == 0) {
          console.log("get users success!")
          that.setData({ users: that.data.users.concat(res.data.users) })
          // 获取未完成清单
          that.getTodolist(0);
        } else {
          console.log("get users result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get users")
        console.log(res);

        wx.hideLoading();
      },
      complete() {
        
      },
    })

  },

  getTodolist : function (is_complete) {
    var that = this

    if (!this.data.login) {
      return
    }

    if ( is_complete == 0 ) {
      wx.showLoading({
        title: '获取未完成清单',
      })
    } else {
      wx.showLoading({
        title: '获取已完成清单',
      })
    }
    

    var user_id = this.data.users[this.data.index].user_id
    if ( !user_id ) {
      user_id = ""
    }

    var start_index = 0;
    if ( is_complete != 0 ) {
      start_index = this.data.history_todolist_items.length;
    }

    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=any_todolist&user_id=' + encodeURIComponent(user_id) + '&is_complete=' + is_complete + '&start_index='+start_index,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("get any todolist success![" + is_complete + "]" )      
          //console.log(res)
          that.CalculateElapsedTime(res.data.todolist);
          if ( is_complete == 0 ) {
            that.setData({ todolist_items: res.data.todolist })
            // 再获取已完成清单
            that.getTodolist(1)
          } else {
            that.setData({ history_todolist_items: that.data.history_todolist_items.concat(res.data.todolist) })
          }          
        } else {
          console.log("get any todolist result123:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get any todolist")
        console.log(res);
      },
      complete() {
        wx.hideLoading();
      },
    })

  },

  usersChange:function(e) {

    if (e.detail.value != this.data.index){
      this.setData({ index: e.detail.value})
    }

    this.data.todolist_items = []
    this.data.history_todolist_items = []
    this.getTodolist(0);
  },

  findFromArray: function (arr, key, val) {
    for (var i = 0; i < arr.length; i++) {
      if (arr[i][key] == val) {
        return arr[i]
      }
    }
    return null
  },

  OnDetail: function (e) {
    //console.log("on detail")

    var that = this;
    var id = e.currentTarget.dataset.id
    var del_type = e.currentTarget.dataset.type

    var item

    if (del_type) {
      item = this.findFromArray(that.data.history_todolist_items, 'id', id);
    } else {
      item = this.findFromArray(that.data.todolist_items, 'id', id);
    }

    if (!item) {
      return;
    }

    //console.log("test....")
    //console.log(item);

    if (del_type) {
      wx.navigateTo({
        url: '../showItem/showItem?id=' + id + '&content=' + item.value + '&start_time=' + item.start_time + '&is_complete=true&username=' + item.nickname ,
      })
    } else {
      wx.navigateTo({
        url: '../showItem/showItem?id=' + id + '&content=' + item.value + '&start_time=' + item.start_time + '&username=' + item.nickname,
      })
    }

  },

  loadMoreHistory: function () {
    console.log('load more history')
    this.getTodolist(1);
  },

  onPullDownRefresh: function () {
    this.data.todolist_items = []
    this.data.history_todolist_items = []
    this.data.users = [{ nick_name: '全部', user_id: null, avatarUrl: null }]
    this.data.index = 0
    this.data.login = false

    this.onLoad()
  },

  CalculateElapsedTime: function (items) {
    var now = Date.now()
    for (var i in items) {
      var start_time = items[i].start_time || 0;
      if (start_time > 0 && start_time <= now) {
        items[i].start_time_txt = this.FormatTime(now - start_time);
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

  FormatTime: function (elapsed) {
    if (elapsed < 0) {
      return ''
    }

    elapsed = parseInt(elapsed / 1000);

    // 小于一天
    if (elapsed < 3600 * 24) {
      var hour = parseInt(elapsed / 3600);
      var minute = parseInt((elapsed % 3600) / 60);
      if (hour > 0) {
        return hour + '小时' + minute + '分'
      }
      return minute + '分'
    }

    var day = parseInt(elapsed / (3600 * 24))
    return day + '天'
  },

  OnIgnore:function() {

  }

})