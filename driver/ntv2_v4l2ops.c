/*
 * NTV2 v4l2 file ops
 *
 * Copyright 2016 AJA Video Systems Inc. All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ntv2_video.h"
#include "ntv2_vb2ops.h"
#include "ntv2_v4l2ops.h"
#include "ntv2_channel.h"
#include "ntv2_features.h"
#include "ntv2_konareg.h"
#include "ntv2_input.h"
#include "ntv2_register.h"

static int ntv2_querycap(struct file *file,
						 void *priv,
						 struct v4l2_capability *cap)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);

	strlcpy(cap->driver, ntv2_module_info()->name, sizeof(cap->driver));
//	strlcpy(cap->card, ntv2_vid->features->device_name, sizeof(cap->card));
	snprintf(cap->card, sizeof(cap->card),
			 "%s Channel %d", ntv2_vid->features->device_name, ntv2_vid->index + 1);
	snprintf(cap->bus_info, sizeof(cap->bus_info),
			 "PCI:%s", pci_name(ntv2_vid->ntv2_dev->pci_dev));
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE |
		V4L2_CAP_READWRITE |
		V4L2_CAP_STREAMING;
	cap->capabilities = cap->device_caps |
		V4L2_CAP_DEVICE_CAPS;

	return 0;
}

static void ntv2_v4l2ops_fill_pix_format(struct ntv2_video_format *vidf,
										 struct ntv2_pixel_format *pixf,
										 struct v4l2_pix_format *pix)
{
	pix->width = vidf->v4l2_timings.bt.width;
	pix->height = vidf->v4l2_timings.bt.height;
	if (vidf->v4l2_timings.bt.interlaced) {
		pix->field = V4L2_FIELD_INTERLACED;
	} else {
		pix->field = V4L2_FIELD_NONE;
	}
	pix->pixelformat = pixf->v4l2_pixel_format;
	pix->bytesperline = ntv2_features_line_pitch(pixf, pix->width);
	pix->sizeimage = pix->bytesperline * pix->height;
	if ((pixf->pixel_flags & ntv2_kona_pixel_rgb) != 0) {
		if ((pixf->pixel_flags & ntv2_kona_pixel_rec2020) != 0) {
			pix->colorspace = 0; // V4L2_COLORSPACE_BT2020;
		} else {
			pix->colorspace = V4L2_COLORSPACE_SRGB;
		}
	} else {
		if ((pixf->pixel_flags & ntv2_kona_pixel_rec601) != 0) {
			pix->colorspace = V4L2_COLORSPACE_SMPTE170M;
		} else if ((pixf->pixel_flags & ntv2_kona_pixel_rec2020) != 0) {
			pix->colorspace = 0; // V4L2_COLORSPACE_BT2020;
		} else {
			pix->colorspace = V4L2_COLORSPACE_REC709;
		}
	}
	pix->priv = 0;
}

bool ntv2_compatible_input_format(struct ntv2_input_format *inpf,
								  struct ntv2_video_format *vidf)
{
	bool match;

	if ((inpf == NULL) ||
		(vidf == NULL))
		return false;

	if ((inpf->frame_rate == ntv2_kona_frame_rate_none) ||
		(vidf->frame_rate == ntv2_kona_frame_rate_none))
		return false;

	match =
		((inpf->video_standard == vidf->video_standard) &&
		 (inpf->frame_rate == vidf->frame_rate) &&
		 ((inpf->frame_flags & ntv2_kona_frame_sd) == (vidf->frame_flags & ntv2_kona_frame_sd)) &&
		 ((inpf->frame_flags & ntv2_kona_frame_hd) == (vidf->frame_flags & ntv2_kona_frame_hd)) &&
		 ((inpf->frame_flags & ntv2_kona_frame_3g) == (vidf->frame_flags & ntv2_kona_frame_3g)) &&
		 ((inpf->frame_flags & ntv2_kona_frame_6g) == (vidf->frame_flags & ntv2_kona_frame_6g)) &&
		 ((inpf->frame_flags & ntv2_kona_frame_12g) == (vidf->frame_flags & ntv2_kona_frame_12g)));

	return match;
}

static struct ntv2_pixel_format
*ntv2_find_pixel_format(struct ntv2_features *features,
						u32 channel_index,
						u32 v4l2_pixel_format)
{
	struct ntv2_pixel_format *pixf;
	int i;

	for (i = 0; i < NTV2_MAX_PIXEL_FORMATS; i++) {
		pixf = ntv2_features_get_pixel_format(features, channel_index, i);
		if (pixf == NULL)
			return NULL;
		if (pixf->v4l2_pixel_format == v4l2_pixel_format)
			return pixf;
	}

	return NULL;
}

static struct ntv2_video_format
*ntv2_find_video_format(struct ntv2_features *features,
						u32 channel_index,
						struct v4l2_dv_timings* v4l2_timings)
{
	struct ntv2_video_format *vidf;
	int i;

	for (i = 0; i < NTV2_MAX_VIDEO_FORMATS; i++) {
		vidf = ntv2_features_get_video_format(features, channel_index, i);
		if (vidf == NULL)
			return NULL;
		if (ntv2_features_match_dv_timings(v4l2_timings, &vidf->v4l2_timings, 0))
			return vidf;
	}

	return NULL;
}

static struct ntv2_video_format
*ntv2_find_input_format(struct ntv2_features *features,
						u32 channel_index,
						struct ntv2_input_format *inpf)
{
	struct ntv2_video_format *vidf;
	int i;

	for (i = 0; i < NTV2_MAX_VIDEO_FORMATS; i++) {
		vidf = ntv2_features_get_video_format(features, channel_index, i);
		if (vidf == NULL)
			return NULL;
		if (ntv2_compatible_input_format(inpf, vidf))
			return vidf;
	}

	return NULL;
}

static int ntv2_try_fmt_vid_cap(struct file *file,
								void *fh,
								struct v4l2_format *format)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct v4l2_pix_format *pix = &format->fmt.pix;
	struct ntv2_pixel_format *pixf;

	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	/* find ntv2 video format to match v4l2 pixel format */
	pixf = ntv2_find_pixel_format(ntv2_vid->features,
								  ntv2_vid->ntv2_chn->index,
								  pix->pixelformat);
	if (pixf != NULL) {
		NTV2_MSG_VIDEO_STATE("%s: try_fmt_vid_cap accept pixel format %c%c%c%c\n",
							 ntv2_vid->name, NTV2_FOURCC_CHARS(&pix->pixelformat));
	} else {
		NTV2_MSG_VIDEO_STATE("%s: try_fmt_vid_cap pixel format %c%c%c%c not supported\n",
							 ntv2_vid->name, NTV2_FOURCC_CHARS(&pix->pixelformat));
		/* return default pixel format if not supported */
		pixf = ntv2_features_get_default_pixel_format(ntv2_vid->features,
													  ntv2_vid->ntv2_chn->index);
	}

	/* fill in v4l2 pixel info */
	ntv2_v4l2ops_fill_pix_format(&ntv2_vid->video_format, pixf, pix);

	return 0;
}

