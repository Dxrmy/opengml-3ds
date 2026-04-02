import os
import re

def replace_in_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False
    
    # Improved regex: matches the object before the .castCoerce
    # We use a non-greedy match for the object to avoid eating the switch '('
    # We look for a dot followed by castCoerce
    
    new_content = content
    patterns = ['size_t', 'uint32_t', 'unsigned int']
    
    for p in patterns:
        # Match only valid identifier/expression characters. 
        # We avoid capturing the leading '(' of a switch or if statement by starting the match 
        # after any character that isn't part of the expression.
        regex = r'([a-zA-Z0-9_\]\)]+)\.castCoerce<' + p + r'>\s*\(\)'
        subst = r'static_cast<' + p + r'>(\1.castCoerce<uint64_t>())'
        new_content = re.sub(regex, subst, new_content)
    
    if content != new_content:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(new_content)
            return True
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
    return False

if __name__ == '__main__':
    count = 0
    # First, restore to a clean state to avoid double-wrapping
    os.system('cd opengml_repo && git checkout src')
    
    for root, _, files in os.walk('opengml_repo/src'):
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.h', '.inc')):
                if replace_in_file(os.path.join(root, file)):
                    print(f"Updated {os.path.join(root, file)}")
                    count += 1
    print(f"Total files updated: {count}")
