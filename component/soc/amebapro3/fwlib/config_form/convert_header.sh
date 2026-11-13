#!/bin/bash

python ./xlsx2header.py ./register_files/AmebaPro3_AIP_register_file_20240925.xlsx sysreg_bandgap.h 22 1 BANDGAP
python ./xlsx2header.py ./register_files/AmebaPro3_AIP_register_file_20240925.xlsx sysreg_regu.h 22 1 REGU
python ./xlsx2header.py ./register_files/AmebaPro3_AIP_register_file_20240925.xlsx sysreg_xtal.h 22 1 XTAL

python ./xlsx2header.py ./register_files/AmebaPro3_CA32_CPU_TOP_register_file_20240925.xlsx sysreg_ca32_vsfc.h 22 1 VSFC
python ./xlsx2header.py ./register_files/AmebaPro3_CA32_CPU_TOP_register_file_20240925.xlsx sysreg_ca32_ctrl.h 22 1 CA32_CTRL

python ./xlsx2header.py ./register_files/AmebaPro3_CPU_SUBSYS_register_file_20240710.xlsx sysreg_km4_mp.h 22 1 KM4_MP
python ./xlsx2header.py ./register_files/AmebaPro3_CPU_SUBSYS_register_file_20240710.xlsx sysreg_km4_np.h 22 1 KM4_NP
python ./xlsx2header.py ./register_files/AmebaPro3_CPU_SUBSYS_register_file_20240710.xlsx sysreg_km0.h 22 1 KM0
python ./xlsx2header.py ./register_files/AmebaPro3_CPU_SUBSYS_register_file_20240710.xlsx sysreg_cssys_aux.h 22 1 CSSYS_AUX

python ./xlsx2header.py ./register_files/AmebaPro3_PAD_register_file_20240923.xlsx sysreg_aon_pad.h 22 1 AON_PAD
python ./xlsx2header.py ./register_files/AmebaPro3_PAD_register_file_20240923.xlsx sysreg_pon_pad.h 22 1 PON_PAD
python ./xlsx2header.py ./register_files/AmebaPro3_PAD_register_file_20240923.xlsx sysreg_core_pad.h 22 1 CORE_PAD

python ./xlsx2header.py ./register_files/AmebaPro3_system_register_file_20240925.xlsx sysreg_aon.h 22 1 AON
python ./xlsx2header.py ./register_files/AmebaPro3_system_register_file_20240925.xlsx sysreg_syson.h 22 1 SYSON
python ./xlsx2header.py ./register_files/AmebaPro3_system_register_file_20240925.xlsx sysreg_core_ns.h 22 1 CORE_NS
python ./xlsx2header.py ./register_files/AmebaPro3_system_register_file_20240925.xlsx sysreg_core_s.h 22 1 CORE_S
python ./xlsx2header.py ./register_files/AmebaPro3_system_register_file_20240925.xlsx sysreg_wloff.h 22 1 WLOFF

python ./xlsx2header.py ./register_files/AmebaPro3_IPC_register_file_20240917.xlsx ameba_ipc.h 22 1

python ./xlsx2header.py ./register_files/AmebaPro3_OTPC_register_file.xlsx ameba_optc.h 22 1 