static int ntv2_s_fmt_vid_cap(struct file *file,
							  void *fh,
							  struct v4l2_format *format)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct v4l2_pix_format *pix = &format->fmt.pix;
	struct ntv2_pixel_format *pixf;

	/* test and fill pixel format */
	if (ntv2_try_fmt_vid_cap(file, fh, format) != 0)
		return -EINVAL;

	/* test for new pixel format */
	if (pix->pixelformat == ntv2_vid->v4l2_format.pixelformat)
		return 0;

	/* no format changes while streaming */
	if (vb2_is_busy(&ntv2_vid->vb2_queue)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* format change while streaming\n",
							 ntv2_vid->name);
		return -EBUSY;
	}

	/* find ntv2 frame format to match the v4l2 pixel format */
	pixf = ntv2_find_pixel_format(ntv2_vid->features,
								  ntv2_vid->ntv2_chn->index,
								  pix->pixelformat);
	if (pixf == NULL) {
		NTV2_MSG_VIDEO_STATE("%s: s_fmt_vid_cap reject pixel format %c%c%c%c\n",
							 ntv2_vid->name, NTV2_FOURCC_CHARS(&pix->pixelformat));
		return -EINVAL;
	}

	NTV2_MSG_VIDEO_STATE("%s: s_fmt_vid_cap accept pixel format %c%c%c%c\n",
						 ntv2_vid->name, NTV2_FOURCC_CHARS(&pix->pixelformat));

	/* update ntv2 pixel format */
	ntv2_vid->pixel_format = *pixf;

	/* update v4l2 pixel format */
	ntv2_v4l2ops_fill_pix_format(&ntv2_vid->video_format,
								 &ntv2_vid->pixel_format,
								 &ntv2_vid->v4l2_format);

	/* update hardware */
