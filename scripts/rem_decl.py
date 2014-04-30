import re
import sys

def main(args):
    for file_name in args[1:]:
        process_file(file_name)

def find_matching_close(s, index):
    assert s[index] == '('
    depth = 1
    curr_index = index + 1
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
ndecl_re = re.compile(r'\bNDECL\((?P<name>[^\)]+)\)')

def process_lines(lines_arg):
    lines = lines_arg[:]
    out_lines = []
    for line in lines:
        out_lines.append(ndecl_re.sub(line, r'\g<name>()'))
    return out_lines

def process_file(file_name):
    with open(file_name, 'r') as f:
        lines = list(f)
    new_lines = process_lines(lines)
    new_file_contents = ''.join(new_lines)
    with open(file_name, 'w') as f:
        f.write(new_file_contents)
