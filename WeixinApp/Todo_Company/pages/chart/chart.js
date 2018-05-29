//index.js
//获取应用实例
const app = getApp()

Page({

  data: {
    login: false,
    navbar: ['半月维保', '季度维保', '半年维保', "年度维保"],
    currentTab: 4,
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

  //响应点击导航栏
  navbarTap: function (e) {
    var that = this;
    //console.log(e.currentTarget.dataset.idx)
    that.setData({
      currentTab: e.currentTarget.dataset.idx,
    })
  },
  

}) //  END OF PAGE