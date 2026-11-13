
import pandas as pd
import sys
import math 
import re

def doxgen_driver_header(ip_name):
    header = '''
/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup ##ip_name##
* @brief ##ip_name## driver modules
* @{
*/    
'''
    return header.replace('##ip_name##', ip_name)
def doxgen_drver_header_end():
    return '/** @} */'+'\n\n'+'/** @} */'
def doxygen_header(reg_name):
    header = '''
/** @defgroup ##reg_name##
 * @brief
 * @{
 */
'''
    
    return header.replace('##reg_name##', reg_name)
def doxygen_reg_type(ip_name):
    type_header = '''
/** @defgroup ##ip_name##_Exported_Types ##ip_name## Exported Types
* @{
*/
/**
* @brief ##ip_name## Register Declaration
*/
'''
    return type_header.replace('##ip_name##', ip_name)
def doxygen_struct_access_type(ip_name):
    access_header = '''
/** @defgroup ##ip_name##_Exported_Struct_Access_Types ##ip_name## Exported Struct Access Types
* @{
*/
/**
* @brief ##ip_name## Struct Access Register Declaration
*/
'''
    return access_header.replace('##ip_name##', ip_name)
def doxygen_header_end():
    return '/** @} */'
def doxygen_comment(rw, init ,comment, offset):
    #return '/*!< {:5} {:2} {} */'.format(rw, str(int(init, 16)), comment).replace("\n", "")
    #return '/*!< {:5} {:3} {} */'.format(rw, init, comment).replace("\n", "")
    if('\n' in comment):
        new_comment = '/*!< {:5} {:3}\n'.format(rw, init)
        split_comment = comment.split('\n')
        for c in split_comment:
            #print(c)
            c = "* " + c
            x = '{{:{}}} {{}}\n'.format(offset).format('', c)
            new_comment += x
        new_comment += '{{:{}}} */'.format(offset).format('')
        return new_comment
    else:    
        return '/*!< {:5} {:3} {} */'.format(rw, init, comment)
def bit_count(s):
    # Remove the brackets
    stripped = s.strip().strip('[]')
    # Check if the format contains a colon
    if ':' in stripped:
        a_str, b_str = stripped.split(':')
        a = int(a_str.strip())
        b = int(b_str.strip())
        return a - b + 1
    else:
        # If there is no colon, return 1 as the result
        return 1
        
def bit_offset(s):
    # Remove the brackets
    stripped = s.strip().strip('[]')
    
    # Check if the format contains a colon
    if ':' in stripped:
        a_str, b_str = stripped.split(':')
        return b_str
    else:
        # If there is no colon, return 1 as the result
        return stripped       
def bit_mask(msb, lsb):
    # Remove the brackets and split the string based on ':'
   
    num_bits = int(msb-lsb+1)
    b = int(lsb)
    
    # Create the bitmask (all 1s for the number of bits)
    mask = (1 << num_bits) - 1
    
    # Return the formatted string with the mask and the shift
    return f"((u32)0x{mask:08X} << {b})"
def bit_gen_value(msb, lsb):
    num_bits = int(msb-lsb+1)
    b = int(lsb)
    
    # Create the bitmask (all 1s for the number of bits)
    mask = (1 << num_bits) - 1
    
    # Return the formatted string with the mask and the shift
    return f"((u32)(((x) & 0x{mask:08X}) << {b}))"    
def bit_get_value(msb, lsb):
    num_bits = int(msb-lsb+1)
    b = int(lsb)
    
    # Create the bitmask (all 1s for the number of bits)
    mask = (1 << num_bits) - 1
    
    # Return the formatted string with the mask and the shift
    return f"((u32)(((x >> {b}) & 0x{mask:08X})))"
rsvd_cnt = -1
def reset_rsvd_cnt():
    global rsvd_cnt
    rsvd_cnt = -1
def get_symbol(s):
    global rsvd_cnt
    if '--' in s:
        rsvd_cnt = rsvd_cnt + 1
        symbol = f'rsvd{rsvd_cnt}'
    elif 'rsvd' in s.lower():
        rsvd_cnt = rsvd_cnt + 1
        symbol = f'rsvd{rsvd_cnt}'
    else:
        symbol = s.replace(' ','_').replace('[', '_').replace(']', '').replace(':', '_')
        
    symbol_striped = symbol.lower()
    symbol_striped = symbol_striped.replace('bit_', '')
    return symbol_striped
def get_regname(s, ip_name, sub_ip_name):
    #print(s)
    reg_name = s.lower().replace("reg_","")
    
    if(ip_name != ""):
        reg_name = reg_name.replace(ip_name.lower() + "_", "")
    
    if( sub_ip_name != ""):
        reg_name = reg_name.replace(sub_ip_name.lower() + "_", "")
    
    return reg_name
