//G.711 A-law encoder
//G.711 U-law encoder
#include "g711_codec.h"

//typedef char u8

#define G711A 1
#define G711U 2
//#define AMPLIFIER

//WARNING: the paramater AMPLIFIER could cause distortion to input signal
#ifdef AMPLIFIER
const float AMPLIFIER_1 = 5.0;//if needed to amplify or attenuate the input signal
const float AMPLIFIER_2 = 0.05;//if needed to amplify or attenuate the output signal
#endif


const int MAX_A = 0x7fff;//A-law 13 bits while the first bit is sign bit, left align
const int BIAS =  0x84	;//132, 0b1000,0100
const int MAX_U = 32635 ;//MAX_A-BIAS


#define	SIGN_BIT	(0x80)		// Sign bit for a A-law byte. 
#define	QUANT_MASK	(0xf)		// Quantization field mask. 
#define	NSEGS		(8)			// Number of A-law segments. 
#define	SEG_SHIFT	(4)			// Left shift for segment number. 
#define	SEG_MASK	(0x70)		// Segment field mask. 
#define	BIAS		(0x84)		// Bias for linear code. 
#define CLIP         8159


static short seg_aend[8] = {0x1F, 0x3F, 0x7F, 0xFF,
							0x1FF, 0x3FF, 0x7FF, 0xFFF
						   };
static short seg_uend[8] = {0x3F, 0x7F, 0xFF, 0x1FF,
							0x3FF, 0x7FF, 0xFFF, 0x1FFF
						   };

static short
search(
	short		val,
	short		*table,
	short		size
)
{
	short		i;

	for (i = 0; i < size; i++) {
		if (val <= *table++) {
			return (i);
		}
	}
	return (size);
}

u8 encodeA(short pcm_val);
short decodeA(unsigned char a_val);
u8 encodeU(short pcm_val);
short decodeU(unsigned char u_val);

/*	G711A, PCMA
	Linear Input Code		Compressed Code
 	-----------------		---------------
 	0000000wxyza			000wxyz
 	0000001wxyza			001wxyz
 	000001wxyzab			010wxyz
 	00001wxyzabc			011wxyz
 	0001wxyzabcd			100wxyz
 	001wxyzabcde			101wxyz
 	01wxyzabcdef			110wxyz
 	1wxyzabcdefg			111wxyz
*/
unsigned char
encodeA(
	short		pcm_val	// 2's complement (16-bit range)
)
{
	short		mask;
	short		seg;
	unsigned char	aval;

	pcm_val = pcm_val >> 3;

	if (pcm_val >= 0) {
		mask = 0xD5;		// sign (7th) bit = 1
	} else {
		mask = 0x55;		// sign bit = 0
		pcm_val = -pcm_val - 1;
	}

	// Convert the scaled magnitude to segment number.
	seg = search(pcm_val, seg_aend, 8);

	// Combine the sign, segment, and quantization bits.

	if (seg >= 8) {	// out of range, return maximum value.
		return (unsigned char)(0x7F ^ mask);
	} else {
		aval = (unsigned char) seg << SEG_SHIFT;
		if (seg < 2) {
			aval |= (pcm_val >> 1) & QUANT_MASK;
		} else {
			aval |= (pcm_val >> seg) & QUANT_MASK;
		}
		return (aval ^ mask);
	}
}

short
decodeA(
	unsigned char	a_val
)
{
	short		t;
	short		seg;

	a_val ^= 0x55;

	t = (a_val & QUANT_MASK) << 4;
	seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
	switch (seg) {
	case 0:
		t += 8;
		break;
	case 1:
		t += 0x108;
		break;
	default:
		t += 0x108;
		t <<= seg - 1;
	}
	return ((a_val & SIGN_BIT) ? t : -t);
}

/*	G711U, PCMU
 	Biased Linear Input Code	Compressed Code
 	------------------------	---------------
 	00000001wxyza				000wxyz
 	0000001wxyzab				001wxyz
 	000001wxyzabc				010wxyz
 	00001wxyzabcd				011wxyz
 	0001wxyzabcde				100wxyz
 	001wxyzabcdef				101wxyz
 	01wxyzabcdefg				110wxyz
 	1wxyzabcdefgh				111wxyz
*/

unsigned char encodeU(
	short		pcm_val	// 2's complement (16-bit range)
)
{
	short		mask;
	short		seg;
	unsigned char	uval;

	// Get the sign and the magnitude of the value.
	// clip the magnitude
	if (pcm_val < 0) {
		if (pcm_val < -(CLIP << 2)) {
			pcm_val = (CLIP << 2);
		} else {
			pcm_val = -pcm_val;
		}
		mask = 0x7F;
	} else {
		if (pcm_val > (CLIP << 2)) {
			pcm_val = (CLIP << 2);
		}
		mask = 0xFF;
	}

	pcm_val = pcm_val >> 2;
	pcm_val += (BIAS >> 2);

	// Convert the scaled magnitude to segment number.
	seg = search(pcm_val, seg_uend, 8);


	//Combine the sign, segment, quantization bits;
	//and complement the code word.

	if (seg >= 8) {	// out of range, return maximum value.
		return (unsigned char)(0x7F ^ mask);
	} else {
		uval = (unsigned char)(seg << 4) | ((pcm_val >> (seg + 1)) & 0xF);
		return (uval ^ mask);
	}

}

