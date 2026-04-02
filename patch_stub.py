import sys

def patch_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    old_code = """void Display::set_culling(bool)
{ }"""

    new_code = """void Display::set_culling(bool)
{ }

void Display::reset_render_state()
{ }"""

    if old_code in content:
        content = content.replace(old_code, new_code)
        print("Patched stub")
    else:
        print("Could not find stub code to patch")

    with open(filepath, 'w') as f:
        f.write(content)

patch_file('src/interpreter/display/Display.cpp')
