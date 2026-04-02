import os
import re
import sys

def analyze_codebase(root_dir):
    stats = {
        'todo_count': 0,
        'fixme_count': 0,
        'stub_count': 0,
        'strcpy_count': 0,
        'sprintf_count': 0,
        'system_count': 0,
        'catch_all_count': 0,
        'files_analyzed': 0,
        'issues': []
    }
    
    for dirpath, _, filenames in os.walk(root_dir):
        if 'external' in dirpath or 'build' in dirpath:
            continue
            
        for file in filenames:
            if file.endswith(('.cpp', '.hpp', '.h', '.c', '.inc')):
                filepath = os.path.join(dirpath, file)
                stats['files_analyzed'] += 1
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        lines = f.readlines()
                        for i, line in enumerate(lines):
                            line_num = i + 1
                            if 'TODO' in line:
                                stats['todo_count'] += 1
                            if 'FIXME' in line:
                                stats['fixme_count'] += 1
                                stats['issues'].append(f"FIXME in {file}:{line_num}")
                            if 'STUB' in line.upper():
                                stats['stub_count'] += 1
                            if re.search(r'\bstrcpy\b', line):
                                stats['strcpy_count'] += 1
                                stats['issues'].append(f"Unsafe strcpy in {file}:{line_num}")
                            if re.search(r'\bsprintf\b', line):
                                stats['sprintf_count'] += 1
                            if re.search(r'\bsystem\s*\(', line):
                                stats['system_count'] += 1
                                stats['issues'].append(f"System call in {file}:{line_num}")
                            if re.search(r'catch\s*\(\.\.\.\)', line):
                                stats['catch_all_count'] += 1
                except Exception as e:
                    print(f"Error reading {filepath}: {e}")

    print("--- Codebase Analysis Results ---")
    print(f"Files Analyzed: {stats['files_analyzed']}")
    print(f"TODOs: {stats['todo_count']}")
    print(f"FIXMEs: {stats['fixme_count']}")
    print(f"STUBs: {stats['stub_count']}")
    print(f"Unsafe 'strcpy' calls: {stats['strcpy_count']}")
    print(f"Unsafe 'sprintf' calls: {stats['sprintf_count']}")
    print(f"OS 'system' calls: {stats['system_count']}")
    print(f"Catch-all exception handlers: {stats['catch_all_count']}")
    
    if stats['issues']:
        print("\nSignificant Issues Found:")
        for issue in stats['issues'][:20]: # show first 20
            print(f" - {issue}")
            
    # Calculate an arbitrary completion percentage based on TODOs/FIXMEs
    # Assuming a baseline of 500 TODOs is 0% and 0 TODOs is 100%
    baseline = 300
    current = stats['todo_count'] + stats['fixme_count']
    percentage = max(0, min(100, int((1 - (current / baseline)) * 100)))
    print(f"\nEstimated Completion Percentage (based on remaining tasks/stubs): ~{percentage}%")

if __name__ == '__main__':
    src_dir = 'src' if len(sys.argv) == 1 else sys.argv[1]
    analyze_codebase(src_dir)
