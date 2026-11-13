/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADDREMAP_HAL_H_
#define _ADDREMAP_HAL_H_


#define H264_SLICE_HEIGHT 	16
#define H265_SLICE_HEIGHT 	64
#define VIDEO_SRAM_SIZE 	196608  //192KB
#define MIN_WRAP_NUM		3
#define MIN_WRAP_NUM_RTT	2




struct addremap_comp {

	unsigned int dram_saddr;
	unsigned int dram_eaddr;
	unsigned char enable;
	unsigned char slice_height;
	unsigned short stride;
	unsigned int out_saddr_offset;
};


struct addremap_comp_enc {

	unsigned int dram_saddr;
	unsigned int dram_eaddr;
	unsigned char enable;
};


struct addremap_hdr {

	unsigned char numerator;
	unsigned char denominator;
	unsigned short max_burst;
	unsigned short burstSize;
	unsigned int addr_base;
};

struct cp_addr_range {
	unsigned int start_addr;
	unsigned int end_addr;
};


struct addremap_partial_sram {

	unsigned int sram_ratio;
	unsigned int sram_size;
	struct cp_addr_range sram_addr[3];
};

struct addremap_parsm_compress {

	struct cp_addr_range compress_sram_addr;
	struct cp_addr_range compress_dram_addr;
	unsigned int headr_addr[2];
	unsigned int headr_size[2];
	unsigned int valid_headr_size[2];
};


struct partial_slice_comp {

	unsigned char handshake_en;
	unsigned char tile_height;
	unsigned short tile_width;
	unsigned char stride;
};

struct addremap_partial_slice {

	struct partial_slice_comp component[3];
	unsigned short tile_in_row;
	unsigned short slice_wrap_num;

};



struct isp_remap_config {

	unsigned char SWrapEnb;
	unsigned char ParSWrapEnb;
	unsigned char RTTEnb;
	unsigned char PaREnb;

	unsigned char int_mask;

	unsigned char IspEncdEnable;
	//unsigned char TscEncdEnable;

	unsigned short SliceHeight;
	unsigned short AlignWidth;

	unsigned char SliceBurstID;
	unsigned char SliceNumWrap;
	unsigned short TotalSliceNum;

	unsigned int SliceNumThreshold;


	struct addremap_comp slice_wrap_component[3];

	struct addremap_partial_sram partial_sram_config;

	struct addremap_parsm_compress compress_component[3];

	//struct addremap_partial_slice partial_slice_config;

	unsigned char CompEnb;

	unsigned char HdrEnb;

	struct addremap_hdr hdr_config;

	unsigned short Slice_Stride;
	unsigned short Slice_num;

};


struct enc_remap_config {

	//unsigned char int_mask;

	unsigned char IspEncdEnable;
	unsigned char TscEncdEnable;


	unsigned char SWrapEnb;
	unsigned char ParSWrapEnb;
	unsigned char RTTEnb;
	unsigned char PaREnb;
	unsigned char CompEnb;

	// refer to isp_remap
	unsigned char SliceBurstID;
	unsigned char SliceNumWrap;
	//unsigned short TotalSliceNum;
	unsigned short SliceHeight;


	struct addremap_comp_enc enc_remap_component[3];

	//unsigned int SliceNumThreshold;

	struct addremap_partial_sram partial_sram_config;

	struct addremap_parsm_compress compress_component[3];

	struct addremap_partial_slice partial_slice_config;


	unsigned short tile_width;
	unsigned short ParSliceWrapNum;

	unsigned short Slice_Stride;
	unsigned short Slice_num;


};




//int hal_addremap_H264_slice_wrap_full_SRAM(unsigned int  width,  unsigned int *y_addr, unsigned int *uv_addr);
int set_hdr_remap(uint32_t width, uint32_t height, uint32_t vhdr_length,  struct addremap_hdr *hdr_config);



#endif  /* _ADDREMAP_HAL_H_ */


