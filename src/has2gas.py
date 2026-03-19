import re
import sys

def convert_line(line):
    # 1. Convert .dc.b to .byte (case-insensitive)
    line = re.sub(r'\.dc\.b', '.byte', line, flags=re.IGNORECASE)

    # 2. Convert string literals '...' to CP932 hex sequences
    def str_replacer(match):
        s = match.group(1)
        try:
            # Encode UTF-8 string to CP932 (SJIS) binary
            b_data = s.encode('cp932')
            # Format as a comma-separated hex string (e.g., 0x93,0xfa)
            return ','.join([f'0x{b:02x}' for b in b_data])
        except UnicodeEncodeError:
            # Error if the character is not supported in CP932
            print(f"Error: Cannot encode '{s}' to CP932", file=sys.stderr)
            sys.exit(1)

    line = re.sub(r"'(.*?)'", str_replacer, line)

    # 3. Convert $xx (Motorola hex) to 0xxx (GAS hex)
    line = re.sub(r'\$([0-9a-fA-F]+)', r'0x\1', line)

    # 4. Remove leading underscores from labels and .globl directives
    #    e.g., _cp932rsc_date -> cp932rsc_date
    line = re.sub(r'(^|\s|(?<=\.globl\s))_([a-zA-Z0-9_]+)', r'\1\2', line)

    # 5. Comment out .end directive (not required in GAS)
    line = re.sub(r'^\s*\.end', '# .end', line, flags=re.IGNORECASE)
    
    return line

if __name__ == "__main__":
    # Read from standard input (expects UTF-8)
    try:
        input_text = sys.stdin.read()
        
        output_lines = []
        for line in input_text.splitlines():
            output_lines.append(convert_line(line))
        
        # Output the final converted text
        sys.stdout.write('\n'.join(output_lines) + '\n')
    except Exception as e:
        sys.stderr.write(f"Runtime Error: {e}\n")
        sys.exit(1)