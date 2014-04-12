import sys
from pathlib import Path

def main(args):
  wd = Path.cwd()
  for c_file in wd.glob('**/*.c'):
    new_path = c_file.with_suffix('.cc')
    c_file.rename(new_path)

def __name__ == '__main__':
  main(sys.argv)
