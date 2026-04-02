import sys
import re

def main():
    if len(sys.argv) < 2:
        print("Usage: extract_errors.py <log_file>")
        return
        
    log_file = sys.argv[1]
    content = ""
    # Try different encodings
    for enc in ['utf-16', 'utf-8', 'latin-1']:
        try:
            with open(log_file, 'r', encoding=enc, errors='ignore') as f:
                content = f.read()
            if content:
                print(f"Read using {enc}")
                break
        except Exception as e:
            continue
            
    if not content:
        print(f"Could not read {log_file}")
        return
        
    # Split by lines for analysis
    lines = content.splitlines()
    
    error_patterns = [
        r' error:',
        r'undefined reference to',
        r'make: \*\*\*',
        r'ld returned 1 exit status',
        r'fatal error:',
        r'version control conflict marker'
    ]
    
    context_patterns = [
        r'In file included from',
        r'At global scope:',
        r'In function',
        r'required from here'
    ]
    
    output = []
    
    for i, line in enumerate(lines):
        is_error = any(re.search(p, line) for p in error_patterns)
        is_context = any(re.search(p, line) for p in context_patterns)
        
        if is_error or is_context:
            output.append(line.strip())
            # If it's a compiler error with file info, try to grab the next few lines (source snippet)
            if ' error:' in line:
                for j in range(1, 5):
                    if i + j < len(lines):
                        next_line = lines[i+j]
                        # stop if we hit another error or context line
                        if any(re.search(p, next_line) for p in error_patterns + context_patterns):
                            break
                        output.append("  " + next_line.strip())
    
    if not output:
        print("No errors found in log.")
    else:
        print(f"Extracted {len(output)} lines of interest:\n")
        # De-duplicate consecutive identical lines
        last = None
        count = 0
        for out in output:
            if out and out != last:
                print(out)
                last = out
                count += 1
            if count > 300:
                print("... (truncated)")
                break

if __name__ == '__main__':
    main()
