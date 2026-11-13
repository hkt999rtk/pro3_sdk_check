/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Verisilicon.                                    --
--                                                                            --
--                   (C) COPYRIGHT 2014 VERISILICON                           --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------*/
#ifdef RATE_CONTROL_BUILD_SUPPORT

#include "vsi_string.h"
//#include "ctu_tools.h"
#include <rate_control_picture.h>
#include "tools.h"
#include "sw_slice.h"
#include <math.h>
#include "enccfg.h"
#include "sw_cu_tree.h"

#ifdef RCSRC_INCLUDE
  #include "enc_log.h"
#else
  #undef RC_LOG_I
  #undef RC_LOG_E
  #undef RC_LOG_D

  #define RC_LOG_I(fmt, ...)
  #define RC_LOG_E(fmt, ...)
  #define RC_LOG_D(fmt, ...)
#endif
/*
 * Using only one leaky bucket (Multible buckets is supported by std).
 * Constant bit rate (CBR) operation, ie. leaky bucket drain rate equals
 * average rate of the stream, is enabled if RC_CBR_HRD = 1. Frame skiping and
 * filler data are minimum requirements for the CBR conformance.
 *
 * Constant HRD parameters:
 *   low_delay_hrd_flag = 0, assumes constant delay mode.
 *       cpb_cnt_minus1 = 0, only one leaky bucket used.
 *         (cbr_flag[0] = RC_CBR_HRD, CBR mode.)
 */

/*------------------------------------------------------------------------------
  Module defines
------------------------------------------------------------------------------*/

/* Define this if strict bitrate control is needed, each window has a strict
 * bit budget. Otherwise a moving window is used for smoother quality.
*/

#define INITIAL_BUFFER_FULLNESS 80 /* Decoder Buffer in procents */
#define MIN_PIC_SIZE 50            /* Procents from picPerPic */

#define DIV(a, b) (((b) != 0) ? ((a) + (SIGN(a) * (b)) / 2) / (b) : (a))
#define DIV2(a, b) \
  (((a) + (SIGN(a) * (b)) / 2) / (b)) /* when b is non-zero value */
#define DSCY 32                       /* n * 32 */
#define DSCBITPERMB 128               //128 /* bitPerMb scaler  */
#define I32_MIN (i32)(1u << 31)

#define I64_MAX 9223372036854775807 /* 2 ^ 63 - 1 */
//#define QP_DELTA          2.0     /* Limit QP change between consecutive frames.*/
#define QP_DELTA 0x200 /* Limit QP change between consecutive frames.*/

//#define QP_DELTA_LIMIT    6.0     /* Threshold when above limit is abandoned. */
#define QP_DELTA_LIMIT 0x600 /* Threshold when above limit is abandoned. */
#define QP_FIXPOINT_0_POINT_1 0x1a

#define INTRA_QP_DELTA (0)
#define WORD_CNT_MAX 65535
#define QP_FRACTIONAL_MASK ((1 << QP_FRACTIONAL_BITS) - 1)
/*------------------------------------------------------------------------------
  Local structures
------------------------------------------------------------------------------*/
#if 0
/* q_step values scaled up by 4 and evenly rounded */
static const i32 q_step[53] = { 3, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11,
                                13, 14, 16, 18, 20, 23, 25, 28, 32, 36, 40, 45, 51, 57, 64, 72, 80, 90,
                                101, 114, 128, 144, 160, 180, 203, 228, 256, 288, 320, 360, 405, 456,
                                513, 577, 640, 720, 810, 896
                              };
#else
#if 0
static const i32 q_step[53] = { 2, 3, 3, 4, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11,
                                13, 14, 16, 18, 20, 22, 25, 28, 31, 35, 39, 44, 50, 56, 63, 71, 79, 88,
                                100, 112, 126, 141, 157, 177, 200, 224, 252, 283, 314, 354, 401, 448,
                                503, 566, 629, 707, 802, 896
                              };
#else
#if 0
static const float q_step[511] ={
  2.475138122,2.503897841,2.532991733,2.562423679,2.592197608,2.622317493,2.652787355,2.683611259,2.71479332 ,2.746337699,
2.778248606,2.8105303  ,2.843187089,2.876223331,2.909643437,2.943451865,2.977653128,3.012251791,3.047252471,3.082659839,
3.118478621,3.154713597,3.191369603,3.228451532,3.265964332,3.303913009,3.342302629,3.381138315,3.42042525 ,3.460168677,
3.5003739  ,3.541046286,3.582191262,3.623814319,3.665921013,3.708516964,3.751607855,3.795199439,3.839297532,3.883908021,
3.929036858,3.974690067,4.020873741,4.067594043,4.11485721 ,4.162669547,4.211037438,4.259967336,4.309465772,4.359539352,
4.410194759,4.461438754,4.513278175,4.565719942,4.618771052,4.672438587,4.726729708,4.781651661,4.837211777,4.893417471,
4.950276243,5.007795682,5.065983465,5.124847358,5.184395216,5.244634986,5.30557471 ,5.367222518,5.42958664 ,5.492675398,
5.556497212,5.621060599,5.686374177,5.752446663,5.819286873,5.88690373 ,5.955306256,6.024503581,6.094504941,6.165319678,
6.236957241,6.309427194,6.382739206,6.456903064,6.531928663,6.607826018,6.684605258,6.76227663 ,6.8408505  ,6.920337354,
7.0007478  ,7.082092572,7.164382523,7.247628638,7.331842027,7.417033928,7.50321571 ,7.590398877,7.678595064,7.767816041,
7.858073716,7.949380134,8.041747482,8.135188087,8.229714419,8.325339095,8.422074875,8.519934672,8.618931544,8.719078704,
8.820389519,8.922877508,9.026556351,9.131439883,9.237542104,9.344877173,9.453459416,9.563303323,9.674423555,9.786834942,
9.900552486,10.01559136,10.13196693,10.24969472,10.36879043,10.48926997,10.61114942,10.73444504,10.85917328,10.9853508 ,
11.11299442,11.2421212 ,11.37274835,11.50489333,11.63857375,11.77380746,11.91061251,12.04900716,12.18900988,12.33063936,
12.47391448,12.61885439,12.76547841,12.91380613,13.06385733,13.21565204,13.36921052,13.52455326,13.681701  ,13.84067471,
14.0014956 ,14.16418514,14.32876505,14.49525728,14.66368405,14.83406786,15.00643142,15.18079775,15.35719013,15.53563208,
15.71614743,15.89876027,16.08349496,16.27037617,16.45942884,16.65067819,16.84414975,17.03986934,17.23786309,17.43815741,
17.64077904,17.84575502,18.0531127 ,18.26287977,18.47508421,18.68975435,18.90691883,19.12660665,19.34884711,19.57366988,
19.80110497,20.03118273,20.26393386,20.49938943,20.73758086,20.97853995,21.22229884,21.46889007,21.71834656,21.97070159,
22.22598885,22.4842424 ,22.74549671,23.00978665,23.27714749,23.54761492,23.82122502,24.09801433,24.37801976,24.66127871,
24.94782897,25.23770878,25.53095683,25.82761225,26.12771465,26.43130407,26.73842103,27.04910652,27.363402  ,27.68134941,
28.0029912 ,28.32837029,28.65753009,28.99051455,29.32736811,29.66813571,30.01286284,30.36159551,30.71438026,31.07126416,
31.43229486,31.79752054,32.16698993,32.54075235,32.91885768,33.30135638,33.6882995 ,34.07973869,34.47572618,34.87631482,
35.28155808,35.69151003,36.1062254 ,36.52575953,36.95016842,37.37950869,37.81383766,38.25321329,38.69769422,39.14733977,
39.60220994,40.06236546,40.52786772,40.99877886,41.47516173,41.95707989,42.44459768,42.93778015,43.43669312,43.94140318,
44.45197769,44.96848479,45.49099342,46.0195733 ,46.55429498,47.09522984,47.64245005,48.19602865,48.75603953,49.32255742,
49.89565793,50.47541755,51.06191365,51.65522451,52.25542931,52.86260815,53.47684207,54.09821304,54.726804  ,55.36269883,
56.0059824 ,56.65674057,57.31506019,57.98102911,58.65473621,59.33627142,60.02572568,60.72319102,61.42876051,62.14252833,
62.86458973,63.59504107,64.33397986,65.0815047 ,65.83771535,66.60271276,67.376599  ,68.15947737,68.95145235,69.75262963,
70.56311615,71.38302007,72.21245081,73.05151907,73.90033683,74.75901738,75.62767532,76.50642658,77.39538844,78.29467953,
79.20441989,80.12473092,81.05573545,81.99755772,82.95032345,83.91415978,84.88919535,85.87556029,86.87338624,87.88280636,
88.90395538,89.93696959,90.98198684,92.0391466 ,93.10858997,94.19045967,95.2849001 ,96.3920573 ,97.51207906,98.64511484,
99.79131586,100.9508351,102.1238273,103.310449 ,104.5108586,105.7252163,106.9536841,108.1964261,109.453608 ,110.7253977,
112.0119648,113.3134811,114.6301204,115.9620582,117.3094724,118.6725428,120.0514514,121.446382 ,122.857521 ,124.2850567,
125.7291795,127.1900821,128.6679597,130.1630094,131.6754307,133.2054255,134.753198 ,136.3189547,137.9029047,139.5052593,
141.1262323,142.7660401,144.4249016,146.1030381,147.8006737,149.5180348,151.2553506,153.0128532,154.7907769,156.5893591,
158.4088398,160.2494618,162.1114709,163.9951154,165.9006469,167.8283196,169.7783907,171.7511206,173.7467725,175.7656127,
177.8079108,179.8739392,181.9639737,184.0782932,186.2171799,188.3809193,190.5698002,192.7841146,195.0241581,197.2902297,
199.5826317,201.9016702,204.2476546,206.620898 ,209.0217172,211.4504326,213.9073683,216.3928522,218.907216 ,221.4507953,
224.0239296,226.6269623,229.2602408,231.9241164,234.6189449,237.3450857,240.1029027,242.8927641,245.7150421,248.5701133,
251.4583589,254.3801643,257.3359194,260.3260188,263.3508614,266.410851 ,269.506396 ,272.6379095,275.8058094,279.0105185,
282.2524646,285.5320803,288.8498032,292.2060763,295.6013473,299.0360695,302.5107013,306.0257063,309.5815538,313.1787181,
316.8176796,320.4989237,324.2229418,327.9902309,331.8012938,335.6566391,339.5567814,343.5022412,347.4935449,351.5312254,
355.6158215,359.7478783,363.9279473,368.1565864,372.4343599,376.7618387,381.1396004,385.5682292,390.0483162,394.5804594,
399.1652635,403.8033404,408.4953092,413.2417961,418.0434344,422.9008652,427.8147365,432.7857043,437.814432 ,442.9015906,
448.0478592,453.2539246,458.5204815,463.8482329,469.2378897,474.6901714,480.2058055,485.7855282,491.4300841,497.1402266,
502.9167178,508.7603286,514.6718389,520.6520376,526.7017228,532.8217021,539.012792 ,545.275819 ,551.6116188,558.0210371,
564.5049292,571.0641605,577.6996065,584.4121525,591.2026947,598.0721391,605.0214026,612.0514127,619.1631075,626.3574363,
633.6353591,640.9978473,648.4458836,655.9804618,663.6025876,671.3132783,679.1135628,687.0044823,694.9870899,703.0624509,
711.2316431,719.4957567,727.8558947,736.3131728,744.8687198,753.5236774,762.2792008,771.1364584,780.0966325,789.1609187,
798.3305269,807.6066808,816.9906184,826.4835921,836.0868689,845.8017304,855.6294731,865.5714087,875.628864 ,885.8031813,
896.0957185

};
#else
static const i32 q_step[511] = {

    0x279,   0x280,   0x288,   0x28f,   0x297,   0x29f,   0x2a7,   0x2af,
    0x2b6,   0x2bf,   0x2c7,   0x2cf,   0x2d7,   0x2e0,   0x2e8,   0x2f1,
    0x2fa,   0x303,   0x30c,   0x315,   0x31e,   0x327,   0x330,   0x33a,
    0x344,   0x34d,   0x357,   0x361,   0x36b,   0x375,   0x380,   0x38a,
    0x395,   0x39f,   0x3aa,   0x3b5,   0x3c0,   0x3cb,   0x3d6,   0x3e2,
    0x3ed,   0x3f9,   0x405,   0x411,   0x41d,   0x429,   0x436,   0x442,
    0x44f,   0x45c,   0x469,   0x476,   0x483,   0x490,   0x49e,   0x4ac,
    0x4ba,   0x4c8,   0x4d6,   0x4e4,   0x4f3,   0x501,   0x510,   0x51f,
    0x52f,   0x53e,   0x54e,   0x55e,   0x56d,   0x57e,   0x58e,   0x59e,
    0x5af,   0x5c0,   0x5d1,   0x5e3,   0x5f4,   0x606,   0x618,   0x62a,
    0x63c,   0x64f,   0x661,   0x674,   0x688,   0x69b,   0x6af,   0x6c3,
    0x6d7,   0x6eb,   0x700,   0x715,   0x72a,   0x73f,   0x754,   0x76a,
    0x780,   0x797,   0x7ad,   0x7c4,   0x7db,   0x7f3,   0x80a,   0x822,
    0x83a,   0x853,   0x86c,   0x885,   0x89e,   0x8b8,   0x8d2,   0x8ec,
    0x906,   0x921,   0x93c,   0x958,   0x974,   0x990,   0x9ac,   0x9c9,
    0x9e6,   0xa03,   0xa21,   0xa3f,   0xa5e,   0xa7d,   0xa9c,   0xabc,
    0xadb,   0xafc,   0xb1c,   0xb3d,   0xb5f,   0xb81,   0xba3,   0xbc6,
    0xbe9,   0xc0c,   0xc30,   0xc54,   0xc79,   0xc9e,   0xcc3,   0xce9,
    0xd10,   0xd37,   0xd5e,   0xd86,   0xdae,   0xdd7,   0xe00,   0xe2a,
    0xe54,   0xe7e,   0xea9,   0xed5,   0xf01,   0xf2e,   0xf5b,   0xf89,
    0xfb7,   0xfe6,   0x1015,  0x1045,  0x1075,  0x10a6,  0x10d8,  0x110a,
    0x113c,  0x1170,  0x11a4,  0x11d8,  0x120d,  0x1243,  0x1279,  0x12b0,
    0x12e8,  0x1320,  0x1359,  0x1392,  0x13cd,  0x1407,  0x1443,  0x147f,
    0x14bc,  0x14fa,  0x1538,  0x1578,  0x15b7,  0x15f8,  0x1639,  0x167b,
    0x16be,  0x1702,  0x1746,  0x178c,  0x17d2,  0x1819,  0x1860,  0x18a9,
    0x18f2,  0x193c,  0x1987,  0x19d3,  0x1a20,  0x1a6e,  0x1abd,  0x1b0c,
    0x1b5d,  0x1bae,  0x1c00,  0x1c54,  0x1ca8,  0x1cfd,  0x1d53,  0x1dab,
    0x1e03,  0x1e5c,  0x1eb6,  0x1f12,  0x1f6e,  0x1fcc,  0x202a,  0x208a,
    0x20eb,  0x214d,  0x21b0,  0x2214,  0x2279,  0x22e0,  0x2348,  0x23b1,
    0x241b,  0x2486,  0x24f3,  0x2561,  0x25d0,  0x2640,  0x26b2,  0x2725,
    0x279a,  0x280f,  0x2887,  0x28ff,  0x2979,  0x29f5,  0x2a71,  0x2af0,
    0x2b6f,  0x2bf0,  0x2c73,  0x2cf7,  0x2d7d,  0x2e05,  0x2e8d,  0x2f18,
    0x2fa4,  0x3032,  0x30c1,  0x3152,  0x31e5,  0x3279,  0x330f,  0x33a7,
    0x3441,  0x34dc,  0x357a,  0x3619,  0x36ba,  0x375c,  0x3801,  0x38a8,
    0x3950,  0x39fb,  0x3aa7,  0x3b56,  0x3c06,  0x3cb9,  0x3d6d,  0x3e24,
    0x3edd,  0x3f98,  0x4055,  0x4114,  0x41d6,  0x429a,  0x4360,  0x4428,
    0x44f3,  0x45c0,  0x4690,  0x4762,  0x4836,  0x490d,  0x49e6,  0x4ac2,
    0x4ba0,  0x4c81,  0x4d65,  0x4e4b,  0x4f34,  0x501f,  0x510e,  0x51ff,
    0x52f3,  0x53ea,  0x54e3,  0x55e0,  0x56df,  0x57e1,  0x58e7,  0x59ef,
    0x5afb,  0x5c0a,  0x5d1b,  0x5e30,  0x5f48,  0x6064,  0x6183,  0x62a5,
    0x63ca,  0x64f3,  0x661f,  0x674f,  0x6882,  0x69b9,  0x6af4,  0x6c32,
    0x6d74,  0x6eb9,  0x7003,  0x7150,  0x72a1,  0x73f6,  0x754f,  0x76ac,
    0x780d,  0x7972,  0x7adb,  0x7c48,  0x7dba,  0x7f30,  0x80aa,  0x8229,
    0x83ac,  0x8534,  0x86c0,  0x8851,  0x89e7,  0x8b81,  0x8d20,  0x8ec4,
    0x906c,  0x921a,  0x93cc,  0x9584,  0x9741,  0x9903,  0x9aca,  0x9c96,
    0x9e68,  0xa03f,  0xa21c,  0xa3fe,  0xa5e6,  0xa7d4,  0xa9c7,  0xabc0,
    0xadbf,  0xafc3,  0xb1ce,  0xb3df,  0xb5f6,  0xb814,  0xba37,  0xbc61,
    0xbe91,  0xc0c8,  0xc306,  0xc54a,  0xc795,  0xc9e6,  0xcc3f,  0xce9e,
    0xd105,  0xd373,  0xd5e8,  0xd864,  0xdae8,  0xdd73,  0xe006,  0xe2a0,
    0xe542,  0xe7ec,  0xea9e,  0xed58,  0xf01a,  0xf2e4,  0xf5b7,  0xf891,
    0xfb75,  0xfe61,  0x10155, 0x10453, 0x10759, 0x10a69, 0x10d81, 0x110a3,
    0x113ce, 0x11702, 0x11a40, 0x11d88, 0x120d9, 0x12434, 0x12799, 0x12b09,
    0x12e82, 0x13206, 0x13594, 0x1392d, 0x13cd1, 0x1407f, 0x14439, 0x147fd,
    0x14bcd, 0x14fa8, 0x1538e, 0x15780, 0x15b7e, 0x15f87, 0x1639d, 0x167bf,
    0x16bed, 0x17028, 0x1746f, 0x178c3, 0x17d23, 0x18191, 0x1860c, 0x18a94,
    0x18f2a, 0x193cd, 0x1987e, 0x19d3d, 0x1a20b, 0x1a6e6, 0x1abd0, 0x1b0c9,
    0x1b5d0, 0x1bae6, 0x1c00c, 0x1c541, 0x1ca85, 0x1cfd9, 0x1d53c, 0x1dab0,
    0x1e034, 0x1e5c9, 0x1eb6e, 0x1f123, 0x1f6ea, 0x1fcc2, 0x202ab, 0x208a6,
    0x20eb3, 0x214d2, 0x21b03, 0x22146, 0x2279c, 0x22e05, 0x23481, 0x23b10,
    0x241b3, 0x24869, 0x24f33, 0x25612, 0x25d05, 0x2640d, 0x26b29, 0x2725b,
    0x279a2, 0x280ff, 0x28872, 0x28ffb, 0x2979a, 0x29f50, 0x2a71d, 0x2af01,
    0x2b6fc, 0x2bf0f, 0x2c73b, 0x2cf7e, 0x2d7db, 0x2e050, 0x2e8de, 0x2f186,
    0x2fa47, 0x30322, 0x30c18, 0x31529, 0x31e54, 0x3279b, 0x330fd, 0x33a7b,
    0x34416, 0x34dcd, 0x357a1, 0x36192, 0x36ba0, 0x375cd, 0x38018};
#endif
#endif
#endif

#ifdef RCP_FIXED_POINT_OPT
#define USE_QP2QSCALE_TABLE
#endif

