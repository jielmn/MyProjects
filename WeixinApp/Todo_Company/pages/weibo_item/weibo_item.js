//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    images: [],
    upload_files_url:[],
    content: null,
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

  OnUploadImageRet : function ( ret )  {
    wx.hideToast();

     // 上传失败
     if ( 0 != ret ) {       
       wx.showModal({
         title: '错误提示',
         content: '上传图片失败',
         showCancel: false,
       })
       return;
     }

     // 上传成功
     this.saveHotPoint();
  },

  // 上传图片
  uploadImage: function (index) {
    var that = this;

    // 没有要上传的下一张图片
    if ( this.data.images.length == index ) {
      that.OnUploadImageRet(0);
      return;
    }

    wx.uploadFile({
      url: app.globalData.serverAddr,
      filePath: this.data.images[index],
      name: 'upload_image',
      header: {
        "Content-Type": "multipart/form-data"
      },
      success: function (res) {
        var result = JSON.parse(res.data);
        // 成功
        if ( result.error == 0 ) {        
          that.data.upload_files_url.push(result.upload_files[0]);
          that.uploadImage( index + 1 );
        } else {
          that.OnUploadImageRet(-1);
        }
      },
      fail: function (res) {
        that.OnUploadImageRet(-1);
      }
    });// end of wx.upload
  }, // end of uploadImage funcion
  
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

    this.data.content = value.item;
    this.data.upload_files_url = [];

    // 如果有图片要上传
    if ( this.data.images.length > 0 ) {

      wx.showToast({
        title: '正在上传图片...',
        icon: 'loading',
        mask: true,
      });  

      // 开始上传第一张图片
      this.uploadImage(0);

    // 保存热点内容  
    } else {
      this.saveHotPoint();
    }
    
  },

  // 保存热点
  saveHotPoint : function () {
    var imageUrlParam = '';
    for ( var i = 0; i < this.data.upload_files_url.length; i++ ) {
      imageUrlParam += '&img' + i + '=' + encodeURIComponent(this.data.upload_files_url[i].filename);
    }
    
    wx.request({
      url: app.globalData.serverAddr + '?type=addhotpoint&open_id=' + encodeURIComponent(app.globalData.openid) + '&item=' + encodeURIComponent(this.data.content) + imageUrlParam,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("addhotpoint success")

          wx.showToast({
            title: '保存成功'
          })

          // 返回
          setTimeout(function () {
            app.globalData.addweibo = true;

            wx.navigateBack();
          }, 800);

        } else {
          console.log("addhotpoint:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to addhotpoint")
      },
      complete() {

      },
    })

  },


  

}) 