//	ntv2_channel_set_pixel_format(ntv2_vid->vid_str,
//								  &ntv2_vid->pixel_format);
	return 0;
}

static int ntv2_g_fmt_vid_cap(struct file *file,
							  void *fh,
							  struct v4l2_format *format)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);

	format->fmt.pix = ntv2_vid->v4l2_format;

	NTV2_MSG_VIDEO_STATE("%s: g_fmt_vid_cap pixel format %c%c%c%c\n",
						 ntv2_vid->name, NTV2_FOURCC_CHARS(&format->fmt.pix.pixelformat));

	return 0;
}

static int ntv2_enum_fmt_vid_cap(struct file *file,
								 void *fh,
								 struct v4l2_fmtdesc *format)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_pixel_format *pixf;

	pixf = ntv2_features_get_pixel_format(ntv2_vid->features,
										  ntv2_vid->ntv2_chn->index,
										  format->index);
	if (pixf == NULL) {
		NTV2_MSG_VIDEO_STATE("%s: enum_fmt_vid_cap index %d  done\n",
							 ntv2_vid->name, format->index);
		return -EINVAL;
	}

	strlcpy(format->description, pixf->name, sizeof(format->description));
	format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format->flags = 0;
	format->pixelformat = pixf->v4l2_pixel_format;

	NTV2_MSG_VIDEO_STATE("%s: enum_fmt_vid_cap index %d  pixel format %c%c%c%c\n",
						 ntv2_vid->name, format->index, NTV2_FOURCC_CHARS(&format->pixelformat));
	return 0;
}

static int ntv2_s_std(struct file *file, void *fh, v4l2_std_id std)
{
	/* no analog input */
	return -ENODATA;
}

static int ntv2_g_std(struct file *file, void *fh, v4l2_std_id *std)
{
	/* no analog input */
	return -ENODATA;
}

static int ntv2_querystd(struct file *file, void *fh, v4l2_std_id *std)
{
	/* no analog input */
	return -ENODATA;

	/* if there is no signal, then *std = 0 */
}

static int ntv2_cropcap(struct file *file,
						void *fh,
						struct v4l2_cropcap *cap)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	u32 standard = ntv2_vid->input_format.video_standard;
	u32 flags = ntv2_vid->input_format.frame_flags;

	if ((cap->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
		(standard == ntv2_kona_video_standard_none))
		return -EINVAL;

	cap->bounds.left = 0;
	cap->bounds.top = 0;
	cap->bounds.width = ntv2_vid->v4l2_timings.bt.width;
	cap->bounds.height = ntv2_vid->v4l2_timings.bt.height;
	cap->defrect = cap->bounds;

	/* aspect ratio x/y x:y doc says y/x but confuses vlc */
	if (standard == ntv2_kona_video_standard_525i) {
		if ((flags & ntv2_kona_frame_16x9) != 0) {
			cap->pixelaspect.numerator = 40;
			cap->pixelaspect.denominator = 33;
		} else {
			cap->pixelaspect.numerator = 10;
			cap->pixelaspect.denominator = 11;
		}
	} else if (standard == ntv2_kona_video_standard_625i) {
		if ((flags & ntv2_kona_frame_16x9) != 0) {
			cap->pixelaspect.numerator = 118;
			cap->pixelaspect.denominator = 81;
		} else {
			cap->pixelaspect.numerator = 59;
			cap->pixelaspect.denominator = 54;
		}
	} else {
		cap->pixelaspect.numerator = 1;
		cap->pixelaspect.denominator = 1;
	}

	NTV2_MSG_VIDEO_STATE("%s: cropcap width %d  height %d  pixel aspect %d:%d\n",
						 ntv2_vid->name,
						 cap->bounds.width,
						 cap->bounds.height,
						 cap->pixelaspect.numerator,
						 cap->pixelaspect.denominator);

	return 0;
}

