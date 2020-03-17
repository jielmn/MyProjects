//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    devices:[],
    spinShow:false
  },

  onLoad: function (options){
    var devices = app.globalData.devices || [];

    this.setData({
      devices:devices
    });
  },

  onSelectDevice:function(e){
    app.log('onSelectDevice', e);
  }




})