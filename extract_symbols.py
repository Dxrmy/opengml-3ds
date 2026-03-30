import re
with open('build.log', encoding='utf-8', errors='ignore') as f:
    text = f.read()
matches = set(re.findall(r"undefined reference to `(_ZN3ogm11interpreter7Display[^']+)'", text))
for m in sorted(matches):
    print(m)