#ifdef USE_QP2QSCALE_TABLE
#ifdef RCP_FIXED_POINT_OPT
static const RCP_32bit qpScaleTable[120] = {
0x366666, 0x36b713, 0x370838, 0x3759d4, 0x37abea, 0x37fe7a, 0x385183, 0x38a509,
0x38f909, 0x394d87, 0x39a282, 0x39f7fb, 0x3a4df2, 0x3aa469, 0x3afb61, 0x3b52d9,
0x3baad3, 0x3c034f, 0x3c5c4f, 0x3cb5d3, 0x3d0fdb, 0x3d6a69, 0x3dc57d, 0x3e2118,
0x3e7d3c, 0x3ed9e7, 0x3f371d, 0x3f94dc, 0x3ff326, 0x4051fd, 0x40b160, 0x411150,
0x4171cf, 0x41d2dd, 0x42347a, 0x4296a9, 0x42f969, 0x435cbb, 0x43c0a1, 0x44251b,
0x448a2a, 0x44efcf, 0x45560b, 0x45bcde, 0x462449, 0x468c4e, 0x46f4ee, 0x475e28,
0x47c7ff, 0x483272, 0x489d84, 0x490934, 0x497583, 0x49e274, 0x4a5006, 0x4abe3a,
0x4b2d12, 0x4b9c8f, 0x4c0cb0, 0x4c7d78, 0x4ceee7, 0x4d60ff, 0x4dd3bf, 0x4e472a,
0x4ebb40, 0x4f3002, 0x4fa571, 0x501b8f, 0x50925b, 0x5109d8, 0x518206, 0x51fae6,
0x52747a, 0x52eec1, 0x5369be, 0x53e572, 0x5461dd, 0x54df00, 0x555cdd, 0x55db75,
0x565ac8, 0x56dad8, 0x575ba7, 0x57dd34, 0x585f81, 0x58e28f, 0x596660, 0x59eaf5,
0x5a704e, 0x5af66c, 0x5b7d52, 0x5c04ff, 0x5c8d76, 0x5d16b8, 0x5da0c4, 0x5e2b9e,
0x5eb745, 0x5f43bc, 0x5fd103, 0x605f1b, 0x60ee06, 0x617dc5, 0x620e59, 0x629fc4,
0x633206, 0x63c521, 0x645917, 0x64ede7, 0x658395, 0x661a20, 0x66b18b, 0x6749d6,
0x67e303, 0x687d13, 0x691808, 0x69b3e3, 0x6a50a4, 0x6aee4e, 0x6b8ce2, 0x6c2c61};


static const RCP_32bit dqpScaleTable[60] = { 																//1/qpScaleTable[i/10] //Q8.24
0x4b4b4b5, 0x4a6de60, 0x49930bc, 0x48bab4f, 0x47e4da5, 0x4711748, 0x46407c4, 0x4571ea8,
0x44a5b82, 0x43dbde2, 0x431455b, 0x424f180, 0x418c1e4, 0x40cb61d, 0x400cdc1, 0x3f50869,
0x3e965ae, 0x3dde529, 0x3d28676, 0x3c74932, 0x3bc2cfa, 0x3b1316e, 0x3a6562c, 0x39b9ad7,
0x390ff10, 0x386827a, 0x37c24ba, 0x371e576, 0x367c452, 0x35dc0f8, 0x353db0f, 0x34a1241,
0x3406638, 0x336d69f, 0x32d6324, 0x3240b72, 0x31acf39, 0x311ae29, 0x308a7ef, 0x2ffbc3f,
0x2f6eaca, 0x2ee3343, 0x2e5955e, 0x2dd10ce, 0x2d4a54b, 0x2cc5289, 0x2c41841, 0x2bbf62a,
0x2b3ebfd, 0x2abf974, 0x2a41e4a, 0x29c5a39, 0x294acfe, 0x28d1656, 0x28595fd, 0x27e2bb4,
0x276d738, 0x26f984a, 0x2686ea9, 0x2615a19};

static const RCP_32bit table_crf_fixedQuant[] = {											////Q8.24 fps = {5 ~240}
0x867a6d, 0x90a6ea, 0x99da1d, 0xa24b06, 0xaa1f48, 0xb171f1, 0xb85733, 0xbede95,
0xc51456, 0xcb024a, 0xd0b07f, 0xd625a1, 0xdb674b, 0xe07a42, 0xe5629d, 0xea23e3,
0xeec12b, 0xf33d29, 0xf79a40, 0xfbda91, 0x1000000, 0x1040c41, 0x10800dd, 0x10bdf35,
0x10fa88e, 0x1135e0b, 0x11700bb, 0x11a9192, 0x11e1174, 0x1218130, 0x124e188, 0x128332f,
0x12b76ca, 0x12eacf4, 0x131d63b, 0x134f328, 0x1380437, 0x13b09dd, 0x13e048a, 0x140f4a6,
0x143da91, 0x146b6a9, 0x1498943, 0x14c52b3, 0x14f1346, 0x151cb45, 0x1547af7, 0x157229e,
0x159c278, 0x15c5ac3, 0x15eebb6, 0x1617588, 0x163f86d, 0x1667497, 0x168ea34};

static const RCP_32bit table_crf_fixedQuant_big[] = {
0x16b5973, 0x1827573, 0x197a968, 0x1ab53df, 0x1bdb8ce, 0x1cf0aee, 0x1df7143, 0x1ef0a89,
0x1fdef77, 0x20c3444, 0x219e9b3, 0x2271ddd, 0x233dcb8, 0x2403089, 0x24c2231, 0x257b96f,
0x262fd0e, 0x26df310, 0x278a0c9};
#endif
#endif

static const i32 qp_tbl[2][36] = {
      /*{27, 32, 36, 40, 44, 51, 58, 65, 72, 84, 96, 108, 119, 138, 156, 174, 192, 223, 253, 285, 314, 349, 384, 419, 453, 503, 553, 603, 653, 719, 784, 864, 0x7FFFFFFF},*/
      {16,  19,  23,  27,  32,  36,  40,  44,  51,  58,  65,  72,
       84,  96,  108, 119, 138, 156, 174, 192, 223, 253, 285, 314,
       349, 384, 419, 453, 503, 553, 603, 653, 719, 784, 864, 0x7FFFFFFF},
      /*{26, 38, 59, 96, 173, 305, 545, 0x7FFFFFFF},*/
      {51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34,
       33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16}};


#define LOW_THRESHOLD_PROPORTION (16)
#define HIGH_THRESHOLD_MULTIPLES (2)

/*------------------------------------------------------------------------------
  Local function prototypes
------------------------------------------------------------------------------*/

static i32 InitialQp(vcencRateControl_s *rc, i32 bits, i32 pels);
static void SourceParameter(vcencRateControl_s *rc, i32 nonZeroCnt);
static void PicSkip(vcencRateControl_s *rc);
static void PicQuantLimit(vcencRateControl_s *rc);
static i32 VirtualBuffer(vcencRateControl_s *rc, i32 timeInc, true_e hrd);
static void PicQuant(vcencRateControl_s *rc);
static i32 gop_avg_qp(vcencRateControl_s *rc);
static i32 new_pic_quant(vcencRateControl_s *rc, true_e useQpDeltaLimit);
static i32 get_avg_bits(linReg_s *p, i32 n);
static void update_tables(linReg_s *p, i32 qp, i32 bits);
static void init_hierarchial_rc(vcencRateControl_s *rc);
static void MaAddFrame(rc_ma_s *ma, i32 frameSizeBits);
static i32 Ma(rc_ma_s *ma);
static i32 moving_totalBit_window_minus1(rc_ma_s *ma);
static void MaCharAddFrame(rc_ma_char *ma, u32 intraCu8Num, u32 skipCu8Num,
                           u32 PBFrame4NRdCost);
static void MaChar(rc_ma_char *ma);
static i32 frame_moving_bits(vcencRateControl_s *rc, i32 maxBits);
static void initFramePredictors(vcencRateControl_s *rc);
static RCP_32bit predictSize(rcPredictor *p, RCP_32bit q, RCP_32bit var);
#ifdef RCP_FIXED_POINT_OPT
static RCP_32bit qp2qScale_10(RCP_32bit qp);
#endif

static void updatePredictor(vcencRateControl_s *rc, RCP_32bit q, RCP_32bit var,
	RCP_32bit bits, i32 qp);
static RCP_32bit qScale2qp(RCP_32bit qScale);
static void updateCost(vcencRateControl_s *rc, RCP_64bit var);
static RCP_32bit getNormQp(i32 qp);
static i32 getPass2Qp(vcencRateControl_s *rc);
void checkSceneByCost(vcencRateControl_s *rc);
RCP_32bit qp2qScale(RCP_32bit qp);
i32 getPredLayerId(vcencRateControl_s *rc);
static RCP_32bit predictWindowSize(vcencRateControl_s *rc, RCP_32bit *costs, i32 *frameNum, i32 q, RCP_32bit targetBits);

#if VBR_RC
i32 expectedISliceSize(vcencRateControl_s *rc, i32 qp, double bitsI);
u32 refineISliceQp(vcencRateControl_s *rc, i32 qp, u32 predBitsI, u32 expBitsI);
i32 detectSceneAndTuneQp(vcencRateControl_s *rc, i32 qp, i32 prev_qp);
void updateBitCntBank(vcencRateControl_s *rc);
void reinitFramePredictors(vcencRateControl_s *rc, i32 isHuge);
void checkSvbrState(vcencRateControl_s *rc);
#endif
i32 predictQp(vcencRateControl_s *rc, RCP_32bit *costs, i32 *frameNum,
              RCP_32bit targetBits);
static i32 roundQpToInt(vcencRateControl_s *rc, i32 qp, i32 prevQp);

void crfInit(vcencRateControl_s *rc) {
#if	defined (RCP_FIXED_POINT_OPT) && defined (USE_QP2QSCALE_TABLE)
  i32 mbtree_offset = rc->pass == 2 ? 54 : 0;
  i32 i, n, m;

  m = rc->crf * 10 + mbtree_offset;
  m = CLIP3(0, 510, m);
  n = m / 60;
  i = m - n * 60;

  rc->rateFactorConstant = dqpScaleTable[i] >> n;
#else
  u32 nmb = rc->ctbPerPic * (rc->ctbSize / 16) * (rc->ctbSize / 16);
  u32 baseCplx = nmb * 120;
  double mbtree_offset = rc->pass == 2 ? (1.0 - 0.6) * 13.5 : 0;
  rc->rateFactorConstant =
      pow(baseCplx, 1 - 1) / qp2qScale((double)rc->crf + mbtree_offset);
#endif
}

i32 detectBitrateAbnormal(vcencRateControl_s *rc) {
  i32 ret = 0;

  i32 bpp = rc->virtualBuffer.bitPerPic;
  i32 gopSize = rc->hierarchial_bit_allocation_GOP_size;
  i32 targetGopSize = rc->targetGopSize * rc->encodedGopFrames / gopSize;
  i32 targetGopSizeMin = targetGopSize - targetGopSize / 4;
  i32 targetGopSizeMax = targetGopSize + targetGopSize / 4;
#if 0
  i32 targetSizeMin = rc->gopHeadTargetPicSize - rc->gopHeadTargetPicSize/8;
  i32 targetSizeMax = rc->gopHeadTargetPicSize + rc->gopHeadTargetPicSize/8;
  if (rc->targetPicSize >= targetSizeMax ||
    rc->targetPicSize <= targetSizeMin)
    return 1;
#else

  if (rc->hierarchial_bit_allocation_GOP_size == 8) {
    targetGopSizeMin = targetGopSize - targetGopSize / 2;
    targetGopSizeMax = targetGopSize + targetGopSize / 2;
  }

  if (rc->actualGopSize >= targetGopSizeMax) ret = 1;
  if (rc->actualGopSize <= targetGopSizeMin) ret = 2;
  RC_LOG_I("Abnormal %d: ActualGopSize %d; TargetMin %d; TargetMax %d\n", ret,
        rc->actualGopSize, targetGopSizeMin, targetGopSizeMax);
#endif
  return ret;
}


/*------------------------------------------------------------------------------

      RcGetHierarchicalQP

      Add Hierarchical delta QP for specific layer

------------------------------------------------------------------------------*/
i32 RcGetHierarchicalQP(vcencRateControl_s *rc, i32 qp, i8 predLayerId, i8 checkAbnormal)
{
  //QP in fractional precision
  i32 delta = rc->rcPred[predLayerId].qpDelta;
  checkAbnormal &= rc->hierarchial_bit_allocation_GOP_size >= 8;
  if (checkAbnormal && (predLayerId == LAYER_B || predLayerId >= LAYER_BREF_0)) {
    i8 abnormal = detectBitrateAbnormal(rc);
    if (abnormal == 2 && predLayerId != LAYER_B) {
      //Need to decrease delta for certain layer
      delta >>= 1;
    }
  }

  //Layer BREFs dynamic delta based on motion score
  if (rc->predLayerId != LAYER_P && predLayerId >= LAYER_BREF_0 && rc->sliceTypePrev != I_SLICE) {
    if (rc->curMotionScores[0][0] != -1 && rc->curMotionScores[0][1] != -1 &&
        rc->curMotionScores[1][0] != -1 && rc->curMotionScores[1][1] != -1) {
      u32 maxHorMotion = MAX(rc->curMotionScores[0][0], rc->curMotionScores[1][0]);
      u32 maxVerMotion = MAX(rc->curMotionScores[0][1], rc->curMotionScores[1][1]);

      int decreaseDelta = (maxHorMotion > 255 || maxVerMotion > 70);
      if (decreaseDelta)  delta >>= 1;
    }
  }
  i32 qpTemp = qp + (delta << QP_FRACTIONAL_BITS);
  i32 ret = CLIP3(rc->qpMin, rc->qpMax, qpTemp);

  return ret;
}

/*------------------------------------------------------------------------------

  VCEncInitRc() Initialize rate control.

------------------------------------------------------------------------------*/
bool_e VCEncInitRc(vcencRateControl_s *rc, u32 newStream) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 i;

#if defined(TRACE_RC) && (DBGOUTPUT == fpRcTrc)
  if (!fpRcTrc) fpRcTrc = fopen("rc.trc", "wt");
#endif

  if ((rc->qpMax > (51 << QP_FRACTIONAL_BITS))) {
    return ENCHW_NOK;
  }
  //bitPerPic
  rc->bpp =
      (((i64)rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum)) * 100 +
       rc->picArea / 2) /
      rc->picArea;

  init_hierarchial_rc(rc);

  //CTB_RC
  rc->seqStart = 1;
  rc->vbrOn = 0;
  rc->picSkipAllowed = 1;
  if (rc->pass == 1) {
    rc->rcMode = VCE_RC_CQP;
  } else {
#if 0
    rc->rcMode = VCE_RC_CVBR;
    if (vb->bufferSize && vb->maxBitRate == vb->bitRate)
      rc->rcMode = VCE_RC_CBR;
    else if (rc->vbr)
      rc->rcMode = VCE_RC_VBR;
#endif
  }

  rc->intraIntervalCtr = rc->intraInterval = rc->bitrateWindow;
  /* QP -1: Initial QP estimation done by RC */
  if (rc->qpHdr == (-1 << QP_FRACTIONAL_BITS)) {
    i32 tmp = rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
    if (rc->picRc) {
      rc->qpHdr = InitialQp(rc, tmp, rc->picArea);
      rc->qpHdr = MAX(rc->qpHdr, (QP_DEFAULT << QP_FRACTIONAL_BITS) - rc->intraQpDelta);
    }
    else
      rc->qpHdr = (QP_DEFAULT << QP_FRACTIONAL_BITS);
    PicQuantLimit(rc);
    rc->finiteQp = rc->qpHdr;
    if ((rc->finiteQp) >= (18 << QP_FRACTIONAL_BITS)) {
      rc->finiteQp -= (18 << QP_FRACTIONAL_BITS);
    } else {
      rc->finiteQp = 0;
    }
  } else {
    i32 tmp = rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
    rc->finiteQp = InitialQp(rc, tmp, rc->picArea);
    if ((rc->finiteQp) >= (18 << QP_FRACTIONAL_BITS)) {
      rc->finiteQp -= (18 << QP_FRACTIONAL_BITS);
    } else {
      rc->finiteQp = 0;
    }
  }

  if ((rc->qpHdr > rc->qpMax) || (rc->qpHdr < rc->qpMin)) {
    return ENCHW_NOK;
  }

  /* HRD needs frame RC and macroblock RC*/
  if (rc->hrd == ENCHW_YES) {
    rc->picRc = ENCHW_YES;
  }

  rc->coeffCntMax = rc->ctbPerPic * rc->ctbSize * rc->ctbSize * 3 / 2;
  rc->frameCoded = ENCHW_YES;
  rc->skipGop = ENCHW_NO;
  rc->sliceTypeCur = I_SLICE;
  rc->sliceTypePrev = P_SLICE;

  rc->qpHdrPrev = rc->qpHdr;
  rc->fixedQp = rc->qpHdr;
  rc->qpISlice = rc->qpHdr;

  vb->bitPerPic = rcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
  rc->qpCtrl.checkPointDistance = 0;
  rc->qpCtrl.checkPoints = 0;

  rc->u32PFrameByQPStep = 0;
  rc->qpHdrPrevI = rc->qpHdr;
  rc->u32PrevIBitCnt = 0;

  /* new rate control algorithm */
  /* API parameter is named bitrateWindow but the actual usage is rate controlling
   * window in frames. RC tries to match the target bitrate inside the
   * window. Each window can contain multiple GOPs and the RC adapts to the
   * intra rate by calculating intraInterval. */

  RC_LOG_I("\nInitRc: picRc\t\t%i  hrd\t%i  picSkip\t%i\n", rc->picRc, rc->hrd,
           rc->picSkip);
  RC_LOG_I("  mbRc\t\t\t%i  qpHdr\t%i  Min,Max\t%i,%i\n", rc->ctbRc, rc->qpHdr,
           rc->qpMin, rc->qpMax);
  RC_LOG_I("  checkPointDistance\t%i\n", rc->qpCtrl.checkPointDistance);

  RC_LOG_I("  CPBsize\t%i\n BitRate\t%i\n BitPerPic\t%i\n", vb->bufferSize,
           vb->bitRate, vb->bitPerPic);

  /* If changing QP between frames don't reset GOP RC.
   * Changing bitrate resets RC the same way as new stream. */
  if (!newStream) return ENCHW_OK;

  /* new rate control algorithm */

  /* API parameter is named bitrateWindow but the actual usage is rate controlling
   * window in frames. RC tries to match the target bitrate inside the
   * window. Each window can contain multiple GOPs and the RC adapts to the
   * intra rate by calculating intraInterval. */
  rc->windowLen = rc->bitrateWindow;
  vb->windowRem = rc->bitrateWindow;
  rc->gopMulti = rc->bitrateWindow * rc->outRateDenom / rc->outRateNum;
  if (rc->gopMulti < 1) rc->gopMulti = 1;
  rc->targetPicSize = 0;
  rc->gopHeadTargetPicSize = 0;
  rc->targetGopSize = 0;
  rc->actualGopSize = 0;
  rc->encodedGopFrames = 0;
  rc->frameBitCnt = 0;
#if VBR_RC
  rc->previousGopSize = 0;
  rc->currentGopCost = 0;
  rc->previousGopCost = 0;
  rc->sceneComplexType = NORMAL_SCENE;
  rc->bitCntBank = 0;
#endif
  rc->gopQpSum = 0;
  rc->gopQpDiv = 0;
  rc->gopPQpSum = 0;
  rc->gopPQpNum = 0;

  vb->picTimeInc = 0;
  vb->realBitCnt = 0;
  vb->virtualBitCnt = 0;

  rc->frameCnt = 0;
  rc->idrPoc = 0;

  rc->ma.pos = rc->ma.count = 0;
  rc->ma.frameRateNumer = rc->outRateNum;
  rc->ma.frameRateDenom = rc->outRateDenom;
  if (rc->outRateDenom)
    rc->ma.length = MAX(LEAST_MONITOR_FRAME,
                        MIN(rc->monitorFrames, RC_MOVING_AVERAGE_FRAMES));
  else
    rc->ma.length = RC_MOVING_AVERAGE_FRAMES;

  rc->ma_char.pos = rc->ma_char.count = 0;
  rc->ma_char.length = MAX(LEAST_MONITOR_FRAME, MIN(rc->monitorFrames / 2,
                                                    RC_MOVING_AVERAGE_FRAMES));

  rc->virtualBuffer.movingMaxRate = //rc->f_tolMovingBitRate -> rc->tolMovingBitRate : floating tolMovingBitRate seems no use
      rcCalculate(rc->virtualBuffer.bitRate, 100 + rc->tolMovingBitRate, 100);

  rc->minPicSize = 0;
  rc->maxPicSize = 0;
  rc->encodedFramesBits = 0;

  if (vb->bufferSize) {
    vb->bucketFullness =
        rcCalculate(vb->bufferSize, INITIAL_BUFFER_FULLNESS, 100);
    if (rc->hrd) {
      rc->gDelaySum = rcCalculate(90000, vb->bufferSize, vb->bitRate);
      rc->gInitialDelay = rcCalculate(90000, vb->bucketFullness, vb->bitRate);
      rc->gInitialDoffs = rc->gDelaySum - rc->gInitialDelay;
#ifdef TRACE_RC
      rc->gBufferMin = vb->bufferSize;
      rc->gBufferMax = 0;
#endif
      rc->sei.icrd = (u32)rc->gInitialDelay;
      rc->sei.icrdo = (u32)rc->gInitialDoffs;

      RC_LOG_I("\n InitialDelay\t%i\n Offset\t\t%i\n", rc->gInitialDelay,
               rc->gInitialDoffs);
      rc->intraframeBitCnt = vb->bitPerPic;
    }
    vb->bucketFullness = vb->bucketLevel = vb->bufferSize - vb->bucketFullness;
  }

  rc->complexity = (RCP_64bit)12 * Q31_FIXED_ONE;
  rc->rcPicComplexity =
      (rc->complexity * rc->ctbPerPic * (rc->ctbSize / 8) * (rc->ctbSize / 8))/ Q31_FIXED_ONE;

  rc->resetIntraPicQp = 0;
  rc->inputSceneChange = 0;

  /* init ctbRc for bit rate mode */
  if (IS_CTBRC_FOR_BITRATE(rc->ctbRc)) {
    ctbRateControl_s *pCtbRateCtrl = &(rc->ctbRateCtrl);
    i32 maxQpStep = (1 << 18) - 1;
    i32 maxRowFactor = (1 << 16) - 1;

    for (i = 0; i < 3; i++) pCtbRateCtrl->models[i].xMin = 2048;

    pCtbRateCtrl->rowFactor =
        ((1 << CTB_RC_ROW_FACTOR_FIXP) + rc->ctbCols / 2) / rc->ctbCols;
    pCtbRateCtrl->qpStep = MIN(maxQpStep, pCtbRateCtrl->qpStep);
    pCtbRateCtrl->rowFactor = MIN(maxRowFactor, pCtbRateCtrl->rowFactor);
  }

  rc->rateFactorConstant = (RCP_32bit)0;
  if (rc->pass == 2 && rc->crf >= 0) {
    crfInit(rc);
  }

  memset(rc->preMotionScoreMax, -1, sizeof(rc->preMotionScoreMax));
  memset(rc->curMotionScores, -1, sizeof(rc->curMotionScores));
  rc->sceneMotionType = NORMAL_MOTION;
  rc->qpToSizeStatic = 0;
  initFramePredictors(rc);

  return ENCHW_OK;
}

