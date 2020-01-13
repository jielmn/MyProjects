//app.js
App({
  onLaunch: function() {
    // 保存变量
    var app_obj = this;

    // 展示本地存储能力
    // var logs = wx.getStorageSync('logs') || []
    // logs.unshift(Date.now())
    // wx.setStorageSync('logs', logs)
    
    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
        console.log("== wx.login success!");
        app_obj.getOpenid(res.code);
      }
    })

    
    // 获取用户信息
    wx.getSetting({
      success: res => {
        if (res.authSetting['scope.userInfo']) {
          // 已经授权，可以直接调用 getUserInfo 获取头像昵称，不会弹框
          wx.getUserInfo({
            success: res => {
              // 可以将 res 发送给后台解码出 unionId
              this.globalData.userInfo = res.userInfo

              // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
              // 所以此处加入 callback 以防止这种情况
              if (this.userInfoReadyCallback) {
                this.userInfoReadyCallback(res)
              }
            }
          })
        }        
      }
    })

    wx.getLocation({
      type: 'wgs84',
      success(res) {
        console.log("== get location", res);
      }
    })
  },

  // 获取open_id
  getOpenid: function(code) {
    console.log("== getting openid");

    var that = this;
    wx.request({
      url: that.globalData.serverAddr + '?type=get_openid&code=' + encodeURIComponent(code),
      data: {},
      method: 'GET',
      success: function(res) {

        console.log("== get openid success")
        that.globalData.openid = res.data.openid;
      }, // success

      // 获取openid失败
      fail() {
        console.log("== get openid failed");
      },

    }); // wx.request
  },

  inArray: function(arr, key, val) {
    for (let i = 0; i < arr.length; i++) {
      if (arr[i][key] === val) {
        return i;
      }
    }
    return -1;
  },

  // ArrayBuffer转16进度字符串示例
  ab2hex: function(buffer) {
    var hexArr = Array.prototype.map.call(
      new Uint8Array(buffer),
      function(bit) {
        return ('00' + bit.toString(16)).slice(-2)
      }
    )
    return hexArr.join('');
  },

  // 字符串转byte
  stringToBytes: function(str) {
    var strArray = new Uint8Array(str.length);
    for (var i = 0; i < str.length; i++) {
      strArray[i] = str.charCodeAt(i);
    }
    const array = new Uint8Array(strArray.length)
    strArray.forEach((item, index) => array[index] = item)
    return array.buffer;
  },

  log: function(str, obj) {
    str = "== " + str;
    if (obj) {
      console.log(str, obj);
    } else {
      console.log(str)
    }
  },

  globalData: {
    userInfo: null,
    serverAddr: "https://telemed-healthcare.cn/easytemp/main",
    openid: null
  }
})