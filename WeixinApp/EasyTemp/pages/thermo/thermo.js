//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    curtemp: 0,
    array: ['我', '小咪', '小明'],
    index: 0,
    bluetoothStr: "正在连接易温读卡器...",
    spinShow: true
  },

  onLoad: function(options) {
    var that = this;    
  },

  onShow: function (options){
    var that = this;

    this.setData({
      bluetoothStr: "正在连接易温读卡器...",
      spinShow: true
    });

    app.openBluetoothAdapter();
    app.bluetoothCallback = function (res) {
      if (res.errCode != 0) {
        that.setData({
          bluetoothStr: "连接易温读卡器失败:" + res.errMsg,
          spinShow: false
        });
      } else {
        that.setData({
          bluetoothStr: "连接易温读卡器成功",
          spinShow: false
        });
      }
    }
  }



})