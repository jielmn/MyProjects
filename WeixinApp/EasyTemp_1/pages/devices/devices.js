//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    devices:[],
    spinShow:false,
    currentId:''
  },

  onLoad: function (options){
    var devices = app.globalData.devices || [];
    var currentId = '';
    if ( app.globalData.device ) {
      currentId = app.globalData.device.deviceId;
    }

    devices.forEach(device => {
      if (device.deviceId == currentId) {
        device.selected = 'selected';
      } else {
        device.selected = '';
      }
    })

    this.setData({
      devices: devices,
      currentId: currentId
    });
  },

  onSelectDevice:function(e){
    // app.log('onSelectDevice', e);

    // 返回
    setTimeout(function () {
      app.globalData.selectedDevice = e.currentTarget.dataset.deviceid
      wx.navigateBack();
    }, 800);
  }




})