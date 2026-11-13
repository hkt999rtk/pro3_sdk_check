#-------------------------------------------------------------------------------
#-       customer data test cases                                             --
#-------------------------------------------------------------------------------

binary_path="path of binary"

#HEVC test cases:

${binary_path}/hevc_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 0 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i door_1080p30_200f.yuv -o door_1080p30_200f.hevc

${binary_path}/hevc_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 0 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 204 \
        -i inside_1080p_30fps.yuv -o inside_1080p_30fps.hevc

${binary_path}/hevc_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 0 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i outdoors_1080p30_200f.yuv -o outdoors_1080p30_200f.hevc

${binary_path}/hevc_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 0 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i camerashift_1080p30_200f.yuv -o camerashift_1080p30_200f.hevc

${binary_path}/hevc_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 0 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i garageopen_1080p30_200f.yuv -o garageopen_1080p30_200f.hevc

${binary_path}/hevc_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 0 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i kitfridge_1080p30_200f.yuv -o kitfridge_1080p30_200f.hevc

#H264 test cases:

${binary_path}/h264_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 1 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i door_1080p30_200f.yuv -o door_1080p30_200f.h264

${binary_path}/h264_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 1 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 204 \
        -i inside_1080p_30fps.yuv -o inside_1080p_30fps.h264

${binary_path}/h264_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 1 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i outdoors_1080p30_200f.yuv -o outdoors_1080p30_200f.h264

${binary_path}/h264_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 1 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i camerashift_1080p30_200f.yuv -o camerashift_1080p30_200f.h264

${binary_path}/h264_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 1 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i garageopen_1080p30_200f.yuv -o garageopen_1080p30_200f.h264

${binary_path}/h264_testenc_vce \
        -f 30 -j 30 -R 30 -B 1000000 --vbr 1 -n 25 -m 48 \
        -w 1920 -h 1080 -r 0 --codecFormat 1 \
        -l 0 --gopSize=1 -U1 -u1 -q-1 -I6 -A-8 --smoothPsnrInGOP=1 --rcQpDeltaRange=15 --picQpDeltaRange=-4:6 --blockRCSize=1 \
        -b 200 \
        -i kitfridge_1080p30_200f.yuv -o kitfridge_1080p30_200f.h264
