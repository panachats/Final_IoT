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
    
    # ตรวจสอบว่าการเรียก API สำเร็จหรือไม่
    if r.status_code == 200:
        # แปลงข้อมูล JSON ที่ได้เป็น JSON ของ Flask และคืนค่า
        return jsonify(r.json())
    else:
        # ถ้าไม่สำเร็จ คืนข้อความผิดพลาด
        return jsonify({"error": "Unable to fetch data"}), 500

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