static int ntv2_s_dv_timings(struct file *file,
							 void *fh,
							 struct v4l2_dv_timings *v4l2_timings)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;
	struct ntv2_video_format *vidf;

	NTV2_MSG_VIDEO_STATE("%s: s_dv_timings\n",
						 ntv2_vid->name);

	/* get video input config */
	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											ntv2_vid->v4l2_input);
	if (config == NULL)
		return -ENODATA;

	/* validate requested timings */
	if (!ntv2_features_valid_dv_timings(ntv2_vid->features,
										v4l2_timings,
										&config->v4l2_timings_cap)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* invalid dv timings\n",
							 ntv2_vid->name);
		return -EINVAL;
	}

	/* find CEA-861 timings */
	if (!ntv2_features_find_dv_timings_cap(ntv2_vid->features,
										   v4l2_timings,
										   &config->v4l2_timings_cap, 0)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* can not find dv timings\n",
							 ntv2_vid->name);
		return -EINVAL;
	}

	/* test for new timings */
	if (ntv2_features_match_dv_timings(v4l2_timings, &ntv2_vid->v4l2_timings, 0))
		return 0;

	/* no timing changes while streaming */
	if (vb2_is_busy(&ntv2_vid->vb2_queue)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* timings change while streaming\n",
							 ntv2_vid->name);
		return -EBUSY;
	}

	/* find ntv2 video format to match v4l2 timings */
	vidf = ntv2_find_video_format(ntv2_vid->features,
								  ntv2_vid->ntv2_chn->index,
								  v4l2_timings);
	if (vidf == NULL) {
		NTV2_MSG_VIDEO_STATE("%s: s_dv_timings reject timing  width %d  height %d  interlaced %d\n",
							 ntv2_vid->name,
							 v4l2_timings->bt.width,
							 v4l2_timings->bt.height,
							 v4l2_timings->bt.interlaced);
		return -EINVAL;
	}

	NTV2_MSG_VIDEO_STATE("%s: s_dv_timings accept timing  width %d  height %d  interlaced %d\n",
						 ntv2_vid->name,
						 v4l2_timings->bt.width,
						 v4l2_timings->bt.height,
						 v4l2_timings->bt.interlaced);

	NTV2_MSG_VIDEO_STATE("%s: set video format %s\n",
						 ntv2_vid->name, vidf->name);

	/* update ntv2 video format and v4l2 timing */
	ntv2_vid->video_format = *vidf;
	ntv2_vid->v4l2_timings = vidf->v4l2_timings;

	/* update v4l2 pixel format */
	ntv2_v4l2ops_fill_pix_format(&ntv2_vid->video_format,
								 &ntv2_vid->pixel_format,
								 &ntv2_vid->v4l2_format);

	/* update hardware */
	ntv2_input_get_input_format(ntv2_vid->ntv2_inp, config, &ntv2_vid->input_format);
//	ntv2_channel_set_input_format(ntv2_vid->vid_str,
//								  &ntv2_vid->input_format);
//	ntv2_channel_set_video_format(ntv2_vid->vid_str,
//								  &ntv2_vid->video_format);
	/* audio input follows video */
//	ntv2_channel_set_input_format(ntv2_vid->aud_str,
//								  &ntv2_vid->input_format);
	return 0;
}

static int ntv2_g_dv_timings(struct file *file,
							 void *fh,
							 struct v4l2_dv_timings *v4l2_timings)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;

	NTV2_MSG_VIDEO_STATE("%s: g_dv_timings\n",
						 ntv2_vid->name);

	/* get video input config */
	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											ntv2_vid->v4l2_input);
	if (config == NULL)
		return -ENODATA;

	*v4l2_timings = ntv2_vid->v4l2_timings;

	return 0;
}

static int ntv2_enum_dv_timings(struct file *file,
								void *fh,
								struct v4l2_enum_dv_timings *enum_timings)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;

	NTV2_MSG_VIDEO_STATE("%s: enum_dv_timings\n",
						 ntv2_vid->name);

	/* get input config */
	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											ntv2_vid->v4l2_input);
	if (config == NULL)
		return -ENODATA;

#ifdef NTV2_ZERO_ENUM_TIMINGS_PAD
	/* make compliance happy */
	enum_timings->pad = 0;
#endif

	return ntv2_features_enum_dv_timings_cap(ntv2_vid->features,
											 enum_timings,
											 &config->v4l2_timings_cap);
}