def get_access(s):
    
    access = 0
    if( 'READ' in str(s)):
        access |= 0x1
    if( 'WRITE' in str(s)):
        access |= 0x2
    
    
    #print("{} {}".format(s, access))
    return access
    
def get_access_name(v):
    if(v == 0x3):
        return '__IO'
    elif(v == 0x2):
        return '__O'
    elif(v == 0x1):
        return '__I'
    else:
        return '__IO'
              
    
def license_header(year):
    license ='''/*
 * Copyright (c) ##year## Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
'''
    
    return license.replace('##year##', str(year))
def section_reg_mark(ip):
    return '/*=========={} Register Address Definition==========*/'.format(ip)
def get_row_index_by_item(df, column_name, value):
    # Find the index where the value in the specified column matches
    row_index = df[df[column_name] == value].index
    if not row_index.empty:
        return row_index[0]  # Return the first match
    else:
        return None  # Return None if the value is not found
def tab2space(content):
    return content.replace("\t", "    ");  
    

def not_rsvd(s):
    if 'RSVD' in s.upper():
        return False
    
    if 'RESERVE' in s.upper():
        return False

    # some RD typo
    if 'REVERSE' in s.upper():
        return False
 
    if 'RESV' in s.upper():
        return False
        
    return True
 
# Function to convert Excel data to C array and write to header file
def excel_to_c_header(xlsx_file, output_file, skip_rows=0, skip_columns=0, sheet_name=0 ,array_name="excel_data" ):
    
    
    if(sheet_name == '0'):
        sheet_name = 0
    
    # Read the Excel file into a DataFrame, skipping the specified number of rows
    df = pd.read_excel(xlsx_file, sheet_name=sheet_name, skiprows=0)
    
    #print(pd.ExcelFile(xlsx_file).sheet_names )
    
    # first start
    first_start = True
    
    #full_ip_name = "RXI_WIFI"
    full_ip_name = pd.ExcelFile(xlsx_file).sheet_names[0]
    print(full_ip_name)
    # convert 'Bit' 'Offset' 'Symbol' 'Description' to C header
    # trick : xlsx of this version, Bit filed content register name and bit field 
    #         when Bit field is register name, in this time, Offset is valid, so we check "[" this symobl to identify new register field start or not
    # Open the output header file
    with open(output_file, 'w') as f:
        # Write header guards
        header_guard = output_file.replace('.', '_').upper()
        f.write(license_header(2024) + "\n")
        f.write(f"#ifndef {header_guard}\n")
        f.write(f"#define {header_guard}\n\n")
        
         # auto generation start
        f.write(f"/* AUTO_GEN_START */\n\n")
        
        
        f.write(doxgen_driver_header(full_ip_name)+"\n")     
        
        reg_name = ""
        bit_array = []
        last_offset = 0
        didx = 0
        
        # Generate BIT mask contorl macro
        reset_rsvd_cnt()
        reg_offset = ""
        symb_list = []
        ip_base = 0
        
        for i, row in df.iterrows():
        
            #print(row)
            #print(row['MSB'])
            #print(row['LSB'])

            bits = row['MSB'] - row['LSB']
            
            if(str(row['Field']) != 'nan'):
                symb = row['Field'].upper()
                
                if(str(row['Addr']) != 'nan'):
                    #print(row['Offset'])                
                    if(first_start):
                        first_start = False
                        ip_base = int(row['Addr'].strip('h'), 16)
                    else:
                        f.write( doxygen_header_end() + "\n\n" )
                        
                    reg_offset = int(row['Addr'].strip('h'), 16) - ip_base    
                        
                    f.write( doxygen_header(row['Name']) )  
                
                # skip RSVD symbol to save header size
                if( not_rsvd(symb) ):
                    
                    #if symb not in symb_list:                    
                    #    symb_list.append(symb)
                    #else:
                    #    symb = symb + f'_{reg_offset:03X}'
                    
                    if(bits >= 2):
                        mask = bit_mask(row['MSB'], row['LSB'])
                        gen_v = bit_gen_value(row['MSB'], row['LSB'])
                        get_v = bit_get_value(row['MSB'], row['LSB'])
                                              
                        comment = doxygen_comment(row['Type'], row['Default'], row['description'], 10+43+40)
                        f.write('#define {:43s} {:40s} {}\n'.format(f'{full_ip_name}_MASK_{symb}', mask, comment))
                        f.write('#define {:43s} {:40s}\n'.format(f'{full_ip_name}_{symb}(x)', gen_v))
                        f.write('#define {:43s} {:40s}\n'.format(f'{full_ip_name}_GET_{symb}(x)', get_v))
                    else:
                        mask = bit_mask(row['MSB'], row['LSB'])
                        #symb = get_symbol(row['Symbol']).upper()
                        comment = doxygen_comment(row['Type'], row['Default'], row['description'], 10+43+40)
                        f.write('#define {:43s} {:40s} {}\n'.format(f'{full_ip_name}_BIT_{symb}', mask, comment))

                    
        f.write( doxygen_header_end() + "\n" )
        f.write("\n\n\n")
        
            
        # Generate REGISTER MAP TYPE
        last_offset = 0
        didx = 0
        f.write( doxygen_reg_type(full_ip_name) + "\n")
        f.write("typedef struct {" + "\n")
        
        reg_info = []   # format "{regname, offset, access}"
        reg_name = ""
        reg_offset = 0
        reg_access = 0
        
        for i, row in df.iterrows():
            if(str(row['Addr']) != 'nan'):
                reg_info.append([reg_name, reg_offset, reg_access])
                reg_name = get_regname(row['Name'], "", "").upper()
                reg_offset = int(row['Addr'].strip('h'), 16)-ip_base
                reg_access = get_access(row['Type'])
               
            else:
                reg_access |= get_access(row['Type'])
                
        reg_info.append([reg_name, reg_offset, reg_access])
        #print(reg_info)
        
        reg_list = []
        #print(reg_info)
        for info in reg_info:
            access = get_access_name(info[2])
            name = info[0]
            offset = info[1]
            
            if(name != ""):
                if(offset != 0 and last_offset + 4 != offset):
                    print(f"insert dummy last {last_offset} curr{offset}" )
                    dummy_cnt = int((offset - last_offset - 4)/4)
                    print(f"dummy count {dummy_cnt}")
                    f.write(tab2space(f"\tuint32_t RSVD_NO_FUNCTION{didx}[{dummy_cnt}];\n"))
                    didx = didx + 1   
                last_offset = offset
                
                while(name in reg_list):
                    split = re.findall(r'[A-Z_a-z]+|\d+', name)
                    if(split[-1].isnumeric()):
                        split[-1] = str(int(split[-1])+1)
                        name = ''.join(split)
                    else:
                        name = name+"1"
                        
                f.write("{:64}{}".format(tab2space(f'\t{access:{4}} uint32_t {name};'), f'/*!< Register,  Address offset:0x{offset:04X} */\n' ) )
                reg_list.append(name)
                

        f.write("} "+ f"{full_ip_name}_TypeDef;\n\n")

        if(ip_base != "0x0"):
            f.write(f"#define {full_ip_name}_BASE (({full_ip_name}_TypeDef *) (0x{ip_base:08X}))\n")   
        
        f.write( doxygen_header_end() + "\n" )
        
        f.write("\n\n/* AUTO_GEN_END */\n\n")
        f.write("/* MANUAL_GEN_START */\n\n")
        f.write("//Please add your defination here\n\n\n")
        f.write("/* MANUAL_GEN_END */\n\n")
        
        f.write(doxgen_drver_header_end() + "\n")
        
        # End the header guard
        f.write(f"#endif  // {header_guard}\n")