short
decodeU(
	unsigned char	u_val
)
{
	short		t;

	// Complement to obtain normal u-law value.
	u_val = ~u_val;


	//Extract and bias the quantization bits. Then
	//shift up by the segment number and subtract out the bias.

	t = ((u_val & QUANT_MASK) << 3) + BIAS;
	t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

	return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}

/*
u8 encodeU(int raw){//U-law encoder
  printf("&");
	int sign = raw & 0x8000;
	if (sign!=0)	raw = -raw;
	if (raw > MAX_U) raw = MAX_U;
	raw+=BIAS;

	int eee = 7;
	for(int eeeMask = 0x4000; (raw & eeeMask)==0 ;eee--, eeeMask>>1){}
	int mmmm =(raw >> (eee + 3)) & 0x0f;
	u8 ulaw = (u8)(sign>>8 | eee << 4 | mmmm);
	return (u8)~ulaw;
}*/
/*
void encoder_init(int mode){
	switch(mode){
	case G711A_ENCODER:
		for (int i=-32768; i<=32767; i++)codec_table[(i&0xFFFF)]=encodeU(i);
	  	break;
	case G711U_ENCODER:
		for (int i=-32768; i<=32767; i++)codec_table[(i&0xFFFF)]=encodeA(i);
	  	break;
	default:
		for (int i=-32768; i<=32767; i++)codec_table[(i&0xFFFF)]=0;
	  	break;
  }
	return;
}
*/
int G711_encoder(short *source, unsigned char *des, int mode, int size)
{
	if (source == NULL) {
		return -1;    //invalid source
	}
	if (des == NULL) {
		return -2;    //invalid destination
	}
#ifdef AMPLIFIER
	switch (mode) {
	case G711A:
		for (int i = 0; i < size; i++)	{
			des[i] = encodeA((short)(source[i] * AMPLIFIER_1));
		}
		return mode;
	case G711U:
		for (int i = 0; i < size; i++)	{
			des[i] = encodeU((short)(source[i] * AMPLIFIER_1));
		}
		return mode;
	default:
		return -3;//invalid mode
	}
#else
	switch (mode) {
	case G711A:
		for (int i = 0; i < size; i++)	{
			des[i] = encodeA(source[i]);
		}
		return mode;
	case G711U:
		for (int i = 0; i < size; i++)	{
			des[i] = encodeU(source[i]);
		}
		return mode;
	default:
		return -3;//invalid mode
	}
#endif
}


int G711_decoder(unsigned char *source, short *des, int mode, int size)
{
	if (source == NULL) {
		return -1;    //invalid source
	}
	if (des == NULL) {
		return -2;    //invalid destination
	}
#ifdef AMPLIFIER
	switch (mode) {
	case G711A:
		for (int i = 0; i < size; i++)	{
			des[i] = (short)(decodeA(source[i]) * AMPLIFIER_2);
		}
		return mode;
	case G711U:
		for (int i = 0; i < size; i++)	{
			des[i] = (short)(decodeU(source[i]) * AMPLIFIER_2);
		}
		return mode;
	default:
		return -3;//invalid mode
	}
#else
	switch (mode) {
	case G711A:
		for (int i = 0; i < size; i++)	{
			des[i] = decodeA(source[i]);
		}
		return mode;
	case G711U:
		for (int i = 0; i < size; i++)	{
			des[i] = decodeU(source[i]);
		}
		return mode;
	default:
		return -3;//invalid mode
	}
#endif
}

//change mono audio data to stereo according to data size & word length in bit
// int mono2stereo(unsigned char *src, unsigned char *dst, int size, int wl)
// {
// 	int i;
// 	unsigned char *ptr1, *ptr2;
// 	ptr1 = src;
// 	ptr2 = dst;

// 	if (wl == 16) {
// 		for (i = 0; i < size; i += 2) {
// 			ptr2[i * 2] = ptr2[i * 2 + 2] = ptr1[i];
// 			ptr2[i * 2 + 1] = ptr2[i * 2 + 3] = ptr1[i + 1];
// 		}
// 		return 0;
// 	} else if (wl == 24) {
// 		for (i = 0; i < size; i += 3) {
// 			ptr2[i * 3] = ptr2[i * 3 + 3] = ptr1[i];
// 			ptr2[i * 3 + 1] = ptr2[i * 3 + 4] = ptr1[i + 1];
// 			ptr2[i * 3 + 2] = ptr2[i * 3 + 5] = ptr1[i + 2];
// 		}
// 		return 0;
// 	} else {
// 		printf("\n\rinvalid word length");
// 		return -1;
// 	}
// }