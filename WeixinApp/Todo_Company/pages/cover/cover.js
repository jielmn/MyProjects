//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    windowWidth: 375,
    windowHeight: 667,

    bkImage: "common.jpg",
  },

  onLoad:function(e) {
    this.setData({ windowWidth: app.globalData.windowWidth || 320 })
    this.setData({ windowHeight: app.globalData.windowHeight || 100 })
    console.log(app.globalData.windowWidth)
    console.log(app.globalData.windowHeight)

    setTimeout(function () {
      this.onEnter();
    }.bind(this), 10000)
  },

  onEnter:function() {
    wx.switchTab({
      url: '../mine/mine',
    })
  },


})