/*------------------------------------------------------------------------------

  InitialQp()  Returns sequence initial quantization parameter.

------------------------------------------------------------------------------*/
#define OFFSET_QP 0
static i32 InitialQp(vcencRateControl_s *rc, i32 bits, i32 pels) {
  const i32 upscale = 20000;
  i32 i = -1;
  //avoid overflow
  i64 bits64 = (i64)bits;

  /* prevents overflow, QP would anyway be 17 with this high bitrate
     for all resolutions under and including 1920x1088 */
  //    if (bits > 1000000)
  //      return 17;

  /* Make room for multiplication */
  pels >>= 8;
  bits64 >>= 5;

  /* Use maximum QP if bitrate way too low. */
  if (!bits64) return 51 << QP_FRACTIONAL_BITS;

  /* Adjust the bits value for the current resolution */
  bits64 *= pels + 250;
  ASSERT(pels > 0);
  ASSERT(bits64 > 0);
  bits64 /= 350 + (3 * pels) / 4;
  bits64 = rcCalculate((i32)bits64, upscale, pels << 6);

  while ((qp_tbl[0][++i] - OFFSET_QP) < bits64) (void)1;

  RC_LOG_I("BPP\t\t%ld\n", bits64);

  i32 retQp = qp_tbl[1][i];
  RC_LOG_I(" Original init QP %d\n", retQp);

  if (rc->hieQpDeltaEnable){
    /*Above initial QP based mainly on 60 intra pic rate,
      need to adjust based on intra pic rate. */
    i32 delta = -1;
    retQp += delta;
    /* Increase less for larger QP, increase more for small QP */
    if (rc->intraInterval <= 15) {
      static const i32 ir15qpTable[36] = 
      {51, 51, 50, 49, 48, 47, 46, 45, 44, 43, 43, 42, 41, 40, 39, 38, 37, 36,
       35, 34, 34, 33, 32, 32, 31, 30, 29, 28, 28, 27, 26, 25, 24, 23, 22, 21};
       retQp = ir15qpTable[i];
    } else if(rc->intraInterval <= 30) {
      static const i32 ir30qpTable[36] = 
      {51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 39, 38, 37, 36, 35,
       34, 33, 32, 31, 30, 30, 29, 28, 27, 26, 26, 25, 24, 23, 22, 21, 20, 19};
      retQp = ir30qpTable[i];
    }
  }

  return retQp << QP_FRACTIONAL_BITS;
}

/*------------------------------------------------------------------------------

      HevcFillerRc

      Stream watermarking. Insert filler NAL unit of certain size after each
      Nth frame.

------------------------------------------------------------------------------*/
u32 HevcFillerRc(vcencRateControl_s *rc, u32 frameCnt) {
  const u8 filler[] = {0, 9, 0, 9, 9, 9, 0, 2, 2, 0};
  u32 idx;

  if (rc->fillerIdx == (u32)(-1)) {
    rc->fillerIdx = sizeof(filler) / sizeof(*filler) - 1;
  }

  idx = rc->fillerIdx;
  if (frameCnt != 0 && ((frameCnt % 128) == 0)) {
    idx++;
  }
  idx %= sizeof(filler) / sizeof(*filler);

  if (idx != rc->fillerIdx) {
    rc->fillerIdx = idx;
    return filler[idx] + 1;
  }
  return 0;
}
/*------------------------------------------------------------------------------
  VirtualBuffer()  Return difference of target and real buffer fullness.
  Virtual buffer and real bit count grow until one second.  After one second
  output bit rate per second is removed from virtualBitCnt and realBitCnt. Bit
  drifting has been taken care.

  If the leaky bucket in VBR mode becomes empty (e.g. underflow), those R * T_e
  bits are lost and must be decremented from virtualBitCnt. (NOTE: Drift
  calculation will mess virtualBitCnt up, so the loss is added to realBitCnt)
------------------------------------------------------------------------------*/
static i32 VirtualBuffer(vcencRateControl_s *rc, i32 timeInc, true_e hrd) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 target;

  vb->picTimeInc += timeInc;

  /* picTimeInc must be in range of [0, timeScale) */
  while (vb->picTimeInc >= vb->timeScale) {
    vb->picTimeInc -= vb->timeScale;
    if (vb->realBitCnt < (I32_MIN + vb->bitRate))
      vb->realBitCnt = I32_MIN;
    else
      vb->realBitCnt -= vb->bitRate;

    if (vb->bucketLevel < (I32_MIN + vb->bitRate))
      vb->bucketLevel = I32_MIN;
    else
      vb->bucketLevel -= vb->bitRate;

    vb->seconds++;
    vb->averageBitRate = vb->bitRate + vb->realBitCnt / vb->seconds;
  }
  vb->virtualBitCnt = rcCalculate(vb->bitRate, vb->picTimeInc, vb->timeScale);
  //printf("vb->virtualBitCnt=%d,vb->picTimeInc=%d,vb->timeScale=%d,vb->bitRate=%d\n",vb->virtualBitCnt,vb->picTimeInc,vb->timeScale,vb->bitRate);

  if (vb->bufferSize) {
    //// Qunying: Just for debug, to remove
    i32 bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
    RC_LOG_I(
        "[RC Frm%d]CPB Status: realBits %d virtualBits %d Fullness %d, ratio "
        "%f\n",
        rc->frameCnt, vb->realBitCnt, vb->virtualBitCnt, bucketFullness,
        bucketFullness * 1.0 / vb->bufferSize);
    ////

    if (hrd && RC_CBR_HRD) {
      /* In CBR mode, bucket _must not_ underflow. Insert filler when
       * needed. */
      vb->bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
    } else {
      vb->bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
      if (vb->bucketFullness < 0) {
        RC_LOG_I(
            "[RC Frm%d]CPB Underflow: realBits %d virtualBits %d Fullness %d\n",
            rc->frameCnt, vb->realBitCnt, vb->virtualBitCnt,
            vb->bucketFullness);

        /* encoder buffer underflow */
        //vb->realBitCnt += vb->virtualBitCnt - vb->bucketLevel;
        vb->bucketLevel = vb->virtualBitCnt;
        vb->bucketFullness = 0;
      } else if (vb->bucketFullness > vb->bufferSize) {
        RC_LOG_I(
            "[RC Frm%d]CPB Overflow: realBits %d virtualBits %d Fullness %d "
            "bufferSize %d\n",
            rc->frameCnt, vb->realBitCnt, vb->virtualBitCnt, vb->bucketFullness,
            vb->bufferSize);

        /* encoder buffer overflow */
        vb->bucketLevel = vb->virtualBitCnt + vb->bufferSize;
        vb->bucketFullness = vb->bufferSize;
      }
    }
  }

  /* Saturate realBitCnt, this is to prevent overflows caused by much greater
     bitrate setting than is really possible to reach */
  if (vb->realBitCnt > 0x1FFFFFFF) vb->realBitCnt = 0x1FFFFFFF;
  if (vb->realBitCnt < -0x1FFFFFFF) vb->realBitCnt = -0x1FFFFFFF;

  target = vb->virtualBitCnt - vb->realBitCnt;
  i32 bitSpaceThrd =
      vb->bitRate * 3 / 4;  // keep bits buffering space at least bitSpaceThrd
  rc->vbrOn = target >= bitSpaceThrd;

  //printf("target=%d,vb->realBitCnt=%d\n",target,vb->realBitCnt);
  /* Saturate target, prevents rc going totally out of control.
     This situation should never happen. */
  if (target > 0x1FFFFFFF) target = 0x1FFFFFFF;
  if (target < -0x1FFFFFFF) target = -0x1FFFFFFF;

  RC_LOG_I("virtualBitCnt:\t%d  realBitCnt:\t%d  bitPerPic:\t%d \n",
           vb->virtualBitCnt, vb->realBitCnt, vb->bitPerPic);
  RC_LOG_I("  diff bits:\t%d  avg bitrate:\t%d\n", target, vb->averageBitRate);
  return target;
}


/*------------------------------------------------------------------------------

      RcMotionTypeDetect

      Detect motion type by current and previous motion scores for GOP1

------------------------------------------------------------------------------*/
void RcMotionTypeDetect(vcencRateControl_s *rc, i32 bitCnt, i32 prevFrameBitCnt)
{
  // Detect motion type based on motion score
  //Only work on GOP1 and 1 pass
  if (rc->pass != 0 || rc->hierarchial_bit_allocation_GOP_size != 1) {
    memset(rc->preMotionScoreMax, -1, sizeof(rc->preMotionScoreMax));
    rc->sceneMotionType = NORMAL_MOTION;
    rc->qpToSizeStatic = 0;
  }
  else if (rc->predLayerId != LAYER_I){
    rc->qpToSizeStatic = 0;
    u32 motionMax = MAX(rc->curMotionScores[0][0], rc->curMotionScores[0][1]);
    if (rc->predLayerId != LAYER_P) {
      motionMax = MAX(motionMax, MAX(rc->curMotionScores[1][0], rc->curMotionScores[1][1]));
    }
    if (motionMax == -1){
      /* Meaning motion score is not supported */
      rc->sceneMotionType = NORMAL_MOTION;
    }
    else {
      motion_type newType = NORMAL_MOTION;
      double preAvgMotionScoreMax = 0; //Init to 0, so that first P frame can tell STATIC or not
      u8 codedIdx = 0;
      for (int i = 3; i >= 0; i--) {
        if (rc->preMotionScoreMax[i] == -1)
          break;
        preAvgMotionScoreMax += rc->preMotionScoreMax[i];
        codedIdx ++;
      }
      double costThresh = (double)(50 - CLIP3(0, 35, rc->bpp * 5))/100.0;
      //costThresh = 0.2;
      preAvgMotionScoreMax = (codedIdx == 0) ? 0 : (preAvgMotionScoreMax / codedIdx);
      if (preAvgMotionScoreMax < 0.25 && motionMax < 1 && rc->PBFrame4NRdCost <= costThresh * rc->picArea)
        newType = STATIC_MOTION;
      else if(codedIdx > 3) {
        if (preAvgMotionScoreMax < 0.25 && motionMax > 30)
          newType = STATIC_TO_LARGE_MOTION;
        else if (rc->sceneMotionType == STATIC_TO_LARGE_MOTION && motionMax > 30 && motionMax >= rc->preMotionScoreMax[1] && rc->preMotionScoreMax[0] < 1)
          newType = STATIC_TO_LARGE_MOTION;
        else if (motionMax < 1 && rc->PBFrame4NRdCost <= costThresh * rc->picArea) {
          if (preAvgMotionScoreMax > 30)
            newType = LARGE_TO_STATIC_MOTION;
          else if (rc->sceneMotionType == LARGE_TO_STATIC_MOTION && rc->preMotionScoreMax[0] != -1 && rc->preMotionScoreMax[0] > 30)
            newType = LARGE_TO_STATIC_MOTION;
        }
      }

      RC_LOG_I("Motion Type %d; Prev Motion Score %d %d %d %d\n", newType, rc->preMotionScoreMax[0],
        rc->preMotionScoreMax[1], rc->preMotionScoreMax[2], rc->preMotionScoreMax[3]);

      rc->preMotionScoreMax[0] = rc->preMotionScoreMax[1];
      rc->preMotionScoreMax[1] = rc->preMotionScoreMax[2];
      rc->preMotionScoreMax[2] = rc->preMotionScoreMax[3];
      rc->preMotionScoreMax[3] = motionMax;
      rc->sceneMotionType = newType;

      if (rc->sliceTypePrev != I_SLICE && (rc->qpHdrPrev >> QP_FRACTIONAL_BITS) == (rc->qpHdr >> QP_FRACTIONAL_BITS)) {
        if (ABS(bitCnt - prevFrameBitCnt) < prevFrameBitCnt * 0.15)
          rc->qpToSizeStatic = 1;
      }
    }
  }
}

/*------------------------------------------------------------------------------
  VCEncAfterPicRc()  Update source model, bit rate target error and linear
  regression model for frame QP calculation. If HRD enabled, check leaky bucket
  status and return RC_OVERFLOW if coded frame must be skipped. Otherwise
  returns number of required filler payload bytes.
------------------------------------------------------------------------------*/
i32 VCEncAfterPicRc(vcencRateControl_s *rc, u32 nonZeroCnt, u32 byteCnt,
                        u32 qpSum, u32 qpNum) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bitPerPic = rc->virtualBuffer.bitPerPic;
  i32 tmp, stat, bitCnt = (i32)byteCnt * 8;
  i32 movingBitRate, bestBits, movingBitBase, realBitCnt, targetBitCnt;
  i32 imin, imax, i;
  i32 algNum = 0;
  i32 bitCntError;
  i32 prevFrameBitCnt = rc->frameBitCnt;
  rc->qpSum = (i32)qpSum;
  rc->qpNum = (i32)qpNum;

#ifndef RCP_FIXED_POINT_OPT
  rc->averageQp =
      qpNum
          ? (float)qpSum / qpNum
          : (float)qpSum / (rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 8 / 8);
#else
  rc->averageQp =
      qpNum
          ? qpSum / qpNum
          : (u64)qpSum * 8 * 8 / rc->ctbPerPic * rc->ctbSize * rc->ctbSize;
#endif
//  printf("average QP= %f\n",(float)rc->averageQp);
  rc->frameBitCnt = bitCnt;
  rc->nonZeroCnt = nonZeroCnt;
  rc->gopBitCnt += bitCnt;
  rc->frameCnt++;
  rc->encodedFramesBits += bitCnt;
  rc->rcPred[rc->predLayerId].qpDeltaPrev = rc->rcPred[rc->predLayerId].qpDelta;

  /* new GOP start, reset some GOP related variables */
  if (rc->predLayerId == LAYER_P || rc->predLayerId == LAYER_I) {
    rc->previousGopSize = rc->actualGopSize;
    rc->gopHeadTargetPicSize = rc->targetPicSize;
    rc->actualGopSize = 0;
    rc->encodedGopFrames = 0;
  }
  rc->actualGopSize += bitCnt;
  rc->encodedGopFrames++;

  if (rc->pass == 1)
    // pass 1 uses constant qp10, it needn't update rc model etc.
    return 0;

  RcMotionTypeDetect(rc, bitCnt, prevFrameBitCnt);

  if (IS_CTBRC_FOR_BITRATE(rc->ctbRc)) {
    if (rc->ctbRateCtrl.qpSumForRc) {
#ifdef RCP_FIXED_POINT_OPT
      RCP_32bit avgQpForRc = ((rc->ctbRateCtrl.qpSumForRc * (1 << QP_FRACTIONAL_BITS) + rc->ctbPerPic/2) / rc->ctbPerPic);  //+0.5
#else
      float avgQpForRc = (rc->ctbRateCtrl.qpSumForRc * 1.0 / rc->ctbPerPic) *
                         (1 << QP_FRACTIONAL_BITS);
#endif

      i32 fracQp = rc->qpHdrPrev & ((1 << QP_FRACTIONAL_BITS) - 1);

      /* smooth the delta for more stable RC & quality */
      i32 delta = ABS((i32)avgQpForRc - (rc->qpHdrPrev-fracQp)) >> (QP_FRACTIONAL_BITS+1);
      delta <<= QP_FRACTIONAL_BITS;
      rc->qpHdrPrev += (avgQpForRc >= rc->qpHdrPrev) ? delta : (-delta);
      rc->qpHdrPrev = CLIP3(0, 51 << QP_FRACTIONAL_BITS, rc->qpHdrPrev);
    }

    i32 idxType = rc->predLayerId < LAYER_BREF_0 ? rc->predLayerId : LAYER_B;
    i32 coreIdx = (rc->frameCnt-1)%rc->coreNum;
    
    //Get which pred models is used for the current encoding frame
    i32 preMemIdx = rc->ctbRateCtrl.Cur[coreIdx].ctbRcModelsIdx;
    // rc->ctbRateCtrl.models[preMemIdx].refCount = rc->ctbRateCtrl.models[preMemIdx].refCount - 1;

    //To get which pred model can be updated, the first three are fixed,B/P/I, and the last three are flexible
    i32 UpdatePreMemIdx = rc->ctbRateCtrl.updateModelIdx;
    rc->ctbRateCtrl.models[UpdatePreMemIdx].frameCnt = rc->frameCnt-1;
    rc->ctbRateCtrl.models[UpdatePreMemIdx].idxType = rc->ctbRateCtrl.models[preMemIdx].idxType;
    rc->ctbRateCtrl.models[UpdatePreMemIdx].valid = 1;
    rc->ctbRateCtrl.models[UpdatePreMemIdx].refCount = 0;
    SWAP(rc->ctbRateCtrl.Cur[coreIdx].ctbMemCurAddr,
         rc->ctbRateCtrl.models[UpdatePreMemIdx].ctbMemPreAddr, ptr_t);
    SWAP(rc->ctbRateCtrl.Cur[coreIdx].ctbMemCurVirtualAddr,
         rc->ctbRateCtrl.models[UpdatePreMemIdx].ctbMemPreVirtualAddr, u32 *);
  }

  /* 1-pass uses the fixed picArea as cost, just tune the coeff in rc mode.
     update cost statistic with 4Ncost for CVBR tuning (2-pass do this in VCEncBeforePicRc) */
  if (rc->pass != 2) {
    rc->pass1CurCost = rc->picArea;
    if (rc->predLayerId != LAYER_I) updateCost(rc, (RCP_64bit)rc->PBFrame4NRdCost);
  }

#ifdef RCP_FIXED_POINT_OPT
  updatePredictor(rc, qp2qScale_10(getNormQp(rc->qpHdrPrev)), rc->pass1CurCost,
                  rc->frameBitCnt, rc->qpHdrPrev);
#else
  updatePredictor(rc, qp2qScale(getNormQp(rc->qpHdrPrev)), rc->pass1CurCost,
                  rc->frameBitCnt, rc->qpHdrPrev);
