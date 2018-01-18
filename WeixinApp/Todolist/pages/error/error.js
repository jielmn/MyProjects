//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    
  },
  
  onAuthorize: function() {
    wx.openSetting({
      success: (res) => {
        if (res.authSetting["scope.userInfo"] == true ) {
          // 获取userinfo
          wx.getUserInfo({
            success: res => {
              console.log("[error] wx.getUserInfo success.")

              // 可以将 res 发送给后台解码出 unionId
              app.globalData.userInfo = res.userInfo
              // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
              // 所以此处加入 callback 以防止这种情况
              if (app.userInfoReadyCallback) {
                app.userInfoReadyCallback(res)
              }

            }
          })

        }
      }
    })    
  }

})
