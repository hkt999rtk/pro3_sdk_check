/* Opt-in test; not included in production CMake. These checks do not exercise
 * IPC or hardware retirement. Run only in an authorized test build. */
#include "video_osd_internal.h"

#define CHECK(c) do { if (!(c)) { failures++; } } while (0)

typedef struct {
	int result;
	int calls;
} retry_fixture_t;

static int retry_operation(void *context)
{
	retry_fixture_t *fixture = context;
	fixture->calls++;
	return fixture->result;
}

int video_osd_sync_test(void)
{
	int failures = 0;
	video_osd_stream_status_t status = {0};
	CHECK(video_osd_sync_check_applied(&status, 0) == 0);
	CHECK(video_osd_sync_check_applied(&status, 1) == VIDEO_OSD_EINVAL);
	status.requested_revision = 5;
	status.applied_revision = 4;
	status.pending = 1;
	CHECK(video_osd_sync_check_applied(&status, 5) == VIDEO_OSD_EBUSY);
	CHECK(video_osd_sync_check_applied(&status, 4) == 0);
	status.applied_revision = 5;
	status.requested_revision = 6;
	/* A newer clock/NN transaction must not prolong the wait for revision 5. */
	CHECK(video_osd_sync_check_applied(&status, 5) == 0);
	status.last_error = VIDEO_OSD_EENCODER;
	CHECK(video_osd_sync_check_applied(&status, 5) == VIDEO_OSD_EBUSY);
	status.pending = 0;
	CHECK(video_osd_sync_check_applied(&status, 5) == VIDEO_OSD_EENCODER);
	status.last_error = 0;
	CHECK(video_osd_sync_check_applied(&status, 6) == VIDEO_OSD_EENCODER);
	status.applied_revision = status.requested_revision = UINT32_MAX;
	CHECK(video_osd_sync_check_applied(&status, UINT32_MAX) == 0);

	/* timeout=0 still attempts once; only EBUSY becomes ETIMEDOUT. */
	const int results[] = {0, VIDEO_OSD_EINVAL, VIDEO_OSD_ENOMEM, VIDEO_OSD_EENCODER, VIDEO_OSD_ENOSPC, VIDEO_OSD_EBUSY};
	for (unsigned i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		retry_fixture_t fixture = {.result = results[i]};
		CHECK(video_osd_sync_retry(retry_operation, &fixture, 0) ==
			  (results[i] == VIDEO_OSD_EBUSY ? VIDEO_OSD_ETIMEDOUT : results[i]));
		CHECK(fixture.calls == 1);
	}
	video_osd_logo_t logo = {0};
	video_osd_timestamp_t clock = {0};
	video_osd_privacy_t privacy = {0};
	video_osd_stream_t stream = {0};
	CHECK(video_osd_logo_hide_sync(&logo, 0) == 0);
	CHECK(video_osd_timestamp_stop_sync(&clock, 0) == 0);
	CHECK(video_osd_privacy_deinit_sync(&privacy, 0) == 0);
	CHECK(video_osd_stream_deinit_sync(&stream, 0) == 0);
	CHECK(video_osd_logo_hide_sync(NULL, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_timestamp_stop_sync(NULL, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_privacy_deinit_sync(NULL, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_stream_deinit_sync(NULL, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_stream_wait_applied(NULL, 0, 0, NULL) == VIDEO_OSD_EINVAL);
	uint32_t revision = 123;
	CHECK(video_osd_privacy_apply_sync(NULL, NULL, &revision, 0) == VIDEO_OSD_EINVAL);
	CHECK(revision == 0);
	/* Sync starts reject invalid/reused contexts before touching the encoder. */
	video_osd_logo_config_t logo_config = {0};
	video_osd_timestamp_config_t clock_config = {0};
	CHECK(video_osd_logo_show_sync(NULL, &logo_config, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_logo_show_sync(&logo, NULL, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_logo_show_sync(&logo, &logo_config, 0) == VIDEO_OSD_EINVAL);
	CHECK(logo.priv == NULL);
	CHECK(video_osd_timestamp_start_sync(NULL, &clock_config, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_timestamp_start_sync(&clock, NULL, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_timestamp_start_sync(&clock, &clock_config, 0) == VIDEO_OSD_EINVAL);
	CHECK(clock.priv == NULL);
	/* Sentinels must never be dereferenced when the context is already live. */
	logo.priv = &logo_config;
	clock.priv = &clock_config;
	CHECK(video_osd_logo_show_sync(&logo, &logo_config, 0) == VIDEO_OSD_EINVAL);
	CHECK(video_osd_timestamp_start_sync(&clock, &clock_config, 0) == VIDEO_OSD_EINVAL);
	CHECK(logo.priv == &logo_config && clock.priv == &clock_config);
	return failures;
}
