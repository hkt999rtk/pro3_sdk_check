import re
import sys
import struct
import os
import math

HEADER_VALUE_1 = 0x35393138
HEADER_VALUE_2 = 0x31313738
ALIGNMENT = 32  # 32-byte alignment
PADDING_BYTE = b'\xFF'  # Padding byte
HEADER_SIZE = 32  # Header size including padding

def eval_origin(expr: str) -> int:
    expr = expr.strip()
    expr = expr.strip('()')
    parts = re.split(r'\+', expr)
    total = 0
    for part in parts:
        part = part.strip()
        if not part:
            continue
        total += int(part, 0)
    return total

def parse_linker_script(script_path, memory_name):
    in_memory_section = False
    origin_address = None

    with open(script_path, 'r') as file:
        for line in file:
            line = line.strip()
            
            if line.startswith('MEMORY'):
                in_memory_section = True
                continue
            
            if in_memory_section:
                if line.startswith('}'):
                    break
                ## Match memory region name (may include spaces and attributes)
                #match = re.match(r'(\w+(?:\s+\w+)*)\s*(?:\(.*?\))?\s*:', line)
                #
                #if match:
                #    region_name = match.group(1).split()[0].strip()
                #else:
                #    region_name = none
                #if region_name == memory_name:
                #    # Look for ORIGIN definition
                #    origin_match = re.search(r'ORIGIN\s*=\s*(\w+)', line)
                #    if origin_match:
                #        print(origin_match.group(1))
                #        origin_address = int(origin_match.group(1), 16)
                #        break
                
                line = line.strip()
                if not line or ':' not in line:
                    continue

                left = line.split(':', 1)[0]
                region_name = left.split()[0]
                
                right = line.split(':', 1)[1]
                       
                if region_name == memory_name and 'ORIGIN' in right:
                    # Look for ORIGIN definition
                    part_after_origin = right.split('ORIGIN', 1)[1]
                    part_after_origin = part_after_origin.split('=', 1)[1]
                    origin_expr = part_after_origin.split(',', 1)[0].strip()
                    
                    if origin_expr:
                        origin_address = eval_origin(origin_expr)
                        #print(hex(origin_address))
                        break

    return origin_address

def align_and_pad_file(file_content):
    original_size = len(file_content)
    padded_size = math.ceil(original_size / ALIGNMENT) * ALIGNMENT
    padding_size = padded_size - original_size
    padded_content = file_content + PADDING_BYTE * padding_size
    return padded_content, padded_size

def add_custom_header(binary_path, output_path, origin_address, image_index):
    try:
        with open(binary_path, 'rb') as binary_file:
            binary_content = binary_file.read()
        
        # Align and pad file content
        padded_content, padded_size = align_and_pad_file(binary_content)
        
        # Adjust origin address
        adjusted_origin = origin_address + HEADER_SIZE
        
        # Prepare custom header
        header = struct.pack('<IIIII', HEADER_VALUE_1, HEADER_VALUE_2, padded_size, adjusted_origin, image_index)
        header_padding = PADDING_BYTE * (HEADER_SIZE - len(header))
        
        with open(output_path, 'wb') as output_file:
            output_file.write(header)
            output_file.write(header_padding)
            output_file.write(padded_content)
        
        print(f"Successfully wrote custom header to {output_path}")
        print(f"Header content:")
        print(f"- Value 1: 0x{HEADER_VALUE_1:08X}")
        print(f"- Value 2: 0x{HEADER_VALUE_2:08X}")
        print(f"- Original file size: {len(binary_content)} bytes")
        print(f"- Padded file size: {padded_size} bytes")
        print(f"- Original Origin address: 0x{origin_address:08X}")
        print(f"- Adjusted Origin address: 0x{adjusted_origin:08X}")
        print(f"- Total file size: {HEADER_SIZE + padded_size} bytes")
    except IOError as e:
        print(f"Error: Unable to read or write file - {e}")
        sys.exit(1)

def main():
    if len(sys.argv) != 6:
        print("Usage: python script.py <linker_script_path> <memory_name> <binary_file_path> <output_file_path> <HDR:image_index>")
        sys.exit(1)
    
    script_path = sys.argv[1]
    memory_name = sys.argv[2]
    binary_path = sys.argv[3]
    output_path = sys.argv[4]
    image_index = int(sys.argv[5], 0)
    
    origin_address = parse_linker_script(script_path, memory_name)
    
    if origin_address is not None:
        print(f"Original Origin address for {memory_name}: 0x{origin_address:08X}")
        add_custom_header(binary_path, output_path, origin_address, image_index)
    else:
        print(f"Memory block '{memory_name}' or its origin address not found in the linker script")

if __name__ == "__main__":
    main()
