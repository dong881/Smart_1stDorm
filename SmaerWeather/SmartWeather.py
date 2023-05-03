from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
import time
import requests
from datetime import datetime, timedelta

# 定義函式，從交通部氣象局網站獲取當天天氣預報
def get_weather_forecast():
    # 獲取當天日期
    delat = 0
    TODAY_Date = datetime.now()
    today = (TODAY_Date+ timedelta(days=delat)).strftime('%Y-%m-%d')
    tomorrow = (TODAY_Date+ timedelta(days=delat+1)).strftime('%Y-%m-%d')
    print(today + " ~ " + tomorrow)
    # 組裝 API URL
    type = "T,PoP6h" #溫度(3h)、降雨機率(6h)
    NowTime = ("0" if(TODAY_Date.hour<10) else "" )+ str(TODAY_Date.hour)
    print(NowTime)
    url = f'https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-D0047-061?Authorization=CWB-D5826CB0-A0C6-4D5D-9CC4-7E62D49FD6EE&limit=8&locationName=%E5%A4%A7%E5%AE%89%E5%8D%80&elementName='+ type +'&timeFrom='+today+'T'+NowTime+'%3A00%3A00&timeTo='+tomorrow+'T'+NowTime+'%3A00%3A00'
    # 用 requests 套件發送 GET 請求獲取資料
    response = requests.get(url)

    # 解析 JSON 資料
    data = response.json()
    # print(data)
    PoPdata = data["records"]["locations"][0]["location"][0]["weatherElement"][1]["time"]
    T_data = data["records"]["locations"][0]["location"][0]["weatherElement"][0]["time"]
    # 從資料中提取出每個時間段的平均溫度
    # print(PoPdata)
    print(T_data)

    TDataList = [d['elementValue'][0]['value'] for d in T_data]
    PopDataList = [d['elementValue'][0]['value'] for d in PoPdata]
    # print(TDataList)
    # print(PopDataList)
    return temperature_to_led_levels(TDataList),PoP_to_led_levels(PopDataList)



# 定義函式，將溫度轉換為 8 階層，用於顯示在 8x8 矩陣上
def temperature_to_led_levels(temperature):
    # 將溫度轉換為整數
    temperature = [int(t) for t in temperature]

    # 計算最大和最小值，用於將溫度轉換為 8 階層
    temperature_min = 16
    temperature_max = 32

    # 將溫度轉換為 8 階層
    levels = []
    for t in temperature:
        if t>temperature_max: t = temperature_max
        if t<temperature_min: t = temperature_min
        level = round(1+6 * (t - temperature_min) / (temperature_max - temperature_min))
        levels.append(level)
    return levels

def PoP_to_led_levels(Pop):
    Pop = [int(t) for t in Pop]
    levels = []
    PoP_level = 60
    for p in Pop:
        levels.append(round(p>PoP_level))
        levels.append(round(p>PoP_level))
    return levels

# initialize SPI interface for the LED matrix
serial = spi(port=0, device=0)
device = max7219(serial, cascaded=1, block_orientation=0, rotate=0)


def START_LOGO():
    with canvas(device) as draw:
        draw.rectangle(device.bounding_box, outline="white", fill="black")
        draw.ellipse([(0, 0), (7, 7)], outline="white", fill="black")
        draw.ellipse([(2, 2), (5, 5)], outline="white", fill="black")
    time.sleep(0.5)
    for intensity in list(range(15,0,-1)):
        device.contrast(intensity * 16)
        time.sleep(0.05)
    for intensity in range(16):
        device.contrast(intensity * 16)
        time.sleep(0.05)


def shift_array(data, index):
    start = 8 - index
    end = start + 8
    shifted_data = data[start:end] + data[:start] + data[end:]
    # shifted_data.append(shifted_data.pop(0))
    return shifted_data

def calculate_output(hour):
    if hour in [1, 2, 3]:
        return 0
    elif hour in [4, 5, 6]:
        return 1
    elif hour in [7, 8, 9]:
        return 2
    elif hour in [10, 11, 12]:
        return 3
    elif hour in [13, 14, 15]:
        return 4
    elif hour in [16, 17, 18]:
        return 5
    elif hour in [19, 20, 21]:
        return 6
    elif hour in [22, 23, 0]:
        return 7
    else:
        return 0

def display_heights(bool,heights,PoP_format):
    # set the brightness level of the LED matrix
    device.contrast(30)
    # display the heights on the LED matrix
    with canvas(device) as draw:
        for i in range(8):
            if bool and i==calculate_output(datetime.now().hour):
                continue 
            height = heights[i]
            for j in range(height):
                draw.point((i, 7-j-1), fill="white")
            if PoP_format[i] == 1:
                draw.point((i, 7), fill="white")
# START_LOGO()

sec = 60*40
T_format = []
PoP_format = []
while 1:
    if sec == 60*40: # 每隔40分鐘更新一次資料
        START_LOGO()
        sec = 0
        ArrayData = get_weather_forecast()
        print(ArrayData)
        T_format   = shift_array(ArrayData[0],calculate_output(datetime.now().hour))
        PoP_format = shift_array(ArrayData[1],calculate_output(datetime.now().hour))
        print(T_format)
        print(PoP_format)
    display_heights(sec%2,T_format,PoP_format)
    sec += 1
    time.sleep(1)
    