/*
 * Query the current video input format
 */
static int ntv2_query_dv_timings(struct file *file,
								 void *fh,
								 struct v4l2_dv_timings *v4l2_timings)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;
	struct ntv2_input_format inpf;
	struct ntv2_video_format *vidf;
	int res;

	NTV2_MSG_VIDEO_STATE("%s: query_dv_timings\n",
						 ntv2_vid->name);

	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											ntv2_vid->v4l2_input);
	if (config == NULL)
		return -ENODATA;
	
	/* get current input format */
	res = ntv2_input_get_input_format(ntv2_vid->ntv2_inp, config, &inpf);
	if (res < 0) {
		NTV2_MSG_VIDEO_STATE("%s: query_dv_timings input %d no signal\n",
							 ntv2_vid->name, ntv2_vid->v4l2_input);
		return -ENOLINK;
	}

	/*find ntv2 timing to match input format */
	vidf = ntv2_find_input_format(ntv2_vid->features,
								  ntv2_vid->index,
								  &inpf);
	if (vidf == NULL) {
		NTV2_MSG_VIDEO_STATE("%s: query_dv_timings no timing for input %d  format %s @ %s\n",
							 ntv2_vid->name,
							 ntv2_vid->v4l2_input,
							 ntv2_video_standard_name(inpf.video_standard),
							 ntv2_frame_rate_name(inpf.frame_rate));
		return -ERANGE;
	}

	// fill in v4l2 timings */
	*v4l2_timings = vidf->v4l2_timings;

	return 0;
}

static int ntv2_dv_timings_cap(struct file *file,
							   void *fh,
							   struct v4l2_dv_timings_cap *cap)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;

	NTV2_MSG_VIDEO_STATE("%s: dv_timings_cap\n",
						 ntv2_vid->name);

	/* get input config */
	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											ntv2_vid->v4l2_input);
	if (config == NULL)
		return -ENODATA;

	*cap = config->v4l2_timings_cap;

	return 0;
}

static int ntv2_enum_input(struct file *file,
						   void *fh,
						   struct v4l2_input *input)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;
//	struct ntv2_input_format format;

	NTV2_MSG_VIDEO_STATE("%s: enum_input %d\n",
						 ntv2_vid->name, input->index);

	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											input->index);
	if (config == NULL)
		return -EINVAL;

	input->type = V4L2_INPUT_TYPE_CAMERA;
	input->std = 0;
	input->status = 0;
	strlcpy(input->name, config->name, sizeof(input->name));
	input->capabilities = V4L2_IN_CAP_DV_TIMINGS;

	/* compliance claims obsolete */
//	ntv2_input_get_input_format(ntv2_vid->ntv2_inp, config, &format);
//	if (format.frame_rate == ntv2_kona_frame_rate_none)
//		input->status = V4L2_IN_ST_NO_SYNC;

	return 0;
}

static int ntv2_s_input(struct file *file, void *fh, unsigned int input)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_input_config *config;
	struct ntv2_video_format *vidf;
	int res;

	NTV2_MSG_VIDEO_STATE("%s: s_input %d\n",
						 ntv2_vid->name, input);

	config = ntv2_features_get_input_config(ntv2_vid->features,
											ntv2_vid->index,
											input);
	if (config == NULL)
		return -EINVAL;
	
	/* no input changes while streaming */
	if (vb2_is_busy(&ntv2_vid->vb2_queue)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* input change while streaming\n",
							 ntv2_vid->name);
		return -EBUSY;
	}

	/* update input */
	ntv2_vid->v4l2_input = input;

	/* update video device */
	ntv2_vid->video_dev.tvnorms = 0;

	/* get current input format */
	res = ntv2_input_get_input_format(ntv2_vid->ntv2_inp, config, &ntv2_vid->input_format);
	if (res < 0) {
		NTV2_MSG_VIDEO_STATE("%s: ntv2_s_input input %d no signal\n",
							 ntv2_vid->name, input);
		goto done;
	}

	/* find video format to match input format */
	vidf = ntv2_find_input_format(ntv2_vid->features,
								  ntv2_vid->index,
								  &ntv2_vid->input_format);
	if (vidf == NULL) {
		NTV2_MSG_VIDEO_STATE("%s: ntv2_s_input no timing for input %d  format %s @ %s\n",
							 ntv2_vid->name,
							 input,
							 ntv2_video_standard_name(ntv2_vid->input_format.video_standard),
							 ntv2_frame_rate_name(ntv2_vid->input_format.frame_rate));
		goto done;
	}

	NTV2_MSG_VIDEO_STATE("%s: set video format %s\n",
						 ntv2_vid->name, vidf->name);

	/* update ntv2 video format and v4l2 timing */
	ntv2_vid->video_format = *vidf;
	ntv2_vid->v4l2_timings = vidf->v4l2_timings;

	/* update hardware */
