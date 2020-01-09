const app = getApp()

Page({
  data: {
    devices: [],
    searchstr: "搜索设备",
    spinShow: false,
    connected: false,
    chs: [],
  },

  openBluetoothAdapter() {
    var that = this;

    if (!this._discoveryStarted) {
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
              if (res.available && !res.discovering && !that._manulstop) {
                that.startBluetoothDevicesDiscovery()
              }
            })
          }
        }
      })
    } else {
      app.log("manul stop BluetoothDevicesDiscovery");
      this.stopBluetoothDevicesDiscovery();
    }

  },

  onBluetoothDevicesDiscoveryTimeout() {
    app.log("onBluetoothDevicesDiscoveryTimeout");
    this.stopBluetoothDevicesDiscovery();
  },

  startBluetoothDevicesDiscovery() {
    if (this._discoveryStarted) {
      return
    }

    this._discoveryStarted = true
    this._timerid = setTimeout(this.onBluetoothDevicesDiscoveryTimeout, 6000);
    this.setData({
      searchstr: "停止搜索",
      spinShow: true
    });

    wx.startBluetoothDevicesDiscovery({
      allowDuplicatesKey: true,
      success: (res) => {
        app.log('startBluetoothDevicesDiscovery success', res)
        this.onBluetoothDeviceFound()
      },
    })
  },

  stopBluetoothDevicesDiscovery() {
    app.log("stopBluetoothDevicesDiscovery");
    wx.stopBluetoothDevicesDiscovery();
    clearTimeout(this._timerid);
    this._discoveryStarted = false;
    // 手动停止标志
    this._manulstop = true;
    this.setData({
      searchstr: "搜索设备",
      spinShow: false
    });
  },

  onBluetoothDeviceFound() {
    wx.onBluetoothDeviceFound((res) => {
      res.devices.forEach(device => {
        if (!device.name && !device.localName) {
          return
        }

        if (device.name.substring(0, 7) != 'TeleMed') {
          return;
        }

        var rssi = device.RSSI + 100;
        if (rssi < 20) {
          device.image = 1;
        } else if (rssi < 40) {
          device.image = 2;
        } else if (rssi < 60) {
          device.image = 3;
        } else if (rssi < 80) {
          device.image = 4;
        } else {
          device.image = 5;
        }

        device.spinShow = false;

        const foundDevices = this.data.devices
        const idx = app.inArray(foundDevices, 'deviceId', device.deviceId)
        const data = {}
        if (idx === -1) {
          data[`devices[${foundDevices.length}]`] = device
        } else {
          data[`devices[${idx}]`] = device
        }
        this.setData(data);
        // app.log(this.data.devices);
      })
    })
  },

  createBLEConnection(e) {
    app.log("createBLEConnection");
    var that = this;

    const ds = e.currentTarget.dataset
    const deviceId = ds.deviceId
    const name = ds.name

    const foundDevices = this.data.devices
    const idx = app.inArray(foundDevices, 'deviceId', deviceId);
    foundDevices[idx].spinShow = true;

    this.setData({devices:foundDevices});

    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        app.log("createBLEConnection success");
        this.setData({
          connected: true,
          name,
          deviceId,
        });
        // app.log(this.data);
        this.getBLEDeviceServices(deviceId)
      },
      fail: (err) => {
        wx.showToast({
          title: '连接失败，错误码:' + err.errCode,
          duration: 2000
        })
      }
    })
    this.stopBluetoothDevicesDiscovery()
  },

  closeBLEConnection() {
    wx.closeBLEConnection({
      deviceId: this.data.deviceId
    })
    this.setData({
      connected: false,
      chs: [],
      canWrite: false,
    })
  },
  
  getBLEDeviceServices(deviceId) {
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
          if (res.services[i].uuid.substring(0, 8) == 'F000FFF0') {
            this.getBLEDeviceCharacteristics(deviceId, res.services[i].uuid)
          }

        }
      }
    })
  },

  getBLEDeviceCharacteristics(deviceId, serviceId) {
    var that = this;

    wx.getBLEDeviceCharacteristics({
      deviceId,
      serviceId,
      success: (res) => {
        app.log('getBLEDeviceCharacteristics success', res.characteristics)
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
            this.setData({
              writeCharacteristicId: item.uuid,
              canWrite: true
            })
            
            this._deviceId = deviceId
            this._serviceId = serviceId
            this._characteristicId = item.uuid
            this.writeBLECharacteristicValue( app.stringToBytes("56") )
            
          }
          if (item.properties.notify || item.properties.indicate) {

            this.setData({
              notifyCharacteristicId: item.uuid
            });

            wx.notifyBLECharacteristicValueChange({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
              state: true,
              success:function(res) {
                that.onBLECharacteristicValueChange();
              },
              fail:function(res) {
                wx.showToast({
                  title: '监听特征值失败',
                  duration: 2000
                })
              }
            })

          }
        }
      },
      fail(res) {
        app.error('getBLEDeviceCharacteristics', res)
      }
    })

    /*
    // 操作之前先监听，保证第一时间获取数据
    wx.onBLECharacteristicValueChange((characteristic) => {
      const idx = app.inArray(this.data.chs, 'uuid', characteristic.characteristicId)
      const data = {}
      if (idx === -1) {
        data[`chs[${this.data.chs.length}]`] = {
          uuid: characteristic.characteristicId,
          value: app.ab2hex(characteristic.value)
        }
      } else {
        data[`chs[${idx}]`] = {
          uuid: characteristic.characteristicId,
          value: app.ab2hex(characteristic.value)
        }
      }
      // data[`chs[${this.data.chs.length}]`] = {
      //   uuid: characteristic.characteristicId,
      //   value: app.ab2hex(characteristic.value)
      // }
      this.setData(data)
    })
    */
  },

  writeBLECharacteristicValue(order) {
    // 向蓝牙设备发送一个0x00的16进制数据
    let buffer = new ArrayBuffer(1)
    let dataView = new DataView(buffer)
    dataView.setUint8(0, Math.random() * 255 | 0)
    wx.writeBLECharacteristicValue({
      deviceId: this._deviceId,
      serviceId: this._serviceId,
      characteristicId: this._characteristicId,
      value: order.slice(0, 20),
      success: function (res) {
        app.log("writeBLECharacteristicValue success")
      },
      fail : function(res) {
        app.log("writeBLECharacteristicValue fail")
      }
    })
  },

  closeBluetoothAdapter() {
    wx.closeBluetoothAdapter()
    this._discoveryStarted = false
  },

  onBLECharacteristicValueChange:function() {
    app.log("onBLECharacteristicValueChange");
    wx.onBLECharacteristicValueChange(function (res) {
      app.log("onBLECharacteristicValueChange", res);
    })
  }

})