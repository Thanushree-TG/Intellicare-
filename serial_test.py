import serial
import time
from googletrans import Translator
translator = Translator()
import pygame
import time
from gtts import gTTS
from mutagen.mp3 import MP3
import time
import urllib

def Play(text1, lang):
    print(text1)
    myobj = gTTS(text=text1, lang='en-US', tld='com', slow=False)
    myobj.save("voice.mp3")
    print('\n------------Playing--------------\n')
    song = MP3("voice.mp3")
    pygame.mixer.init()
    pygame.mixer.music.load('voice.mp3')
    pygame.mixer.music.play()
    time.sleep(song.info.length)
    pygame.quit()
    time.sleep(1)

def LangTranslator(to_lang, get_sentence):
    from_lang = 'en'
    text_to_translate = translator.translate(get_sentence,src= from_lang,dest= to_lang)
    text = text_to_translate.text
    print(text)
    return text

ser = serial.Serial(
                  'COM3',
                  baudrate = 9600,
                  parity=serial.PARITY_NONE,
                  stopbits=serial.STOPBITS_ONE,
                  bytesize=serial.EIGHTBITS,                  
                  timeout=1
                  )

def ReadData():
    while True:
        try:
            data = ser.readline()
            data = data.decode('utf-8', 'ignore')
            data = data.strip()
            if data:
                data1 = data.split(',')
                if len(data1) == 9:
                    print(data1)
                    f = open('data.txt', 'w')
                    f.write(data)
                    f.close()
                    
                    data = data1
                    print(data)
                    f = open('language.txt', 'r')
                    lang = f.read()
                    f.close()

                    if data[7] != 'No commands':
                        Text = LangTranslator(lang, data[7])
                        i = 0
                        while True:
                            Play(Text, lang)
                            i+=1
                            if i > 4:
                                break
                    api_key = "74QGK0AGT5A5JBPS"
                    URL='https://api.thingspeak.com/update?api_key={}&field1={}&field2={}&field3={}&field4={}&field5={}&field6={}&field7={}'.format(api_key,int(float(data[0])),int(float(data[1])),int(float(data[2])),int(float(data[3])),int(float(data[4])),int(float(data[5])),int(float(data[6])))
                    res=urllib.request.urlopen(URL)
                    print(res)
        except Exception as e:
            print(e)
        time.sleep(1)
ReadData()