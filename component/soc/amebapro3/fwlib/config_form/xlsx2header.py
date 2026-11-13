
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
    #print(init)
    try:
        init_str = f'{int(f"{int(init)}",16):x}'
    except:
        try:
            init_str = f'{int(f"{int(init, 16)}",16):x}'
        except:
            init_str = init
            
    if( str(comment) == 'nan'):
        comment = "None"
    #print(f"{val:x}")
    #print(init_str)
    #return '/*!< {:5} {:2} {} */'.format(rw, str(int(init, 16)), comment).replace("\n", "")
    return '/*!< {:5} {:3} {} */'.format(rw, init_str, comment).replace("\n", " ")
    
def doxygen_comment_blk(rw, init ,comment, offset):
    #return '/*!< {:5} {:2} {} */'.format(rw, str(int(init, 16)), comment).replace("\n", "")
    #return '/*!< {:5} {:3} {} */'.format(rw, init, comment).replace("\n", "")

    try:
        init_str = f'{int(f"{int(init)}",16):x}'
    except:
        try:
            init_str = f'{int(f"{int(init, 16)}",16):x}'
        except:
            init_str = init
    
    if( str(comment) == 'nan'):
        comment = "None"
    
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
def bit_mask(s):
    # Remove the brackets and split the string based on ':'
    stripped = s.strip().strip('[]')
    
    if ':' in stripped:
        a_str, b_str = stripped.split(':')
        # Convert the values to integers
        a = int(a_str.strip())
        b = int(b_str.strip())
        # Calculate the number of bits (a - b)
        num_bits = a - b + 1       
    else:
        num_bits = 1
        b = int(stripped.strip())
    
    # Create the bitmask (all 1s for the number of bits)
    mask = (1 << num_bits) - 1
    
    # Return the formatted string with the mask and the shift
    return f"((u32)0x{mask:08X} << {b})"
def bit_gen_value(s):
    # Remove the brackets and split the string based on ':'
    stripped = s.strip().strip('[]')
    a_str, b_str = stripped.split(':')
    
    # Convert the values to integers
    a = int(a_str.strip())
    b = int(b_str.strip())
    
    # Calculate the number of bits (a - b)
    num_bits = a - b + 1
    
    # Create the bitmask (all 1s for the number of bits)
    mask = (1 << num_bits) - 1
    
    # Return the formatted string with the mask and the shift
    return f"((u32)(((x) & 0x{mask:08X}) << {b}))"    
def bit_get_value(s):
    # Remove the brackets and split the string based on ':'
    stripped = s.strip().strip('[]')
    a_str, b_str = stripped.split(':')
    
    # Convert the values to integers
    a = int(a_str.strip())
    b = int(b_str.strip())
    
    # Calculate the number of bits (a - b)
    num_bits = a - b + 1
    
    # Create the bitmask (all 1s for the number of bits)
    mask = (1 << num_bits) - 1
    
    # Return the formatted string with the mask and the shift
    return f"((u32)(((x >> {b}) & 0x{mask:08X})))"
rsvd_cnt = -1
def reset_rsvd_cnt():
    global rsvd_cnt
    rsvd_cnt = -1
def get_symbol(s, strip_ip_name=None):
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
    
    #print(symbol_striped)
    if strip_ip_name != None and strip_ip_name != "":
        ip_name = strip_ip_name.lower()
        symbol_striped = symbol_striped.replace(f'{ip_name}_', '')
    #print(symbol_striped)
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
    if( 'R' in str(s)):
        access |= 0x1
    if( 'W' in str(s)):
        access |= 0x2
    
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
# Function to convert Excel data to C array and write to header file

# header_format : bit 0 block comment,                       0: disable, 1: enable
#                 bit 1 regname with prepend IP name,        0: disable, 1: enable
#                 bit 2 strip register field ip name ,       0: disable, 1: enable

