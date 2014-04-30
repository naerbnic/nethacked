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

def until_unchanged(line, fn):
    while True:
        new_line = fn(line)
        if new_line == line:
            return new_line
        line = new_line

def fix_ndecl(line):
   m = ndecl_re.search(line);
   if not m:
       return line
   actual_start = m.start(0)
   actual_end = find_matching_close(line, m.end(0))
   contents = line[m.end(0):actual_end - 1]
   return line[:actual_start] + contents + '()' + line[actual_end:]

def process_lines(lines_arg):
    lines = lines_arg[:]
    out_lines = []
    for line in lines:
        out_lines.append(until_unchanged(line, fix_ndecl));
        
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
