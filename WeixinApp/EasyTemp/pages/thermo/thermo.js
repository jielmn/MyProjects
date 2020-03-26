//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    curtemp: 0,
    curtempColor: '#a5eddf',
    members: [],
    index: 0,
    curtagId: null,    
    bluetoothStr: "正在连接易温读卡器...",
    spinShow: true,
    connected:false,
    mesureBusy: false,
    lastDeviceId:null,
  },

  timerId: null,  
  curTempItem: null,
  measureTimerId: null,

  onLoad: function(options) {
    var that = this;
    app.temperatureCallback = function(item) {

      clearTimeout(that.measureTimerId);
      that.measureTimerId = null;
      that.setData({
        mesureBusy: false
      });

      if ( item.errno != 0 ) {        
        wx.showToast({
          title: '测温失败: ' + item.errMsg,
          icon: 'none',
          duration: 2000
        })

        return;
      }

      app.innerAudioContext.src = '/sound/ping.wav'; //链接到音频的地址
      app.innerAudioContext.play();

      var index = 0;
      var bindindex = app.inArray(app.globalData.tagsbinding, 'tagid', item.tagid);
      if ( bindindex >= 0 ) {
        index = app.inArray(that.data.members, 'id', app.globalData.tagsbinding[bindindex].memberid );
        if ( index < 0 ) {
          index = 0;
        }
      }

      // app.log("tagsbinding:", app.globalData.tagsbinding);
      // app.log("tagid is " + item.tagid );
      // app.log("index is " + index );

      that.setData({
        curtemp: item.temperature,
        curtempColor: 'white',
        curtagId:item.tagid,
        index: index
      });

      that.curTempItem = item;
      clearTimeout(that.timerId);
      that.timerId = setTimeout(function() {
        that.setData({
          curtempColor: '#a5eddf'
        });
      }, 5000);

      app.uploadTemperature(item);
    }

    app.uploadTemperatureCallback = function(res) {
      if ( res.errCode == 0 ) {
        var tmp = [];
        tmp.push(app.globalData.mine);
        var members = tmp.concat(app.globalData.members);
        that.setData({
          members: members
        })
      } else {
        var duration = res.duration || 2000;
        wx.showToast({
          title: res.errMsg,
          icon: 'none',
          duration: duration
        })
      }
    }

    app.bluetoothCallback = function(res) {
      if (res.errCode && res.errCode != 0) {
        that.setData({
          bluetoothStr: "连接易温读卡器失败:" + res.errMsg,
          spinShow: false,
          connected: false
        });
      } else {
        if (res.myCode && res.myCode != 0) {
          that.setData({
            bluetoothStr: "正在连接易温读卡器...",
            spinShow: true,
            connected: false
          });
        } else {
          that.setData({
            bluetoothStr: "连接易温读卡器成功",
            spinShow: false,
            connected: true
          });
          that.lastDeviceId = res.myDeviceId;
        }
      }

      app.bindingCallback = function(res) {
        if (res.errCode != 0) {
          wx.showToast({
            title: res.errMsg,
            icon: 'none',
            duration: 2000
          })
        } else {
          var tmp = [];
          tmp.push(app.globalData.mine);
          var members = tmp.concat(app.globalData.members);
          that.setData({
            members: members
          })
        }
      }
    }

  },

  onShow: function(options) {
    //app.log('thermo page on show');
    //app.log('app.isBluetoothStoped:', app.isBluetoothStoped );
    //app.log('app.globalData.selectedDevice:', app.globalData.selectedDevice );
    //app.log('that.lastDeviceId:', this.lastDeviceId);

    var that = this;

    if (app.isBluetoothStoped) {

      this.setData({
        bluetoothStr: "没有连接易温读卡器",
        spinShow: false
      });

      app.openBluetoothAdapter();
    } else if ( app.globalData.selectedDevice != that.lastDeviceId ) {
      app.log('reconnect device');
      that.setData({
        bluetoothStr: "正在连接易温读卡器...",
        spinShow: true,
        connected: false
      });
      app.reconnect( that.lastDeviceId, app.globalData.selectedDevice )
      app.globalData.selectedDevice = null;
    }

    var tmp = [];
    tmp.push(app.globalData.mine);
    var members = tmp.concat(app.globalData.members);
    this.setData({
      members: members
    })

    if (this.data.curtagId) {
      var index = 0;
      var bindindex = app.inArray(app.globalData.tagsbinding, 'tagid', that.data.curtagId);
      if (bindindex >= 0) {
        index = app.inArray(that.data.members, 'id', app.globalData.tagsbinding[bindindex].memberid);
        if (index < 0) {
          index = 0;
        }
      }
      this.setData({
        index: index
      })
    }
  },

  bindPickerChange: function(e) {
    // app.log("bindPickerChange",e);
    var index = parseInt(e.detail.value);

    this.setData({
      index: index
    })

    app.binding(this.data.curtagId, this.data.members[index], this.curTempItem);


  },

  onPullDownRefresh: function () {
    app.refresh();
    wx.stopPullDownRefresh(); 
  },

  onRefreshBluetooth:function() {
    app.refresh();
    wx.stopPullDownRefresh(); 
    // this.test();
  },

  test:function() {
    wx.chooseLocation({
      success: function (res){
        app.log("wx.chooseLocation success", res)
      }
    })
  },

  onMesure:function() {
    this.measureTimerId = setTimeout(this.onMesureTimeout, 10000);
    app.measureTemp();

    this.setData({
      mesureBusy: true
    });

  },

  onMesureTimeout:function() {
    wx.showToast({
      title: '测温超时',
      icon: 'none',
      duration: 2000
    })
    this.measureTimerId = null;
    this.setData({
      mesureBusy: false
    });
  },

  onSelectDevice:function(){
    //app.log('on select device')
    app.globalData.selectedDevice = null;
    wx.navigateTo({ url: '../devices/devices' })
  }




})