#endif
  if (rc->predLayerId == LAYER_P) {
    rc->gopPQpSum += rc->qpHdrPrev;
    rc->gopPQpNum++;
  }

  /*Update I frame average qp and complexity info in CRF mode*/
  if (rc->pass == 2 && rc->crf >= 0) {
    //printf("Average QP used by CMODEL: %f\n", rc->averageQp);
    if (rc->sliceTypeCur == I_SLICE) {
      int intQp = rc->qpHdr >> QP_FRACTIONAL_BITS;
#ifndef RCP_FIXED_POINT_OPT
      double fracQp = (rc->qpHdr & 255) / (1 << QP_FRACTIONAL_BITS);
      double realQp = intQp + fracQp;
#endif
      if (rc->crf_iCostAvg > 0) {
        rc->crf_iCostAvg =
            (rc->crf_iCostAvg * rc->crf_iFrames + (int)(rc->pass1CurCost)) /
            (rc->crf_iFrames + 1);

#ifndef RCP_FIXED_POINT_OPT
        rc->crf_iQpAvg =
            (rc->crf_iQpAvg * rc->crf_iFrames + realQp) / (rc->crf_iFrames + 1);
#endif
      } else {
        rc->crf_iCostAvg = (int)(rc->pass1CurCost);
#ifndef RCP_FIXED_POINT_OPT
		rc->crf_iQpAvg = realQp;
#endif
      }
      rc->crf_iFrames++;
    }
  }

  if (rc->sliceTypeCur != I_SLICE) {
    rc->u32PFrameByQPStep = (2 * rc->frameBitCnt *
                             (q_step[(rc->qpHdr * 10) >> QP_FRACTIONAL_BITS] >>
                              QP_FRACTIONAL_BITS)) >>
                            2;
  } else {
    rc->qpHdrPrevI = rc->qpHdrPrev;
    rc->u32PrevIBitCnt = bitCnt;
  }

  if (rc->targetPicSize) {
    tmp = ((bitCnt - rc->targetPicSize) * 100) / rc->targetPicSize;
  } else {
    tmp = 0;
  }
  RC_LOG_I("\nAFTER PIC RC:\n");

  RC_LOG_I(
      "RC QP(qpHdr %2.2f avgQp %2.2f) BITS %d  BitErr/target\t%7i%%  "
      "BitErr/avg\t%7d%%\n",
      rc->qpHdr / 256.0, (float)rc->averageQp, bitCnt, tmp,
      ((bitCnt - bitPerPic) * 100) / (bitPerPic + 1));

  MaAddFrame(&rc->ma, bitCnt);

  if (rc->sliceTypeCur != I_SLICE) {
    MaCharAddFrame(&rc->ma_char, rc->intraCu8Num, rc->skipCu8Num,
                   rc->PBFrame4NRdCost);

    /* trace the QP over GOP, excluding Intra QP */
    rc->gopQpSum += rc->qpHdrPrev;
    rc->gopQpDiv++;
  }

  /* Update rcPred previous info */
  rc->rcPred[rc->predLayerId].qpPrev = rc->qpHdrPrev;
  rc->rcPred[rc->predLayerId].prevFrameBits = rc->frameBitCnt;
  rc->rcPred[rc->predLayerId].prevTargetBits = rc->targetPicSize;

  /* Scene change process. Clean RC Model for P frames. */
  /* clean scene change related flags */
  rc->inputSceneChange = 0;
  if (rc->sliceTypeCur == I_SLICE) rc->resetIntraPicQp = 0;

  /* Calculate the source parameter only for INTER frames */
  if (rc->sliceTypeCur != I_SLICE) SourceParameter(rc, rc->nonZeroCnt);

  /* Post-frame skip if HRD buffer overflow */
  if (rc->picSkipAllowed && (rc->hrd == ENCHW_YES) &&
      (bitCnt > (vb->bufferSize - vb->bucketFullness))) {
    RC_LOG_I("Be: %7i  ", vb->bucketFullness);
    RC_LOG_I("fillerBits %5i  ", 0);
    RC_LOG_I("bitCnt %d  spaceLeft %d  ", bitCnt,
             (vb->bufferSize - vb->bucketFullness));
    RC_LOG_I("bufSize %d  bucketFullness %d  bitPerPic %d\n", vb->bufferSize,
             vb->bucketFullness, bitPerPic);
    RC_LOG_I("HRD overflow, frame discard\n");
    rc->frameCoded = ENCHW_NO;

    /*update sliceTypePre even HRD overflow, otherwise,
      if IFrame overflow happen, next followed IFrame has no chance to use RC decide new qp in PicQuant()
       then leading to HRD overflow forever
     */
    rc->sliceTypePrev = rc->sliceTypeCur;

    return VCENCRC_OVERFLOW;
  } else {
    i32 vbBitCnt = bitCnt;
    /* reset the bits accumulation status for --vbr mode when */
    /* get best quality with average bitrate and instant bitrate both small */
    if ((rc->rcMode == VCE_RC_VBR || rc->vbr) && rc->vbrOn &&
        (rc->qpHdr <= rc->qpMin) && (vbBitCnt < rc->virtualBuffer.bitPerPic)) {
      vbBitCnt = rc->virtualBuffer.bitPerPic;
    }

    vb->bucketLevel = CLIP_I32_ADD(vb->bucketLevel, vbBitCnt);
    vb->bucketFullness = CLIP_I32_ADD(vb->bucketFullness, vbBitCnt);
    vb->realBitCnt = CLIP_I32_ADD(vb->realBitCnt, vbBitCnt);
  }

  rc->sliceTypePrev = rc->sliceTypeCur;
  RC_LOG_I("plot\t%4i\t%4i\t%8i\t%8i\t%8i\t%8i\t%8i\t%8i\t%8i\n", rc->frameCnt,
           rc->qpHdr, rc->targetPicSize, bitCnt, bitPerPic, rc->gopAvgBitCnt,
           vb->realBitCnt - vb->virtualBitCnt, vb->bitRate, vb->bucketFullness);
#if VBR_RC
  updateBitCntBank(rc);
#endif

  if (vb->bufferSize == 0) return 0;

  tmp = 0;
  u8 bFiller =
      (rc->hrd == ENCHW_YES && RC_CBR_HRD) || (rc->fillerData == ENCHW_YES);
  if (bFiller) {
    /* Bits needed to prevent bucket underflow */
    tmp = bitPerPic - vb->bucketFullness;
    if (tmp > 0) {
      tmp = (tmp + 7) / 8;
      vb->bucketFullness += tmp * 8;
      vb->realBitCnt += tmp * 8;
      vb->bucketLevel += tmp * 8;
    }
    RC_LOG_I("fillerBits %5i\n", tmp * 8);
  }

  if (rc->hrd == ENCHW_YES) {
    /* Update Buffering Info */
    stat = vb->bufferSize - vb->bucketFullness;

    rc->gInitialDelay = rcCalculate(90000, stat, vb->bitRate);
    rc->gInitialDoffs = rc->gDelaySum - rc->gInitialDelay;

    rc->sei.icrd = (u32)rc->gInitialDelay;
    rc->sei.icrdo = (u32)rc->gInitialDoffs;

    RC_LOG_I("initialDelay: %5i  ", rc->gInitialDelay);
    RC_LOG_I("initialDoffs: %5i\n", rc->gInitialDoffs);
    RC_LOG_I("Be: %7i  ", vb->bucketFullness);
  }

#ifdef TRACE_RC
  if (vb->bucketFullness < rc->gBufferMin) {
    rc->gBufferMin = vb->bucketFullness;
  }
  if (vb->bucketFullness > rc->gBufferMax) {
    rc->gBufferMax = vb->bucketFullness;
  }
  RC_LOG_I("\nLeaky Bucket Min: %i (%d%%)  Max: %i (%d%%)\n", rc->gBufferMin,
           rc->gBufferMin * 100 / vb->bufferSize, rc->gBufferMax,
           rc->gBufferMax * 100 / vb->bufferSize);
#endif
  return tmp;
}

void initDeltaLimit(vcencRateControl_s *rc, int gopSize){
  if (gopSize > 8) {
    rc->picQpDeltaMin = -10;
    rc->picQpDeltaMax = 10;
  } else if (gopSize > 4) {
    rc->picQpDeltaMin = -8;
    rc->picQpDeltaMax = 8;
  } else {
    rc->picQpDeltaMin = -4;
    rc->picQpDeltaMax = 4;
  }
}

static void init_hierarchial_rc(vcencRateControl_s *rc) {
  if (rc->hierarchial_bit_allocation_GOP_size < 1)
    rc->hierarchial_bit_allocation_GOP_size = 1;

  //Currently only support default GOP structure, need to fix for customized
  i32 gopSize = rc->hierarchial_bit_allocation_GOP_size;
  rc->gopLayerFrameNum[LAYER_I] = 0;
  rc->gopLayerFrameNum[LAYER_P] = 1;
  if (!rc->hieQpDeltaEnable) {
    rc->gopLayerFrameNum[LAYER_BREF_0] = gopSize > 2;
    rc->gopLayerFrameNum[LAYER_B] = gopSize - 1 - (gopSize > 2);
  }
  else {
    //Setup number of frames in different layer in GOP
    switch(gopSize) {
      case 2:
        rc->gopLayerFrameNum[LAYER_B] = 1;
        break;
      case 3:
        rc->gopLayerFrameNum[LAYER_B] = 1;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        break;
      case 4:
        rc->gopLayerFrameNum[LAYER_B] = 2;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        break;
      case 5:
        rc->gopLayerFrameNum[LAYER_B] = 2;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        rc->gopLayerFrameNum[LAYER_BREF_1] = 1;
        break;
      case 6:
        rc->gopLayerFrameNum[LAYER_B] = 2;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        rc->gopLayerFrameNum[LAYER_BREF_1] = 2;
        break;
      case 7:
        rc->gopLayerFrameNum[LAYER_B] = 3;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        rc->gopLayerFrameNum[LAYER_BREF_1] = 2;
        break;
      case 8:
        rc->gopLayerFrameNum[LAYER_B] = 4;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        rc->gopLayerFrameNum[LAYER_BREF_1] = 2;
        break;
      case 16:
        rc->gopLayerFrameNum[LAYER_B] = 8;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 1;
        rc->gopLayerFrameNum[LAYER_BREF_1] = 2;
        rc->gopLayerFrameNum[LAYER_BREF_2] = 4;
        break;
      default: 
        ASSERT(gopSize == 1);
        rc->gopLayerFrameNum[LAYER_B] = 0;
        rc->gopLayerFrameNum[LAYER_BREF_0] = 0;
        rc->gopLayerFrameNum[LAYER_BREF_1] = 0;
        rc->gopLayerFrameNum[LAYER_BREF_2] = 0;
    }
    //Setup hierarchical delta for each layer
    if (gopSize == 1) {
      //No Hierarchical structure, need no qp delta
    }
    else if (gopSize <= 4) {
      rc->rcPred[LAYER_BREF_0].qpDelta = 2;
      rc->rcPred[LAYER_BREF_1].qpDelta = 6;//No affect
      rc->rcPred[LAYER_BREF_2].qpDelta = 6;//No affect
      rc->rcPred[LAYER_B].qpDelta = 6;

      if (rc->picRc == ENCHW_NO)
        rc->rcPred[LAYER_P].qpDelta = -1;
    }
    else if (gopSize <= 8) {
      rc->rcPred[LAYER_BREF_0].qpDelta = 5;
      rc->rcPred[LAYER_BREF_1].qpDelta = 6;
      rc->rcPred[LAYER_BREF_2].qpDelta = 7;//No affect
      rc->rcPred[LAYER_B].qpDelta = 8;

      if (rc->picRc == ENCHW_NO)
        rc->rcPred[LAYER_P].qpDelta = -2;
    }
    else {
      ASSERT(gopSize == 16);
      //Not Tested Yet
      rc->rcPred[LAYER_BREF_0].qpDelta = 4;
      rc->rcPred[LAYER_BREF_1].qpDelta = 6;
      rc->rcPred[LAYER_BREF_2].qpDelta = 7;
      rc->rcPred[LAYER_B].qpDelta = 7;

      if (rc->picRc == ENCHW_NO)
        rc->rcPred[LAYER_P].qpDelta = -4;
    }
    initDeltaLimit(rc, rc->hierarchial_bit_allocation_GOP_size);
  }

  rc->hierarchial_bit_allocation_GOP_size_previous =
        rc->hierarchial_bit_allocation_GOP_size;
}

void crfPicFixedQuant(vcencRateControl_s *rc) {
#define BASE_FRAME_DURATION 0.04

#if	defined (RCP_FIXED_POINT_OPT) && defined (USE_QP2QSCALE_TABLE)
  i32 q, qp;  //qscale
  i32 fps = (2 * rc->outRateNum + rc->outRateDenom) / (2 * rc->outRateDenom);

   // Scale and units are obtained from rateNum and rateDenom for videos with fixed frame rates.
  if (fps < 5) fps = 5;

  if (fps > 240) fps = 240;

  if (fps < 60)
  	q = table_crf_fixedQuant[fps - 5];
  else
  	q = table_crf_fixedQuant_big[(fps + 5) / 10 - 6];

  // avoid NaN's in the Rceq
  q = ((i64)q * LL_FIXED) / rc->rateFactorConstant;

  qp = qScale2qp(q);

  rc->qpHdr = qp;

#else
  double q, qp;  //qscale
  double fps = (double)rc->outRateNum / rc->outRateDenom;
  // Scale and units are obtained from rateNum and rateDenom for videos with fixed frame rates.
  q = pow(BASE_FRAME_DURATION * fps, 1.0 - 0.6);

  // avoid NaN's in the Rceq
  q /= rc->rateFactorConstant;
  qp = qScale2qp(q);

  rc->qpHdr = qp * (1 << QP_FRACTIONAL_BITS);
#endif
}

i32 AdjustQpByCpbStatus(vcencRateControl_s *rc, i32 qp) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bufferFill = vb->bucketFullness;  // encoder buffer fullness (Fe)
  i32 maxBitRate = vb->maxBitRate;      // peak transmission bit rate (R)
  i32 bufferSize = vb->bufferSize;      // buffer size (B)
  i32 bufferRate = vb->bufferRate;
  i32 isCbr = maxBitRate <= vb->bitRate && rc->crf < 0;
  i32 qpBase = qp, bits;
  RCP_32bit targetMaxFill = bufferSize, targetMinFill = 0;
  i32 terminate = 0;
  i32 nframes = 0;

  /* Only for I/P frames with cpb buffer */
  if (bufferSize == 0 || rc->predLayerId == LAYER_B || rc->predLayerId >= LAYER_BREF_0) return qp;

  qp >>= QP_FRACTIONAL_BITS;
  while (terminate != 3 && qp >= 0 && qp <= 51) {
    nframes = 1;

    RCP_32bit cost = rc->pass == 2 ? rc->pass1CurCost : rc->picArea;
    RCP_32bit bits =
        predictSize(&(rc->rcPred[rc->predLayerId]), qp2qScale((RCP_32bit)qp), cost);
    RCP_32bit buffer_fill_cur = bufferFill + bits - bufferRate;
    if (rc->predLayerId == LAYER_P) {
      if (rc->pass == 2) {
        //2 pass do not support separate predictor for each layer
        i32 bframes = MAX(0, rc->hierarchial_bit_allocation_GOP_size - 1);
        i32 predId = bframes > 1 ? LAYER_BREF_0 : LAYER_B;
        while (bframes && buffer_fill_cur >= 0 && buffer_fill_cur <= bufferSize) {
          nframes++;
          cost = rc->pass1GopCost[predId];
          //2 PASS delta should be 0 now
          RCP_32bit qpB = qp;//RcGetHierarchicalQP(rc, qp, predId);
          bits = predictSize(&(rc->rcPred[predId]), qp2qScale(qpB), cost);
          buffer_fill_cur += (bits - bufferRate);
  
          predId = B_SLICE;
          bframes--;
        }
      } else {
        for (int i = LAYER_BREF_0; i < 7; i++) {
          int id = (i == 6)? LAYER_B : i;
          if (rc->gopLayerFrameNum[id] == 0)
            continue;
          for (int j = 0; j < rc->gopLayerFrameNum[id]; j++) {
            if (buffer_fill_cur < 0 || buffer_fill_cur > bufferSize) break;
            nframes++;
            i32 qpBtemp = RcGetHierarchicalQP(rc, qp << QP_FRACTIONAL_BITS, id, 0);
            RCP_32bit qpB = (RCP_32bit)(qpBtemp >> QP_FRACTIONAL_BITS);
            bits = predictSize(&(rc->rcPred[id]), qp2qScale(qpB), cost);
            buffer_fill_cur += (bits - bufferRate);
          }
          if (buffer_fill_cur < 0 || buffer_fill_cur > bufferSize) break;
        }
      }
    }

#ifdef RCP_FIXED_POINT_OPT
    i32 bufferSizeMin = (rc->predLayerId == LAYER_I) ? ((bufferSize*9+5)/10) : ((bufferSize+1)/2);
    targetMaxFill = CLIP3(bufferSizeMin, bufferSize,
                        bufferFill - nframes * bufferRate / 2);
#else
    double overTh = rc->predLayerId == LAYER_I ? 0.9 : 0.5;
    targetMaxFill = CLIP3(bufferSize * overTh, bufferSize,
                        bufferFill - nframes * bufferRate / 2);
#endif
    if (buffer_fill_cur > targetMaxFill) {
      //avoid overflow, decrease frame bits
      qp++;
      terminate |= 1;
      continue;
    }

    targetMinFill = (RCP_32bit)MIN((RCP_32bit)bufferSize * 0, bufferFill + nframes * bufferRate / 2);
    if (isCbr && buffer_fill_cur < targetMinFill) {
      // if cbr, need to avoid underflow. However VBR mode allows underflow by spec HRD part.
      qp--;
      terminate |= 2;
      continue;
    }
    break;
  }
  qp <<= QP_FRACTIONAL_BITS;

  /* adjust target size by cpb status */
  if (nframes && rc->predLayerId == P_SLICE) {
    i32 targetMaxBits = bufferRate * nframes + targetMaxFill - bufferFill;
    i32 targetMinBits = bufferRate * nframes + targetMinFill - bufferFill;
    i32 oldGopBits = rc->targetGopSize;
    rc->targetGopSize = CLIP3(targetMinBits, targetMaxBits, rc->targetGopSize);

    if (rc->targetGopSize != oldGopBits)
      rc->targetPicSize = rc->targetGopSize / nframes;
  }

  if (!isCbr) {
    /* vbv_max_rate > bitrate --> channel transmit speed > actual streaming speed
        --> cpb buffer may underflow since we use cpb_max_rate (> actual streaming speed) to calc cpb status
        --> cpb buffer fill is over-estimated (but actually not so much fill)
        --> frame bits should be small --> q needs a big value
      */
    qp = MAX(qp, qpBase);
  }

  i32 minQp =
      rc->rcPred[rc->predLayerId].qpPrev + (rc->picQpDeltaMin << QP_FRACTIONAL_BITS);
  qp = MAX(minQp, qp);

  /* limit qp adjustment for 1-pass because frame cost is not estimated to predict frame size */
  if (rc->pass != 2) {
    i32 qpDeltaTh = 2;
    minQp = qpBase - (qpDeltaTh << QP_FRACTIONAL_BITS);
    qp = MAX(minQp, qp);

    i32 maxQp = qpBase + (qpDeltaTh << QP_FRACTIONAL_BITS);
    qp = MIN(maxQp, qp);
  }

  qp = CLIP3(rc->qpMin, rc->qpMax, qp);

  return qp;
}

