//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    
  },

  onLoad:function() {

  },
  
  formSubmit:function(e) {    
    if (!app.globalData.openid){
      wx.showModal({
        title: '错误',
        content: '没有获取到open id',
        showCancel: false,
      })
      return;
    }

    var value = e.detail.value;
    if (value.item == '' || value.item == null) {

      wx.showModal({
        title: '错误',
        content: '清单内容不能为空',
        showCancel:false,
      })

      return;
    }


    // 保存清单
    wx.request({
      url: 'https://telemed-healthcare.cn/todolist/main?type=additem&open_id=' + encodeURIComponent(app.globalData.openid) + '&item=' + encodeURIComponent(value.item),
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          console.log("additem success")

          wx.showToast({
            title: '保存成功'
          })

          // 返回
          setTimeout(function () {
            app.globalData.additem = true
            app.globalData.content = value.item
            app.globalData.itemid = res.data.id;

            wx.navigateBack();
          }, 800);

        } else {
          console.log("additem:")
          console.log(res);
        }
      },
      fail() {
        console.log("failed to additem")
      },
      complete() {
        
      },
    })

    
        
  }

})
