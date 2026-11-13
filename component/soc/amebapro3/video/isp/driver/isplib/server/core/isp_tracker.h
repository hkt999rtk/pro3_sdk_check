#ifndef _ISP_TRACKER_H_INC_
#define _ISP_TRACKER_H_INC_

#define TIME_TRACKER 0

#if TIME_TRACKER
void time_tracker_set_rt(void);
void time_tracker_init(void);
void time_tracker_begin(const char *str);
void time_tracker_end(const char *str);
void time_tracker_done(void);
#else
#define time_tracker_set_rt()
#define time_tracker_init()
#define time_tracker_begin(str)
#define time_tracker_end(str)
#define time_tracker_done()
#endif

#endif /* _ISP_TRACKER_H_INC_ */