/* calculate min target size and max target size per cpb buffer status */
void CalcTargetSizeRange(vcencRateControl_s *rc) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 maxPicSizeBase = rc->maxPicSize;

  i32 cost = rc->pass == 2 ? rc->pass1CurCost : rc->picArea;
  i32 predBits =
      predictSize(&(rc->rcPred[rc->predLayerId]),
                  qp2qScale(rc->qpHdr >> QP_FRACTIONAL_BITS), cost);
  /* adjust B-frame QP to make targetPicSize no more than maxPicSize decided by tolMovingBitrate */
  if (rc->predLayerId == LAYER_B || rc->predLayerId >= LAYER_BREF_0) {
    while (rc->maxPicSize && predBits > rc->maxPicSize &&
           rc->qpHdr <= (rc->qpMax - (1 << QP_FRACTIONAL_BITS))) {
      /* not set the simple scene type for cvbr in this case since b frame qp is changed */
      if (rc->sceneComplexType == SIMPLE_SCENE) rc->sceneComplexType = NORMAL_SCENE;

      rc->qpHdr += (1 << QP_FRACTIONAL_BITS);
      predBits =
          predictSize(&(rc->rcPred[rc->predLayerId]),
                      qp2qScale(rc->qpHdr >> QP_FRACTIONAL_BITS), cost);
    }
  }

  /* adjust maxPicSize for P-frame in one GOP since P-frame size is much more than B-frame usually */
  if (rc->predLayerId == LAYER_P) {
    /* calculate target GOP size */
    rc->predGopBits = predBits;
    RCP_32bit q = rc->qpHdr >> QP_FRACTIONAL_BITS;
    if (rc->pass == 2) {
      //2 pass do not support separate predictor for each layer
      i32 bframes = rc->hierarchial_bit_allocation_GOP_size - 1;
      if (bframes > 1) {
        cost = rc->pass1GopCost[LAYER_BREF_0];
        //2 pass use 0 delta now
        RCP_32bit qp = q;//RcGetHierarchicalQP(rc, q, LAYER_BREF_0);
        rc->predGopBits += predictSize(&(rc->rcPred[LAYER_BREF_0]), qp2qScale(qp), cost);
        bframes--;
      }
      if (bframes) {
        cost = rc->pass1GopCost[LAYER_B];
        RCP_32bit qp = q;//RcGetHierarchicalQP(rc, q, LAYER_B);
        rc->predGopBits +=
            predictSize(&(rc->rcPred[LAYER_B]), qp2qScale(qp), cost) * bframes;
      }
    } else {
      for (int i = 0; i < 6; i++) {
        if (i == LAYER_I || i == LAYER_P || rc->gopLayerFrameNum[i] == 0)
          continue;
        i32 qpTemp = (i32) q;
        qpTemp = RcGetHierarchicalQP(rc, qpTemp << QP_FRACTIONAL_BITS, i, 0);
        RCP_32bit qp = (RCP_32bit)(qpTemp >> QP_FRACTIONAL_BITS);
        rc->predGopBits += predictSize(&(rc->rcPred[i]),
                          qp2qScale(qp), rc->picArea) *  rc->gopLayerFrameNum[i];
      }
    }

    /* adjust maxPicSize for P-frame */
    if (rc->hierarchial_bit_allocation_GOP_size > 1 && maxPicSizeBase) {
      if (rc->predGopBits > 0)
        rc->maxPicSize = ((u64)rc->maxPicSize) * predBits *
                         rc->hierarchial_bit_allocation_GOP_size /
                         rc->predGopBits;

      /* not reduce maxPicSize for P-frame */
      rc->maxPicSize = MAX(maxPicSizeBase, rc->maxPicSize);
      maxPicSizeBase = rc->maxPicSize;
    }
  }

  /* re-allocate target size considering the whole GOP since P/B frame sizes are different */
  /* I_SLICE bits not allocated for IPB cases */
  /* TODO: optimize 2-pass targetPicSize */
  if (rc->predLayerId == I_SLICE || rc->pass == 2)
    rc->targetPicSize = predBits;
  else if (rc->predLayerId != I_SLICE && rc->hierarchial_bit_allocation_GOP_size > 1 && rc->predGopBits)
    rc->targetPicSize = ((i64)predBits * rc->targetGopSize + rc->predGopBits/2) / rc->predGopBits;

  /* adjust maxPicSize by cpb status to aviod overflow */
  if (vb->bufferSize) {
    /* fill the empty space no more than 90% for I-frame or 50% for P/B frame */
    i32 maxSizeTh1 = (i32)(((i64)(vb->bufferSize - vb->bucketFullness) *
                     (rc->predLayerId == LAYER_I ? 9 : 5)) / 10);
    /* keep at least 5% empty space in cpb */
    i32 maxSizeTh2 = (i32)((i64)vb->bufferSize * 95 / 100 - vb->bucketFullness);

    rc->maxPicSize = vb->bufferRate + MIN(maxSizeTh1, maxSizeTh2);

    /* avoid too small maxPicSize */
    rc->maxPicSize = MAX(vb->bufferRate / 2, rc->maxPicSize);

    /* consider both tolMovingBitrate and cpb status */
    if (maxPicSizeBase) rc->maxPicSize = MIN(maxPicSizeBase, rc->maxPicSize);

    /* not limit min pic size, just add filler data to avoid underflow if necessary. */
    rc->minPicSize = 0;
  }
  if (rc->maxPicSize)
    rc->targetPicSize =
        CLIP3(rc->minPicSize, rc->maxPicSize, rc->targetPicSize);
}
/*------------------------------------------------------------------------------
  VCEncBeforePicRc()  Update virtual buffer, and calculate picInitQp for current
  picture , and coded status.
------------------------------------------------------------------------------*/
void VCEncBeforePicRc(vcencRateControl_s *rc, u32 timeInc, u32 sliceType,
                      bool use_ltr_cur, struct sw_picture *pic) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 i, rcWindow = 1, intraBits = 0, tmp = 0;

  bool bTargetBitsExtremSmall = HANTRO_FALSE;

  rc->frameCoded = ENCHW_YES;
  rc->sliceTypeCur = sliceType;
  rc->minPicSize = rc->maxPicSize = 0;
  if (rc->pass == 1) {
    // pass 1 uses constant qp10 to encode
    rc->qpHdr = CU_TREE_QP << QP_FRACTIONAL_BITS;
    /* Store the start QP, before ROI adjustment */
    rc->qpHdrPrev = rc->qpHdr;

    /* reset counters */
    rc->qpSum = 0;
    rc->qpLastCoded = rc->qpHdr;
    rc->qpTarget = rc->qpHdr;
    rc->nonZeroCnt = 0;

    return;
  }

  if (rc->hierarchial_bit_allocation_GOP_size_previous != rc->hierarchial_bit_allocation_GOP_size) {
    init_hierarchial_rc(rc);
  }

  rc->predLayerId = getPredLayerId(rc);
  if (rc->predLayerId == LAYER_I || rc->predLayerId == LAYER_P) rc->qpFactor = 0;

  if (rc->pass == 2 && rc->crf >= 0) {
    crfPicFixedQuant(rc);
  } else {
    RC_LOG_I("\nBEFORE PIC RC: pic=%d\n", rc->frameCnt);
    RC_LOG_I("Frame type:\t%8i  timeInc:\t%8i\n", sliceType, timeInc);
    tmp = VirtualBuffer(rc, (i32)timeInc, rc->hrd);
    //printf("\n VirtualBuffer delta=%d\n",tmp);
    for (i = 0; i < CHECK_POINTS_MAX; i++) {
      rc->qpCtrl.wordCntTarget[i] = 0;
    }

    if (vb->windowRem == 0) {
      vb->windowRem = rc->windowLen - 1;
    } else {
      vb->windowRem--;
    }
    //printf("rc->encoded_frame_number=%d\n",rc->encoded_frame_number);
    /* Calculate target size for this picture. Adjust the target bitPerPic
     * with the cumulated error between target and actual bitrates (tmp).
     * Also take into account the bits used by intra frame starting the GOP. */
    if (rc->sliceTypeCur != I_SLICE && rc->intraInterval > 1 &&
        rc->intraInterval > rc->intraIntervalCtr) {
      /* GOP bits that are used by intra frame. Amount of bits
       * "stolen" by intra from each inter frame in the GOP. */
      intraBits = vb->bitPerPic * rc->intraInterval *
                  get_avg_bits(&rc->gop, 10) /
                  100;  //TODO gops avg ratio or prev gop ratio?
      intraBits -= vb->bitPerPic;
      intraBits /= (rc->intraInterval - 1);
      intraBits = MAX(0, intraBits);
    }
    RC_LOG_I(
        "\nBEFORE PIC RC: intraBits=%d intraInterval=%d intraIntervalCtr=%d\n",
        intraBits, rc->intraInterval, rc->intraIntervalCtr);

    /* Compensate the virtual bit buffer for intra frames "stealing" bits
     * from inter frames because after intra frame the bit buffer seems
     * to overflow but the following inters will level the buffer. */
    tmp += intraBits * (rc->intraInterval - rc->intraIntervalCtr);

#ifdef RC_WINDOW_STRICT
    /* In the end of window don't be too strict with matching the error
     * otherwise the end of window tends to twist QP. */
    rcWindow = MAX(MAX(3, rc->windowLen / 6), vb->windowRem);
    //printf("\n rcWindow=%d\n",rcWindow);
#else
    /* Actually we can be fairly easy with this one, let's make it
     * a moving window to smoothen the changes. */
    rcWindow = MAX(1, rc->windowLen);
#endif

    rc->targetPicSize = vb->bitPerPic - intraBits + DIV(tmp, rcWindow);

    /* Limit the target to a realistic minimum that can be reached.
     * Setting target lower than this will confuse RC because it can never
     * be reached. Frame with only skipped mbs == 96 bits. */// carl 96 should be changed to what number
    bTargetBitsExtremSmall =
        ((96 + rc->ctbRows * rc->ctbSize) > rc->targetPicSize) ? HANTRO_TRUE
                                                               : HANTRO_FALSE;
    rc->targetPicSize = MAX(96 + rc->ctbRows * rc->ctbSize, rc->targetPicSize);

    RC_LOG_I("intraRatio: %3i%%\tintraBits: %7i\tbufferComp: %7i\n",
             get_avg_bits(&rc->gop, 10), intraBits, DIV(tmp, rcWindow));
    RC_LOG_I("WndRem: %4i  \n", vb->windowRem);
    RC_LOG_I("intraInterval: %d intraIntervalCtr: %d Complexity: %f \n",
             rc->intraInterval, rc->intraIntervalCtr,
             (float)((double)rc->rcPicComplexity * rc->reciprocalOfNumBlk8/Q31_FIXED_ONE));
    RC_LOG_I("Tr: %7d  \n", rc->targetPicSize);

    if (rc->picSkipAllowed && rc->picSkip) PicSkip(rc);
    /* skip current GOP on RC overflow */
    if (rc->skipGop)
      rc->frameCoded = ENCHW_NO;

    /* determine initial quantization parameter for current picture */
    PicQuant(rc);
    if (bTargetBitsExtremSmall && (rc->qpHdr < rc->qpHdrPrev) &&
        (rc->sliceTypeCur != I_SLICE) && 1) {
      rc->qpHdr = MAX(rc->qpHdrPrevGop, rc->qpHdr);
      RC_LOG_I("ExtremSmallReset QP %d %2.2f \n", rc->qpHdr, rc->qpHdr / 256.0);
    }
  }

  /* quantization parameter user defined limitations */
  PicQuantLimit(rc);

  if (rc->sliceTypeCur == I_SLICE) {
    if (rc->fixedIntraQp)
      rc->qpHdr = rc->fixedIntraQp;
    else if (rc->sliceTypePrev != I_SLICE) {
      if (rc->IFrameQpVisualTuning && (rc->picRc == ENCHW_YES))
        rc->intraQpDelta = MIN(rc->intraQpDelta, -3 << QP_FRACTIONAL_BITS);

      i32 iQpDelta = rc->intraQpDelta;
      //Hierarchical QP delta for I Slice
      if (rc->hieQpDeltaEnable)
        iQpDelta += (-1 << QP_FRACTIONAL_BITS);
#if 0 /* Not adjust I frame QP by only local complexity without motion info */
      if (IS_CTBRC_FOR_QUALITY(rc->ctbRc) && rc->frameCnt && (rc->pass != 2)) {
        i32 complexity =
            (i32)(((u64)rc->rcPicComplexity * rc->reciprocalOfNumBlk8 + Q31_FIXED_HALF)/Q31_FIXED_ONE);
        i32 iQpDeltaComplex = (MAX(complexity, 9) - 19) << QP_FRACTIONAL_BITS;
        if (iQpDeltaComplex < 0) {
          i32 minTmp = (26 << QP_FRACTIONAL_BITS) - rc->qpHdr;
          minTmp = MIN(0, minTmp);
          iQpDeltaComplex = MAX(minTmp, iQpDeltaComplex);
        }

        if ((iQpDeltaComplex > 0) && (iQpDelta > 0))
          iQpDelta = MAX(iQpDeltaComplex, iQpDelta);
        else if ((iQpDeltaComplex < 0) && (iQpDelta < 0))
          iQpDelta = MIN(iQpDeltaComplex, iQpDelta);
        else
          iQpDelta += iQpDeltaComplex;

        RC_LOG_I("Complex = %d, iQpDelta = %d, QpByRc = %f \n", complexity,
                 iQpDelta >> QP_FRACTIONAL_BITS,
                 (float)rc->qpHdr / (1 << QP_FRACTIONAL_BITS));
      }
#endif
      rc->qpHdr += iQpDelta /*+ rc->intraQpDeltaAdaptive*/;

      if (rc->picRc && (rc->ma_char.count != 0)) {
        MaChar(&rc->ma_char);

        //double dPBFrame4NRdCost = rc->ma_char.average_PBFrame4NRdCost/(double)(rc->picArea/8/8);
        //double dIntraPercentage = rc->ma_char.average_intraCu8Num/(double)(rc->picArea/8/8);
        //double dSkipCu8Percentage =  rc->ma_char.average_skipCu8Num/(double)(rc->picArea/8/8);

        /*Static scene: adj. I Frame qpOffset to smooth bits allocation*/
        //if(dPBFrame4NRdCost <=10 && dIntraPercentage<= 0.0016)
#ifdef RCP_FIXED_POINT_OPT
        if ((i32)rc->ma_char.average_PBFrame4NRdCost <=
                (rc->picArea >> 6) * 10 &&
            (u64)rc->ma_char.average_intraCu8Num * 10000 <= (u64)(rc->picArea >> 6) * 16 &&
            (!rc->inputSceneChange))
#else
        if ((i32)rc->ma_char.average_PBFrame4NRdCost <=
                (rc->picArea >> 6) * 10 &&
            rc->ma_char.average_intraCu8Num <= (rc->picArea >> 6) * 0.0016 &&
            (!rc->inputSceneChange))
#endif
        {
          //user set u32StaticSceneIbitPercent to try to control static scene I-Frame bits percentage
          if (rc->u32StaticSceneIbitPercent && rc->virtualBuffer.bitRate) {
            bool bAdd = HANTRO_FALSE;
            bool bSub = HANTRO_FALSE;
            u32 u32IBitRation =
                (u32)((((u64)rc->u32PrevIBitCnt * rc->outRateNum) /
                       rc->outRateDenom / rc->intraInterval) *
                      100 / rc->virtualBuffer.bitRate);
            if (u32IBitRation > (rc->u32StaticSceneIbitPercent + 15)) {
              rc->qpHdr = rc->qpHdrPrevI + (2 << QP_FRACTIONAL_BITS);
              bAdd = HANTRO_TRUE;
            } else if (u32IBitRation > (rc->u32StaticSceneIbitPercent + 5)) {
              rc->qpHdr = rc->qpHdrPrevI + (1 << QP_FRACTIONAL_BITS);
              bAdd = HANTRO_TRUE;
            } else if (u32IBitRation >= (rc->u32StaticSceneIbitPercent - 5))
              rc->qpHdr = rc->qpHdrPrevI;
            else if ((u32IBitRation > (rc->u32StaticSceneIbitPercent - 15)) &&
                     (rc->qpHdr >= rc->qpHdrPrevI)) {
              rc->qpHdr = rc->qpHdrPrevI - (1 << QP_FRACTIONAL_BITS);
              bSub = HANTRO_TRUE;
            } else if (rc->qpHdr >=
                       (rc->qpHdrPrevI - (1 << QP_FRACTIONAL_BITS))) {
              rc->qpHdr = rc->qpHdrPrevI - (2 << QP_FRACTIONAL_BITS);
              bSub = HANTRO_TRUE;
            }
            if (((rc->virtualBuffer.realBitCnt -
                  rc->virtualBuffer.virtualBitCnt) >
                 rc->virtualBuffer.bitRate) &&
                (bAdd == HANTRO_FALSE))
              rc->qpHdr = rc->qpHdr + (1 << QP_FRACTIONAL_BITS);
            else if (((rc->virtualBuffer.virtualBitCnt -
                       rc->virtualBuffer.realBitCnt) >
                      rc->virtualBuffer.bitRate) &&
                     (bSub == HANTRO_FALSE))
              rc->qpHdr = rc->qpHdr - (1 << QP_FRACTIONAL_BITS);
          }
          //Avoid larger qp bias from the qp of the previous P-Frame
          rc->qpHdr = MAX(rc->qpHdr, (rc->qpHdrPrev - (6 << QP_FRACTIONAL_BITS)));
        }
        RC_LOG_I("StaticSceneIbitPercent %d adj QP %d %2.2f \n",
                 rc->u32StaticSceneIbitPercent, rc->qpHdr, rc->qpHdr / 256.0);
      }
    }

    if (rc->intraIntervalCtr >= 1) rc->intraInterval = rc->intraIntervalCtr;
    rc->intraIntervalCtr = 1;
  } else {
    rc->intraIntervalCtr++;

    /* Check that interval is repeating */
    if (rc->intraIntervalCtr > rc->intraInterval)
      rc->intraInterval = rc->intraIntervalCtr;
#if 1
    if (rc->sliceTypeCur != I_SLICE) {
      //printf("model b qp=%d\n ",rc->qpHdr);
      //B frm not referenced
      if (rc->picRc != ENCHW_YES) rc->qpHdr += rc->frameQpDelta;
      if (use_ltr_cur) rc->qpHdr += rc->longTermQpDelta;
      if (rc->qpHdr > (51 << QP_FRACTIONAL_BITS))
        rc->qpHdr = (51 << QP_FRACTIONAL_BITS);
      //printf("final  b qp=%d\n ",rc->qpHdr);
    }
#endif
  }

  /* quantization parameter user defined limitations */
  PicQuantLimit(rc);

  /* adjust frame qp for cpb control */
  if (rc->virtualBuffer.bufferSize)
    rc->qpHdr = AdjustQpByCpbStatus(rc, rc->qpHdr);

  /* set target size range for ctbRc */
  CalcTargetSizeRange(rc);

  if (rc->sliceTypeCur == I_SLICE)
  {
    /* rc->maxIprop == 100 : disable    */
    /* Control  ratio of bits of I frame and P frame, the values of minIprop and maxIprop
     * are used for the range of clamping IP ratio    */
    if(rc->maxIprop < 100 && rc->sliceTypePrev != I_SLICE && (rc->pass != 2)){
      i32 tmp = rcCalculate(rc->virtualBuffer.bitRate, rc->outRateDenom, rc->outRateNum);
      i32 gop = rc->intraInterval;
      i32 upper_limit_targetPicSize = tmp * gop * rc->maxIprop / (gop + rc->maxIprop - 1);
      i32 lower_limit_targetPicSize = tmp * gop * rc->minIprop / (gop + rc->minIprop - 1);
      i32 upper_qp = predictQp(rc, NULL, NULL, upper_limit_targetPicSize) << QP_FRACTIONAL_BITS;
      i32 lower_qp = predictQp(rc, NULL, NULL, lower_limit_targetPicSize) << QP_FRACTIONAL_BITS;
      // rc->qpHdr = MAX(rc->qpHdr, upper_qp);
      RC_LOG_I("gop=%d, upper_limit=%d, lower_limit=%d, upper_qp=%d, lower_qp=%d, qbHdr=%d\n",
      gop, upper_limit_targetPicSize, lower_limit_targetPicSize, upper_qp/256, lower_qp/256, rc->qpHdr/256);
      rc->qpHdr = CLIP3(upper_qp, lower_qp, rc->qpHdr);
      RC_LOG_I("final qbHdr=%d, qb=%d\n",rc->qpHdr, rc->qpHdr/256);

      RC_LOG_I("\"IProp_qp\":%d,\"IProp_maxsize\":%d,\"IProp_minsize\":%d,",
                                rc->qpHdr/256,
                                upper_limit_targetPicSize,
                                lower_limit_targetPicSize);

      rc->targetPicSize = CLIP3(lower_limit_targetPicSize,upper_limit_targetPicSize,rc->targetPicSize);
      rc->maxPicSize = MIN(rc->maxPicSize,upper_limit_targetPicSize);
    }
  }

  /* Store the start QP, before ROI adjustment */
  rc->qpHdrPrev = rc->qpHdr;

  if (rc->predLayerId == LAYER_I ||
      (rc->predLayerId == LAYER_P && rc->qpHdr < rc->qpMax))
    rc->qpFactor = 0;

  /* reset counters */
  rc->qpSum = 0;
  rc->qpLastCoded = rc->qpHdr;
  rc->qpTarget = rc->qpHdr;
  rc->nonZeroCnt = 0;

  RC_LOG_I("Frame coded\t%8d  \n", rc->frameCoded);
  RC_LOG_I("Frame qpHdr\t%8d %2.2f  \n", rc->qpHdr, rc->qpHdr / 256.0);
  RC_LOG_I("GopRem:\t%8d  \n", vb->windowRem);
  RC_LOG_I("Target bits:\t%8d  \n", rc->targetPicSize);
  RC_LOG_I("\nintraBits:\t%8d  \n", intraBits);
  RC_LOG_I("bufferComp:\t%8d  \n", DIV(tmp, rcWindow));

  for (i = 0; i < CHECK_POINTS_MAX; i++) {
    RC_LOG_I("CP %i  mbNum %4i  wTarg %5i\n", i,
             (rc->qpCtrl.checkPointDistance * (i + 1)),
             rc->qpCtrl.wordCntTarget[i] * 32);
  }

  if (rc->sliceTypeCur == I_SLICE) rc->idrPoc = rc->frameCnt;

  rc->sei.crd = MAX(1, rc->frameCnt - rc->idrPoc);

  if (pic != NULL)
    rc->sei.dod = pic->sps->max_num_reorder_pics[0] + rc->idrPoc + pic->poc -
                  rc->frameCnt;
  else
    rc->sei.dod = 0;
}

/*------------------------------------------------------------------------------

  SourceParameter()  Source parameter of last coded frame. Parameters
  has been scaled up by factor 256.

------------------------------------------------------------------------------*/
void SourceParameter(vcencRateControl_s *rc, i32 nonZeroCnt) {
  //ASSERT(rc->qpSum <= 51 * rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
  ASSERT(nonZeroCnt <= rc->coeffCntMax);
  ASSERT(nonZeroCnt >= 0 && rc->coeffCntMax >= 0);

  /* AVOID division by zero */
  if (nonZeroCnt == 0) {
    nonZeroCnt = 1;
  }

  rc->srcPrm =
      rcCalculate(rc->frameBitCnt, rc->ctbSize * rc->ctbSize, nonZeroCnt);

  RC_LOG_I("nonZeroCnt %6i, srcPrm %i\n", nonZeroCnt,
           rc->srcPrm / (rc->ctbSize * rc->ctbSize));
}

/*------------------------------------------------------------------------------
  PicSkip()  Decrease framerate if not enough bits available.
------------------------------------------------------------------------------*/
void PicSkip(vcencRateControl_s *rc) {
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bitAvailable = vb->virtualBitCnt - vb->realBitCnt;
  i32 skipIncLimit = -vb->bitPerPic / 3;
  i32 skipDecLimit = vb->bitPerPic / 3;

  /* When frameRc is enabled, skipFrameTarget is not allowed to be > 1
   * This makes sure that not too many frames is skipped and lets
   * the frameRc adjust QP instead of skipping many frames */
  if (((rc->picRc == ENCHW_NO) || (vb->skipFrameTarget == 0)) &&
      (bitAvailable < skipIncLimit)) {
    vb->skipFrameTarget++;
  }

  if ((bitAvailable > skipDecLimit) && vb->skipFrameTarget > 0) {
    vb->skipFrameTarget--;
  }

  if (vb->skippedFrames < vb->skipFrameTarget) {
    vb->skippedFrames++;
    rc->frameCoded = ENCHW_NO;
  } else {
    vb->skippedFrames = 0;
  }
}