//	ntv2_channel_set_input_format(ntv2_vid->vid_str,
//								  &ntv2_vid->input_format);
//	ntv2_channel_set_video_format(ntv2_vid->vid_str,
//								  &ntv2_vid->video_format);
//	ntv2_channel_set_pixel_format(ntv2_vid->vid_str,
//								  &ntv2_vid->pixel_format);
	/* audio input follows video */
//	ntv2_channel_set_input_format(ntv2_vid->aud_str,
//								  &ntv2_vid->input_format);

done:
	/* update v4l2 pixel format */
	ntv2_v4l2ops_fill_pix_format(&ntv2_vid->video_format,
								 &ntv2_vid->pixel_format,
								 &ntv2_vid->v4l2_format);
	return 0;
}

static int ntv2_g_input(struct file *file, void *fh, unsigned int *input)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);

	*input = ntv2_vid->v4l2_input;

	NTV2_MSG_VIDEO_STATE("%s: g_input %d\n",
						 ntv2_vid->name, *input);

	return 0;
}

static int ntv2_g_parm(struct file *file, void *fh, struct v4l2_streamparm *sp)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);

	if (sp->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	sp->parm.capture.timeperframe.numerator =
		ntv2_frame_rate_duration(ntv2_vid->video_format.frame_rate);
	sp->parm.capture.timeperframe.denominator = 
		ntv2_frame_rate_scale(ntv2_vid->video_format.frame_rate);
	sp->parm.capture.readbuffers = 8;

	NTV2_MSG_VIDEO_STATE("%s: g_parm  frame rate %d/%d  buffers %d\n",
						 ntv2_vid->name,
						 sp->parm.capture.timeperframe.numerator,
						 sp->parm.capture.timeperframe.denominator,
						 sp->parm.capture.readbuffers);

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int ntv2_g_register (struct file *file, void *priv,
							  struct v4l2_dbg_register *reg)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_register *vid_reg = ntv2_vid->ntv2_dev->vid_reg;

	reg->val = ntv2_register_read(vid_reg, reg->reg);
	reg->size = 4;

	return 0;
}

static int ntv2_s_register (struct file *file, void *priv,
							  const struct v4l2_dbg_register *reg)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	struct ntv2_register *vid_reg = ntv2_vid->ntv2_dev->vid_reg;

	ntv2_register_write(vid_reg, reg->reg, reg->val);

	return 0;
}
#endif

/* 
 * The control handler
 */
static int ntv2_s_ctrl(struct v4l2_ctrl *ctrl)
{
	/*struct ntv2_video *ntv2_vid =
		container_of(ctrl->handler, struct ntv2_video, ctrl_handler);*/

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		break;
	case V4L2_CID_CONTRAST:
		break;
	case V4L2_CID_SATURATION:
		break;
	case V4L2_CID_HUE:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int ntv2_vdev_open(struct file *file)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);

	NTV2_MSG_VIDEO_STATE("%s: vdev_open\n",
						 ntv2_vid->name);

	/* update the input format on first open */
	if (atomic_inc_and_test(&ntv2_vid->video_ref)) {
		ntv2_s_input(file, file->private_data, ntv2_vid->v4l2_input);
	}

	return v4l2_fh_open(file);
}

static int ntv2_vdev_release(struct file *file)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	int ret;

	NTV2_MSG_VIDEO_STATE("%s: vdev_close\n",
						 ntv2_vid->name);

	ret = vb2_fop_release(file);

	atomic_dec(&ntv2_vid->video_ref);

	return ret;
} 

static const struct v4l2_ctrl_ops ntv2_ctrl_ops = {
	.s_ctrl = ntv2_s_ctrl,
};