def excel_to_c_header(xlsx_file, output_file, skip_rows=0, skip_columns=0, sheet_name=0 , header_format=1, array_name="excel_data" ):

    header_format_i = int(header_format)
    
    hfmt_blkcmt = int(header_format_i)&0x1
    hfmt_regip = int(header_format_i)&0x2
    hfmt_strip = int(header_format_i)&0x4
    
    print(f"block comment : {hfmt_blkcmt}, reg name with ip {hfmt_regip}, reg feild strip ip name {hfmt_strip}")
    
    # Parser header to get IP name and Base Address
    df_head = pd.read_excel(xlsx_file, sheet_name=sheet_name, nrows=30)
 
    try:
        format = 0  # no format
        if( str(df_head.iloc[0, 1])=='Property' ):
            format = 2 # WLAFE
            skip_row = 1

        if( str(df_head.iloc[1, 1]) == 'Peripherals'):
            format = 1   # default format, 1st sheet
            skip_row = 2
        
        if( str(df_head.iloc[0, 1]) == 'Peripherals'):
            format = 1   # default format, not 1st sheet
            skip_row = 1
        
        if( format == 1):
            if( str(df_head.iloc[1, 1]) == 'Peripherals'):
                skip_row = 2
            else:
                skip_row = 1
         
            df_ip = pd.read_excel(xlsx_file, sheet_name=sheet_name, nrows=skip_rows, skiprows=skip_row)
        
            name_idx = get_row_index_by_item(df_ip, 'Peripherals', 'name')
            base_idx = get_row_index_by_item(df_ip, 'Peripherals', 'baseAddress')
            
            print(df_ip.loc[name_idx, 'Content'])
            print(df_ip.loc[base_idx, 'Content'])
            
            ip_name = str(df_ip.loc[name_idx, 'Content']).upper()
            ip_base = str(df_ip.loc[base_idx, 'Content'])
            
            ip_base = ip_base.replace("_", "")
        elif ( format == 2 ):
            df_ip = pd.read_excel(xlsx_file, sheet_name=sheet_name, nrows=skip_rows, skiprows=skip_row)
            
            #print(df_ip)
        
            name_idx = get_row_index_by_item(df_ip, 'Property', 'block')
            base_idx = get_row_index_by_item(df_ip, 'Property', 'base')
            
            #print(name_idx)
            #print(base_idx)
            print(df_ip.loc[name_idx, 'Value'])
            print(df_ip.loc[base_idx, 'Value'])
            
            ip_name = str(df_ip.loc[name_idx, 'Value']).upper()
            ip_base = str(df_ip.loc[base_idx, 'Value'])
            
            ip_base = ip_base.replace("_", "")
        else:
            ip_base = "0x0"
            ip_name = ""           
    except:
        ip_base = "0x0"
        ip_name = ""
        

    #if( str(df_head.iloc[21, 1]) == 'Offset' ):
    #    data_skip_row = 22
    #else:
    #    data_skip_row = 23
    data_skip_row = 0
    for i in range(0, 25):
        for j in range(0, 5):
            #print("{},{}:{}".format(i,j,df_head.iloc[i, j]))
            if(df_head.iloc[i, j]=='Offset'):
                print("{},{}:{}".format(i,j,df_head.iloc[i, j]))
                data_skip_row = i+1
                skip_columns = j
 
   
    
    if(sheet_name != 0):
        full_ip_name = sheet_name.upper()
        sub_ip_name = sheet_name.upper()
    else:
        full_ip_name = ip_name
        sub_ip_name = ""
       
    # Read the Excel file into a DataFrame, skipping the specified number of rows
    df = pd.read_excel(xlsx_file, sheet_name=sheet_name, skiprows=data_skip_row)
    
    # If skip_columns is greater than 0, adjust the DataFrame to exclude the first n columns
    if skip_columns > 0:
        df = df.iloc[:, skip_columns:]
    #print(df)
    # first start
    first_start = True
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
        
        if( sub_ip_name != "" and ip_name!="" ):
            f.write(f"/* {sub_ip_name} is a part of {ip_name} */\n\n")
        
        f.write(doxgen_driver_header(full_ip_name)+"\n")
        
        
        
        reg_name = ""
        bit_array = []
        last_offset = 0
        didx = 0
        
        if False:
            # not enable, C union structure control type, Z2
            f.write(doxygen_struct_access_type(full_ip_name) + "\n")
            
            f.write("{:72} /*!< (@ {:10}) {:80}*/\n".format("typedef struct {", ip_base, f"{full_ip_name} Structure") )
            
            # Generate C structure contorl type
            for i, row in df.iterrows():
                if "[" not in str(row['Bit']):
                    if bit_array != []:
                        for bit_item in reversed(bit_array):
                            f.write(bit_item)
                        bit_array = []
                        f.write(tab2space("\t\t} ") + "{}\n".format(f"{reg_name}_b;") )
                        f.write(tab2space("\t} ;\n"))
                        
                    offset = int(row['Offset'].strip('h'), 16)
                    
                    if(offset != 0 and last_offset + 4 != offset):
                        print(f"insert dummy last {last_offset} curr{offset}" )
                        dummy_cnt = int((offset - last_offset - 4)/4)
                        print(f"dummy count {dummy_cnt}")
                        f.write(tab2space(f"\tuint32_t rsvd_no_function{didx}[{dummy_cnt}];\n"))
                        didx = didx + 1    
                            
                    last_offset = offset
                    
                    reg_name = get_regname(row['Bit'], "", "")
                    
                    #reg_name = row['Bit'].lower().replace("reg_","").replace(ip_name.lower() + "_", "")
                    #if( sub_ip_name != ""):
                    #    reg_name = reg_name.replace(sub_ip_name.lower() + "_", "")
                        
                    f.write("{:72}\n".format(tab2space("\tunion {")) )
                    f.write("{:72} /*!< (@ {:10}) {:80}*/\n".format(tab2space(f"\t\tuint32_t {reg_name};"), f'0x{offset:08X}', "") )
                    f.write("{:72}\n".format(tab2space(tab2space("\t\tstruct {"))) )
                else:
                    if(hfmt_strip != 0):
                        symb = get_symbol(row['Symbol'], full_ip_name)
                    else:
                        symb = get_symbol(row['Symbol'])
                    bits = bit_count(row['Bit'])
                    bit_array.append( "{:72} /*!< {:10} {:80}*/\n".format(tab2space(f"\t\t\tuint32_t {symb}:{bits};"), row['Bit'].replace(":", "..") ,  str(row['Description']).replace("\n", " ")) )
            # Final touch
            if bit_array != []:
                for bit_item in reversed(bit_array):
                    f.write(bit_item)
                bit_array = []
                f.write(tab2space("\t\t} ") + "{}\n".format(f"{reg_name}_b;") )
                f.write(tab2space("\t} ;\n"))
            f.write("} " + "{};\n".format(f"{full_ip_name}_Type") )
            
            # add control base and name
            if(ip_base != "0x0"):
                f.write(f"#define {full_ip_name}_DEV (({full_ip_name}_Type *) ({ip_base}))\n")
            
            # end struct access type
            f.write( doxygen_header_end() + "\n\n" )
        
       
        
        # Generate BIT mask contorl macro
        reset_rsvd_cnt()
        reg_offset = ""
        symb_list = []
        
        for i, row in df.iterrows():
            if "[" in str(row['Bit']):
                bits = bit_count(row['Bit'])
                
                if(hfmt_strip != 0):
                    symb = get_symbol(row['Symbol'], full_ip_name).upper()
                else:
                    symb = get_symbol(row['Symbol']).upper()
                
                # skip RSVD symbol to save header size
                if( 'RSVD' not in symb ):
                    
                    if symb not in symb_list:                    
                        symb_list.append(symb)
                    else:
                        symb = symb + f'_{reg_offset:03X}'
                    
                    if(bits >= 2):
                        mask = bit_mask(row['Bit'])
                        gen_v = bit_gen_value(row['Bit'])
                        get_v = bit_get_value(row['Bit'])
                        
                        #comment = doxygen_comment_old(row['Access'], row['INI'], row['Description'])
                        if(hfmt_blkcmt != 0):
                            comment = doxygen_comment_blk(row['Access'], row['INI'], row['Description'], 10+43+40)
                        else:
                            comment = doxygen_comment(row['Access'], row['INI'], row['Description'], 10+43+40)
                        f.write('#define {:43s} {:40s} {}\n'.format(f'{full_ip_name}_MASK_{symb}', mask, comment))
                        f.write('#define {:43s} {:40s}\n'.format(f'{full_ip_name}_{symb}(x)', gen_v))
                        f.write('#define {:43s} {:40s}\n'.format(f'{full_ip_name}_GET_{symb}(x)', get_v))
                    else:
                        mask = bit_mask(row['Bit'])
                        #symb = get_symbol(row['Symbol']).upper()
                        #comment = doxygen_comment_old(row['Access'], row['INI'], row['Description'])
                        if(hfmt_blkcmt != 0):
                            comment = doxygen_comment_blk(row['Access'], row['INI'], row['Description'], 10+43+40)
                        else:
                            comment = doxygen_comment(row['Access'], row['INI'], row['Description'], 10+43+40)
                        f.write('#define {:43s} {:40s} {}\n'.format(f'{full_ip_name}_BIT_{symb}', mask, comment))
                
            else:
                if(str(row['Offset']) != 'nan'):
                    #print(row['Offset'])
                    reg_offset = int(row['Offset'].strip('h'), 16)
                    if(first_start):
                        first_start = False
                    else:
                        f.write( doxygen_header_end() + "\n\n" )
                    f.write( doxygen_header(row['Bit']) )    
        f.write( doxygen_header_end() + "\n" )
        f.write("\n\n\n")
        
        if False:
        # not enable, Generate REGISTER OFFSET, easily conflict with same name from other module
            f.write(section_reg_mark(full_ip_name) + "\n")
            for i, row in df.iterrows():
                if "[" not in str(row['Bit']):
                    offset = int(row['Offset'].strip('h'), 16)
                    f.write('#define {:43s} {:40}\n'.format(row['Bit'], f'0x{offset:04X}'))
            
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
            if(str(row['Offset']) != 'nan'):
                reg_info.append([reg_name, reg_offset, reg_access])
                reg_name = get_regname(row['Bit'], "", "").upper()
                reg_offset = int(row['Offset'].strip('h'), 16)
                reg_access = 0
            else:
                if "[" in str(row['Bit']):
                    reg_access |= get_access(row['Access'])
                
        reg_info.append([reg_name, reg_offset, reg_access])
        
        reg_list = []
        #print(reg_info)
        for info in reg_info:
            access = get_access_name(info[2])
            name = info[0].rstrip()
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
                    
                if(hfmt_regip != 0):        
                    f.write("{:64}{}".format(tab2space(f'\t{access:{4}} uint32_t {ip_name.upper()}_{name};'), f'/*!< Register,  Address offset:0x{offset:04X} */\n' ) )
                else:
                    f.write("{:64}{}".format(tab2space(f'\t{access:{4}} uint32_t {name};'), f'/*!< Register,  Address offset:0x{offset:04X} */\n' ) )
                reg_list.append(name)
                
        # for i, row in df.iterrows():
            # if "[" not in str(row['Bit']):
                # offset = int(row['Offset'].strip('h'), 16)
                # if(offset != 0 and last_offset + 4 != offset):
                    # print(f"insert dummy last {last_offset} curr{offset}" )
                    # dummy_cnt = int((offset - last_offset - 4)/4)
                    # print(f"dummy count {dummy_cnt}")
                    # f.write(tab2space(f"\tuint32_t RSVD_NO_FUNCTION{didx}[{dummy_cnt}];\n"))
                    # didx = didx + 1   
                # last_offset = offset
                
                # reg_name = get_regname(row['Bit'], "", "")
                # #reg_name = row['Bit'].lower().replace("reg_","").replace(ip_name.lower() + "_", "")
                # #if( sub_ip_name != ""):
                # #    reg_name = reg_name.replace(sub_ip_name.lower() + "_", "")
                # reg_name = reg_name.upper()
                # access = get_access_name(get_access(row['Access']))
                
                # f.write("{:64}{}".format(tab2space(f'\t{access} uint32_t {reg_name};'), f'/*!< Register,  Address offset:0x{offset:04X} */\n' ) )
                
        f.write("} "+ f"{full_ip_name}_TypeDef;\n\n")
        
        if(ip_base != "0x0"):
            f.write(f"#define {full_ip_name}_BASE (({full_ip_name}_TypeDef *) ({ip_base}))\n")
        
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
    if len(sys.argv) < 3 or len(sys.argv) > 7:
        print("Usage: python convert_excel_to_c.py <input_xlsx_file> <output_header_file> [<skip_rows>] [<skip_columns>] [<sheet_name>]")
        print('skip_rows: ignore first n rows, normally set the number to the line before " Offset, Bit, Access, INI", in my case , this is 22')
        print('skip_columns: ignore first n column, normally set the number to the column of Offset - 1, in my case, this is 1' )
        sys.exit(1)
    # Get the command-line arguments
    excel_file = sys.argv[1]  # First argument is the Excel file
    output_header = sys.argv[2]  # Second argument is the output C header file
    skip_rows = int(sys.argv[3]) if len(sys.argv) >= 4 else 0  # Optional third argument is the number of rows to skip, default to 0
    skip_columns = int(sys.argv[4]) if len(sys.argv) >= 5 else 0  # Optional fourth argument is the number of columns to skip, default to 0
    sheet_name = sys.argv[5] if len(sys.argv) >= 6 else 0  # Optional fifth argument is the sheet name, default to first sheet
    header_format = sys.argv[6] if len(sys.argv) == 7 else 1  # Optional fifth argument is the sheet name, default to first sheet
    # Convert the Excel file to C header
    excel_to_c_header(excel_file, output_header, skip_rows, skip_columns, sheet_name, header_format)
    sheet_name_display = sheet_name if sheet_name else "first sheet"
    print(f"Converted {excel_file}, sheet '{sheet_name_display}' to {output_header} successfully. Skipped {skip_rows} row(s) and {skip_columns} column(s).")
