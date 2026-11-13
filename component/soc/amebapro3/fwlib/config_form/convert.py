import sys
import csv 
from pathlib import Path

def main(argv):

    with open(argv[0], newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        print("/* Auto generating by convert.py -- */")
        print("#ifndef _{}_H".format(Path(argv[0]).stem.upper()))
        print("#define _{}_H".format(Path(argv[0]).stem.upper()))

        print("/**************************************************************************//**")
        print("* @defgroup AmebaPro3_Address_Map {} Address Map".format(Path(argv[0]).stem.upper()))
        print("* @{")
        print("*****************************************************************************/")

        for row in reader:
            if(row['Port name'] != ''):
                port_name = row['Port name'].upper()
                if('MEM' not in port_name and 'REG' not in port_name):
                    port_name += '_REG'
                print('#define {:<24} {:<12} /*ID:{:<6} TYPE:{:<8} TOP_ADDR:{:<8} SIZE:{:<5} CLK_DOMAIN:{}*/'.format( port_name.upper().replace("_SLV","")+"_BASE", row['Base address'], row['ID'], row['Interface Type'], row['Top address'], row['Size'], row['Clk Domain'].upper()))


        print("/***************************************************************************************************************************/")
        print("/*************************************************** Secure control Base ***************************************************/")
        print("/***************************************************************************************************************************/")
        csvfile.seek(0)
        reader = csv.DictReader(csvfile)
        for row in reader:
            if(row['Port name'] != '' and row['Base address'] != '' and row['Security Attribute']!='NS'):
                secure_base = int(row['Base address'], 0)
                secure_top = int (row['Top address'], 0)
                if(secure_base & 0xf0000000 == 0x80000000 ):
                    secure_base |= (1<<29)
                    secure_top |= (1<<29)
                else:
                    secure_base |= (1<<28)
                    secure_top |= (1<<28) 

                port_name = row['Port name'].upper()
                if('MEM' not in port_name and 'REG' not in port_name):
                    port_name += '_REG'
                    
                print('#define {:<24} {:<12} /*ID:{:<6} TYPE:{:<8} TOP_ADDR:{:<8} SIZE:{:<5} CLK_DOMAIN:{}*/'.format( port_name.upper().replace("_SLV","")+"_BASE_S", hex(secure_base), row['ID'], row['Interface Type'], hex(secure_top), row['Size'], row['Clk Domain'].upper()))

        print("/** @} End of group AmebaPro3_Address_Map */")
        print("#endif /*_{}_H*/".format(Path(argv[0]).stem.upper()))


if __name__ == "__main__":
   main(sys.argv[1:])

