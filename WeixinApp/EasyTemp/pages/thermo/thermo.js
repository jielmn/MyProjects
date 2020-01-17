//index.js
//获取应用实例
const app = getApp()

Page({
  data: {
    curtemp: 0,
    curtempColor: '#a5eddf',
    members: [],
    index: 0,
    curtagId: null,
    bluetoothStr: "正在连接易温读卡器...",
    spinShow: true
  },

  timerId: null,  

  onLoad: function(options) {
    var that = this;
    app.temperatureCallback = function(item) {

      // app.innerAudioContext.src = '/sound/bongo.mp3'; //链接到音频的地址
      // app.innerAudioContext.play();

      that.setData({
        curtemp: item.temperature,
        curtempColor: 'white',
        curtagId:item.tagid
      });

      clearTimeout(that.timerId);
      that.timerId = setTimeout(function() {
        that.setData({
          curtempColor: '#a5eddf'
        });
      }, 5000);

      app.uploadTemperature(item);
    }

    app.bluetoothCallback = function(res) {
      if (res.errCode && res.errCode != 0) {
        that.setData({
          bluetoothStr: "连接易温读卡器失败:" + res.errMsg,
          spinShow: false
        });
      } else {
        if (res.myCode && res.myCode != 0) {
          that.setData({
            bluetoothStr: "正在连接易温读卡器...",
            spinShow: true
          });
        } else {
          that.setData({
            bluetoothStr: "连接易温读卡器成功",
            spinShow: false
          });
        }
      }
    }

  },

  onShow: function(options) {
    var that = this;

    if (app.isBluetoothStoped) {

      this.setData({
        bluetoothStr: "没有连接易温读卡器",
        spinShow: false
      });

      app.openBluetoothAdapter();
    }

    var members = [{
      id: 0,
      name: '我'
    }];
    app.globalData.members.forEach(item => {
      var member = {};
      member.id = item.id;
      member.name = item.name;
      members.push(member);
    })
    this.setData({
      members: members
    })

  },

  bindPickerChange: function(e) {
    // app.log("bindPickerChange",e);
    var index = parseInt(e.detail.value);

    this.setData({
      index: index
    })

    app.binding(this.data.curtagId,this.data.members[index]);


  }




})