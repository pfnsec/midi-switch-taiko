
import hid
from pprint import pprint

vid = 0x0F0D
pid = 0x00F0	# Change it for your device

for h in hid.enumerate():
    if h['usage'] == 5:
        pprint(h)


h = hid.device()
h.open(vid, pid)
print(f'Device manufacturer: {h.get_manufacturer_string()}')

res = h.write([3, 1])
print(h.error())
old_buf = []
h.close()
quit()
while(True):
    buf = h.read(512)
    if(buf != old_buf):
        keys = []
        for i in buf:
            keys += [int.from_bytes([i], 'big')]
        print(keys)
    old_buf = buf