//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    id:null,
    content:null,
    start_time:null,
    start_time_txt:null,
    is_complete:false,
  },

  onLoad:function(e) {
    this.setData({id:e.id,content:e.content,start_time:e.start_time})
    var date = new Date(parseInt(e.start_time))
    this.setData({ start_time_txt: this.formatTime(date) })
    if ( e.is_complete ) {
      this.setData({ is_complete: true })
    }
  },
  
  formatTime:function(date) {
    const year = date.getFullYear()
    const month = date.getMonth() + 1
    const day = date.getDate()
    const hour = date.getHours()
    const minute = date.getMinutes()
    const second = date.getSeconds()

    return [year, month, day].map(this.formatNumber).join('/') + ' ' + [hour, minute, second].map(this.formatNumber).join(':')
  },

  formatNumber : function( n ) {
    n = n.toString()
    return n[1] ? n : '0' + n
  },

  formSubmit:function() {
    wx.navigateBack()
  },

})