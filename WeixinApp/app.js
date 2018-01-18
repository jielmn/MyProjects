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
      }
    })

    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
        console.log("wx.login success.")
        // 登录成功后获取openid
        app_obj.getOpenid(res.code);
      }
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

          }
        })

      }
    })


    // 测试用
    /*
    wx.request({
      url: 'http://118.25.26.186:8080/helloapp/login.htm',
      method: 'GET',
      success: (res) => {

      }
    })
    */
  },

  getOpenid: function (code) {
    console.log("getting openid")
    var that = this;
    wx.request({
      url: 'https://api.weixin.qq.com/sns/jscode2session?appid=' + that.globalData.appid + '&secret=' + that.globalData.secret + '&js_code=' + code + '&grant_type=authorization_code',
      data: {},
      method: 'GET',
      success: function (res) {
        var obj = {};
        console.log("ok, openid=" + res.data.openid)
        obj.openid = res.data.openid;
        obj.expires_in = Date.now() + res.data.expires_in;
        obj.session_key = res.data.session_key;
        that.globalData.openid = res.data.openid;     

        if (that.openidReadyCallback) {
          that.openidReadyCallback(res.data.openid)
        }
        
      }
    });
  },


  globalData: {    
    appid: "wx692cfffa15fdf5d9",
    secret: "3ba624f6c7add4542b0140414bea8f56",
    
    userInfo: null,
    openid:null,
    windowWidth:0,
    windowHeight:0
  }

})