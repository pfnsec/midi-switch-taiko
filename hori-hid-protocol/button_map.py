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

buttons = ["l_rim", "r_rim", "l_head", "r_head"]

neutral = [0, 0, 15, 128, 128, 128, 128, 0]

old_buf = []
while(True):
    buf = h.read(512)
    if(buf != old_buf):
        keys = []
        for i, b in buf.enumerate():
            keys += [int.from_bytes([b], 'big') - neutral[i]]
        print(keys)
    old_buf = buf