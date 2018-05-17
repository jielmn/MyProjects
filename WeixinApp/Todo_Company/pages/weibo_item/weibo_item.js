//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    images: [],
  },

  onLoad:function() {

  },

  OnAddImg:function() {
    var that = this;
    var cnt = 0;
    if ( this.data.images.length < 9 ) {
      cnt = 9 - this.data.images.length;
    }
    if ( cnt <= 0 ) {
      wx.showModal({
        title: '提示',
        content: '不能选择更多的图片',
        showCancel: false,
      })
      return;
    }

    wx.chooseImage({
      count: cnt,  //最多可以选择的图片总数  
      sizeType: ['original', 'compressed'], // 可以指定是原图还是压缩图，默认二者都有  
      sourceType: ['album', 'camera'], // 可以指定来源是相册还是相机，默认二者都有  
      success: function (res) {
        // 返回选定照片的本地文件路径列表，tempFilePath可以作为img标签的src属性显示图片  
        that.setData({ images: that.data.images.concat(res.tempFilePaths) })
      }
    });
  },

  OnPreviewImg:function(e) {
    //console.log('OnPreview');
    //console.log(e.target.dataset.index);
    var that = this;
    //console.log(that.data.images);

    wx.previewImage({
      current: that.data.images[e.target.dataset.index], // 当前显示图片的http链接
      urls: that.data.images                             // 需要预览的图片http链接列表
    })
  },

  OnDelImg:function(e) {
    this.data.images.splice(e.target.dataset.index, 1) 
    this.setData({ images: this.data.images })
  },
  
  //添加Banner  
  formSubmit: function (e) {
    var that = this;
    var value = e.detail.value;
    if (value.item == '' || value.item == null) {
      wx.showModal({
        title: '提示',
        content: '前线热点内容不能为空',
        showCancel: false,
      })
      return; 
    }

    if ( that.data.images.length > 0 ) {
      wx.showToast({
        title: '正在上传...',
        icon: 'loading',
        mask: true,
        duration: 10000
      });  
    }

    var upload_ret = 0;
    for (var i = 0; i < that.data.images.length && upload_ret == 0; i++) {
      wx.uploadFile({
        url: app.globalData.serverAddr,
        filePath: that.data.images[i],
        name: 'uploadfile_ant',
        formData: {
          'imgIndex': i
        },
        header: {
          "Content-Type": "multipart/form-data"
        },
        success: function (res) {
          var result = JSON.parse(res.data);
          // 成功
          if ( result.error != 0 ) {
            upload_ret = -1;
          }
        },
        fail: function (res) {
          upload_ret = -1;          
        }

      });// end of wx.upload

    }// end of for

    wx.hideToast();
    if ( upload_ret != 0 ) {
      wx.showModal({
        title: '错误提示',
        content: '上传图片失败',
        showCancel: false,
      })
    }
    
  }  

  

}) 
