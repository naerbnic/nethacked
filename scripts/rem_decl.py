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


def until_unchanged(line, fn):
    while True:
        new_line = fn(line)
        if new_line == line:
            return new_line
        print(new_line)
        line = new_line

argdecl_re = re.compile(r'\b(F|V)DECL\s*\(')
contents_re = re.compile(r'(?P<name>[^,]+),\s*\((?P<args>.*)\)\s*')

def fix_fvdecl(line):
    m = argdecl_re.search(line);
    if not m:
        return line
    actual_start = m.start(0)
    actual_end = find_matching_close(line, m.end(0))
    if actual_end < 0:
        return line
    contents = line[m.end(0):actual_end - 1]
    m2 = contents_re.match(contents)
    if not m2:
        return line
    return (line[:actual_start] + m2.group('name') +
        '(' + m2.group('args') + ')' + line[actual_end:])

ndecl_re = re.compile(r'\bNDECL\s*\(')

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
        line = until_unchanged(line, fix_ndecl);
        line = until_unchanged(line, fix_fvdecl);
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
