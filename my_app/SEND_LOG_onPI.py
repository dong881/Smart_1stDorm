import serial                                      # add Serial library for serial communication
import requests
import sys

session = requests.Session()

API_URL = "https://script.google.com/macros/s/AKfycbyutB2OyhAhG7E6m_8OE9CQDR_CmMxhYxntRXwVgcdNLX7qPOdplrVPlQwITwpci8qy/exec"


try:
    Arduino_Serial = serial.Serial('/dev/ttyUSB0',115200)       # Initialize serial and Create Serial port object called Arduino_Serial
except:
    Arduino_Serial = serial.Serial('com9',115200)       # Initialize serial and Create Serial port object called Arduino_Serial

while True:
    try:
        orignalData = Arduino_Serial.readline().strip().decode("utf-8")
        if orignalData == "ON" or orignalData == "OFF" :
            session.get(API_URL+"?Fun=saveData&In=" + orignalData) #傳送記錄目前狀態
    except BaseException as e:
        print(str(e))
        session.get(API_URL+"?Fun=saveData2&In=OFF") #跳掉預設關燈
        Arduino_Serial.close()
        sys.exit()