//获取应用实例
const app = getApp()

Page({
  data: {
    userInfo: {},
    hasUserInfo: false,
    userLocation:false,
    canIUse: wx.canIUse('button.open-type.getUserInfo'),
    members: [],
    hidden: true,
    newMemberName: '',
    hidden1: true,
    hidden2: true,
  },

  newMemberName: '',
  updateIndex: null,
  delIndex:null,

  onLoad: function() {
    var that = this;

    if (app.globalData.userInfo) {
      this.setData({
        userInfo: app.globalData.userInfo,
        hasUserInfo: true
      })
    } else if (this.data.canIUse) {
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
      userLocation: app.userLocation == 2
    })

    this.setData({
      members: app.globalData.members
    })

    app.addMemberCallback = function(res) {
      if (res.errCode != 0) {
        wx.showToast({
          title: res.errMsg,
          icon: 'none',
          duration: 2000
        })
      } else {
        that.setData({
          members: app.globalData.members
        })
      }
    }

    app.updateMemberCallback = function(res) {
      if (res.errCode != 0) {
        wx.showToast({
          title: res.errMsg,
          icon: 'none',
          duration: 2000
        })
      } else {
        that.setData({
          members: app.globalData.members
        })
      }
    }

    app.deleteMemberCallback = function(res) {
      if (res.errCode != 0) {
        wx.showToast({
          title: res.errMsg,
          icon: 'none',
          duration: 2000
        })
      } else {
        that.setData({
          members: app.globalData.members
        })
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
    this.newMemberName = '';
    this.setData({
      newMemberName: '',
      hidden: !this.data.hidden
    })
  },

  confirmMember: function(e) {
    if (this.newMemberName == '') {
      return;
    }

    this.setData({
      hidden: !this.data.hidden
    })
    // app.log("new member:", this.newMemberName);
    app.addMember(this.newMemberName);
  },

  cancelMember: function(e) {
    this.setData({
      hidden: !this.data.hidden
    })
  },

  inputMemberName: function(e) {
    this.newMemberName = e.detail.value;
  },

  onEditMember: function(e) {
    var index = e.target.dataset.index;
    this.updateIndex = index;
    // app.log("onEditMember: " +index);
    this.newMemberName = this.data.members[index].name;
    this.setData({
      oldMemberName: this.data.members[index].name,
      hidden1: !this.data.hidden1
    })
  },

  cancelMember1() {
    this.setData({
      hidden1: !this.data.hidden1
    })
  },

  confirmMember1() {
    if (this.newMemberName == '') {
      return;
    }

    if (this.newMemberName == this.data.members[this.updateIndex].name) {
      return;
    }

    this.setData({
      hidden1: !this.data.hidden1
    })
    // app.log("update name:", this.newMemberName);
    // app.log("update item:", this.data.members[this.updateIndex]);
    app.updateMember(this.data.members[this.updateIndex], this.newMemberName);
  },

  updateMemberName(e) {
    this.newMemberName = e.detail.value;
  },

  onDelMember(e) {
    var index = e.target.dataset.index;
    this.delIndex = index;
    // app.log("del member : " + index);

    this.setData({
      hidden2: !this.data.hidden2
    })
  },

  confirmMember2:function() {
    this.setData({
      hidden2: !this.data.hidden2
    })

    app.delMember( this.data.members[this.delIndex].id );
  },

  cancelMember2:function() {
    this.setData({
      hidden2: !this.data.hidden2
    })    
  },

  onlogin:function() {
    var that = this;

    if ( app.userLocation == 0 ) {
      wx.getLocation({
        type: 'gcj02',
        isHighAccuracy: true,
        highAccuracyExpireTime: 5000,
        success(res) {
          app.userLocation = 2;
          app.log("get location", res);
          app.lat = res.latitude;
          app.lng = res.longitude;

          that.setData({
            userLocation:true
          })
        },
        fail(res) {
          app.userLocation = 1;
        }
      })
    } else {
      wx.openSetting({      
        success: res => {
          if (res.authSetting['scope.userLocation']) {
            app.userLocation = 2;
            that.setData({
              userLocation: true
            })
            wx.getLocation({
              type: 'gcj02',
              isHighAccuracy: true,
              highAccuracyExpireTime: 5000,
              success(res) {
                app.log("get location", res);
                app.lat = res.latitude;
                app.lng = res.longitude;
              }
            })
          }
        }
      })
    }
    


    

  }

})