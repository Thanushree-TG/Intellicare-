from flask import Flask, render_template, request, jsonify
import threading
from datetime import datetime
from googletrans import Translator

app = Flask(__name__)
app.secret_key = 'your_secret_key_here'

translator = Translator()


sensor_history = []


def LangTranslator(to_lang, text):
    from_lang = 'en'
    valid_langs = ['en', 'ta', 'hi', 'kn']

    # fallback if invalid
    if to_lang not in valid_langs:
        to_lang = 'en'

    try:
        translated = translator.translate(text, src=from_lang, dest=to_lang)
        return translated.text
    except Exception as e:
        print("Translation error:", e)
        return text  # fallback


def GetData():
    from serial_test import ReadData
    ReadData()


@app.route('/')
def home():
    return render_template('index.html')


@app.route('/scan_qr')
def scan_qr():
    from reader import QRReader
    d = QRReader()

    if d == 'IntelliCare':
        return render_template('dashboard.html')
    else:
        return render_template('index.html', msg='Invalid QR code')


@app.route('/dashboard')
def dashboard():
    return render_template('dashboard.html')


@app.route('/Language/<lng>')
def Language(lng):
    valid_langs = ['en', 'ta', 'hi', 'kn']

    if lng not in valid_langs:
        lng = 'en'

    with open('language.txt', 'w') as f:
        f.write(lng)

    return render_template('dashboard.html', msg=f"Language set to {lng}")



@app.route('/get_sensor_data')
def get_sensor_data():
    try:
        # Read sensor file
        with open('data.txt', 'r') as f:
            data = f.read().split(',')

        # Read language
        try:
            with open('language.txt', 'r') as f:
                lang = f.read().strip()
        except:
            lang = 'en'

        # Translate text safely
        Text = LangTranslator(lang, data[7])
        Motion = LangTranslator(lang, data[8])

        sensor_data = {
            'oxygen': int(float(data[5])), 
            'blood_pressure': {
                'systolic': int(float(data[4])),
                'diastolic': int(float(data[4])) // 2
            },
            'flex': [
                int(float(data[0])),
                int(float(data[1])),
                int(float(data[2])),
                int(float(data[3]))
            ],
            'temperature': int(float(data[6])),
            'notification': Text,
            'motion': Motion,
            'time': datetime.now()
        }

        # Save for summary
        sensor_history.append(sensor_data)

        # Optional alerts
        if data[7] != 'No commands':
            print('Send notification')

        if data[8] == "Motion: Detected":
            print('Send motion alert')

        return jsonify(sensor_data)

    except Exception as e:
        print("Error in get_sensor_data:", e)
        return jsonify({"error": str(e)}), 500



def calculate_summary():
    today = datetime.now().date()
    today_data = [d for d in sensor_history if d["time"].date() == today]

    spo2 = [d["oxygen"] for d in today_data if d.get("oxygen") is not None]



    return {
        "spo2_min": min(spo2) if spo2 else 0,
        "spo2_max": max(spo2) if spo2 else 0,
        "spo2_avg": round(sum(spo2)/len(spo2), 2) if spo2 else 0,

    }


@app.route('/daily-summary')
def daily_summary():
    summary = calculate_summary()

    if not summary:
        return jsonify({"error": "No data available"})

    return jsonify(summary)




if __name__ == '__main__':
    t1 = threading.Thread(target=GetData)
    t1.daemon = True
    t1.start()

    app.run(debug=True, host="0.0.0.0",)