/*------------------------------------------------------------------------------
  PicQuant()  Calculate quantization parameter for next frame. In the beginning
                of window use previous GOP average QP and otherwise find new QP
                using the target size and previous frames QPs and bit counts.
------------------------------------------------------------------------------*/
void PicQuant(vcencRateControl_s *rc) {
  true_e useQpDeltaLimit = ENCHW_YES;
  i32 i, predictOrder, predictBits, predictQP_Err, bestIQP, bestBits, bestI,
      bitError, total_count;
  i32 bpp_accurate;
  i32 movingBitMax, movingBitMin, movingBitBase;
  bestI = 0;

  if (rc->picRc != ENCHW_YES) {
    rc->qpHdr = rc->fixedQp;
    RC_LOG_I("R/cx:  xxxx  QP: xx xx D:  xxxx newQP: xx\n");
    if (rc->sliceTypeCur != I_SLICE) {
      rc->qpHdr = RcGetHierarchicalQP(rc, rc->fixedQp, rc->predLayerId, 0);
    }
    return;
  }

  /* determine initial quantization parameter for current picture */
  if (rc->sliceTypeCur == I_SLICE) {
    /* Default intra QP == average of prev frame and prev GOP average */
    rc->qpHdrPrevGop = gop_avg_qp(rc);
    if (rc->hieQpDeltaEnable) {
    rc->qpHdr = (rc->qpHdrPrev + rc->qpHdrPrevGop) / 2;
    if (rc->pass == 0 && rc->sliceTypePrev != I_SLICE && !rc->rcPred[rc->predLayerId].usePrevQp) {
      //Use previous P frame qp instead of last coded frame, unless unpredictable QP change happened
      //In that case, need to consider scene change or large motion
      rc->qpHdr = (rc->rcPred[LAYER_P].qpPrev + rc->qpHdrPrevGop) / 2;
      if (rc->sliceTypePrev == B_SLICE){
        i32 predQp = rc->rcPred[LAYER_P].qpPrev + (rc->rcPred[LAYER_B].qpDeltaPrev << QP_FRACTIONAL_BITS);
        predQp = CLIP3(rc->qpMin, rc->qpMax, predQp);
        RC_LOG_I("  I QP: prev avg %d, prev qp %d, prev P qp %d; prev delta %d; pred qp %d\n",
          rc->qpHdrPrevGop, rc->qpHdrPrev, rc->rcPred[LAYER_P].qpPrev, rc->rcPred[LAYER_B].qpDeltaPrev,
          predQp);
        if (predQp != rc->qpHdrPrev)
          rc->qpHdr = (rc->qpHdrPrev + rc->qpHdrPrevGop) / 2;
      }
    }
  } else {
    rc->qpHdr = (rc->qpHdrPrev + rc->qpHdrPrevGop) / 2;
  }
  //rc->qpHdr = roundQpToInt(rc, rc->qpHdr, rc->rcPred[rc->predLayerId].qpPrev);
#if 0 /* Not adjust I frame QP by the empirical intial QP for it's not very reliable */
    if (rc->pass == 0 && rc->qpHdr > rc->qpISlice) {
      if (rc->qpHdr - rc->qpISlice >= (6 << QP_FRACTIONAL_BITS))
        /* GOP QP >> I slice QP, it indicates scene is complexity,
         I slice qp is biased to GOP qp to make I slice qp not too small */
        rc->qpHdr = (rc->qpHdrPrev + rc->qpHdrPrevGop + rc->qpISlice) / 3;
      else
        rc->qpHdr = (rc->qpHdr + rc->qpISlice) / 2;
    }
#endif
    /* reset intra QP if scene changed */
    if (rc->resetIntraPicQp || rc->inputSceneChange) {
      i32 resetQp = InitialQp(rc, rc->virtualBuffer.bitPerPic, rc->picArea);
      rc->qpHdr = MAX(resetQp, rc->qpHdr);
    }

    i32 pass2Reset = (rc->frameCnt == 0) && (rc->pass == 2);
    if (pass2Reset) useQpDeltaLimit = ENCHW_NO;

    /* If all frames are intra we calculate new QP for intra the same way as for inter */
    if (rc->sliceTypePrev == I_SLICE || pass2Reset) {
      rc->qpISlice = rc->qpHdr = new_pic_quant(rc, useQpDeltaLimit);

      if (pass2Reset) {
        if (rc->IFrameQpVisualTuning) {
          i32 initQp = InitialQp(rc, rc->virtualBuffer.bitPerPic, rc->picArea);
          if (rc->qpHdr > initQp) rc->qpHdr = (3 * rc->qpHdr + 5 * initQp) >> 3;
        }
        for (i = 0; i < 4; i++) rc->rcPred[i].qp = rc->qpHdr;
      }
    }

    if(rc->changePos != 90 && rc->sliceTypePrev != I_SLICE && (rc->pass != 2)){
      i32 resetQp = InitialQp(rc, (rc->virtualBuffer.bitPerPic * rc->changePos / 100), rc->picArea);
      RC_LOG_I("orignal qbHdr=%d, changePos qbHdr=%d\n",rc->qpHdr/256, resetQp/256);
      rc->qpHdr = MAX(resetQp, rc->qpHdr);
    }
  } else {
    /* detect scene type, then actively allocate bits from bank for complex scene */
    if (VBR_RC == 2 && rc->predLayerId == LAYER_P) {
      checkSceneByCost(rc);
      if (rc->sceneComplexType == COMPLEX_SCENE && rc->bitCntBank > 0) {
        i32 targetBitsBase = rc->targetPicSize;
        i32 bankWindow =
            MAX(rc->bitrateWindow, rc->hierarchial_bit_allocation_GOP_size);
        i32 targetBits = rc->targetPicSize + rc->bitCntBank / bankWindow;

        /* take these bits out of bank for current GOP */
        i32 bankConsumed = (targetBits - targetBitsBase) *
                           rc->hierarchial_bit_allocation_GOP_size;
        bankConsumed = MIN(rc->bitCntBank, bankConsumed);
        rc->virtualBuffer.realBitCnt -= bankConsumed;
        rc->bitCntBank -= bankConsumed;

        rc->targetPicSize = MIN(2 * targetBitsBase, targetBits);
      }
    }

    movingBitBase = MIN(rc->virtualBuffer.bitPerPic, rc->i32MaxPicSize);
    movingBitBase = (i32)MIN(rc->u32PFrameByQPStep, (u32)movingBitBase);
    movingBitMin = movingBitBase / 10;
    movingBitMax =
        MIN((((i64)rc->virtualBuffer.movingMaxRate * rc->ma.frameRateDenom) /
             rc->ma.frameRateNumer),
            rc->i32MaxPicSize);
    movingBitMax = frame_moving_bits(rc, movingBitMax);
    movingBitMax = MAX(movingBitMin, movingBitMax);
    rc->maxPicSize = movingBitMax;

    rc->targetPicSize = CLIP3(movingBitMin, movingBitMax, rc->targetPicSize);
    if (rc->sliceTypeCur == P_SLICE)
      rc->targetPicSize =
          CLIP3(rc->minPicSizeP, rc->maxPicSizeP, rc->targetPicSize);
    else
      rc->targetPicSize =
          CLIP3(rc->minPicSizeB, rc->maxPicSizeB, rc->targetPicSize);

    /* Calculate new QP by matching to previous inter frames R-Q curve */
    RC_LOG_I("P/B_pic targetPicSize %d, movingMin/Max [%d %d]\n",
             rc->targetPicSize, movingBitMin, movingBitMax);

    /* set targetGopSize here only considering bits allocation results, not predicted by rcMocel
       to avoid inaccurate results caused by inaccurate rcModel params */
    if (rc->predLayerId == LAYER_P)
      rc->targetGopSize =
          rc->hierarchial_bit_allocation_GOP_size * rc->targetPicSize;

    rc->qpHdr = new_pic_quant(rc, useQpDeltaLimit);
  }
}

/*------------------------------------------------------------------------------

  PicQuantLimit()

------------------------------------------------------------------------------*/
void PicQuantLimit(vcencRateControl_s *rc) {
  rc->qpHdr = MIN(rc->qpMax, MAX(rc->qpMin, rc->qpHdr));
}

/*------------------------------------------------------------------------------

  Calculate()  I try to avoid overflow and calculate good enough result of a*b/c

------------------------------------------------------------------------------*/
i32 rcCalculate(i32 a, i32 b, i32 c) {
  u32 left = 32;
  u32 right = 0;
  u32 shift;
  i32 sign = 1;
  i32 tmp;

  if (a == 0 || b == 0) {
    return 0;
  }
  //else if ((a * b / b) == a && c != 0)
  else if ((i64)a * (i64)b < (1LL << 31) && c != 0) {
    return (a * b / c);
  }
  if (a < 0) {
    sign = -1;
    a = -a;
  }
  if (b < 0) {
    sign *= -1;
    b = -b;
  }
  if (c < 0) {
    sign *= -1;
    c = -c;
  }

  if (c == 0) {
    return 0x7FFFFFFF * sign;
  }

  if (b > a) {
    tmp = b;
    b = a;
    a = tmp;
  }

  for (--left; (((u32)a << left) >> left) != (u32)a; --left)
    ;
  left--; /* unsigned values have one more bit on left,
               we want signed accuracy. shifting signed values gives
               lint warnings */

  while (((u32)b >> right) > (u32)c) {
    right++;
  }

  if (right > left) {
    return 0x7FFFFFFF * sign;
  } else {
    shift = left - right;
    return (i32)((((u32)a << shift) / (u32)c * (u32)b) >> shift) * sign;
  }
}

