from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi
from luma.core.render import canvas
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT
import time

serial = spi(port=0, device=0)
device = max7219(serial, cascaded=1, block_orientation=0, rotate=0)
print("Created device")
# start demo
msg = "12345678"
print(msg)
show_message(device, msg, fill="white", font=proportional(CP437_FONT))
time.sleep(1)

msg = "Slow"
print(msg)
show_message(device, msg, fill="white", font=proportional(LCD_FONT), scroll_delay=0.1)

time.sleep(1)
device.contrast(0x60)
with canvas(device) as draw:
    text(draw, (0, 0), "A", fill="white", font=proportional(CP437_FONT))
time.sleep(2)

with canvas(device) as draw:
    draw.rectangle(device.bounding_box, outline="white", fill="black")
    # time.sleep(2)
    # 僅一個 8*8 看不出橢圓的形狀
    # draw.ellipse([(0, 0), (7, 7)], outline="white", fill="black")

time.sleep(2)

for _ in range(5):
    for intensity in range(16):
        device.contrast(intensity * 16)
        time.sleep(0.1)

device.contrast(0x80)
time.sleep(1)