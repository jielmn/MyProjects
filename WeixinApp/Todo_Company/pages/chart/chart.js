//index.js
//获取应用实例
const app = getApp()

Page({

  data: {
    login: false,
  },

  // 加载
  onLoad: function (options) {
    var that = this;
  },

  // 下拉更新
  onPullDownRefresh: function () {
    // 重置数据

    this.onLoad()
  },
  

}) //  END OF PAGE