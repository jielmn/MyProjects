//app.js
App({
  onLaunch: function () {
    
    // 保存变量
    var app_obj = this;

    // 获取屏幕宽度和高度
    wx.getSystemInfo({
      success: function (res) {
        app_obj.globalData.windowWidth = res.windowWidth
        app_obj.globalData.windowHeight = res.windowHeight
      },
    })

    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
        console.log("wx.login success.")
        // 登录成功后获取openid
        app_obj.getOpenid(res.code);
      },
    })

    // 获取用户信息
    wx.getSetting({
      success: res => {

        // 已经授权，可以直接调用 getUserInfo 获取头像昵称，不会弹框
        wx.getUserInfo({
          success: res => {
            console.log("wx.getUserInfo success.")

            // 可以将 res 发送给后台解码出 unionId
            app_obj.globalData.userInfo = res.userInfo

            // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
            // 所以此处加入 callback 以防止这种情况
            if (app_obj.userInfoReadyCallback) {
              app_obj.userInfoReadyCallback(res)
            }

          },

          // 如果失败，通知失败回调
          fail(){
            if (app_obj.userInfoFailCallback) {
              app_obj.userInfoFailCallback()
            }            
          },

        }) // end of wx.getUserInfo

      }, // end of success
    })

  }, // end of onLaunch

  // 获取open_id
  getOpenid: function (code) {
    console.log("getting openid")
    var that = this;
    wx.request({
      url: 'https://api.weixin.qq.com/sns/jscode2session?appid=' + that.globalData.appid + '&secret=' + that.globalData.secret + '&js_code=' + code + '&grant_type=authorization_code',
      data: {},
      method: 'GET',
      success: function (res) {
        console.log("ok, openid=" + res.data.openid)
        that.globalData.openid = res.data.openid;     
        // openid成功的回调函数
        if (that.openidReadyCallback) {
          that.openidReadyCallback(res.data.openid)
        }        
      },   // success

      // 获取openid失败
      fail() {
        if (that.openidFailCallback ) {
          that.openidFailCallback();
        }
      },
    }); // wx.request
  },


  globalData: {    
    appid: "wx692cfffa15fdf5d9",
    secret: "3ba624f6c7add4542b0140414bea8f56",
    
    userInfo: null,
    openid:null,
    windowWidth:0,
    windowHeight:0,

    userInfoReadyCallback:null,
    userInfoFailCallback:null,

    openidReadyCallback:null,
    openidFailCallback:null,

    additem:false,
    content:null,
  }

})