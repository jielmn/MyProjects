//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    windowWidth: 375,
    windowHeight: 667,

    bkImage: null,
  },

  onLoad:function(e) {
    var that = this;
    
    this.setData({ windowWidth: app.globalData.windowWidth || 320 })
    this.setData({ windowHeight: app.globalData.windowHeight || 100 })
    console.log(app.globalData.windowWidth)
    console.log(app.globalData.windowHeight)

    app.globalData.timer_bk = setTimeout(function () {
      wx.switchTab({
        url: '../mine/mine',
      });
    }.bind(this), 5000)

    wx.request({
      url: app.globalData.serverAddr + '?type=bk',
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {

          console.log("get background success!")
          //console.log(res);
          that.setData({ bkImage: res.data.bkimage})
          
        } else {
          console.log("get background result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get todolbackgroundist")
      },
      complete() {
        
      },
    })


  },

  onEnter:function() {
    clearTimeout(app.globalData.timer_bk);
    wx.switchTab({
      url: '../mine/mine',
    })
  },


})
