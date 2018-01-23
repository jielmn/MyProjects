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
          
          that.getTodolist();
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

  getTodolist : function () {
    var that = this

    if (!this.data.login) {
      return
    }

    wx.showLoading({
      title: '获取正在进行清单',
    })

    var user_id = this.data.users[this.data.index].user_id
    if ( !user_id ) {
      user_id = ""
    }

    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=any_todolist&user_id='+user_id,
      method: 'GET',
      success: (res) => {

        if (res.data.error != null && res.data.error == 0) {
          console.log("get any todolist success!")      
          //console.log(res)
          that.setData({ todolist_items:res.data.todolist})
        } else {
          console.log("get any todolist result123:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get any todolist")
        console.log(res);
        wx.hideLoading();
      },
      complete() {
        //wx.hideLoading();
      },
    })

  },

  usersChange:function(e) {

    if (e.detail.value != this.data.index){
      this.setData({ index: e.detail.value})
    }

    this.getTodolist();
  }

})