//index.js
//获取应用实例
const app = getApp()

Page({

  data: {
    login: false,
    weibo_items: [
      { src0: "https://www.telemed-healthcare.cn/todolist_company/images/10_1.jpg", id: 1, avatarUrl: "https://www.telemed-healthcare.cn/todolist_company/images/10_1.jpg", name: "测试1", content: "123", src1: "https://www.telemed-healthcare.cn/todolist_company/images/10_1.jpg", src2: "https://www.telemed-healthcare.cn/todolist_company/images/10_1.jpg", src3: "https://www.telemed-healthcare.cn/todolist_company/images/10_1.jpg", src4: "https://www.telemed-healthcare.cn/todolist_company/images/10_1.jpg"},
      { src0: "https://www.telemed-healthcare.cn/todolist_company/images/1_1.jpg", id: 2, avatarUrl: "https://www.telemed-healthcare.cn/todolist_company/images/1_1.jpg", name: "测试2", content: "456", src1: "https://www.telemed-healthcare.cn/todolist_company/images/1_1.jpg", src2: "https://www.telemed-healthcare.cn/todolist_company/images/1_1.jpg" }
       ]
  },

  // 加载
  onLoad: function (options) {
    var that = this;

    // 是否在服务器登录 
    this.setData({ login: app.globalData.login })

    app.loginReadyCallback = function () {
      console.log("login callback..., in front")
      that.setData({ login: true })
      // 获取微博数据
    }

    //获取微博数据

  },

  // 下拉更新
  onPullDownRefresh: function () {
    // 重置数据

    this.onLoad()
  },

  toAddWeibo:function() {
    wx.navigateTo({ url: '../weibo_item/weibo_item' })
  },

  onReachBottom() {
    console.log('--front.onReachBottom')
  },

}) //  END OF PAGE