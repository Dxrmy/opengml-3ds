import struct
import shutil
import os

with open('dummy.win', 'wb') as f:
    f.write(b'FORM')
    f.write(struct.pack('<I', 8))
    f.write(b'GEN8')
    f.write(struct.pack('<I', 0))

target = r'C:\Users\kmric\AppData\Roaming\Citra\sdmc\3ds\OpenGML\data.win'
os.makedirs(os.path.dirname(target), exist_ok=True)
shutil.copy('dummy.win', target)
print(f"Created dummy.win and copied to {target}")