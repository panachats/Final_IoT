from flask import Flask, render_template
import requests
from paho.mqtt import client as mqtt_client


broker = "172.20.10.3"
port = 1883


def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client("server")
    client.on_connect = on_connect
    client.connect(broker, port)
    return client




app = Flask(__name__)
server_ip = "http://172.20.10.3:80"
@app.route("/")
def home():
    return render_template("index.html")


@app.route("/nodemcu/last")
def nodemcu():
    headers = {'Accept': 'application/json'}
    r = requests.get(server_ip + '/ESP8266?_sort=id&_order=desc&_limit=1', headers=headers)
    
    if r.status_code == 200:
        return r.json()
    else:
        print("error Unable to fetch data")


@app.route('/nodemcu/Buzz/<numericStatus>')
def send_buzzer_status(numericStatus):
    client.publish('Buzzer', numericStatus)
    client.loop()
    
    return f'Buzzer status set to: {numericStatus}'



if __name__ == "__main__":
    client = connect_mqtt()
    app.run(host='0.0.0.0', debug=True)
