//index.js
//获取应用实例
const app = getApp()

Page({

  data: {
    login: false,
    weibo_items: [],
  },

  // 加载
  onLoad: function (options) {
    var that = this;

    // 是否在服务器登录 
    this.setData({ login: app.globalData.login })

    app.loginReadyCallback = function () {
      console.log("login callback..., in front")
      that.setData({ login: true })
      // 获取微博数据
      that.getWeibo();
    }

    //获取微博数据
    this.getWeibo();
  },

  // 下拉更新
  onPullDownRefresh: function () {
    // 重置数据
    this.data.weibo_items = []

    this.onLoad()
  },

  toAddWeibo:function() {
    wx.navigateTo({ url: '../weibo_item/weibo_item' })
  },

  onReachBottom() {
    console.log('--front.onReachBottom')
    this.getWeibo();
  },

  getWeibo : function() {
    var that = this;

    // 没有登录
    if (!this.data.login) {
      return
    }

    wx.showLoading({
      title: '获取前线热点',
    })

    var start_index = 0;
    start_index = this.data.weibo_items.length;

    wx.request({
      url: app.globalData.serverAddr + '?type=weibo&start_index=' + start_index + '&open_id=' + encodeURIComponent(app.globalData.openid),
      method: 'GET',
      success: (res) => {

        //console.log('===============')
        //console.log(res);
        if (res.data.error != null && res.data.error == 0) {
          console.log("get weibo success!");
          var weibo_items = that.data.weibo_items.concat(res.data.weibo_items);
          for (var i in weibo_items) {
            weibo_items[i].readers_text = weibo_items[i].readers.join('，');
            var date = new Date(parseInt(weibo_items[i].pubtime));
            weibo_items[i].pubtime_text = this.formatTime(date);
          }
          that.setData({ weibo_items: weibo_items });
          //console.log(that.data.weibo_items);
        } else {
          console.log("get weibo result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to get weibo")
        console.log(res);
      },
      complete() {
        wx.hideLoading();
      },
    })

  },

  OnPreviewImg:function(e) {
    var that = this;
    // console.log(e);

    var index = e.target.dataset.index;
    var subindex = e.target.dataset.subindex;

    var cur_img = 'https://telemed-healthcare.cn/todolist_company/' + that.data.weibo_items[index].images[subindex];
    var images  = [];

    for ( var i = 0; i < that.data.weibo_items[index].images.length; i++ ) {
      images.push('https://telemed-healthcare.cn/todolist_company/' + that.data.weibo_items[index].images[i]);
    }
    wx.previewImage({
      current: cur_img, // 当前显示图片的http链接
      urls: images      // 需要预览的图片http链接列表
    })
    
    
  },

  onShow: function () {
    // 如果从additem页面返回
    if (app.globalData.addweibo) {
      this.onPullDownRefresh();

      app.globalData.weibo = false
    }
  },

  OnIgnore: function () {

  },

  OnDelete:function(e) {
    //console.log("delete weibo....")
    //console.log(e.currentTarget.dataset.id)
    var that = this;
    var id = e.currentTarget.dataset.id
    
    wx.request({
      url: app.globalData.serverAddr + '?type=delete_weibo&id=' + id,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("delete success!")
          wx.showToast({
            title: '删除成功'
          })

          that.deleteFromArray(that.data.weibo_items, 'id', id)
          that.setData({ weibo_items: that.data.weibo_items })
        } else {
          console.log("delete result:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to delete")
        console.log(res);
      },
      complete() {

      },
    })
  },

  deleteFromArray: function (arr, key, val) {
    for (var i = 0; i < arr.length; i++) {
      if (arr[i][key] == val) {
        arr.splice(i, 1);
        break;
      }
    }
  },

  formatTime: function (date) {
    const year = date.getFullYear()
    const month = date.getMonth() + 1
    const day = date.getDate()
    const hour = date.getHours()
    const minute = date.getMinutes()
    const second = date.getSeconds()

    return [year, month, day].map(this.formatNumber).join('-') + ' ' + [hour, minute, second].map(this.formatNumber).join(':')
  },

  formatNumber: function (n) {
    n = n.toString()
    return n[1] ? n : '0' + n
  },

}) //  END OF PAGE