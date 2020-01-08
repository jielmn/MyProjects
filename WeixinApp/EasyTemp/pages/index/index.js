const app = getApp()

Page({
  data: {
    devices: [],
    searchstr: "搜索设备",
    spinShow:false,
    connected: false,
    chs: [],
  },

  openBluetoothAdapter() {
    var that = this;

    if (!this._discoveryStarted) {
      console.log("== openBluetoothAdapter");
      wx.openBluetoothAdapter({
        success: (res) => {
          console.log('== openBluetoothAdapter success', res);
          that.startBluetoothDevicesDiscovery()
        },
        fail: (res) => {
          console.log("== openBluetoothAdapter failed", res);
          // 10001错误是手机没有开蓝牙开关
          if (res.errCode === 10001) {
            wx.onBluetoothAdapterStateChange(function(res) {
              console.log('== onBluetoothAdapterStateChange', res)
              if (res.available && !res.discovering && !that._manulstop ) {
                that.startBluetoothDevicesDiscovery()
              }
            })
          }
        }
      })
    } else {
      console.log("== closeBluetoothAdapter");
      this.stopBluetoothDevicesDiscovery();
    }

  },

  onBluetoothDevicesDiscoveryTimeout() {
    console.log("== onBluetoothDevicesDiscoveryTimeout");
    this.stopBluetoothDevicesDiscovery();
  },

  startBluetoothDevicesDiscovery() {
    if (this._discoveryStarted) {
      return
    }

    this._discoveryStarted = true
    this._timerid = setTimeout(this.onBluetoothDevicesDiscoveryTimeout, 10000);
    this.setData({
      searchstr: "停止搜索",
      spinShow:true
    });

    wx.startBluetoothDevicesDiscovery({
      allowDuplicatesKey: true,
      success: (res) => {
        console.log('== startBluetoothDevicesDiscovery success', res)
        this.onBluetoothDeviceFound()
      },
    })
  },

  stopBluetoothDevicesDiscovery() {
    console.log("== stopBluetoothDevicesDiscovery");
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
        const foundDevices = this.data.devices
        const idx = app.inArray(foundDevices, 'deviceId', device.deviceId)
        const data = {}
        if (idx === -1) {
          data[`devices[${foundDevices.length}]`] = device
        } else {
          data[`devices[${idx}]`] = device
        }
        console.log(data);
        this.setData(data);
        console.log(this.data.devices);
      })
    })
  },

  createBLEConnection(e) {
    const ds = e.currentTarget.dataset
    const deviceId = ds.deviceId
    const name = ds.name
    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        this.setData({
          connected: true,
          name,
          deviceId,
        })
        this.getBLEDeviceServices(deviceId)
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
          if (res.services[i].isPrimary) {
            this.getBLEDeviceCharacteristics(deviceId, res.services[i].uuid)
            return
          }
        }
      }
    })
  },
  getBLEDeviceCharacteristics(deviceId, serviceId) {
    wx.getBLEDeviceCharacteristics({
      deviceId,
      serviceId,
      success: (res) => {
        console.log('getBLEDeviceCharacteristics success', res.characteristics)
        for (let i = 0; i < res.characteristics.length; i++) {
          let item = res.characteristics[i]
          if (item.properties.read) {
            wx.readBLECharacteristicValue({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
            })
          }
          if (item.properties.write) {
            this.setData({
              canWrite: true
            })
            this._deviceId = deviceId
            this._serviceId = serviceId
            this._characteristicId = item.uuid
            this.writeBLECharacteristicValue()
          }
          if (item.properties.notify || item.properties.indicate) {
            wx.notifyBLECharacteristicValueChange({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
              state: true,
            })
          }
        }
      },
      fail(res) {
        console.error('getBLEDeviceCharacteristics', res)
      }
    })
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
  },
  writeBLECharacteristicValue() {
    // 向蓝牙设备发送一个0x00的16进制数据
    let buffer = new ArrayBuffer(1)
    let dataView = new DataView(buffer)
    dataView.setUint8(0, Math.random() * 255 | 0)
    wx.writeBLECharacteristicValue({
      deviceId: this._deviceId,
      serviceId: this._deviceId,
      characteristicId: this._characteristicId,
      value: buffer,
    })
  },
  closeBluetoothAdapter() {
    wx.closeBluetoothAdapter()
    this._discoveryStarted = false
  },
})