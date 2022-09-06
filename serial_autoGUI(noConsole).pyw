# gesture control python program for controlling certain functions in windows pc
# Code by BalaAppu
# Website: www.electronicshub.org
import serial                                      # add Serial library for serial communication
import pyautogui                                   # add pyautogui library for programmatically controlling the mouse and keyboard.
import sys
import subprocess #執行VBS檔
import time
import requests
# subprocess.call("cscript ON_FAN.vbs") # works
# subprocess.call("cscript OFF_FAN.vbs") # works

API_URL = "https://script.google.com/macros/s/AKfycbyNwPDbggeSyJu9YITR6dGuBz4z5X_t84rwUrF-nbSibrkMcuOznFIHDX2EFybbiy2C/exec"
session = requests.Session()
from win10toast import ToastNotifier
toaster = ToastNotifier()

Arduino_Serial = serial.Serial('com9',115200)       # Initialize serial and Create Serial port object called Arduino_Serial

switchCount = 3
notifierCount = 200
leaveTimes = 0
sitDownTimes = 0
nowStateIsLight = True
ActiveWork = True
# Arduino_Serial.write("ON".encode("utf-8"))
# Arduino_Serial.write("OFF".encode("utf-8"))

session.get(API_URL+"?Fun=saveData&In=ON") #開機預設已在位置上

def SWITCH_STATES(state):
    global nowStateIsLight
    if state and not nowStateIsLight: 
        Arduino_Serial.write("ON".encode("utf-8"))
        nowStateIsLight = True
    elif not state and nowStateIsLight: 
        Arduino_Serial.write("OFF".encode("utf-8"))
        nowStateIsLight = False
    session.get(API_URL+"?Fun=saveData&In="+("ON" if state else "OFF")) #傳送記錄目前狀態


def SHOW_TOAST(text):
    toaster.show_toast("Warning!~",
                        text,
                        icon_path=None,
                        duration=5,
                        threaded=(True))

while True:
    try:
        orignalData = Arduino_Serial.readline().strip().decode("utf-8")
        if orignalData == "ON" or orignalData == "OFF" : continue
        incoming_data = int(orignalData) # read the serial data and print it as line
        print(incoming_data)                            # print the incoming Serial data
        
        if incoming_data < 65 and not ActiveWork:
            sitDownTimes += 1
            
        if sitDownTimes > 10:            
            sitDownTimes = 0
            if not ActiveWork: 
# =============================================================================
#              # 偵測到坐下
# =============================================================================
                SWITCH_STATES(True)    
                # pyautogui.hotkey('ctrl')
                print("=== AUTO OPEN (",incoming_data,"cm)")
                ActiveWork = True
# =============================================================================

        if incoming_data > 105 and ActiveWork:
            sitDownTimes = 0
            leaveTimes += 1
            if leaveTimes > 10:
                leaveTimes = 0
                print("AUTO CLOSE SCREEN and LED ~~(",incoming_data,"cm)")
                # pyautogui.hotkey('ctrl','alt', 'b')
                SWITCH_STATES(False)
                ActiveWork = False
        elif incoming_data < 60:
            leaveTimes = 0
            notifierCount += 1
            if notifierCount > 200:
                notifierCount = 0
                print("TOO CLOSE !! (",incoming_data,"cm)")
                SHOW_TOAST("離螢幕太近了喔 ("+str(incoming_data)+"cm)")
        else:
            leaveTimes = 0
            notifierCount = 195
        
# =============================================================================
#                 # elif incoming_data < 42:
        #     print("=== AUTO SWITCH (",incoming_data,"cm)")
        #     if switchCount > 2:
        #         switchCount = 0
        #         pyautogui.keyDown('alt')                   # performs "alt+tab" operation which switches the tab
        #         pyautogui.press('tab') 
        #         pyautogui.keyUp('alt')
# =============================================================================
        
        # if 'next' in incoming_data:                    # if incoming data is 'next'
        #     pyautogui.hotkey('ctrl', 'pgdn')           # perform "ctrl+pgdn" operation which moves to the next tab
            
        # if 'previous' in incoming_data:                # if incoming data is 'previous'
        #     pyautogui.hotkey('ctrl', 'pgup')           # perform "ctrl+pgup" operation which moves to the previous tab
    
        # if 'down' in incoming_data:                    # if incoming data is 'down'
        #     #pyautogui.press('down')                   # performs "down arrow" operation which scrolls down the page
        #     pyautogui.scroll(-100) 
             
        # if 'up' in incoming_data:                      # if incoming data is 'up'
        #     #pyautogui.press('up')                      # performs "up arrow" operation which scrolls up the page
        #     pyautogui.scroll(100)
            
        # if 'change' in incoming_data:                  # if incoming data is 'change'
        #     pyautogui.keyDown('alt')                   # performs "alt+tab" operation which switches the tab
        #     pyautogui.press('tab')
        #     pyautogui.keyUp('alt')
            
        incoming_data = 200                            # clears the data
    except :
        session.get(API_URL+"?Fun=saveData&In=OFF") #跳掉預設關燈
        Arduino_Serial.close()
        sys.exit()
        
        
# =============================================================================
#         Exception as e:
#         print(e)
# =============================================================================