static const struct v4l2_ioctl_ops ntv2_ioctl_ops = {
	.vidioc_querycap = ntv2_querycap,
	.vidioc_try_fmt_vid_cap = ntv2_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap = ntv2_s_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap = ntv2_g_fmt_vid_cap,
	.vidioc_enum_fmt_vid_cap = ntv2_enum_fmt_vid_cap,

	.vidioc_g_std = ntv2_g_std,
	.vidioc_s_std = ntv2_s_std,
	.vidioc_querystd = ntv2_querystd,

	.vidioc_cropcap = ntv2_cropcap,

	.vidioc_s_dv_timings = ntv2_s_dv_timings,
	.vidioc_g_dv_timings = ntv2_g_dv_timings,
	.vidioc_enum_dv_timings = ntv2_enum_dv_timings,
	.vidioc_query_dv_timings = ntv2_query_dv_timings,
	.vidioc_dv_timings_cap = ntv2_dv_timings_cap,

	.vidioc_enum_input = ntv2_enum_input,
	.vidioc_g_input = ntv2_g_input,
	.vidioc_s_input = ntv2_s_input,

	.vidioc_g_parm = ntv2_g_parm,

	.vidioc_reqbufs = vb2_ioctl_reqbufs,
	.vidioc_create_bufs = vb2_ioctl_create_bufs,
	.vidioc_querybuf = vb2_ioctl_querybuf,
	.vidioc_qbuf = vb2_ioctl_qbuf,
	.vidioc_dqbuf = vb2_ioctl_dqbuf,
	.vidioc_expbuf = vb2_ioctl_expbuf,
	.vidioc_streamon = vb2_ioctl_streamon,
	.vidioc_streamoff = vb2_ioctl_streamoff,

#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_register = ntv2_g_register,
	.vidioc_s_register = ntv2_s_register,
#endif

	.vidioc_log_status = v4l2_ctrl_log_status,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static const struct v4l2_file_operations ntv2_fops = {
	.owner = THIS_MODULE,
	.open = ntv2_vdev_open,
	.release = ntv2_vdev_release,
	.unlocked_ioctl = video_ioctl2,
	.read = vb2_fop_read,
	.mmap = vb2_fop_mmap,
	.poll = vb2_fop_poll,
};

int ntv2_v4l2ops_configure(struct ntv2_video *ntv2_vid)
{
	struct video_device *video_dev;
	struct v4l2_ctrl_handler *hdl;

	/* initialize the ctrl handler */
	hdl = &ntv2_vid->ctrl_handler;
	v4l2_ctrl_handler_init(hdl, 4);
	ntv2_vid->ctrl_init = true;

	/* add the controls */
	v4l2_ctrl_new_std(hdl, &ntv2_ctrl_ops,
			  V4L2_CID_BRIGHTNESS, 0, 255, 1, 127);
	v4l2_ctrl_new_std(hdl, &ntv2_ctrl_ops,
			  V4L2_CID_CONTRAST, 0, 255, 1, 16);
	v4l2_ctrl_new_std(hdl, &ntv2_ctrl_ops,
			  V4L2_CID_SATURATION, 0, 255, 1, 127);
	v4l2_ctrl_new_std(hdl, &ntv2_ctrl_ops,
			  V4L2_CID_HUE, -128, 127, 1, 0);
	if (hdl->error) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* add control failed code %d\n",
							 ntv2_vid->name, hdl->error);
		return hdl->error;
	}

	/* assign v4l2 ctrl handler */
//	ntv2_vid->v4l2_dev.ctrl_handler = hdl;

	/* assign video ops */
	video_dev = &ntv2_vid->video_dev;
	video_dev->fops = &ntv2_fops;
	video_dev->ioctl_ops = &ntv2_ioctl_ops;

	/* no analog standards */
	video_dev->tvnorms = 0;
	ntv2_vid->v4l2_std = 0;
	ntv2_vid->v4l2_input = 0;

	/* fill in the initial format/timing */
	ntv2_vid->v4l2_timings = ntv2_vid->video_format.v4l2_timings;
	ntv2_v4l2ops_fill_pix_format(&ntv2_vid->video_format,
								 &ntv2_vid->pixel_format,
								 &ntv2_vid->v4l2_format);

	return 0;
}
