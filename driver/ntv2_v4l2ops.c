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
						struct v4l2_dv_timings* v4l2_timings,
						bool drop_frame)
{
	struct ntv2_video_format *vidf;
	int i;

	for (i = 0; i < NTV2_MAX_VIDEO_FORMATS; i++) {
		vidf = ntv2_features_get_video_format(features, channel_index, i);
		if (vidf == NULL)
			return NULL;
		if (ntv2_features_match_dv_timings(v4l2_timings, &vidf->v4l2_timings, 0) &&
			(drop_frame == ntv2_frame_rate_drop(vidf->frame_rate)))
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

	/* update video state */
	ntv2_video_update(ntv2_vid);

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

#ifdef NTV2_USE_PIXEL_ASPECT
static int ntv2_pixelaspect(struct file *file,
							void *priv,
							int type,
							struct v4l2_fract *f)
{
	struct ntv2_video *ntv2_vid = video_drvdata(file);
	u32 standard = ntv2_vid->input_format.video_standard;
	u32 flags = ntv2_vid->input_format.frame_flags;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	/* aspect ratio x/y x:y doc says y/x but confuses vlc */
	if (standard == ntv2_kona_video_standard_525i) {
		if ((flags & ntv2_kona_frame_16x9) != 0) {
			f->numerator = 40;
			f->denominator = 33;
		} else {
			f->numerator = 10;
			f->denominator = 11;
		}
	} else if (standard == ntv2_kona_video_standard_625i) {
		if ((flags & ntv2_kona_frame_16x9) != 0) {
			f->numerator = 118;
			f->denominator = 81;
		} else {
			f->numerator = 59;
			f->denominator = 54;
		}
	} else {
		f->numerator = 1;
		f->denominator = 1;
	}

	NTV2_MSG_VIDEO_STATE("%s: pixelaspect pixel aspect %d:%d\n",
						 ntv2_vid->name,
						 f->numerator,
						 f->denominator);

	return 0;
}
#else
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
#endif

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

	/* no timing changes while streaming */
	if (vb2_is_busy(&ntv2_vid->vb2_queue)) {
		/* test for new timings */
		if (ntv2_features_match_dv_timings(v4l2_timings, &ntv2_vid->v4l2_timings, 0))
			return 0;

		NTV2_MSG_VIDEO_ERROR("%s: *error* timings change while streaming\n",
							 ntv2_vid->name);
		return -EBUSY;
	}

	/* find ntv2 video format to match v4l2 timings */
	vidf = ntv2_find_video_format(ntv2_vid->features,
								  ntv2_vid->ntv2_chn->index,
								  v4l2_timings,
								  ntv2_vid->drop_frame);
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

	/* update video state */
	ntv2_video_update(ntv2_vid);

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

	/* fill in v4l2 timings */
	*v4l2_timings = vidf->v4l2_timings;

	/* save the drop frame hint */
	ntv2_vid->drop_frame = ntv2_frame_rate_drop(vidf->frame_rate);

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

	/* save the drop frame hint */
	ntv2_vid->drop_frame = ntv2_frame_rate_drop(vidf->frame_rate);

done:
	/* update v4l2 pixel format */
	ntv2_v4l2ops_fill_pix_format(&ntv2_vid->video_format,
								 &ntv2_vid->pixel_format,
								 &ntv2_vid->v4l2_format);

	/* update video state */
	ntv2_video_update(ntv2_vid);

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
	A bunch of this fixed point code was lifted from libfixmath
	https://github.com/PetteriAimonen/libfixmath

	it is MIT licensed
	eventually this code needs to go in its own file
*/

static const int32_t ntv2_fp16_maximum  = 0x7FFFFFFF; /*!< the maximum value of fp16 */
static const int32_t ntv2_fp16_minimum  = 0x80000000; /*!< the minimum value of fp16 */
static const int32_t ntv2_fp16_max_positive = 0x7FFeFFFF; /*!< the minimum value of fp16 */
static const int32_t ntv2_fp16_overflow = 0x80000000; /*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */
static const int32_t ntv2_fp16_one = 0x00010000; /*!< fp16 value of 1 */
static const int32_t ntv2_fp16_e   = 178145;     /*!< fp16 value of e */

int32_t ntv2_fp_make(int32_t val, int32_t frac)
{
	int32_t fp;
	fp = ((val&0xffff) << 16) + (frac&0xffff);
	return fp;
}

int16_t ntv2_fp_round(int32_t in_fp)
{
	int16_t retValue;

	if ( in_fp < 0 )
	{
	  retValue = (int16_t)(-((-in_fp+0x8000)>>16));
	}
	else
	{
	  retValue = (int16_t)((in_fp + 0x8000)>>16);
	}
	return retValue;
}

int32_t ntv2_fp_frac(int32_t in_fp)
{
  int32_t retValue;

  if ( in_fp < 0 )
  {
	retValue = -in_fp&0xffff;
  }
  else
  {
	retValue = in_fp&0xffff;
  }

  return retValue;
}

int32_t ntv2_fp_trunc(int32_t in_fp)
{
  return (in_fp>>16);
}

int16_t ntv2_fp_mix(int16_t min, int16_t max, int32_t mixer)
{
	int32_t result = (max-min)*mixer+min;
	return ntv2_fp_round(result);
}

int32_t ntv2_fp_mul16(int32_t x, int32_t y)
{
	int64_t product = (int64_t)x * (int64_t)y;
	int32_t result = (int32_t)(product >> 16);
	bool negative_x = (bool)(x & ntv2_fp16_overflow);
	bool negative_y = (bool)(y & ntv2_fp16_overflow);
	bool negative_result = negative_x != negative_y;

	if (!negative_result && (result == ntv2_fp16_maximum || result & ntv2_fp16_overflow))
	{
		//over flow, clamp to max positive number
		result = 0x7ffeffff;
	}

	return result;
}

int32_t ntv2_fp_div16(int32_t x, int32_t y)
{
	return ((int64_t)x * (1 << 16)) / y;
}

int32_t ntv2_fp_sqrt(int32_t inValue)
{
	uint8_t  neg = (inValue < 0);
	uint32_t num = (neg ? -inValue : inValue);
	uint32_t result = 0;
	uint32_t bit;
	uint8_t  n;

	// Many numbers will be less than 15, so
	// this gives a good balance between time spent
	// in if vs. time spent in the while loop
	// when searching for the starting value.
	if (num & 0xFFF00000)
		bit = (uint32_t)1 << 30;
	else
		bit = (uint32_t)1 << 18;

	while (bit > num) bit >>= 2;

	// The main part is executed twice, in order to avoid
	// using 64 bit values in computations.
	for (n = 0; n < 2; n++)
	{
		// First we get the top 24 bits of the answer.
		while (bit)
		{
			if (num >= result + bit)
			{
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else
			{
				result = (result >> 1);
			}
			bit >>= 2;
		}

		if (n == 0)
		{
			// Then process it again to get the lowest 8 bits.
			if (num > 65535)
			{
				// The remainder 'num' is too large to be shifted left
				// by 16, so we have to add 1 to result manually and
				// adjust 'num' accordingly.
				// num = a - (result + 0.5)^2
				//	 = num + result^2 - (result + 0.5)^2
				//	 = num - result - 0.5
				num -= result;
				num = (num << 16) - 0x8000;
				result = (result << 16) + 0x8000;
			}
			else
			{
				num <<= 16;
				result <<= 16;
			}

			bit = 1 << 14;
		}
	}

	// Finally, if next bit would have been 1, round the result upwards.
	if (num > result)
	{
		result++;
	}

	return (neg ? -(int32_t)result : (int32_t)result);
}

int32_t ntv2_fp_exp16(int32_t inValue)
{
	bool neg;
	int32_t result;
	int32_t term;
	uint8_t i;

	if(inValue == 0        ) return ntv2_fp16_one;
	if(inValue == ntv2_fp16_one) return ntv2_fp16_e;
	if(inValue >= 681391   ) return ntv2_fp16_maximum;
	if(inValue <= -772243  ) return 0;

	/* The algorithm is based on the power series for exp(x):
	 * http://en.wikipedia.org/wiki/Exponential_function#Formal_definition
	 *
	 * From term n, we get term n+1 by multiplying with x/n.
	 * When the sum term drops to zero, we can stop summing.
	 */

	// The power-series converges much faster on positive values
	// and exp(-x) = 1/exp(x).
	neg = (inValue < 0);
	if (neg) inValue = -inValue;

	result = inValue + ntv2_fp16_one;
	term = inValue;

	for (i = 2; i < 30; i++)
	{
		term = ntv2_fp_mul16(term, ntv2_fp_div16(inValue, ntv2_fp_make(i, 0)));
		result += term;

		if ((term < 500) && ((i > 15) || (term < 20)))
			break;
	}

	if (neg) result = ntv2_fp_div16(ntv2_fp16_one, result);

	return result;
}

int32_t ntv2_fp_log16(int32_t inValue)
{
	const int32_t e_to_fourth = 3578144;
	int32_t guess = ntv2_fp_make(2, 0);
	int32_t delta;
	int scaling = 0;
	int count = 0;
	int32_t e = 0;

	if (inValue <= 0)
		return ntv2_fp16_minimum;

	// Bring the value to the most accurate range (1 < x < 100)
	while (inValue > ntv2_fp_make(100, 0))
	{
		inValue = ntv2_fp_div16(inValue, e_to_fourth);
		scaling += 4;
	}

	while (inValue < ntv2_fp16_one)
	{
		inValue = ntv2_fp_mul16(inValue, e_to_fourth);
		scaling -= 4;
	}

	do
	{
		// Solving e(x) = y using Newton's method
		// f(x) = e(x) - y
		// f'(x) = e(x)
		e = ntv2_fp_exp16(guess);
		delta = ntv2_fp_div16(inValue - e, e);

		// It's unlikely that logarithm is very large, so avoid overshooting.
		if (delta > ntv2_fp_make(3, 0))
			delta = ntv2_fp_make(3, 0);

		guess += delta;
	} while ((count++ < 10)
		&& ((delta > 1) || (delta < -1)));

	return guess + ntv2_fp_make(scaling, 0);
}

int32_t ntv2_fp_pow16(int32_t fp_x, int32_t fp_y)
{
	int32_t result;
	bool negative_x;

	if (fp_x == 0) return 0;

	result = ntv2_fp_exp16(ntv2_fp_mul16(fp_y, ntv2_fp_log16(fp_x)));
	negative_x = (bool)(fp_x & ntv2_fp16_overflow);

	if (!negative_x && (result == ntv2_fp16_maximum))
	{
		//over flow, clamp to max positive number
		result = 0x7ffeffff;
	}

	return result;
}

int32_t ntv2_fp_lift_gamma_gain(int32_t input_val, int32_t fp16_lift, int32_t fp16_gamma, int32_t fp16_gain)
{
	// pow(input, gamma) * gain + lift
	int32_t fp_val;
	int32_t fp_gamma_part;
	int32_t fp_gamma_gain;
	int32_t fp_result;

	fp_val = ntv2_fp_make(input_val, 0);
	fp_gamma_part = ntv2_fp_pow16(fp_val, fp16_gamma);
	if (fp_gamma_part >= ntv2_fp16_max_positive && (fp16_gain >> 16))
	{
		// multiplying the maximum positive with a multiplier over 1 will overflow
		return ntv2_fp16_max_positive;
	}
	fp_gamma_gain = ntv2_fp_mul16(fp_gamma_part, fp16_gain);
	if (fp_gamma_gain >= ntv2_fp16_max_positive || (fp_gamma_gain & ntv2_fp16_overflow))
	{
		// catch any overflows from multiplication
		return ntv2_fp16_max_positive;
	}

	fp_result = fp_gamma_gain + fp16_lift;
	if (fp16_lift > 0 && fp_result < 0)
	{
		// if lift is positive and result is negative got an overflow
		fp_result = ntv2_fp16_max_positive;
	}

	return fp_result;
}

#define ntv2_max(x,y)     (((x) > (y)) ? (x):(y))
#define ntv2_min(x,y)     (((x) > (y)) ? (y):(x))
#define ntv2_clamp(x,y,z) (ntv2_min(ntv2_max((x),(y)),(z)))

/* 
 * The control handler
 */
static int ntv2_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct ntv2_video *ntv2_vid =
		container_of(ctrl->handler, struct ntv2_video, ctrl_handler);

	struct ntv2_channel_stream *stream = ntv2_vid->vid_str;
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_register *vid_reg = ntv2_chn->vid_reg;

	int32_t fp_maxVal = ntv2_fp_make(1024, 0);
	int32_t fp_val = 0;
	int32_t fp_range;
	int32_t fp_percentage;
	int32_t fp_result;
	int32_t int_result;
	int lut_bank = ntv2_chn->index;
	int i=0;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		stream->video.brightness = ctrl->val;
		fp_val = ntv2_fp_make(ctrl->val, 0);
		fp_range  = ntv2_fp_make(ctrl->maximum - ctrl->minimum, 0);
		fp_percentage = ntv2_fp_div16(fp_val, fp_range);
		fp_result = ntv2_fp_mul16(fp_maxVal, fp_percentage);
		stream->video.brightness_fp16 = fp_result;

		//int_result = ntv2_fp_round(fp_result);
		//NTV2_MSG_INFO("%s in ntv2_s_ctrl V4l2_CID_BRIGHTNESS, value = %d, lift value = %d", "sean", ctrl->val, int_result);
		break;
	case V4L2_CID_GAMMA:
		stream->video.gamma = ctrl->val;
		fp_val = ntv2_fp_make(ctrl->val, 0);
		fp_range = ntv2_fp_make(100, 0);
		fp_result = ntv2_fp_div16(fp_val, fp_range);
		stream->video.gamma_fp16 = fp_result;

		//int_result = ntv2_fp_round(fp_result);
		//NTV2_MSG_INFO("%s in ntv2_s_ctrl V4L2_CID_GAMMA, value = %d, gamma value = %d, gamma value fp16 = %d", "sean", ctrl->val, int_result, fp_result);
		break;
	case V4L2_CID_GAIN:
		stream->video.gain = ctrl->val;
		fp_val = ntv2_fp_make(ctrl->val, 0);
		fp_range = ntv2_fp_make(100, 0);
		fp_result = ntv2_fp_div16(fp_val, fp_range);
		stream->video.gain_fp16 = fp_result;

		//int_result = ntv2_fp_round(fp_result);
		//NTV2_MSG_INFO("%s in ntv2_s_ctrl V4L2_CID_GAIN, value = %d, gain value = %d, gain value fp16 = %d", "sean", ctrl->val, int_result, fp_result);
		break;
	case V4L2_CID_SATURATION:
		break;
	case V4L2_CID_HUE:
		break;
	default:
		return -EINVAL;
	}

	// calculate LUTs
	for (i = 0; i < 1024; i++) {
		fp_result = ntv2_fp_lift_gamma_gain(i,
											stream->video.brightness_fp16,
											stream->video.gamma_fp16,
											stream->video.gain_fp16);
		int_result = ntv2_fp_round(fp_result);
		stream->video.lut_red[i]   = (u16)ntv2_clamp(0, int_result, 1023);
		stream->video.lut_green[i] = (u16)ntv2_clamp(0, int_result, 1023);
		stream->video.lut_blue[i]  = (u16)ntv2_clamp(0, int_result, 1023);
	}
	ntv2_lut_set_enable(vid_reg, stream->video.lut_index, true);
	ntv2_lut_set_color_correction_host_access_bank_v2(vid_reg, ntv2_chn->index, lut_bank);
	ntv2_lut_write_10bit_tables(vid_reg, true, stream->video.lut_red, stream->video.lut_green, stream->video.lut_blue);
	ntv2_lut_set_enable(vid_reg, stream->video.lut_index, false);

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

#ifdef NTV2_USE_PIXEL_ASPECT
	.vidioc_g_pixelaspect = ntv2_pixelaspect,
#else	
	.vidioc_cropcap = ntv2_cropcap,
#endif
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
			  V4L2_CID_BRIGHTNESS, -100, 100, 1, 0);
	v4l2_ctrl_new_std(hdl, &ntv2_ctrl_ops,
			  V4L2_CID_GAMMA, 0, 200, 1, 100);
	v4l2_ctrl_new_std(hdl, &ntv2_ctrl_ops,
			  V4L2_CID_GAIN, 0, 200, 1, 100);
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
	ntv2_vid->v4l2_dev.ctrl_handler = hdl;

	v4l2_ctrl_handler_setup(hdl);

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
