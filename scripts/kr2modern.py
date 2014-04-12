import re
import sys

fn_line_re = re.compile(r'^(?P<name>[a-zA-Z_][a-zA-Z0-9_]*)\((?P<args>.*)\)\s*$')

def main(args):
    for file_name in args[1:]:
        process_file(file_name)

def rev_range(start, stop):
    return range(stop - 1, start - 1, -1)

decl_line_re = re.compile(r'^([^;]*);.*$')

def find_prev_fn(lines, index):
    """ Finds the previous line which matches the function syntax """;
    for prev_index in rev_range(0, index):
        curr_line = lines[prev_index]
        
        m = fn_line_re.match(curr_line)
        if m:
            return prev_index

        m = decl_line_re.match(curr_line)
        if not m:
            return -1

    return -1;

class ParsedFunction:
    def __init__(self, name, args):
        self.name = name
        self.args = args

def parse_fn_line(fn_line):
    m = fn_line_re.match(fn_line)
    assert m
    args_string = m.group('args')
    # args_string should be a comma-separated sequence of simple identifiers
    args = [ arg.strip() for arg in args_string.split(',') if not len(arg.strip()) == 0 ]
    return ParsedFunction(m.group('name'), args)

c_param_re = re.compile(r'^\**([a-zA-Z_][a-zA-Z0-9_]*)(\[\])*$')

def extract_arg_decls(decl_line):
    m = decl_line_re.match(decl_line)
    assert m, "Bad match for decl line: '{}'".format(decl_line)
    parts = list(map(str.strip, m.group(1).split(',')))
    head_words = list(filter(bool, re.split(r'\s+', parts[0])))
    base_type = ' '.join(head_words[:-1])
    parts[0] = head_words[-1]

    args = {}
    for part in parts:
        param_match = c_param_re.match(part)
        if not param_match: 
            return None
        args[param_match.group(1)] = base_type + ' ' + part
    return args

def process_arg_decls(decl_lines):
    arg_decl_map = {}
    for decl_line in decl_lines:
        line_decl_map = extract_arg_decls(decl_line)
        if line_decl_map is None:
            return None
        arg_decl_map.update(line_decl_map)
    return arg_decl_map

def find_first_start_brace(lines):
    for i, line in enumerate(lines):
        if line.startswith('{'):
            return i
    return -1

def process_lines(lines_arg):
    lines = lines_arg[:]
    out_lines = []
    while True:
        brace_index = find_first_start_brace(lines)
        if brace_index < 0:
            break
        fn_line_index = find_prev_fn(lines, brace_index)
        # It has to be greater than zero, since the line before is the
        # type decl of the function.
        if fn_line_index <= 0:
            out_lines.extend(lines[0:brace_index + 1])
            del lines[0:brace_index + 1]
            continue
        arg_decl_lines = lines[fn_line_index + 1:brace_index]
        arg_decl_map = process_arg_decls(arg_decl_lines)
        if arg_decl_map is None:
            out_lines.extend(lines[0:brace_index + 1])
            del lines[0:brace_index + 1]
            continue

        type_line = lines[fn_line_index - 1].strip()
        parsed_fn = parse_fn_line(lines[fn_line_index])
        arg_decls = [arg_decl_map[arg] for arg in parsed_fn.args]
        new_fn_line = (
                type_line + ' ' + parsed_fn.name + '(' +
                ', '.join(arg_decls) + ') {\n')
        out_lines.extend(lines[0:fn_line_index - 1])
        del lines[0:brace_index + 1]
        out_lines.append(new_fn_line)

    out_lines.extend(lines)
    return out_lines

            


def process_file(file_name):
    lines = list(open(file_name, 'r'))
    new_lines = process_lines(lines)
    print(''.join(new_lines))

if __name__ == '__main__':
    main(sys.argv)
