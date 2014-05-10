import os
import re
import sys

def main(args):
    process_file(args[1])

header_re = re.compile(r'^/\* ### (\w+)\.c ### \*/$')

def get_headers(lines_arg):
    lines = lines_arg[:]
    headers = {}
    curr_name = None
    curr_lines = []
    for line in lines:
        m = header_re.match(line)
        if m:
            if curr_name is not None:
                headers[curr_name] = ''.join(curr_lines)
            curr_lines = []
            curr_name = m.group(1)
        else:
            curr_lines.append(line)
    if curr_name is not None:
        headers[curr_name] = ''.join(curr_lines)
    return headers

def create_header(f, basename, text):
    guard = basename.upper() + "_H_"
    template = """
// TODO(BNC): Add Docs
#ifndef {0}
#define {0}
{1}
#endif  // {0}
"""
    file_contents = template.format(guard, text)
    print ('=== {0} ==='.format(f))
    print (file_contents)

def find_last(lst, pred):
    for i in range(len(lst) - 1, -1, -1):
        if pred(lst[i]):
            return i;

def update_header(f, basename, text):
    lines = file_lines(f)
    endif_index = find_last(lines, lambda x: re.search(r'^#endif', x))
    print ("==> UPDATE: {0}: {1}".format(f, endif_index))
    lines[endif_index:endif_index] = [text]
    print (''.join(lines))


def handle_header(basename, text):
    header_path = os.path.join('include', basename + '.h')
    if os.path.exists(header_path):
        update_header(header_path, basename, text)
    else:
        create_header(header_path, basename, text)

def file_lines(name):
    with open(name, 'r') as f:
        return list(f)

def process_file(file_name):
    with open(file_name, 'r') as f:
        lines = list(f)
    headers = get_headers(lines)
    for basename, text in headers.items():
        handle_header(basename, text)

if __name__ == '__main__':
    main(sys.argv)
