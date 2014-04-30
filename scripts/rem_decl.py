import re
import sys

def main(args):
    for file_name in args[1:]:
        process_file(file_name)

def find_matching_close(s, index):
    depth = 1
    curr_index = index
    while curr_index < len(s) and depth > 0:
        if s[curr_index] == '(':
            depth += 1
        elif s[curr_index] == ')':
            depth -= 1
        curr_index += 1
    if depth == 0:
        return curr_index
    else:
        return -1

argdecl_re = re.compile(r'\b(F|V)DECL\(')
ndecl_re = re.compile(r'\bNDECL\s*\(')

def process_lines(lines_arg):
    lines = lines_arg[:]
    out_lines = []
    for line in lines:
        m = ndecl_re.search(line);
        if m:
            actual_start = m.start(0)
            actual_end = find_matching_close(line, m.end(0))
            contents = line[m.end(0):actual_end - 1]
            line = line[:actual_start] + contents + '()' + line[actual_end:]
        out_lines.append(line);
        
    return out_lines

def process_file(file_name):
    with open(file_name, 'r') as f:
        lines = list(f)
    new_lines = process_lines(lines)
    new_file_contents = ''.join(new_lines)
    with open(file_name, 'w') as f:
        f.write(new_file_contents)

if __name__ == '__main__':
    main(sys.argv)
