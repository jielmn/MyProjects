
//获取应用实例
const app = getApp()

Page({
  data: {
    userInfo: {},
    hasUserInfo: false,
    canIUse: wx.canIUse('button.open-type.getUserInfo'),
    members: [],
    hidden:true,
    newMemberName:'',
  },

  newMemberName:'',

  onLoad: function () {
    if (app.globalData.userInfo) {
      this.setData({
        userInfo: app.globalData.userInfo,
        hasUserInfo: true
      })
    } else if (this.data.canIUse){
      // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
      // 所以此处加入 callback 以防止这种情况
      app.userInfoReadyCallback = res => {
        this.setData({
          userInfo: res.userInfo,
          hasUserInfo: true
        })
      }
    } else {
      // 在没有 open-type=getUserInfo 版本的兼容处理
      wx.getUserInfo({
        success: res => {
          app.globalData.userInfo = res.userInfo
          this.setData({
            userInfo: res.userInfo,
            hasUserInfo: true
          })
        }
      })
    }

    this.setData({
      members: app.globalData.members
    })

    app.addMemberCallback = function(res) {
      if ( res.errCode != 0 ) {
        wx.showToast({
          title: res.errMsg,
          icon:'none',
          duration: 2000
        })
      }  else {

      }    
    }
  },

  getUserInfo: function(e) {
    console.log(e)
    app.globalData.userInfo = e.detail.userInfo
    this.setData({
      userInfo: e.detail.userInfo,
      hasUserInfo: true
    })
  },

  onAddMember() {
    this.setData({
      newMemberName:'',
      hidden:!this.data.hidden
    })
  },

  confirmMember:function(e) {
    this.setData({
      hidden: !this.data.hidden
    })
    // app.log("new member:", this.newMemberName);
    app.addMember(this.newMemberName);
  },

  cancelMember: function (e){
    this.setData({
      hidden: !this.data.hidden
    })
  },

  inputMemberName:function(e) {
    this.newMemberName = e.detail.value;
  }
})
