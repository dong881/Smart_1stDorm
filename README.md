# smartDorm
◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻ <br>
<h3>🟫 使用的材料</h3>
1. 超音波距離感測器 HC-SR04<br>
2. 繼電器模組<br>
3. WS2812 LED x60<br>
4. Arduino UNO<br>
5. PC or Raspberry PI 3B<br>
<br>
◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻◻<br>
<br>
<h3>💡 安裝一個自動化控制宿舍桌上的檯燈</h3>

- [ ] 準備好上述材料
- [ ] 將Arduino 線路接好 (腳位配置於Arduino code)
  - Trig Pin = 10
  - Echo Pin = 9
  - Relay Pin = 8
  - WE2812 LED Pin = 5
- 目前僅開源至此 (API及其他功能目前因需google帳號，故僅供個人使用)

<br>
v2.0
為減輕電腦負擔，將上傳資料工作交由 Raspberry PI 3B<br>

========================================<br>
🟦 Arduino負責軟硬體：<br>
- 超聲波HC-SR04 判斷座位前方距離(cm) <br>
![20230312_155936](https://user-images.githubusercontent.com/52557611/224533636-3deba800-9bbf-4b24-af16-cb10af14a1af.jpg)
![20230312_164517](https://user-images.githubusercontent.com/52557611/224534057-08bf7a03-9a30-4226-beea-5a533c9f66c3.jpg)

- 根據條件判斷開關燈時機 (debounce)<br>
  - 超過115cm 關燈
  - 少於 85cm 開燈
  
![20230312_164256](https://user-images.githubusercontent.com/52557611/224533972-a9600f35-b98c-4d99-885f-5ee16261475d.jpg)
![20230312_164647](https://user-images.githubusercontent.com/52557611/224534145-7167a4df-d977-4176-bb35-ccb26818387c.jpg)

- 依據模式不同配置WS2812 LEDs<br>
  - 開燈時根據距離變換顏色(離螢幕越近越紅，越遠越綠);<br>前20顆保持閃爍浪漫黃燈
  - 關燈時整條顯示弱光同色浪漫燈光，持續變色約1小時候關閉
- 在觸發開/關燈時透過序列阜送出狀態<br>
- 獨立運作僅需供電即可使用
<br>
🔴🔴🔴🔴🔴🔴 v2.0 功能 目前僅開源至以上功能 🔴🔴🔴🔴🔴🔴<br><br><br>
========================================<br>
🟥 Raspberry PI 負責溝通：<br><br>
- 所有功能已包裝成Docker Image並上傳Docker Hub
- 灌好PI OS僅需運行container即可動作

    docker run -d --restart always -p 8080:8080 --privileged --device=/dev/ttyUSB0 dongdonghsu/dormpiapp:lastest

- 需透過NAT技術打穿外網請求(8080 port)，推薦使用 [PiTunnel(後來發現僅14天免費試用)](https://www.pitunnel.com/)
- 可以使用[cpolar](https://dashboard.cpolar.com)，雖然IP會浮動，但是可以透過爬蟲更新

1. 透過USB序列阜和 Arduino持續溝通<br>
2. 收到新狀態則發送HTTP GET request to GAP API<br>
3. 本身作為外部的一台API server可供外部做GET Request觸發pyhton script(sw.py)
![20230312_164322](https://user-images.githubusercontent.com/52557611/224533997-94f0f8f5-ee9b-45ff-919f-05fdcd2c76fa.jpg)

========================================<br>
🟧 雲端Google App Script (GAS) 負責 API端口：<br>
1. 接收資料串入做判斷 <br>
2. 加上 Time code 後將資料輸出至Google sheet <br>
3. 透過 Google 自家生態鏈利用 GAS 排版 Google Sheet <br>

🟩 Google Sheet 
- 轉換累計分鐘數為易讀公式：
    =TEXT(SUM(D3:D)/60,"0")&"小時："&MOD(SUM(D3:D),60)&"分"
    
- 判斷離開時間條件格式設定公式：
    =IF(AND(C5=1,C4=0), "true", "false")="true"

- 判斷坐下時間條件格式設定公式：
    =IF(AND(C5=0,C4=1), "true", "false")="true"
    
- 圖表格式設定：
![image](https://user-images.githubusercontent.com/52557611/224533583-99f4b4e1-5dd5-43af-82aa-3d4675445682.png)

========================================<br>
DEMO: <br>
![image](https://user-images.githubusercontent.com/52557611/224530371-9bab8421-0825-483d-80f9-b56bc1333983.png)
> 利用試算表條件格式設定公式分辨，🟥紅色：坐下時間、⬜灰色：離開時間以及單次所待的時間長度

![image](https://user-images.githubusercontent.com/52557611/200508630-db91f790-70d0-4f37-9a2f-77011b82b5fb.png)
- 新增分析每周每時所待分布圖
![image](https://user-images.githubusercontent.com/52557611/236018799-790f053e-4940-4565-be04-e7c836d6dbb3.png)
![image](https://user-images.githubusercontent.com/52557611/200508716-a0b63234-7bbd-4126-899f-baeafdf9d480.png)
