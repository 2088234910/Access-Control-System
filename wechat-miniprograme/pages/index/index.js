// index.js

var mqtt = require('/../../utils/mqtt.min')
var client = null

Page({
  data: {
  },
  onLoad() {
    this.connectMqtt()
  },
  connectMqtt:function() {
    const options = {
      connectTimeout:4000,
      clientId:'d63968a5884ec2f072c0f3ec72fc1c11',
      port:8084,
      username:'d63968a5884ec2f072c0f3ec72fc1c11',
      password:'aassdd'
    }
    client = mqtt.connect('wxs://t.yoyolife.fun:8084/mqtt',options)
    client.on('connect', (connAck) => {
      console.log('服务器连接成功', connAck);
      client.subscribe('/iot/6872/device', { qos: 0 }, (err) => {
        if (!err) {
          console.log('订阅成功');
        } else {
          console.error('订阅失败', err);
        }
      });
    });
    client.on('message',function(topic,message){
      console.log('收到'+message.toString())
      // 如果需要更新页面，使用this.setData来更新data对象
      // this.setData({ someKey: message.toString() });
    })
  },
  taphere() {
    console.log('helloworld')
  },
})
