from flask import Flask, jsonify, render_template
import requests
app = Flask(__name__)
server_ip = "http://192.168.0.178:80"
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

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
