//app.js
App({

  userInfoReadyCallback: null,
  bluetoothCallback: null,
  temperatureCallback: null,
  innerAudioContext: null,
  addMemberCallback: null,
  updateMemberCallback: null,
  deleteMemberCallback: null,
  isBluetoothStoped: true,
  uploadMinTemperature: 0.0,
  lat: 0,
  lng: 0,

  globalData: {
    userInfo: null,
    serverAddr: "https://telemed-healthcare.cn/easytemp/main",
    openid: null,
    logined: false,
    members: [],
    tagsbinding: [],
    mine:{id:0,name:'我'},

    // 蓝牙状态
    discoveryStarted: false,
    stopDiscover: false,
    timerid: null,
    devices: null,
    device: null
  },

  onLaunch: function() {
    // 保存变量
    var app_obj = this;

    // 展示本地存储能力
    // var logs = wx.getStorageSync('logs') || []
    // logs.unshift(Date.now())
    // wx.setStorageSync('logs', logs)

    //this.innerAudioContext = wx.createInnerAudioContext(); //新建一个createInnerAudioContext();
    //this.innerAudioContext.autoplay = true; //音频自动播放设置
    //this.innerAudioContext.src = null,
    //this.innerAudioContext.volume = 0.1;
    //this.innerAudioContext.onPlay(() => { }); //播放音效

    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
        app_obj.log("wx.login success!");
        app_obj.getOpenid(res.code);
      }
    })


    // 获取用户信息
    wx.getSetting({
      success: res => {
        if (res.authSetting['scope.userInfo']) {
          // 已经授权，可以直接调用 getUserInfo 获取头像昵称，不会弹框
          wx.getUserInfo({
            success: res => {
              // 可以将 res 发送给后台解码出 unionId
              this.globalData.userInfo = res.userInfo

              // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
              // 所以此处加入 callback 以防止这种情况
              if (this.userInfoReadyCallback) {
                this.userInfoReadyCallback(res)
              }
            }
          })
        }
      }
    })

    wx.getLocation({
      type: 'wgs84',
      success(res) {
        app_obj.log("get location", res);
        app_obj.lat = res.latitude;
        app_obj.lng = res.longitude;
      }
    })
  },

  // 获取open_id
  getOpenid: function(code) {
    this.log("getting openid");

    var that = this;
    wx.request({
      url: that.globalData.serverAddr + '?type=get_openid&code=' + encodeURIComponent(code),
      data: {},
      method: 'GET',
      success: function(res) {

        that.log("get openid success", res);
        that.globalData.openid = res.data.openid;
        that.globalData.logined = res.data.logined || false;
        that.globalData.members = res.data.members || [];
        that.globalData.tagsbinding = res.data.tagsbinding || [];
        var lastTemp = res.data.lasttemperature || [];

        var members = that.globalData.members;
        members.forEach(member => {
          var idx = that.inArray(lastTemp, 'memberid', member.id);
          if ( idx >= 0 ) {
            member.lasttemperature = lastTemp[idx].temperature;
            member.time = lastTemp[idx].time;
          }
        })

        // that.log("members: ", that.globalData.members);
        var idx = that.inArray(lastTemp, 'memberid', 0);
        if ( idx >= 0 ) {
          that.globalData.mine.lasttemperature = lastTemp[idx].temperature;
          that.globalData.mine.time = lastTemp[idx].time;
        }
        // that.log("mine: ", that.globalData.mine);


      }, // success

      // 获取openid失败
      fail() {
        that.log("get openid failed");
      },

    }); // wx.request
  },

  inArray: function(arr, key, val) {
    for (let i = 0; i < arr.length; i++) {
      if (arr[i][key] === val) {
        return i;
      }
    }
    return -1;
  },

  // ArrayBuffer转16进度字符串示例
  ab2hex: function(buffer) {
    var hexArr = Array.prototype.map.call(
      new Uint8Array(buffer),
      function(bit) {
        return ('00' + bit.toString(16)).slice(-2)
      }
    )
    return hexArr.join('');
  },

  // 字符串转byte
  stringToBytes: function(str) {
    var strArray = new Uint8Array(str.length);
    for (var i = 0; i < str.length; i++) {
      strArray[i] = str.charCodeAt(i);
    }
    const array = new Uint8Array(strArray.length)
    strArray.forEach((item, index) => array[index] = item)
    return array.buffer;
  },

  log: function(str, obj) {
    str = "== " + str;
    if (obj) {
      console.log(str, obj);
    } else {
      console.log(str)
    }
  },

  /*蓝牙操作*/
  openBluetoothAdapter() {
    var app = this;
    var that = this;

    if (!this.isBluetoothStoped) {
      this.log("isBluetoothStoped is false, openBluetoothAdapter return");
      return;
    }

    app.log("openBluetoothAdapter");
    wx.openBluetoothAdapter({
      success: (res) => {
        app.log('openBluetoothAdapter success', res);
        that.startBluetoothDevicesDiscovery()
      },
      fail: (res) => {
        app.log("openBluetoothAdapter failed", res);
        // 10001错误是手机没有开蓝牙开关
        if (res.errCode === 10001) {
          wx.onBluetoothAdapterStateChange(function(res) {
            app.log('onBluetoothAdapterStateChange', res)
            if (res.available && !res.discovering && !that.globalData.stopDiscover) {
              that.startBluetoothDevicesDiscovery()
            }
          })
        } else {
          if (that.bluetoothCallback) {
            if (!res.errCode)
              res.errCode = -1;
            that.bluetoothCallback(res);
          }
        }
      }
    })

  },

  startBluetoothDevicesDiscovery() {
    var app = this;
    var that = this;

    this.globalData.discoveryStarted = false
    this.globalData.stopDiscover = false;
    this.globalData.timerid = null
    this.globalData.devices = new Array();
    this.globalData.device = null

    if (this.bluetoothCallback) {
      var res = {}
      res.myCode = 1;
      that.bluetoothCallback(res);
    }

    this.globalData.discoveryStarted = true
    this.globalData.timerid = setTimeout(this.onBluetoothDevicesDiscoveryTimeout, 6000);
    this.isBluetoothStoped = false;

    wx.startBluetoothDevicesDiscovery({
      allowDuplicatesKey: true,
      success: (res) => {
        app.log('startBluetoothDevicesDiscovery success', res)
        that.onBluetoothDeviceFound()
      },
    })
  },

  onBluetoothDeviceFound() {
    var app = this;
    var that = this;

    wx.onBluetoothDeviceFound((res) => {
      res.devices.forEach(device => {

        if (!device.name && !device.localName) {
          return
        }

        if (device.name.substring(0, 7).toLowerCase() != 'telemed') {
          return;
        }

        var foundDevices = that.globalData.devices
        const idx = app.inArray(foundDevices, 'deviceId', device.deviceId)

        if (idx === -1) {
          foundDevices.push(device);
        } else {
          foundDevices[idx] = device
        }

      })
    })
  },

  onBluetoothDevicesDiscoveryTimeout() {
    var app = this;
    var that = this;

    app.log("onBluetoothDevicesDiscoveryTimeout");
    this.stopBluetoothDevicesDiscovery();

    this.log("found devices", this.globalData.devices);
    var device = this.getNearestDevice();
    this.globalData.device = device;

    if (device == null) {
      this.log("not found telemed device!");
      if (this.bluetoothCallback) {
        var res = {}
        res.errCode = -1;
        res.errMsg = "没有找到易温读卡器"
        that.bluetoothCallback(res);
      }

      setTimeout(that.startBluetoothDevicesDiscovery, 5000);

    } else {
      this.createBLEConnection();
    }
  },

  stopBluetoothDevicesDiscovery() {
    var app = this;
    var that = this;

    if (!this.globalData.discoveryStarted)
      return;

    app.log("stopBluetoothDevicesDiscovery");
    wx.stopBluetoothDevicesDiscovery();
    clearTimeout(this.globalData.timerid);
    this.globalData.discoveryStarted = false;
    // 手动停止标志
    this.globalData.stopDiscover = true;
  },

  getNearestDevice() {
    var devices = this.globalData.devices;
    var foundDevice = null;
    devices.forEach(device => {
      if (device) {
        foundDevice = device;
      } else if (device.RSSI > foundDevice.RSSI) {
        foundDevice = device;
      }
    })
    return foundDevice;
  },

  createBLEConnection(e) {
    var app = this;
    var that = this;

    app.log("createBLEConnection");
    const deviceId = this.globalData.device.deviceId
    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        app.log("createBLEConnection success");
        that.getBLEDeviceServices(deviceId)
      },
      fail: (err) => {
        if (this.bluetoothCallback) {
          that.bluetoothCallback(err);
          setTimeout(that.startBluetoothDevicesDiscovery, 10000);
        }
      }
    })
    that.stopBluetoothDevicesDiscovery()
  },

  closeBLEConnection() {
    wx.closeBLEConnection({
      deviceId: this.globalData.device.deviceId
    })
  },

  getBLEDeviceServices(deviceId) {
    var that = this;
    var app = this;

    wx.getBLEDeviceServices({
      deviceId,
      success: (res) => {
        for (let i = 0; i < res.services.length; i++) {
          /*
          if (res.services[i].isPrimary) {
            app.log("== primary service" + res.services[i].uuid);
            this.getBLEDeviceCharacteristics(deviceId, res.services[i].uuid)
            return
          }
          */

          /*易温的服务*/
          if (res.services[i].uuid.substring(0, 8).toLowerCase() == 'f000fff0') {
            that.getBLEDeviceCharacteristics(deviceId, res.services[i].uuid)
          }

        }
      },
      failed: (res) => {
        if (that.bluetoothCallback) {
          that.bluetoothCallback(res);
        }
        setTimeout(that.startBluetoothDevicesDiscovery, 10000);
      }
    })
  },

  getBLEDeviceCharacteristics(deviceId, serviceId) {
    var that = this;
    var app = this;

    wx.getBLEDeviceCharacteristics({
      deviceId,
      serviceId,
      success: (res) => {
        app.log('getBLEDeviceCharacteristics success', res)
        for (let i = 0; i < res.characteristics.length; i++) {
          let item = res.characteristics[i]

          if (item.properties.read) {
            /*
            wx.readBLECharacteristicValue({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
            })
            */
          }

          if (item.properties.write) {
            // that.writeBLECharacteristicValue(deviceId, serviceId, item.uuid, app.stringToBytes("56"))
          }

          if (item.properties.notify || item.properties.indicate) {
            wx.notifyBLECharacteristicValueChange({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
              state: true,
              success: function(res) {
                that.onBLECharacteristicValueChange();
              },
              fail: function(res) {
                if (that.bluetoothCallback) {
                  that.bluetoothCallback(res);
                }
              }
            })

          }
        }

        if (that.bluetoothCallback) {
          that.bluetoothCallback(res);
        }
      },
      fail(res) {
        app.error('getBLEDeviceCharacteristics', res)
        if (that.bluetoothCallback) {
          that.bluetoothCallback(res);
        }
        setTimeout(that.startBluetoothDevicesDiscovery, 10000);
      }
    })
  },

  writeBLECharacteristicValue: function(deviceId, serviceId, characterId, order) {
    var app = this;
    var that = this;

    wx.writeBLECharacteristicValue({
      deviceId: deviceId,
      serviceId: serviceId,
      characteristicId: characterId,
      // 这里的value是ArrayBuffer类型
      value: order.slice(0, 20),
      success: function(res) {
        app.log("write characterid success");
      },

      fail: function(res) {
        app.log("write characterid failed");
      }
    })
  },

  onBLECharacteristicValueChange: function() {
    var app = this;
    var that = this;

    wx.onBLECharacteristicValueChange(function(res) {
      app.log("onBLECharacteristicValueChange", res);
      var resValue = app.ab2hex(res.value);
      app.log("received: ", resValue);

      var data = new Uint8Array(res.value);
      var item = {};
      item.tagid = resValue.substr(2, 16);
      item.temperature = data[9] + data[10] / 100;
      item.battery = data[13] * 100 + data[14] * 10 + data[15];
      app.log("formated item: ", item);

      if (that.temperatureCallback) {
        that.temperatureCallback(item);
      }

    })
  },

  onHide: function() {
    // Do something when hide.
    wx.closeBluetoothAdapter();
    this.isBluetoothStoped = true;
  },

  onShow: function() {

  },

  addMember: function(newMemberName) {
    var that = this;
    var app = this;
    var ret = {
      errCode: -1,
      errMsg: "添加成员失败!"
    };

    if (!this.globalData.logined) {
      this.log("not logined, give up to add member!");
      if (this.addMemberCallback) {
        this.addMemberCallback(ret);
      }
      return;
    }

    if (newMemberName == '') {
      if (this.addMemberCallback) {
        this.addMemberCallback(ret);
      }
      return;
    }

    var members = this.globalData.members;
    var found = false;
    members.forEach(member => {
      if (!found && member.name == newMemberName) {
        found = true;
      }
    })

    if (found) {
      this.log("duplicated member name, give up to add member!");
      if (this.addMemberCallback) {
        this.addMemberCallback(ret);
      }
      return;
    }

    wx.request({
      url: app.globalData.serverAddr + '?type=addmember&openid=' + encodeURIComponent(this.globalData.openid) + '&membername=' + encodeURIComponent(newMemberName),
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          that.log("addmember success", res);
          that.globalData.members.push({
            id: res.data.id,
            name: newMemberName
          })
          ret.errCode = 0;
        } else {
          that.log("addmember failed", res);
        }
      },
      fail: (res) => {
        that.log("addmember failed to wx.request", res);
      },
      complete: () => {
        if (that.addMemberCallback) {
          that.addMemberCallback(ret);
        }
      }
    })
  },

  updateMember: function(member, newName) {
    var that = this;
    var app = this;
    var ret = {
      errCode: -1,
      errMsg: "更新成员失败!"
    };

    // this.log("update to " + newName + ": ", member);

    if (!this.globalData.logined) {
      this.log("not logined, give up to update member!");
      if (this.updateMemberCallback) {
        this.updateMemberCallback(ret);
      }
      return;
    }

    if (newName == '') {
      if (this.addMemberCallback) {
        this.addMemberCallback(ret);
      }
      return;
    }

    // 如果名字没有变动
    if (member.name == newName) {
      if (this.updateMemberCallback) {
        this.updateMemberCallback(ret);
      }
      return;
    }

    var members = this.globalData.members;
    var found = false;
    members.forEach(item => {
      if (!found && item.id != member.id && item.name == newName) {
        found = true;
      }
    })

    // 如果发现重名
    if (found) {
      this.log("duplicated member name, give up to add member!");
      if (this.updateMemberCallback) {
        this.updateMemberCallback(ret);
      }
      return;
    }

    wx.request({
      url: app.globalData.serverAddr + '?type=updatemember&openid=' + encodeURIComponent(this.globalData.openid) + '&membername=' + encodeURIComponent(newName) + '&memberid=' + member.id,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          that.log("upate member success", res);
          members.forEach(item => {
            if (item.id == member.id) {
              item.name = newName;
            }
          })
          ret.errCode = 0;
        } else {
          that.log("update member failed", res);
        }
      },
      fail: (res) => {
        that.log("update member failed to wx.request", res);
      },
      complete: () => {
        if (that.updateMemberCallback) {
          that.updateMemberCallback(ret);
        }
      }
    })
  },

  delMember: function(memberId) {
    var that = this;
    var app = this;
    var ret = {
      errCode: -1,
      errMsg: "删除成员失败!"
    };

    if (!this.globalData.logined) {
      this.log("not logined, give up to delete member!");
      if (this.deleteMemberCallback) {
        this.deleteMemberCallback(ret);
      }
      return;
    }

    // this.log("delMember: " + memberId);

    wx.request({
      url: app.globalData.serverAddr + '?type=delmember&openid=' + encodeURIComponent(this.globalData.openid) + '&memberid=' + memberId,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          that.log("delete member success", res);
          var members = that.globalData.members;
          var index = app.inArray(members, 'id', memberId)
          members.splice(index, 1);
          ret.errCode = 0;
        } else {
          that.log("delete member failed", res);
        }
      },
      fail: (res) => {
        that.log("delete member failed to wx.request", res);
      },
      complete: () => {
        if (that.deleteMemberCallback) {
          that.deleteMemberCallback(ret);
        }
      }
    })
  },

  uploadTemperature: function(item) {
    var that = this;
    var app = this;

    if (!this.globalData.logined) {
      this.log("not logined, give up to upload temperature!");
      return;
    }

    if (item.temperature <= this.uploadMinTemperature) {
      this.log("uploadTemperature temperature " + item.temperature + " < min temperature " + this.uploadMinTemperature + ", return");
      return;
    }

    var url = app.globalData.serverAddr + '?type=uploadtemperature&openid=' + encodeURIComponent(this.globalData.openid) + '&tagid=' + item.tagid + "&temperature=" + item.temperature.toFixed(1) + "&lat=" + this.lat + "&lng=" + this.lng;
    // this.log("uplodate temperature url is: " + url );

    wx.request({
      url: url,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          that.log("upload temperature success", res);
          // var date = new Date(res.data.time);
          // that.log("time is: ", that.formatTime(date));
        } else {
          that.log("upload temperature failed", res);
        }
      },
      fail: (res) => {
        that.log("upload temperature failed to wx.request", res);
      },
      complete: () => {

      }
    })
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

  binding:function(tagid, member) {
    var that = this;
    var app = this;

    if (!this.globalData.logined) {
      this.log("not logined, give up to bind tag!");
      return;
    }

    this.log("binding tag " + tagid + " to " + member.name + "[" + member.id + "]");

    var url = app.globalData.serverAddr + '?type=binding&openid=' + encodeURIComponent(this.globalData.openid) + '&tagid=' + tagid + "&memberid=" + member.id;
    // this.log("binding url is: " + url );

    wx.request({
      url: url,
      method: 'GET',
      success: (res) => {
        if (res.data.error != null && res.data.error == 0) {
          that.log("bind tag to member success", res);
        } else {
          that.log("bind tag to member failed", res);
        }
      },
      fail: (res) => {
        that.log("bind tag to member failed to wx.request", res);
      },
      complete: () => {

      }
    })

  },

})