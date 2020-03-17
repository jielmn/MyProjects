//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    devices:[{a:1},{b:2}]
  },

  onSelectDevice:function(e){
    app.log('onSelectDevice', e);
  }




})