import sys
import pandas as pd
import numpy as np


fmt_str="\t{:<24}={:>4}, {}"

def main(argv):

    df=pd.read_excel(argv[0])
    if(df is None):
        return
        
    nmp=df.values
    out_file = argv[1]
    
    with open(out_file, 'w') as f:

        print("#ifndef __{}__".format(out_file.replace('.','_').upper()), file=f)
        print("#define __{}__".format(out_file.replace('.','_').upper()), file=f)
        print("\n/* This file is AUTO GENERATED */\n", file=f)
        ############### NP

        print("/** @defgroup NPIRQn_enum\n  * @{\n  */\n", file=f)
        print("#if defined (CONFIG_ARM_CORE_CM4_NP)", file=f)
        print("enum NPIRQn {", file=f)
        print("\t/******  Processor Exceptions Numbers ********/", file=f)
        print(fmt_str.format("NonMaskableInt_IRQn", -14, "/*!< 2 Non Maskable Interrupt */"), file=f)
        print(fmt_str.format("HardFault_IRQn", -13, "/*!< 3 Hard Fault, all classes of Fault */"), file=f)
        print(fmt_str.format("MemoryManagement_IRQn", -12, "/*!< 4 Cortex-M3 Memory Management Interrupt */"), file=f)
        print(fmt_str.format("BusFault_IRQn", -11, "/*!< 5 Cortex-M3 Bus Fault Interrupt */"), file=f)
        print(fmt_str.format("UsageFault_IRQn", -10, "/*!< 6 Cortex-M3 Usage Fault Interrupt */"), file=f)
        print(fmt_str.format("SVCall_IRQn", -5, "/*!< 11 Cortex-M3 SV Call Interrupt  */"), file=f)
        print(fmt_str.format("DebugMonitor_IRQn", -4, "/*!< 12 Cortex-M3 Debug Monitor Interrupt */"), file=f)
        print(fmt_str.format("PendSV_IRQn", -2, "/*!< 14 Cortex-M3 Pend SV Interrupt */"), file=f)
        print(fmt_str.format("SysTick_IRQn", -1, "/*!< 15 Cortex-M3 System Tick Interrupt */"), file=f)
        print("\n\t/******  Specific Interrupt Numbers ************/", file=f)
        for item in nmp:
            if(not np.isnan(item[2])):
                print(fmt_str.format( (item[1].replace(' ','_')+"_IRQ").upper(), int(item[2]), "" ), file=f)
                
        print("};\n", file=f)
        print("#endif\n", file=f)
        print("/**\n  * @}\n  */\n", file=f)
        
        ################ VP
        
        print("/** @defgroup VPIRQn_enum\n  * @{\n  */\n", file=f)
        print("#if defined (CONFIG_ARM_CORE_CM4_VP)", file=f)
        print("enum VPIRQn {", file=f)
        print("\t/******  Processor Exceptions Numbers ********/", file=f)
        print(fmt_str.format("NonMaskableInt_IRQn", -14, "/*!< 2 Non Maskable Interrupt */"), file=f)
        print(fmt_str.format("HardFault_IRQn", -13, "/*!< 3 Hard Fault, all classes of Fault */"), file=f)
        print(fmt_str.format("MemoryManagement_IRQn", -12, "/*!< 4 Cortex-M3 Memory Management Interrupt */"), file=f)
        print(fmt_str.format("BusFault_IRQn", -11, "/*!< 5 Cortex-M3 Bus Fault Interrupt */"), file=f)
        print(fmt_str.format("UsageFault_IRQn", -10, "/*!< 6 Cortex-M3 Usage Fault Interrupt */"), file=f)
        print(fmt_str.format("SVCall_IRQn", -5, "/*!< 11 Cortex-M3 SV Call Interrupt  */"), file=f)
        print(fmt_str.format("DebugMonitor_IRQn", -4, "/*!< 12 Cortex-M3 Debug Monitor Interrupt */"), file=f)
        print(fmt_str.format("PendSV_IRQn", -2, "/*!< 14 Cortex-M3 Pend SV Interrupt */"), file=f)
        print(fmt_str.format("SysTick_IRQn", -1, "/*!< 15 Cortex-M3 System Tick Interrupt */"), file=f)
        print("\n\t/******  Specific Interrupt Numbers ************/", file=f)
        for item in nmp:
            if(not np.isnan(item[3])):
                print(fmt_str.format( (item[1].replace(' ','_')+"_IRQ").upper(), int(item[3]), "" ), file=f)
                
        print("};\n", file=f)
        print("#endif\n", file=f)
        print("/**\n  * @}\n  */\n", file=f)
        
        ################### FP
        
        print("/** @defgroup FPIRQn_enum\n  * @{\n  */\n", file=f)
        print("#if defined (CONFIG_ARM_CORE_CM0)", file=f)
        print("enum FPIRQn {", file=f)
        print("\t/******  Processor Exceptions Numbers ********/", file=f)
        print(fmt_str.format("NonMaskableInt_IRQn", -14, "/*!< 2 Non Maskable Interrupt */"), file=f)
        print(fmt_str.format("HardFault_IRQn", -13, "/*!< 3 Hard Fault, all classes of Fault */"), file=f)
        print(fmt_str.format("MemoryManagement_IRQn", -12, "/*!< 4 Cortex-M3 Memory Management Interrupt */"), file=f)
        print(fmt_str.format("BusFault_IRQn", -11, "/*!< 5 Cortex-M3 Bus Fault Interrupt */"), file=f)
        print(fmt_str.format("UsageFault_IRQn", -10, "/*!< 6 Cortex-M3 Usage Fault Interrupt */"), file=f)
        print(fmt_str.format("SVCall_IRQn", -5, "/*!< 11 Cortex-M3 SV Call Interrupt  */"), file=f)
        print(fmt_str.format("DebugMonitor_IRQn", -4, "/*!< 12 Cortex-M3 Debug Monitor Interrupt */"), file=f)
        print(fmt_str.format("PendSV_IRQn", -2, "/*!< 14 Cortex-M3 Pend SV Interrupt */"), file=f)
        print(fmt_str.format("SysTick_IRQn", -1, "/*!< 15 Cortex-M3 System Tick Interrupt */"), file=f)
        print("\n\t/******  Specific Interrupt Numbers ************/", file=f)
        for item in nmp:
            if(not np.isnan(item[4])):
                print(fmt_str.format( (item[1].replace(' ','_')+"_IRQ").upper(), int(item[4]), "" ), file=f)
                
        print("};\n", file=f)
        print("#endif\n", file=f)
        print("/**\n  * @}\n  */\n", file=f)    
        
        
        ##################### AP
        
        print("/** @defgroup APIRQn_enum\n  * @{\n  */\n", file=f)
        print("#if defined (CONFIG_ARM_CORE_CA32)", file=f)
        print("enum APIRQn {", file=f)
        print("\t/******  Processor Exceptions Numbers ********/", file=f)
        print(fmt_str.format("SGI0_Edge",  0, "/* software generated interrupt 0 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI1_Edge",  1, "/* software generated interrupt 2 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI2_Edge",  2, "/* software generated interrupt 3 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI3_Edge",  3, "/* software generated interrupt 4 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI4_Edge",  4, "/* software generated interrupt 5 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI5_Edge",  5, "/* software generated interrupt 6 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI6_Edge",  6, "/* software generated interrupt 7 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI7_Edge",  7, "/* software generated interrupt 8 (recommended non-secure) */"), file=f)
        print(fmt_str.format("SGI8_Edge",  8, "/* software generated interrupt 9 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI9_Edge",  9, "/* software generated interrupt 4 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI10_Edge", 10, "/* software generated interrupt 5 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI11_Edge", 11, "/* software generated interrupt 6 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI12_Edge", 12, "/* software generated interrupt 7 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI13_Edge", 13, "/* software generated interrupt 8 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI14_Edge", 14, "/* software generated interrupt 9 (recommended secure) */"), file=f)
        print(fmt_str.format("SGI15_Edge", 15, "/* software generated interrupt 9 (recommended secure) */"), file=f)
        print("\t", file=f)
        print(fmt_str.format("PPI6_IRQ", 25, "/* Virtual maintenance interrupt. */"), file=f)
        print(fmt_str.format("PPI5_IRQ", 26, "/* Hypervisor timer event. */"), file=f)
        print(fmt_str.format("PPI4_IRQ", 27, "/* Virtual timer event. */"), file=f)
        print(fmt_str.format("PPI0_IRQ", 28, "/* Legacy nFIQ signal. Not used. */"), file=f)
        print(fmt_str.format("PPI1_IRQ", 29, "/* Secure physical timer event. */"), file=f)
        print(fmt_str.format("PPI2_IRQ", 30, "/* Non-secure physical timer event. */"), file=f)
        print(fmt_str.format("PPI3_IRQ", 31, "/* Legacy nIRQ signal. Not used. */"), file=f)
        print("\n\t/******  Specific Interrupt Numbers ************/", file=f)
        for item in nmp:
            if(not np.isnan(item[5])):
                print(fmt_str.format( (item[1].replace(' ','_')+"_IRQ").upper(), int(item[5])+32, "" ), file=f)
                
        print("};\n", file=f)
        print("#endif\n", file=f)
        print("/**\n  * @}\n  */\n", file=f)        
        
        #############################
        
        print("#endif //__{}__".format(out_file.replace('.','_').upper()), file=f)
        
if __name__ == "__main__":
   main(sys.argv[1:])        