/*------------------------------------------------------------------------------
  gop_avg_qp()  Average quantization parameter of P frames since previous I.
------------------------------------------------------------------------------*/
i32 gop_avg_qp(vcencRateControl_s *rc) {
  i32 tmp = rc->qpHdrPrev;
  i32 maxIntraBitRatio = 95; /* Percentage of total GOP bits. */

  rc->gopLastBitCnt = rc->gopBitCnt;

  /* Average QP of previous GOP inter frames */
  if ((rc->pass == 2 || rc->hieQpDeltaEnable) && rc->gopPQpSum && rc->gopPQpNum)
    tmp = DIV(rc->gopPQpSum, rc->gopPQpNum);
  else if (rc->gopQpSum && rc->gopQpDiv) {
    tmp = DIV(rc->gopQpSum, rc->gopQpDiv);
  }

  /* Average bit count per frame for previous GOP (intra + inter) */
  rc->gopAvgBitCnt = DIV(rc->gopBitCnt, (rc->gopQpDiv + 1));
  /* Ratio of intra_frame_bits/all_gop_bits % for previous GOP */
  if (rc->gopBitCnt) {
#if 0
    i32 prevIntraNormBits = rcCalculate(rc->rcPred[I_SLICE].prevFrameBits,
      DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
    i32 gopIntraBitRatio =
      rcCalculate(prevIntraNormBits, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16, DSCBITPERMB) * 100;
#else
    i32 gopIntraBitRatio = rc->rcPred[I_SLICE].prevFrameBits * 100;
#endif
    gopIntraBitRatio = DIV(gopIntraBitRatio, rc->gopBitCnt);
    /* Limit GOP intra bit ratio to leave room for inters. */
    gopIntraBitRatio = MIN(maxIntraBitRatio, gopIntraBitRatio);
    update_tables(&rc->gop, tmp, gopIntraBitRatio);
  }
  rc->gopQpSum = 0;
  rc->gopQpDiv = 0;
  rc->gopBitCnt = 0;
  rc->gopPQpSum = 0;
  rc->gopPQpNum = 0;

  return tmp;
}

/*
  Adjust QP for first GOP after first IDR based on IDR sse
*/
i32 RcAdjustFirstGopQP(vcencRateControl_s *rc, i32 qp)
{
  //QP in fractional precision
  i32 retQp = qp;
  u8 gopSize = rc->hierarchial_bit_allocation_GOP_size;


  i32 prevIntraQp = rc->rcPred[LAYER_I].qpPrev >> QP_FRACTIONAL_BITS;
  int tblIdx = 0;

  if (rc->pass != 0) return retQp;
  while (tblIdx < 35 && qp_tbl[1][++tblIdx] > prevIntraQp) (void)1;
  const float simpleThresh[36] = {
    0.20, 0.20, 0.19, 0.18, 0.17, 0.16, 0.11, 0.10, 0.09, 0.08,
    0.077, 0.06, 0.057, 0.05, 0.048, 0.047, 0.045, 0.04, 0.037, 0.035,
    0.02, 0.018, 0.016, 0.015, 0.014, 0.013, 0.01, 0.009, 0.008, 0.005,
    0.005, 0.004, 0.003, 0.002, 0.001, 0.0
  };

  i32 prevIntraSSE = rc->hierarchial_sse[gopSize - 1][0];
  float percent = (float)prevIntraSSE / rc->picArea;

  //Simple texture, not appropriate initial QP
  if (percent < simpleThresh[tblIdx]) {
    i32 thIdx = tblIdx;
    while(percent < simpleThresh[thIdx++]) (void)1;
    retQp = (MIN(qp_tbl[1][thIdx - 1], prevIntraQp - 2)) << QP_FRACTIONAL_BITS;

    //For large GOP size, need to consider motion complexity
    //Since there is no motion info at this instance, 
    //  reduce range of delta to keep it stable
    if (gopSize >= 4) {
      i32 deltaTemp = (float)(retQp - qp) / 1.5;
      if (gopSize >= 8) deltaTemp = (retQp - qp) >> 1;
      //Only provide integer delta
      i32 delta = deltaTemp >> QP_FRACTIONAL_BITS;
      retQp = qp + (delta << QP_FRACTIONAL_BITS);
    }
  }
  return retQp;
}

i32 sceneMotionTuneQp(vcencRateControl_s *rc, i32 qp, true_e useQpDeltaLimit)
{
  u8 checkSceneMotion = (rc->predLayerId != LAYER_I) && (rc->sceneMotionType != NORMAL_MOTION);
  if ((rc->rcMode == VCE_RC_CVBR) && 
      (rc->sceneComplexType == SIMPLE_SCENE && rc->sceneMotionType == STATIC_MOTION))
  {
    checkSceneMotion = 0;
  }
  if (checkSceneMotion) {
    i32 deltaQ = qp - rc->rcPred[rc->predLayerId].qpPrev;
    if (rc->sceneMotionType == STATIC_MOTION) {
      //To handle unappropriate initial QP
      if (rc->frameCnt < 9)
        deltaQ = deltaQ * 0.8;
      else if (rc->qpToSizeStatic)
        deltaQ = deltaQ * 0.4;
      else
        deltaQ = deltaQ >> 1;
      //If user provide delta limit, special handle to avoid extreme case
      //e.g. delta limit = 1 or 0.5 deltaQ still exceed limit
      if (useQpDeltaLimit) {
        i32 staticDeltaQMin = (i32)(rc->picQpDeltaMin * 0.8);
        i32 staticDeltaQMax = (i32)(rc->picQpDeltaMax * 0.8);

        /* Not force the abs deltaQ < 1.0 if ctbRc=1 is disabled */
        if (IS_CTBRC_FOR_QUALITY(rc->ctbRc) == 0) {
          staticDeltaQMin = MIN(-1, staticDeltaQMin);
          staticDeltaQMax = MAX(1,  staticDeltaQMax);
        }
        deltaQ = CLIP3((staticDeltaQMin << QP_FRACTIONAL_BITS),
                        (staticDeltaQMax << QP_FRACTIONAL_BITS), deltaQ);
      }
    }
    else if (rc->sceneMotionType == STATIC_TO_LARGE_MOTION && deltaQ > 0)
      deltaQ = 1.2 * deltaQ;
    else if (rc->sceneMotionType == LARGE_TO_STATIC_MOTION && deltaQ < 0)
      deltaQ = 1.2 * deltaQ;

    RC_LOG_I("Pic Quant motion type %d; qpToSizeStatic %d; Original Delta %d; final qp %d\n", 
      rc->sceneMotionType, rc->qpToSizeStatic, qp - rc->rcPred[rc->predLayerId].qpPrev,
      deltaQ + rc->rcPred[rc->predLayerId].qpPrev);

    qp = deltaQ + rc->rcPred[rc->predLayerId].qpPrev;
  }

  return qp;
}

i32 roundQpToInt(vcencRateControl_s *rc, i32 qp, i32 prevQp)
{
  /*round qp towards to previous qp if ctbRc=1 is disabled(but still follow regular rounding-off)*/
  if (IS_CTBRC_FOR_QUALITY(rc->ctbRc) == 0 && (qp & QP_FRACTIONAL_MASK)) {
    i32 deltaQ = qp - prevQp;
    i32 offset = (1 << (QP_FRACTIONAL_BITS - 1));
    if ((qp & QP_FRACTIONAL_MASK) == (1 << (QP_FRACTIONAL_BITS - 1))){
      offset = (deltaQ < 0) ? (1 << (QP_FRACTIONAL_BITS - 1)) : 0;
    }
    /*i32 deltaQ = qp - prevQp;
    i32 offset = deltaQ >= 0 ? 0: QP_FRACTIONAL_MASK;*/
    qp = ((qp + offset) >> QP_FRACTIONAL_BITS) << QP_FRACTIONAL_BITS;
  }
  return qp;
}

/*------------------------------------------------------------------------------
  new_pic_quant()  Calculate new quantization parameter from the 2nd degree R-Q
  equation. Further adjust Qp for "smoother" visual quality.
------------------------------------------------------------------------------*/
static i32 new_pic_quant(vcencRateControl_s *rc, true_e useQpDeltaLimit) {
  i64 diff_prev = 0, tmp = 0, diff, diff_best = 0x7FFFFFFFFF;
  i32 qp_prev = 0, qp_best = rc->rcPred[rc->predLayerId].qpPrev,
      qp = rc->rcPred[rc->predLayerId].qpPrev;

  if (rc->pass == 2) {
    i32 id = rc->predLayerId;
    i32 reinit = 0;
    i32 firstI = (id == LAYER_I) && (rc->frameCnt == 0);
    i32 deltaQp = (id == LAYER_I) ? (rc->intraQpDelta >> QP_FRACTIONAL_BITS) : 0;
  PREDICT_PASS2_QP:
    qp_best = qp = getPass2Qp(rc);
#if VBR_RC
    if (firstI == 0) qp = detectSceneAndTuneQp(rc, qp, rc->rcPred[id].qpPrev);
#endif

    RCP_32bit pred_bits =
        predictSize(&(rc->rcPred[id]),
		qp2qScale((RCP_32bit)((qp >> QP_FRACTIONAL_BITS) + deltaQp)),
		rc->pass1CurCost);

#if VBR_RC
    i32 qpI = qp >> QP_FRACTIONAL_BITS;
    if (rc->initialQpTuning && firstI == 1 && reinit == 0 && qpI <= 30) {
      if (pred_bits > 16 * rc->virtualBuffer.bitPerPic) {
        reinitFramePredictors(rc, 1);
        reinit = 1;
      } else if (pred_bits > 12 * rc->virtualBuffer.bitPerPic) {
        reinitFramePredictors(rc, 0);
        reinit = 1;
      }
      RC_LOG_I("    pred_bits %.2f = %.2f x bufferRate %d bitrate %d qp %d\n",
               (float)pred_bits, (float)pred_bits / rc->virtualBuffer.bitPerPic,
               rc->virtualBuffer.bitPerPic, rc->virtualBuffer.bitRate,
               qp >> QP_FRACTIONAL_BITS);

      if (reinit) goto PREDICT_PASS2_QP;
    }
    RC_LOG_I("    pred_bits %.2f = %.2f x bufferRate %d qp %d\n", (float)pred_bits,
             (float)pred_bits / rc->virtualBuffer.bitPerPic,
             rc->virtualBuffer.bitPerPic, qp >> QP_FRACTIONAL_BITS);
#endif

    rc->rcPred[id].qpPrev = (rc->rcPred[id].qp >= 0) ? rc->rcPred[id].qp : qp;

    if ((id == B_SLICE) || (id == 3)) useQpDeltaLimit = ENCHW_NO;
    goto new_pic_quant_end;
  }

  if (rc->predLayerId == LAYER_P && rc->rcPred[rc->predLayerId].usePrevQp) {
    i32 retQp = RcGetHierarchicalQP(rc, qp, rc->predLayerId, 0);
    if (rc->hieQpDeltaEnable) 
      retQp = RcAdjustFirstGopQP(rc, retQp);
    rc->rcPred[rc->predLayerId].qpPrev = retQp;
    return retQp;
  }

  if (rc->predLayerId == LAYER_B || rc->predLayerId >= LAYER_BREF_0) {
    i32 baseQp = rc->rcPred[LAYER_P].qp;
    if (!rc->hieQpDeltaEnable && rc->rcPred[rc->predLayerId].usePrevQp)
      baseQp = qp;
    i32 retQp = RcGetHierarchicalQP(rc, baseQp, rc->predLayerId, 1);
    rc->rcPred[rc->predLayerId].qpPrev = retQp;
    return retQp;
  }

  qp = predictQp(rc, NULL, NULL, rc->targetPicSize) << QP_FRACTIONAL_BITS;
  if ((rc->rcMode == VCE_RC_CVBR))
    qp = detectSceneAndTuneQp(rc, qp, rc->rcPred[rc->predLayerId].qpPrev);

  //motion type QP tune, only work for single pass and GOP 1
  qp = sceneMotionTuneQp(rc, qp, useQpDeltaLimit);
  // Rounding QP to keep qp and prevQp in same precision(No fractional value)
  qp = roundQpToInt(rc, qp, rc->rcPred[rc->predLayerId].qpPrev);
  i32 lastActualBits =
      rc->predLayerId == LAYER_I ? 0 : rc->rcPred[rc->predLayerId].prevFrameBits;
  i32 lastTargetBits =
      rc->predLayerId == LAYER_I ? 0 : rc->rcPred[rc->predLayerId].prevTargetBits;
  i32 currentTargetBits =
      predictSize(&(rc->rcPred[rc->predLayerId]),
                  qp2qScale(qp >> QP_FRACTIONAL_BITS), rc->picArea);
  if (lastActualBits && lastTargetBits) {
    /* Qunying: To remove, just for debug purpose */
    RC_LOG_I(
        "[RC Frm%d]Adj QP: currentTarget=%d, lastRealBits=%d, qp=%.2f, "
        "qpPrev=%.2f, realBits=%d, targetBits=%d\n",
        rc->frameCnt, currentTargetBits, lastActualBits,
        qp * 1.0 / (1 << QP_FRACTIONAL_BITS),
        rc->rcPred[rc->predLayerId].qpPrev * 1.0 / (1 << QP_FRACTIONAL_BITS),
        rc->encodedFramesBits, rc->virtualBuffer.bitPerPic * rc->frameCnt);

    /* This condition means that R(Q) history conflict with R(Q) of last frame,
       just make Qp same as that of last frame */
    if (((currentTargetBits >= lastActualBits) &&
         (qp >= rc->rcPred[rc->predLayerId].qpPrev)) ||
        ((currentTargetBits <= lastActualBits) &&
         (qp <= rc->rcPred[rc->predLayerId].qpPrev))) {
      qp = rc->rcPred[rc->predLayerId].qpPrev;

      /* Given certain target bits, the coeffMin in rc model may limit the minimal qp for some still scenes,
         adjust QP if the real bits are far less than target bits */
#ifndef RCP_FIXED_POINT_OPT
      if (currentTargetBits > lastActualBits * 1.1 && rc->frameCnt > 60 &&
          rc->encodedFramesBits <
              rc->virtualBuffer.bitPerPic * rc->frameCnt * ((float)(100-rc->tolUnderflow) / 100))
#else
      if (currentTargetBits * 100 > lastActualBits * 110  && rc->frameCnt > 60 &&
          rc->encodedFramesBits * 100 <
              rc->virtualBuffer.bitPerPic * rc->frameCnt * (100 - rc->tolUnderflow))
#endif
      {
        qp -= (1 << QP_FRACTIONAL_BITS);
      }
    }
  }

new_pic_quant_end:

  qp = CLIP3(0, 0x3300, qp);
  RC_LOG_I("new adj QP %d %2.2f, qp_prev %d, D: %ld\n", qp, qp / 256.0, qp_prev,
           diff_prev - diff_best);

  i32 resetQp = 0;
  if (rc->inputSceneChange) {
    resetQp = InitialQp(rc, rc->virtualBuffer.bitPerPic, rc->picArea);
    qp = MAX(qp, resetQp);
  }

  qp = roundQpToInt(rc, qp, rc->rcPred[rc->predLayerId].qpPrev);

  /* Limit Qp change for smoother visual quality */
  if (useQpDeltaLimit) {
    i32 minQp = rc->rcPred[rc->predLayerId].qpPrev +
                (rc->picQpDeltaMin << QP_FRACTIONAL_BITS);
    i32 maxQp = rc->rcPred[rc->predLayerId].qpPrev +
                (rc->picQpDeltaMax << QP_FRACTIONAL_BITS);
    qp = CLIP3(minQp, maxQp, qp);
  }

  RC_LOG_I("new final QP %2.2f, resetQP %d, limit %d\n", qp / 256.0, resetQp,
           useQpDeltaLimit);

  return qp;
}

/*------------------------------------------------------------------------------
  get_avg_bits()
------------------------------------------------------------------------------*/
static i32 get_avg_bits(linReg_s *p, i32 n) {
  i32 i;
  i64 sum = 0;
  i32 pos = p->pos;

  if (!p->len) return 0;

  if (n == -1 || n > p->len) n = p->len;

  i = n;

  while (i--) {
    if (pos)
      pos--;
    else
      pos = p->len - 1;
    sum += p->bits[pos];
    if (sum < 0) {
      return I64_MAX / (n - i);
    }
  }
  return DIV(sum, n);
}

/*------------------------------------------------------------------------------
  update_tables()  only statistics of PSLICE, please.
------------------------------------------------------------------------------*/
static void update_tables(linReg_s *p, i32 qp, i32 bits) {
  const i32 clen = RC_TABLE_LENGTH;
  i32 tmp = p->pos;

  p->qp_prev = qp;
  p->qs[tmp] = q_step[((qp * 10) >> QP_FRACTIONAL_BITS)];
  p->bits[tmp] = bits;

  RC_LOG_D("upd table: qs %i  bits %i  qp %d %2.2f\n", p->qs[tmp], bits, qp,
           qp / 256.0);

  if (++p->pos >= clen) {
    p->pos = 0;
  }
  if (p->len < clen) {
    p->len++;
  }
}

static void MaAddFrame(rc_ma_s *ma, i32 frameSizeBits) {
  ma->frame[ma->pos++] = frameSizeBits;

  if (ma->pos == ma->length) ma->pos = 0;

  if (ma->count < ma->length) ma->count++;
}

/*------------------------------------------------------------------------------
    Calculate average bitrate of moving window
------------------------------------------------------------------------------*/
static i32 Ma(rc_ma_s *ma) {
  i32 i;
  unsigned long long sum = 0; /* Using 64-bits to avoid overflow */

  for (i = 0; i < ma->count; i++) {
    sum += ma->frame[i];
    //printf("i=%d,ma->frame[i]=%d\n",i,ma->frame[i]);
  }

  if (!ma->frameRateDenom) return 0;

  sum = sum / ma->count;

  return sum * (ma->frameRateNumer + ma->frameRateDenom - 1) /
         ma->frameRateDenom;
}

static void MaCharAddFrame(rc_ma_char *ma, u32 intraCu8Num, u32 skipCu8Num,
                           u32 PBFrame4NRdCost) {
  ma->intraCu8Num[ma->pos] = intraCu8Num;
  ma->skipCu8Num[ma->pos] = skipCu8Num;
  ma->PBFrame4NRdCost[ma->pos] = PBFrame4NRdCost;

  ma->pos++;

  if (ma->pos == ma->length) ma->pos = 0;

  if (ma->count < ma->length) ma->count++;
}

static void MaChar(rc_ma_char *ma) {
  i32 i;
  unsigned long long sum0 = 0, sum1 = 0, sum2 = 0;

  for (i = 0; i < ma->count; i++) {
    sum0 += ma->intraCu8Num[i];
    sum1 += ma->skipCu8Num[i];
    sum2 += ma->PBFrame4NRdCost[i];
  }

  ma->average_intraCu8Num = sum0 / ma->count;
  ma->average_skipCu8Num = sum1 / ma->count;
  ma->average_PBFrame4NRdCost = sum2 / ma->count;
}

/*------------------------------------------------------------------------------
    Calculate average bitrate of moving window -1
------------------------------------------------------------------------------*/
static i32 moving_totalBit_window_minus1(rc_ma_s *ma) {
  i32 i = 0;
  i32 sum = 0; /* Using 64-bits to avoid overflow */
  i32 readptr;
  readptr = ma->pos - 1;
  if (readptr < 0) readptr = ma->length - 1;
  while (1) {
    if (i == ma->count - 1) break;
    sum += ma->frame[readptr];
    readptr--;
    if (readptr < 0) readptr = ma->length - 1;
    i++;
  }
  return sum;
}

/*------------------------------------------------------------------------------
    Calculate the minimum remaining bits per frame for all ongoing slide windows
------------------------------------------------------------------------------*/
static i32 frame_moving_bits(vcencRateControl_s *rc, i32 maxBits) {
  rc_ma_s *ma = &(rc->ma);
  i32 targetBits = rc->targetPicSize;
  i32 winLen = rc->monitorFrames;
  i32 i = 0;
  i32 sum = 0;
  i32 len = winLen - 1;
  i32 bits = maxBits;
  i32 totalBits = maxBits * winLen;
  i32 pos = ma->pos;
  i32 tmp;
  i32 minBits = MIN(maxBits, targetBits) / 2;

  for (i = len; i > 0; i--) {
    if (ma->count < (len - i + 1)) break;

    pos--;
    if (pos < 0) pos = ma->length - 1;

    sum += ma->frame[pos];
    tmp = (totalBits - sum) / i;
    bits = MIN(tmp, bits);

    if (bits <= minBits) break;
  }

  /* To avoid too small target bits */
  bits = MAX(minBits, bits);
  return bits;
}

/******************************************************************
  -Pass2 RC
*******************************************************************/
void initFramePredictors(vcencRateControl_s *rc) {
  RCP_64bit coeffs[6] = {0,};
  RCP_64bit coeffMins[6] = {0,};
  RCP_64bit decay = RCP_DATA_TYPE(0.5);

  i32 minDiv = 4;

  /* for cbr mode, set lower decay and coeffMin to avoid encoder buffer underflow */
  if (rc->pass == 2) {
    if (rc->visualLmdTuning) {
      coeffs[LAYER_I]      = RCP_DATA_TYPE(1.0);
      coeffs[LAYER_P]      = RCP_DATA_TYPE(0.7);
      coeffs[LAYER_BREF_0] = RCP_DATA_TYPE(0.4);
      coeffs[LAYER_BREF_1] = RCP_DATA_TYPE(0.35);
      coeffs[LAYER_BREF_2] = RCP_DATA_TYPE(0.35);
      coeffs[LAYER_B]      = RCP_DATA_TYPE(0.35);
    } else {
      coeffs[LAYER_I]      = RCP_DATA_TYPE(1.2);
      coeffs[LAYER_P]      = RCP_DATA_TYPE(0.8);
      coeffs[LAYER_BREF_0] = RCP_DATA_TYPE(0.35);
      coeffs[LAYER_BREF_1] = RCP_DATA_TYPE(0.2);
      coeffs[LAYER_BREF_2] = RCP_DATA_TYPE(0.2);
      coeffs[LAYER_B]      = RCP_DATA_TYPE(0.2);
    }

    decay = RCP_DATA_TYPE(0.999);   //1.0; //To astrict p->count to 1000
    minDiv = (rc->rcMode == VCE_RC_CBR || rc->rcMode == VCE_RC_ABR) ? 4 : 2;
    for (int i = 0; i < 6; i++) {
      coeffMins[i] = coeffs[i] / minDiv;
    }
  } else {
    coeffs[LAYER_I]      = RCP_DATA_TYPE(4.0);
    coeffs[LAYER_P]      = RCP_DATA_TYPE(1.0);
    coeffs[LAYER_BREF_0] = RCP_DATA_TYPE(0.5);
    coeffs[LAYER_BREF_1] = RCP_DATA_TYPE(0.45);
    coeffs[LAYER_BREF_2] = RCP_DATA_TYPE(0.35);
    coeffs[LAYER_B]      = rc->hieQpDeltaEnable? (RCP_DATA_TYPE(0.3)) : (RCP_DATA_TYPE(0.25));
    decay = RCP_DATA_TYPE(0.5);
    minDiv = (rc->rcMode == VCE_RC_CBR || rc->rcMode == VCE_RC_ABR) ? 8 : 4;

    for (int i = 0; i < 6; i++) {
      coeffMins[i] = coeffs[i] / minDiv;
    }
  }

  for (i32 i = 0; i < 6; i++) {
    rc->rcPred[i].coeff = coeffs[i];
    rc->rcPred[i].coeffMin = coeffMins[i];
    rc->rcPred[i].decay = decay;
    rc->rcPred[i].count = RCP_DATA_TYPE(1.0);
    rc->rcPred[i].offset = 0;
    rc->rcPred[i].qp = -1;
    rc->rcPred[i].qpPrev = rc->qpHdr;
    rc->rcPred[i].usePrevQp = 1;

    /* initialize the cost statistic params */
    rc->rcPred[i].shortTermCost = 0;
    rc->rcPred[i].shortTermCostDecay = RCP_DATA_TYPE(0.5);
    rc->rcPred[i].shortTermCostCount = 0;

    rc->rcPred[i].longTermCost = 0;
    rc->rcPred[i].longTermCostDecay = RCP_DATA_TYPE(0.999);
    rc->rcPred[i].longTermCostCount = 0;

    rc->rcPred[i].cost = 0;
    rc->rcPred[i].prevFrameBits = 0;
    rc->rcPred[i].prevTargetBits = 0;
    //rc->rcPred[i].qpDelta = 0;
    rc->rcPred[i].qpDeltaPrev = 0;
  }
}

#if VBR_RC
void reinitFramePredictors(vcencRateControl_s *rc, i32 isHuge) {
  // I > 8 bpp
  const RCP_64bit coeffsBigI[6] = 
    {RCP_DATA_TYPE(0.5), RCP_DATA_TYPE(1.0), RCP_DATA_TYPE(1.5),
     RCP_DATA_TYPE(0.75), RCP_DATA_TYPE(0.75), RCP_DATA_TYPE(0.75)};
  // I > 16 bpp
  const RCP_64bit coeffsHugeI[6] = 
    {RCP_DATA_TYPE(0.5), RCP_DATA_TYPE(1.2), RCP_DATA_TYPE(2.0),
     RCP_DATA_TYPE(0.75), RCP_DATA_TYPE(0.75), RCP_DATA_TYPE(0.75)};
  const RCP_64bit *coeffs = isHuge ? coeffsHugeI : coeffsBigI;

  for (i32 i = 0; i < 6; i++) rc->rcPred[i].coeff = coeffs[i];
}
#endif

/* update cost statistic for CVBR */
void updateCost(vcencRateControl_s *rc, RCP_64bit var){
  rcPredictor *p = &(rc->rcPred[rc->predLayerId]);
  RCP_64bit cost = var * LL_FIXED / rc->picArea;

  p->shortTermCostCount = p->shortTermCostDecay * p->shortTermCostCount / LL_FIXED;
  p->shortTermCost = p->shortTermCost * p->shortTermCostDecay / LL_FIXED;
  p->shortTermCostCount += LL_FIXED;
  p->shortTermCost += cost;

  p->longTermCostCount = p->longTermCostCount * p->longTermCostDecay / LL_FIXED;
  p->longTermCost = p->longTermCost * p->longTermCostDecay / LL_FIXED;
  p->longTermCostCount += LL_FIXED;
  p->longTermCost += cost;

  p->cost = cost;

  RC_LOG_I(
      " ================= predId==%d cost %f short %f long %f count %f %f "
      "decay %f %f\n",
      rc->predLayerId, (float)cost/ LL_FIXED, (float)p->shortTermCost / p->shortTermCostCount,
      (float)p->longTermCost / p->longTermCostCount, (float)p->shortTermCostCount / LL_FIXED,
      (float)p->longTermCostCount/ LL_FIXED, (float)p->shortTermCostDecay / LL_FIXED, (float)p->longTermCostDecay / LL_FIXED);
}

void updatePredictor(vcencRateControl_s *rc, RCP_32bit q, RCP_32bit var, RCP_32bit bits, i32 qp)
{
  if (var < 10) return;

  rcPredictor *p = &(rc->rcPred[rc->predLayerId]);
  const RCP_64bit range = 2;
  RCP_64bit old_coeff = p->coeff * LL_FIXED / p->count;
  RCP_64bit old_offset = (p->offset / p->count) * LL_FIXED;
  RCP_64bit new_coeff = MAX(((i64)bits * q - old_offset) / var, p->coeffMin);
  RCP_64bit new_coeff_clipped =
      CLIP3(old_coeff / range, old_coeff * range, new_coeff);
  RCP_64bit new_offset = (i64)bits * q - new_coeff_clipped * var;

  if (new_offset >= 0)
    new_coeff = new_coeff_clipped;
  else
    new_offset = 0;

  RCP_64bit delta = new_coeff - old_coeff;
  if (rc->hieQpDeltaEnable) {
    //P frame Coeff can be a representitive for B frame
    if (rc->predLayerId == LAYER_P && new_coeff > old_coeff) {
      rc->rcPred[LAYER_BREF_0].coeff += (delta / 4) * (rc->rcPred[LAYER_BREF_0].count / LL_FIXED);
      rc->rcPred[LAYER_BREF_1].coeff += (delta / 4) * (rc->rcPred[LAYER_BREF_1].count / LL_FIXED);
      rc->rcPred[LAYER_BREF_2].coeff += (delta / 8) * (rc->rcPred[LAYER_BREF_2].count / LL_FIXED);
      rc->rcPred[LAYER_B].coeff += (delta / 8) * (rc->rcPred[LAYER_B].count / LL_FIXED);
    }
  }

  RCP_64bit decay = p->decay;
  if (rc->sceneMotionType == STATIC_MOTION && rc->predLayerId != I_SLICE && rc->frameCnt > 9)
    decay = RCP_DATA_TYPE(0.9);

  if (rc->hieQpDeltaEnable && p->usePrevQp == 1)
    decay = 0;

  p->count = p->count * decay / LL_FIXED;
  p->coeff = p->coeff * decay / LL_FIXED;
  p->offset = (p->offset / LL_FIXED) * decay;
  p->count += LL_FIXED;
  p->coeff += new_coeff;
  p->offset += new_offset;
  p->qp = qp;
  p->usePrevQp = 0;
  /* Qunying: To remove, just for debug purpose */
  RC_LOG_I(
      "[RC Frm%d]Upd predId %d. OLD coeff %.4f offset %.2f; NEW coeff %.4f "
      "offset %.2f; bits %.0f q %.2f var %.0f predSize %.0f \n",
      rc->frameCnt, rc->predLayerId, (float)old_coeff/LL_FIXED, (float)old_offset/LL_FIXED, (float)p->coeff / p->count,
      (float)p->offset / p->count, (float)bits, (float)q/LL_FIXED, (float)var, (float)((double)old_coeff * var + old_offset) / q);
 }


RCP_32bit predictSize(rcPredictor *p, RCP_32bit q, RCP_32bit var) {
  RCP_32bit result;

  result = ((p->coeff * var + p->offset) / ((q * p->count) / LL_FIXED));

  return result;
}

#ifdef RCP_FIXED_POINT_OPT
/* Fix point log2 with k bit decimal */
static i32 LOG2_FIXED(uint64_t x, int k) { //Q8.24
  int r = 0;
  uint64_t i;
  for (i = x; i; i >>= 1) ++r;
  --r;
  /* normalized to Q1.31 */
  if (r > 31)
    x >>= (r - 31);
  else
    x <<= (31 - r);
  /* compute decimal bits */
  while (k--) {
    x *= x;
    x >>= 31;
    if (x >> 32) {
      x >>= 1;
      r = ((r << 1) | 1);
    } else
      r = (r << 1);
  }
//  printf("log2 : %d\n", r);

  return r;
}
#endif

RCP_32bit qScale2qp(RCP_32bit qScale) {
#ifdef RCP_FIXED_POINT_OPT

  i32 ret;
  u64 q = (u64)qScale * RCP_DATA_TYPE(1/0.85);

  ret = (12 * (1 << QP_FRACTIONAL_BITS) + 6 * (LOG2_FIXED(q, QP_FRACTIONAL_BITS) - (2*FIXED_FRACTIONAL_BITS)*(1<<QP_FRACTIONAL_BITS) ));

  return ret;
#else
  return 12.0 + 6.0 * (double)LOG2(qScale / 0.85);
#endif
}

#if defined (USE_QP2QSCALE_TABLE) && defined (RCP_FIXED_POINT_OPT)
RCP_32bit qp2qScale_10(RCP_32bit qp) {
  RCP_32bit ret;

  int i, n;

  qp = CLIP3(0, 1020, qp);
  n = qp / 120;
  i = qp -  n * 120;
  ret = (1 << n) * qpScaleTable[i];

  return ret;
}
#endif
RCP_32bit qp2qScale(RCP_32bit qp) {
  RCP_32bit ret;

#ifdef USE_QP2QSCALE_TABLE
#ifdef RCP_FIXED_POINT_OPT
  int i, n;
  qp = CLIP3(0, 51, qp);
  n = qp / 6;
  i = qp -  n * 6;
  ret = (1 << n) * qpScaleTable[i*20];
#endif
#else
  ret =  0.85 * pow(2.0, (qp - 12.0) / 6.0);
#endif

  return ret;
}

#ifdef RCP_FIXED_POINT_OPT
RCP_32bit getNormQp(i32 qp) { return (RCP_32bit)(qp*20 + (1 << (QP_FRACTIONAL_BITS-1)))/ (1 << QP_FRACTIONAL_BITS); }
#else
double  getNormQp(i32 qp) { return (double)qp / (1 << QP_FRACTIONAL_BITS); }
#endif

i32 getPredLayerId(vcencRateControl_s *rc) {
  i32 id = -1;
  i32 gopPoc = (i32)rc->gopPoc, gopSize = rc->hierarchial_bit_allocation_GOP_size;
  if (!rc->hieQpDeltaEnable) {
    //2 pass do not support independent predictor for each layer
    id = rc->sliceTypeCur;
    if (rc->sliceTypeCur == B_SLICE) {
      if ((i32)rc->gopPoc == (rc->hierarchial_bit_allocation_GOP_size - 1)) {
        id = LAYER_P;
      } else if ((rc->hierarchial_bit_allocation_GOP_size > 2) &&
                 ((i32)rc->gopPoc ==
                  (rc->hierarchial_bit_allocation_GOP_size / 2 - 1))) {
        id = LAYER_BREF_0;
      }
    }

    if (rc->pass == 2)
      rc->rcPred[LAYER_P].decay = RCP_DATA_TYPE(0.999);
  
    //for pass2 gop4->gop1, reduce decay and change pass1 cost to match with gop1
    if (rc->pass == 2 && rc->hierarchial_bit_allocation_GOP_size == 1 && rc->agopPass1GopPicIdx >= 0)
    {
      id = LAYER_P;
      rc->rcPred[id].decay = RCP_DATA_TYPE(0.3);
      
      if ((rc->agopPass1GopPicIdx == 0))
        rc->pass1CurCost /= 1.4;
      else if (rc->agopPass1GopPicIdx == 1)
        rc->pass1CurCost /= 1.2;
      else
        rc->pass1CurCost *= 1.1;
    }
    
  } else {
    switch(rc->sliceTypeCur){
      case I_SLICE:
        id = LAYER_I;
        break;
      case P_SLICE:
        id = LAYER_P;
        break;
      case B_SLICE:
        id = LAYER_B;
        if (gopPoc == (gopSize - 1))
          id = LAYER_P;
        else if (gopSize > 2) {
          i32 layer0Bref = gopSize / 2 - 1;
          if (gopPoc == layer0Bref)
            id = LAYER_BREF_0;
          else if (gopSize > 4) {
            i32 layer1Bref0 = (gopSize/2 > 2)? layer0Bref - (gopSize/2 - gopSize/4) : -1;
            i32 layer1Bref1 = ((gopSize + 1)/2 > 2)? layer0Bref + (gopSize + 1)/4 : -1;
            if (gopPoc == layer1Bref0 || gopPoc == layer1Bref1)
              id = LAYER_BREF_1;
            else if(gopSize > 8) {
              //GOP 16, currently hard coding, need to fix
              ASSERT(gopSize == 16);
              if (gopPoc == 1 || gopPoc == 5 || gopPoc == 9 || gopPoc == 13)
                id = LAYER_BREF_2;
            }
          }
        }
        break;
      default: ASSERT(0);
    }
  }

  ASSERT(id >= 0);
  return id;
}

RCP_32bit predictWindowSize(vcencRateControl_s *rc, RCP_32bit *costs, i32 *frameNum, i32 q, RCP_32bit targetBits)
{
  i32 i, frms = 0;
  RCP_32bit bits = 0, diff = 0;
  RCP_32bit qp = 0;

  if (rc->pass != 2) {
    bits =
        predictSize(&(rc->rcPred[rc->predLayerId]), qp2qScale((RCP_32bit)q), rc->picArea);

    frms++;
    if (rc->predLayerId == LAYER_P) {
      for (i = 0; i < 6; i++) {
        if (i == LAYER_I || i == LAYER_P || rc->gopLayerFrameNum[i] == 0)
          continue;

        i32 qpTemp = RcGetHierarchicalQP(rc, q << QP_FRACTIONAL_BITS, i, 0);
        qp = (RCP_32bit)(qpTemp >> QP_FRACTIONAL_BITS);
        bits += predictSize(&(rc->rcPred[i]), qp2qScale(qp), rc->picArea) *  rc->gopLayerFrameNum[i];
        frms += rc->gopLayerFrameNum[i];
      }
    }

    goto predictWindowSizeEnd;
  }

  if (rc->sliceTypeCur == I_SLICE) {
    RCP_32bit bitsIntra = 0, bitsInter = 0;
    i32 frmsInter = 0, frmsIntra = 0;
    //2 pass only considered 4 layer, B,P,I,BREF0
    for (i = 0; i < 4; i++) {
      frms += frameNum[i];
      if (frameNum[i]) {
        i32 iq = q;
        if (i == LAYER_I && RC_IFRAME_EST_WITH_DELTA) {
          iq = q + (rc->intraQpDelta >> QP_FRACTIONAL_BITS);
          iq = CLIP3(0, 51, iq);
        }

        RCP_32bit tmpBits =
            predictSize(&(rc->rcPred[i]), qp2qScale((RCP_32bit)iq), costs[i])* frameNum[i];

        if (i == LAYER_I) {
          bitsIntra += tmpBits;
          frmsIntra += frameNum[i];
        } else {
          bitsInter += tmpBits;
          frmsInter += frameNum[i];
        }
      }
    }

    //TODO: explicit setting intraInterval by API, not initialized by bitrateWindowSize
    if (frmsInter && (frmsInter < frmsIntra * (rc->intraInterval - 1))) {
      if (frmsIntra) bits = bitsIntra / frmsIntra;
      bits += (bitsInter / frmsInter) * (rc->intraInterval - 1);
      frms = rc->intraInterval;
    } else
      bits = bitsIntra + bitsInter;
  } else {
    for (i = 0; i < 4; i++) {
      if (frameNum[i] && (i != LAYER_I)) {
        frms += frameNum[i];
        bits += predictSize(&(rc->rcPred[i]), qp2qScale((RCP_32bit)q), costs[i]) *
                frameNum[i];
      }
    }
  }

predictWindowSizeEnd:
  targetBits *= frms;
  if (targetBits) {
    diff = bits - targetBits;
    if (q >= (rc->qpMax >> QP_FRACTIONAL_BITS) && bits > targetBits && rc->qpFactor <= LL_FIXED) {
#ifdef RCP_FIXED_POINT_OPT
      rc->qpFactor = ((RCP_64bit)bits * LL_FIXED + (RCP_64bit)targetBits / 2)/ targetBits;
#else
      rc->qpFactor = bits / targetBits;
#endif
    }
  }
  return diff;
}

i32 predictQp(vcencRateControl_s *rc, RCP_32bit *costs, i32 *frameNum,
              RCP_32bit targetBits)
{
  RCP_32bit deltaBits;
  RCP_32bit diff, diff0;

  i32 i;
  i32 q = rc->rcPred[rc->predLayerId].qp;
  q = (q < 0) ? 26 : (q >> QP_FRACTIONAL_BITS);

  deltaBits = predictWindowSize(rc, costs, frameNum, q, targetBits);
  diff0 = ABS(deltaBits);
  if (diff0 == 0) return q;

  i32 step = (deltaBits > 0) ? 1 : -1;
  for (q = q + step; (q >= 0) && (q <= 51); q += step) {
    deltaBits = predictWindowSize(rc, costs, frameNum, q, targetBits);
    diff = ABS(deltaBits);
    if (diff >= diff0) {
      q -= step;
      break;
    }
    diff0 = diff;
  }

  q = CLIP3(0, 51, q);

  /* To remove, just for debug purpose */
  if (rc->pass != 2) {
    RCP_32bit var = rc->picArea;
    i32 layerBits[6] = {0,0,0,0,0,0}, sumBits = 0;
    if (rc->predLayerId == LAYER_P) {
      //P layer predict for whole GOP
      for (i = 0; i < 6; i++) {
        if (i == LAYER_I || rc->gopLayerFrameNum[i] == 0)
          continue;

        i32 qpTemp = RcGetHierarchicalQP(rc, q << QP_FRACTIONAL_BITS, i, 0);
        RCP_32bit qp = (RCP_32bit)(qpTemp >> QP_FRACTIONAL_BITS);
        i32 bits = predictSize(&(rc->rcPred[i]), qp2qScale(qp), rc->picArea) *  rc->gopLayerFrameNum[i];
        layerBits[i] += bits;
        sumBits += bits;
      }
    }

    RC_LOG_I(
        "[RC Frm%d]Prd QP: coeff %.4f offset %.2f, var %.0f, targetBits "
        "%.0fx%d=%.0f, q %d, qscale %.2f, predSize %d + %d + %d + %d + %d=%d\n",
        rc->frameCnt,
        (float)rc->rcPred[rc->predLayerId].coeff / rc->rcPred[rc->predLayerId].count,
        (float)rc->rcPred[rc->predLayerId].offset / rc->rcPred[rc->predLayerId].count, (float)var,
        (float)targetBits, rc->hierarchial_bit_allocation_GOP_size,
        (float)targetBits * rc->hierarchial_bit_allocation_GOP_size, q,
        (float)qp2qScale((RCP_32bit)q) / LL_FIXED, layerBits[LAYER_P], layerBits[LAYER_BREF_0],
        layerBits[LAYER_BREF_1], layerBits[LAYER_BREF_2], layerBits[LAYER_B], sumBits);
  }

  return q;
}

#if VBR_RC
u32 getGopSumCost(vcencRateControl_s *rc) {
  u32 sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += rc->pass1GopCost[i] * rc->pass1GopFrameNum[i];
  }
  return sum;
}

