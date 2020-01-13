// pages/welcome.js

const app = getApp()

Page({

  /**
   * 页面的初始数据
   */
  data: {
    
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    this._timerid = setTimeout( function() {

      if (!app.globalData.openid) {
        /*
        wx.showToast({
          title: '未获取到open id',
          duration: 2000
        })
        */
        return;
      }
      
      wx.getUserInfo({
        success:function(res){
          wx.switchTab({
            url: '../thermo/thermo'
          });
        },
        fail:function(res) {
          /*
          wx.showToast({
            title: '获取用户信息失败',
            duration: 2000
          })
          */
          return;
        }
      })
    }, 5000)
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    clearTimeout(this._timerid);
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },

  onStart: function(e) {
    app.log("onStart");
    
    if ( e.detail.errMsg != "getUserInfo:ok") {
      wx.showToast({
        title: '获取用户信息失败',
        duration: 2000
      })
      return;
    }

    if ( !app.globalData.openid ) {
      wx.showToast({
        title: '未获取到open id',
        duration: 2000
      })
      return;
    }

    wx.switchTab({
      url: '../thermo/thermo'
    });

  }
})