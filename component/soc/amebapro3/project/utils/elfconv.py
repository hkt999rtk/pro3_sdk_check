from elftools.elf.elffile import ELFFile
import os
import sys
import json
 
def objcopy_j_ram_dump(input_elf_path, section_names):
    """
    Extract the content of specified sections from an ELF file and create a single binary dump
    starting from the minimal starting address.
 
    :param input_elf_path: Path to the input ELF file.
    :param section_names: List of section names to extract.
    :return: Tuple containing the binary data and its size.
    """
    try:
        # Open the ELF file
        with open(input_elf_path, 'rb') as elf_file:
            elf = ELFFile(elf_file)
 
            # Collect sections and determine the minimal start address and maximum end address
            sections = []
            min_address = None
            max_address = 0
 
            for section_name in section_names:
                section = elf.get_section_by_name(section_name)
                if not section:
                    print(f"Ignore: Section '{section_name}' not found in the ELF file.")
                    continue
 
                sections.append(section)
                start = section['sh_addr']
                end = start + section['sh_size']
 
                if min_address is None or start < min_address:
                    min_address = start
                if end > max_address:
                    max_address = end
 
            if min_address is None:
                print("Error: No valid sections found.")
                return None, 0
 
            # Calculate the size
            dump_size = max_address - min_address
            
            # Create a byte array initialized with 0xFF
            ram_dump = bytearray([0xFF] * dump_size)
 
            # Copy section data into the appropriate positions in the RAM dump
            for section in sections:
                start = section['sh_addr'] - min_address
                size = section['sh_size']
                data = section.data()
                ram_dump[start:start+size] = data
 
            print(f"Generated RAM dump of size {dump_size} bytes.")
            return ram_dump, dump_size
 
    except Exception as e:
        print(f"Error: {e}")
        return None, 0
 
def find_symbol_address(input_elf_path, symbol_name):
    """
    Search for a symbol's address in the ELF file.
 
    :param input_elf_path: Path to the input ELF file.
    :param symbol_name: Name of the symbol to search for.
    """
    try:
        with open(input_elf_path, 'rb') as elf_file:
            elf = ELFFile(elf_file)
            symtab = elf.get_section_by_name('.symtab')
 
            if not symtab:
                print("Error: Symbol table not found in the ELF file.")
                return None
 
            for symbol in symtab.iter_symbols():
                if symbol.name == symbol_name:
                    address = symbol['st_value']
                    print(f"Symbol '{symbol_name}' found at address: 0x{address:08x}")
                    return address
 
            print(f"Symbol '{symbol_name}' not found in the ELF file.")
            return None
 
    except Exception as e:
        print(f"Error: {e}")
        return None
 
def process_json_input(mode, input_elf_path, json_file_path, output_file):
    """
    Process JSON input to extract sections and find a symbol, generate RAM dump, and prepend header.
    
    :param mode: for bootloader or for application
    :param input_elf_path: Path to the input ELF file.
    :param json_file_path: Path to the JSON file containing section names, symbol name, and alignment size.
    :param output_file: Path to the output binary file for the RAM dump.
    """
   
    try:
        if(json_file_path != None and input_elf_path != None):
            with open(json_file_path, 'r') as json_file:
                data = json.load(json_file)
     
            section_names = data.get("sections", [])
            symbol_name = data.get("entry")
            align_size = data.get("align_size", 1)
            boot_idx = int(data.get("boot_idx", "0x01010101"), 16)
     
            symbol_address = None
            if symbol_name:
                symbol_address = find_symbol_address(input_elf_path, symbol_name)
     
            # Generate the RAM dump binary
            ram_dump, output_size = objcopy_j_ram_dump(input_elf_path, section_names)
     
            if ram_dump is None:
                #print("Failed to generate RAM dump.")
                #return
                print("Insert empty RAM dump.")
                output_size = 0
                ram_dump = bytearray()
                symbol_address = 0x0c000000
        else:
            output_size = 0
            symbol_address = 0x0c000000
            symbol_name = "None"
            boot_idx = 0xffffffff
            ram_dump = bytearray()
            #print(ram_dump)
            align_size = 128

        # caculate aligned size
        aligned_size = ((32 + output_size + align_size - 1) // align_size) * align_size
        
        # header size 32, total subimage (with header) align to aligned_size
        aligned_padding_size = aligned_size - 32 - output_size;  
            
        # Prepare header information
        header_data = bytearray()
        if(mode == 'boot'):
            header_data += (0x96969999).to_bytes(4, byteorder='little')  # Prepend signature0
            header_data += (0xfc66cc3f).to_bytes(4, byteorder='little')  # Prepend signature1
        elif(mode == 'app'):
            header_data += (0x35393138).to_bytes(4, byteorder='little')  # Prepend signature0
            header_data += (0x31313738).to_bytes(4, byteorder='little')  # Prepend signature1
        else:
            print("Error : wrong mode, should be boot or app")
            sys.exit(1)
            
        header_data += (output_size+aligned_padding_size).to_bytes(4, byteorder='little')
 
        if symbol_address is not None:
            header_data += symbol_address.to_bytes(4, byteorder='little')
        else:
            header_data += (0xFFFFFFFF).to_bytes(4, byteorder='little')
 
        header_data += boot_idx.to_bytes(4, byteorder='little')  # Append boot_idx
 
        # Extend to 32 bytes with 0xFF padding
        header_data += bytearray([0xFF] * (32 - len(header_data)))
        
        # padding array
        padding = bytearray([0xFF] * aligned_padding_size)
 
        # Prepend header to RAM dump and write to output file
        with open(output_file, 'wb') as out_file:
            out_file.write(header_data + ram_dump + padding)
 
        print("\nHeader and Summary:")
        print(f"Output binary size: {output_size} bytes" if output_size else "No binary output generated.")
        print(f"Symbol '{symbol_name}' address: 0x{symbol_address:08x}" if symbol_address else f"Symbol '{symbol_name}' not found." if symbol_name else "No symbol searched.")
        print(f"Boot index: {boot_idx:08x}")
 
    except Exception as e:
        print(f"Error: {e}")
 
if __name__ == "__main__":
    import argparse
 
    parser = argparse.ArgumentParser(description="Process ELF file with sections and symbol info from JSON input")
    parser.add_argument("-m", "--mode", help="bootloader mode or application mode")
    parser.add_argument("-e", "--elf", help="Path to the ELF file")
    parser.add_argument("-j", "--json", help="Path to the JSON file containing section names, symbol name, and alignment size")
    parser.add_argument("-o", "--output", help="Path to the output binary file for the RAM dump")
 
    args = parser.parse_args()
 
    #print("{} {} {}".format(args.elf, args.json, args.output))
    
    if(args.mode == None):
        print("Error : should use -m boot or -m app to specify mode")
        sys.exit(1)
 
    process_json_input(args.mode, args.elf, args.json, args.output)