/* decide scene type by comparing current cost and short term cost to long term cost */
void checkSceneByCost(vcencRateControl_s *rc) {
  /* This method is only for CVBR mode */
  if (rc->rcMode != VCE_RC_CVBR) return;

  if (rc->predLayerId != LAYER_P) {
    if (rc->predLayerId == LAYER_I) rc->sceneComplexType = NORMAL_SCENE;

    return;
  }

  //if pass2 gop4->gop1 happens, then it should be complex scene
  if (rc->pass == 2 && rc->hierarchial_bit_allocation_GOP_size == 1 && 
      rc->agopPass1GopPicIdx >= 0)
  {
    rc->sceneComplexType = COMPLEX_SCENE;
    return;
  }

  rc->sceneComplexType = NORMAL_SCENE;
  /* update 2-pass cost statistics here since we can get current cost in advance.
     Should use lookahead costs instead of current cost? */
  if (rc->pass == 2) updateCost(rc, (RCP_64bit)rc->pass1CurCost);

  if (rc->rcPred[rc->predLayerId].shortTermCostCount > 0 &&
      rc->rcPred[rc->predLayerId].longTermCostCount > 0) {
#ifdef RCP_FIXED_POINT_OPT
    i64 shortTermCost = (rc->rcPred[rc->predLayerId].shortTermCost * LL_FIXED) / rc->rcPred[rc->predLayerId].shortTermCostCount;
    i64 simpleTh = rc->rcPred[rc->predLayerId].longTermCost * RCP_DATA_TYPE(0.9) / rc->rcPred[rc->predLayerId].longTermCostCount;
    i64 complexTh = rc->rcPred[rc->predLayerId].longTermCost * LL_FIXED / rc->rcPred[rc->predLayerId].longTermCostCount;
#else
    double shortTermCost = rc->rcPred[rc->predLayerId].shortTermCost /
                           rc->rcPred[rc->predLayerId].shortTermCostCount;
    double longTermCost = rc->rcPred[rc->predLayerId].longTermCost /
                          rc->rcPred[rc->predLayerId].longTermCostCount;

    /* not consider BFrame costs for gopSize > 1 for simple method, since BFrame number is variable */
#if 0
    if (rc->predId == P_SLICE)
    {
      i32 bframes = rc->hierarchial_bit_allocation_GOP_size - 1;
      if (bframes > 1 && rc->rcPred[3].shortTermCostCount > 0 && rc->rcPred[3].longTermCostCount > 0)
      {
        shortTermCost += rc->rcPred[3].shortTermCost / rc->rcPred[3].shortTermCostCount;
        longTermCost  += rc->rcPred[3].longTermCost  / rc->rcPred[3].longTermCostCount;
        bframes --;
      }
      if (bframes && rc->rcPred[B_SLICE].shortTermCostCount > 0 && rc->rcPred[B_SLICE].longTermCostCount > 0)
      {
        shortTermCost += (rc->rcPred[B_SLICE].shortTermCost / rc->rcPred[B_SLICE].shortTermCostCount) * bframes;
        longTermCost  += (rc->rcPred[B_SLICE].longTermCost  / rc->rcPred[B_SLICE].longTermCostCount) * bframes;
      }
    }
#endif

    /* decide scene type */
    double simpleTh = longTermCost * 0.9;
    double complexTh = longTermCost * 1.0;
#endif
    if (rc->rcPred[rc->predLayerId].cost < simpleTh && shortTermCost < simpleTh &&
        rc->encodedFramesBits * 10 >
            rc->virtualBuffer.bitPerPic * rc->frameCnt * 8 )
      rc->sceneComplexType = SIMPLE_SCENE;
    else if (rc->rcPred[rc->predLayerId].cost > complexTh)
      rc->sceneComplexType = COMPLEX_SCENE;

    RC_LOG_I(
        "[RC Frm%d]cost %.2f shortCost %.2f simpleTh %.2f "
        "complexTh %.2f scene %d tooLow %d\n",
        rc->frameCnt, (float)rc->rcPred[rc->predLayerId].cost/LL_FIXED, (float)shortTermCost/LL_FIXED,
        (float)simpleTh/LL_FIXED, (float)complexTh/LL_FIXED, rc->sceneComplexType,
        rc->encodedFramesBits <=
            rc->virtualBuffer.bitPerPic * rc->frameCnt * 0.8);
  }
  return;
}

i32 detectSceneAndTuneQp(vcencRateControl_s *rc, i32 qp, i32 prev_qp) {
  /* This method is only for CVBR mode */
  if (rc->rcMode != VCE_RC_CVBR) return qp;

  if (rc->predLayerId != LAYER_P) return qp;

  if (VBR_RC == 1) {
    i32 tmpBits = 0;
    i32 scene = NORMAL_SCENE;
    i32 id = rc->predLayerId;
    char sceneName[3][32] = {"SIMPLE SCENE", "NORMAL SCENE", "COMPLEX SCENE"};

    if (rc->pass == 2) {
      rc->previousGopCost = rc->currentGopCost;
      rc->currentGopCost = getGopSumCost(rc);
      if (rc->previousGopCost == 0) return qp;

      if (rc->hierarchial_bit_allocation_GOP_size == 4) {
        /* in GOP4, if gop cost hasn't a big decrease, treat it as complex scene */
        //if (rc->currentGopCost > (rc->previousGopCost - rc->previousGopCost/8))
        if (rc->currentGopCost >= rc->previousGopCost) scene = COMPLEX_SCENE;

        /* if gop cost has a big decrease, treat it as simple scene */
        if (rc->currentGopCost <
            (rc->previousGopCost - rc->previousGopCost / 4))
          scene = SIMPLE_SCENE;

        /* GOP8 -> GOP4, it generally is complex scene */
        if (rc->hierarchial_bit_allocation_GOP_size_previous == 8)
          scene = COMPLEX_SCENE;
      }

      if (rc->hierarchial_bit_allocation_GOP_size == 8) {
        /* in GOP8, if gop cost hasn't a big increase, treat it as simple scene */
        //if (rc->currentGopCost < (rc->previousGopCost + rc->previousGopCost/8))
        if (rc->currentGopCost <= rc->previousGopCost) scene = SIMPLE_SCENE;

        /* in GOP8, if gop cost has a big increase, treat it as complex scene */
        if (rc->currentGopCost >
            (rc->previousGopCost + rc->previousGopCost / 4))
          scene = COMPLEX_SCENE;

        /* GOP4 -> GOP8, it generally is simple scene */
        if (rc->hierarchial_bit_allocation_GOP_size_previous == 4)
          scene = SIMPLE_SCENE;
      }

      RC_LOG_I("    %s qp %d prev_qp %d currentGopCost %d previousGopCost %d\n",
               sceneName[scene], qp >> QP_FRACTIONAL_BITS,
               prev_qp >> QP_FRACTIONAL_BITS, rc->currentGopCost,
               rc->previousGopCost);

      rc->sceneComplexType = scene;
    } else {
      /* SLICE-P */
      /*
      * actualGopSize < targetGopSize: this gop is simple scene, otherwise complex scene
      * targetGopSize < previousGopSize: previous gop is complex scene that occupied too many bits
      */
      i32 diff0 = rc->previousGopSize / 8;
      i32 diff1 = rc->targetGopSize / 4;
      i32 prev_scene = rc->sceneComplexType;
      scene = NORMAL_SCENE;

      if (rc->previousGopSize > 0 &&
          rc->actualGopSize < (rc->previousGopSize - diff0))
        scene = SIMPLE_SCENE;
      else if (rc->actualGopSize < (rc->targetGopSize - diff1))
        scene = SIMPLE_SCENE;
      else if ((rc->previousGopSize > 0 &&
                rc->actualGopSize > (rc->previousGopSize + diff0)) ||
               rc->actualGopSize > (rc->targetGopSize + diff1))
        scene = COMPLEX_SCENE;

      RC_LOG_I(
          "    %s previousGopSize %d actualGopSize %d targetGopSize %d diff0 "
          "%d diff1 %d prev_qp %d qp %d\n",
          sceneName[scene], rc->previousGopSize, rc->actualGopSize,
          rc->targetGopSize, diff0, diff1, prev_qp >> QP_FRACTIONAL_BITS,
          qp >> QP_FRACTIONAL_BITS);

      rc->sceneComplexType = scene;
    }
  }

  if (rc->sceneComplexType == SIMPLE_SCENE) {
    /* only if bitBank has no limitation or current bitBank < the limitation */
    RC_LOG_I("    %s qp %d ->\n", "SIMPLE SCENE", qp >> QP_FRACTIONAL_BITS);
    qp = MAX(qp, (qp + prev_qp) / 2);
    //qp = MAX(qp, prev_qp);
    RC_LOG_I(" %d\n", qp >> QP_FRACTIONAL_BITS);
  } else if (rc->sceneComplexType == COMPLEX_SCENE && rc->bitCntBank > 0) {
    RC_LOG_I("    %s qp %d ->\n", "COMPLEX SCENE", qp >> QP_FRACTIONAL_BITS);
    qp = MIN(qp, (qp + prev_qp) / 2);
    RC_LOG_I(" %d\n", qp >> QP_FRACTIONAL_BITS);
  }

  return qp;
}

void updateBitCntBank(vcencRateControl_s *rc) {
  i32 tmpBits = 0;
  i32 scene = rc->sceneComplexType;
  rcVirtualBuffer_s *vb = &rc->virtualBuffer;

  if (rc->encodedGopFrames < rc->hierarchial_bit_allocation_GOP_size) return;

  /* This method is only for CVBR mode */
  if (rc->rcMode != VCE_RC_CVBR) return;

  if (scene == SIMPLE_SCENE) {
    tmpBits = rc->targetGopSize - rc->actualGopSize;
    if (tmpBits > 0) {
      vb->realBitCnt += tmpBits;
      rc->bitCntBank += tmpBits;
      RC_LOG_I("    %s gop%d gopBitGap %6d = %8d - %8d bitCntBank %8d\n",
               "SIMPLE_SCENE", rc->hierarchial_bit_allocation_GOP_size, tmpBits,
               rc->targetGopSize, rc->actualGopSize, rc->bitCntBank);
    }
  } else if (scene == COMPLEX_SCENE) {
    tmpBits = rc->targetGopSize - rc->actualGopSize;
    tmpBits = MAX(tmpBits, -rc->bitCntBank);
    if (tmpBits < 0) {
      vb->realBitCnt += tmpBits;
      rc->bitCntBank += tmpBits;
      RC_LOG_I("    %s gop%d gopBitGap %6d = %8d - %8d bitCntBank %8d\n",
               "COMPLEX_SCENE", rc->hierarchial_bit_allocation_GOP_size,
               tmpBits, rc->targetGopSize, rc->actualGopSize, rc->bitCntBank);
    }
  } else {
    tmpBits = rc->targetGopSize - rc->actualGopSize;
    RC_LOG_I("    %s gop%d gopBitGap %6d = %8d - %8d bitCntBank %8d\n",
             "NORMAL_SCENE", rc->hierarchial_bit_allocation_GOP_size, tmpBits,
             rc->targetGopSize, rc->actualGopSize, rc->bitCntBank);
  }

  /* Qunying: Debug purpose, to remove */
  RC_LOG_I(
      "[RC Frm%d]scene %d targetGopSize %d actualGopSize %d tmpBits %d bank "
      "%d\n",
      rc->frameCnt, scene, rc->targetGopSize, rc->actualGopSize, tmpBits,
      rc->bitCntBank);

  RC_LOG_I(
      "[RC Frm%d]encodedBits %d targetBits %d realBits %d virtualBits %d\n",
      rc->frameCnt, rc->encodedFramesBits, vb->bitPerPic * rc->frameCnt,
      vb->realBitCnt, vb->virtualBitCnt);
}

#endif

i32 getPass2Qp(vcencRateControl_s *rc) {
  i32 q, i, gopFrames;
  i32 id = rc->predLayerId;
  i32 bSlice = (id == LAYER_B) || (id >= LAYER_BREF_0);
  i32 firstP = (id == LAYER_P) && (rc->frameCnt == 1);

  if (bSlice) {
    q = rc->rcPred[LAYER_P].qp;
    if ((rc->hierarchial_bit_allocation_GOP_size <= 8) || (0 == detectBitrateAbnormal(rc)))
      goto get_pass2_qp_end;
  }
  q = predictQp(rc, rc->pass1AvgCost, rc->pass1FrameNum, rc->targetPicSize);

  if ((id == LAYER_P) && (rc->pass1EstCostAll == 0)) {
    i32 shortQp = predictQp(rc, rc->pass1GopCost, rc->pass1GopFrameNum,
                            rc->targetPicSize);
    q = (q >= shortQp) ? q : (q + shortQp + 1) >> 1;
  }

  //for pass2 gop4->gop1, make QP stable to get better quality
  if ((id != LAYER_I) && (rc->hierarchial_bit_allocation_GOP_size == 1 && rc->agopPass1GopPicIdx >= 0))
  {
    i32 shortQp = (rc->rcPred[LAYER_B].qp >> QP_FRACTIONAL_BITS);
    q = CLIP3(shortQp-1, shortQp+1, q);
  }

  q <<= QP_FRACTIONAL_BITS;
  if (bSlice || firstP) {
    q = MAX(q, rc->rcPred[LAYER_P].qp);
  }

get_pass2_qp_end:

#if 0
  for (i32 i = 0; i < 4; i ++)
  {
    printf ("================= predictor %d coeff = %f \n", i, rc->rcPred[i].coeff);
  }
#endif

  return q;
}

#endif
