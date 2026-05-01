import re
import requests

url = "https://cdn.jsdelivr.net/npm/@tabler/icons-webfont@3.33.0/dist/tabler-icons.css"
css = requests.get(url).text

pattern = re.compile(r'\.ti-([a-z0-9-]+):before\s*\{\s*content:\s*"\\([a-f0-9]+)";\s*\}')

entries = []

for name, code in pattern.findall(css):
    enum_name = "ICON_" + name.upper().replace("-", "_")
    value = f"0x{code.upper()}"
    entries.append((enum_name, value))

# Sort for stability
entries.sort()

print("#pragma once")
print("#include <stdint.h>\n")

# Enum (index-based)
print("typedef enum {")
for name, _ in entries:
    print(f"    {name},")
print("    ICON_COUNT")
print("} Icon;\n")

# Codepoint table
print("static const uint16_t IconCodepoints[ICON_COUNT] = {")
for name, value in entries:
    print(f"    [{name}] = {value},")
print("};")