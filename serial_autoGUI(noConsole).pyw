import serial                                      # add Serial library for serial communication
import pyautogui                                   # add pyautogui library for programmatically controlling the mouse and keyboard.
import sys
import subprocess #執行VBS檔
import time
import requests
# subprocess.call("cscript ON_FAN.vbs") # works
# subprocess.call("cscript OFF_FAN.vbs") # works

API_URL = "https://script.google.com/macros/s/AKfycbyXGOBIQyUtYFDaCPXK_HGjILYlMlMBSj3g5-aFcvAvFZG8niOk2PwY06MFvc3zcmV-/exec"
session = requests.Session()
from win10toast import ToastNotifier
toaster = ToastNotifier()

Arduino_Serial = serial.Serial('com9',115200)       # Initialize serial and Create Serial port object called Arduino_Serial

switchCount = 3
notifierCount = 200
leaveTimes = 0
sitDownTimes = 0
noOneTimes = 0
nowStateIsLight = True
ActiveWork = True

session.get(API_URL+"?Fun=saveData&In=ON") #開機預設已在位置上

def SWITCH_STATES(state):
    global nowStateIsLight, ActiveWork
    ActiveWork = state
    if state and not nowStateIsLight: 
        # Arduino_Serial.write("ON".encode("utf-8"))
        nowStateIsLight = True
    elif not state and nowStateIsLight: 
        Arduino_Serial.write("OFF".encode("utf-8"))
        nowStateIsLight = False
    session.get(API_URL+"?Fun=saveData&In="+("ON" if state else "OFF")) #傳送記錄目前狀態

def SET_LED_COLOR():
    Arduino_Serial.write("FUN".encode("utf-8"))

def SHOW_TOAST(text):
    toaster.show_toast("Warning!~",
                        text,
                        icon_path=None,
                        duration=5,
                        threaded=(True))

def CLOSE_SCREEN():
    pyautogui.hotkey('ctrl','alt', 'b')

while True:
    try:
        orignalData = Arduino_Serial.readline().strip().decode("utf-8")
        if orignalData == "ON" or orignalData == "OFF" : continue
        incoming_data = int(orignalData) # read the serial data and print it as line
        print(incoming_data, notifierCount) # print the incoming Serial data
        
        if ActiveWork: #判斷模式(Active: 我在位置工作[準備偵測離開時關燈、提醒不要離螢幕太近])
            if incoming_data > 105:
                sitDownTimes = 0
                leaveTimes += 1
                if leaveTimes > 1: #15
                    leaveTimes = 0
                    print("AUTO CLOSE SCREEN and LED ~~(",incoming_data,"cm)")
                    SWITCH_STATES(False)
            elif incoming_data < 60:
                leaveTimes = 0
                notifierCount += 1
                if incoming_data < 20: SET_LED_COLOR()
                if notifierCount > 10: #10
                    notifierCount = 0
                    print("TOO CLOSE !! (",incoming_data,"cm)")
# =============================================================================
#                     if incoming_data < 20: #特殊指令
#                         CLOSE_SCREEN()
#                         SWITCH_STATES(False)
# =============================================================================
                    SHOW_TOAST("離螢幕太近了喔 ("+str(incoming_data)+"cm)")
            else: # (60~105) 重置變數INIT value
                leaveTimes = 0
                notifierCount = 8 #8
        else: # not ActiveWork
            if incoming_data > 85:  sitDownTimes = 0
            else:                   sitDownTimes += 1
            if sitDownTimes == 0: # 完全沒有人坐下
                noOneTimes += 1
                T = 5 #20
                if noOneTimes == T: #20
                    CLOSE_SCREEN() # 貼心關螢幕
                if noOneTimes > T+1: noOneTimes = T+1
            elif sitDownTimes > 7: # 偵測到坐下 (確定觸發坐下指令)   
                sitDownTimes = 0
                SWITCH_STATES(True)    
                print("=== AUTO OPEN (",incoming_data,"cm)")
            else: # sitDownTimes沒有持續等於0 就重置
                noOneTimes = 0
                    
    except :
        session.get(API_URL+"?Fun=saveData&In=OFF") #跳掉預設關燈
        Arduino_Serial.close()
        sys.exit()
        
        
# see error log
# =============================================================================
#         Exception as e:
#         print(e)
# =============================================================================

# Serial skill
# =============================================================================
#   Arduino_Serial.write("ON".encode("utf-8"))
#   Arduino_Serial.write("OFF".encode("utf-8"))
# =============================================================================

# pyautogui skill
# =============================================================================
#         pyautogui.hotkey('ctrl')
#         pyautogui.keyDown('alt')                   # performs "alt+tab" operation which switches the tab
#         pyautogui.press('tab') 
#         pyautogui.keyUp('alt')
#         pyautogui.hotkey('ctrl', 'pgdn')           # perform "ctrl+pgdn" operation which moves to the next tab
#         pyautogui.hotkey('ctrl', 'pgup')           # perform "ctrl+pgup" operation which moves to the previous tab
#         pyautogui.press('down')                   # performs "down arrow" operation which scrolls down the page
#         pyautogui.scroll(-100) 
#         pyautogui.press('up')                      # performs "up arrow" operation which scrolls up the page
#         pyautogui.scroll(100)
# =============================================================================


# pyautogui.hotkey('ctrl') #貼心開螢幕