# Main function to handle command-line arguments
if __name__ == "__main__":
    if len(sys.argv) < 3 or len(sys.argv) > 6:
        print("Usage: python convert_excel_to_c.py <input_xlsx_file> <output_header_file> [<skip_rows>] [<skip_columns>] [<sheet_name>]")
        print('skip_rows: ignore first n rows, normally set the number to the line before " Offset, Bit, Access, INI", in my case , this is 22')
        print('skip_columns: ignore first n column, normally set the number to the column of Offset - 1, in my case, this is 1' )
        sys.exit(1)
    # Get the command-line arguments
    excel_file = sys.argv[1]  # First argument is the Excel file
    output_header = sys.argv[2]  # Second argument is the output C header file
    skip_rows = int(sys.argv[3]) if len(sys.argv) >= 4 else 0  # Optional third argument is the number of rows to skip, default to 0
    skip_columns = int(sys.argv[4]) if len(sys.argv) >= 5 else 0  # Optional fourth argument is the number of columns to skip, default to 0
    sheet_name = sys.argv[5] if len(sys.argv) == 6 else 0  # Optional fifth argument is the sheet name, default to first sheet
    # Convert the Excel file to C header
    excel_to_c_header(excel_file, output_header, skip_rows, skip_columns, sheet_name)
    sheet_name_display = sheet_name if sheet_name else "first sheet"
    print(f"Converted {excel_file}, sheet '{sheet_name_display}' to {output_header} successfully. Skipped {skip_rows} row(s) and {skip_columns} column(s).")
