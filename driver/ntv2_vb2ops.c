/*
 * NTV2 video buffer ops
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

#ifdef NTV2_USE_VB2_DMA_SG
#include <media/videobuf2-dma-sg.h>
#else
#include <media/videobuf2-vmalloc.h>
#endif

#ifdef NTV2_USE_VB2_V4L2_BUFFER
#define to_ntv2_vb2buf(vb) \
	container_of((to_vb2_v4l2_buffer(vb)), struct ntv2_vb2buf, vb2_v4l2_buffer)
#else
#define to_ntv2_vb2buf(vb) \
	container_of(vb, struct ntv2_vb2buf, vb2_buffer)
#endif

/*
 * Setup the constraints of the queue
 */
#ifdef NTV2_USE_QUEUE_SETUP_DEVICE
static int ntv2_queue_setup(struct vb2_queue *vq,
							unsigned int *nbuffers, unsigned int *nplanes,
							unsigned int sizes[], struct device *alloc_ctxs[])
#else
#ifdef NTV2_USE_QUEUE_SETUP_NO_FORMAT
static int ntv2_queue_setup(struct vb2_queue *vq,
							unsigned int *nbuffers, unsigned int *nplanes,
							unsigned int sizes[], void *alloc_ctxs[])
#else
#ifdef NTV2_USE_QUEUE_SETUP_PARG
static int ntv2_queue_setup(struct vb2_queue *vq, const void *parg,
							unsigned int *nbuffers, unsigned int *nplanes,
							unsigned int sizes[], void *alloc_ctxs[])
#else
static int ntv2_queue_setup(struct vb2_queue *vq, const struct v4l2_format *fmt,
							unsigned int *nbuffers, unsigned int *nplanes,
							unsigned int sizes[], void *alloc_ctxs[])
#endif
#endif
#endif
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vq);
#ifndef NTV2_USE_QUEUE_SETUP_NO_FORMAT
#ifdef NTV2_USE_QUEUE_SETUP_PARG
	const struct v4l2_format *fmt = parg;
#endif
#endif
	unsigned long flags;

	if (ntv2_vid == NULL)
		return -EPERM;

	NTV2_MSG_VIDEO_STATE("%s: vb2 queue setup\n", ntv2_vid->name);

	/* require at least 3 buffers */
	if (vq->num_buffers + *nbuffers < 3)
		*nbuffers = 3 - vq->num_buffers;

#ifdef NTV2_USE_QUEUE_SETUP_NO_FORMAT
	/* check image size */
	if (*nplanes)
		return sizes[0] < ntv2_vid->v4l2_format.sizeimage ? -EINVAL : 0;

	/* configure returned parameters */
	*nplanes = 1;
	sizes[0] = ntv2_vid->v4l2_format.sizeimage;
#else
	/* check image size */
	if (fmt && fmt->fmt.pix.sizeimage < ntv2_vid->v4l2_format.sizeimage) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* vb2 queue setup format image size too small (%d < %d)\n",
							 ntv2_vid->name,
							 (int)fmt->fmt.pix.sizeimage,
							 (int)ntv2_vid->v4l2_format.sizeimage);
		return -EINVAL;
	}

	/* configure returned parameters */
	*nplanes = 1;
	sizes[0] = fmt ? fmt->fmt.pix.sizeimage : ntv2_vid->v4l2_format.sizeimage;
#endif

	/* reset the queue */
	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	INIT_LIST_HEAD(&ntv2_vid->vb2buf_list);
	ntv2_vid->vb2buf_index = 0;
	ntv2_vid->vb2_start = false;
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);

	return 0;
}

/*
 * Initialize the buffer after creation
 */
static int ntv2_vb2buf_init(struct vb2_buffer *vb)
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vb->vb2_queue);
	struct ntv2_vb2buf *ntv2_buf = to_ntv2_vb2buf(vb);
	unsigned long flags;

	NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer init %d\n",
						  ntv2_vid->name, ntv2_vid->vb2buf_index);

	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	ntv2_buf->index = ntv2_vid->vb2buf_index++;
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);
	ntv2_buf->ntv2_vid = ntv2_vid;
	ntv2_buf->num_pages = 0;
	ntv2_buf->sgtable = NULL;
	ntv2_buf->init = true;

	return 0;
}

/*
 * Allocate buffer resources and prepare for queue
 */
static int ntv2_vb2buf_prepare(struct vb2_buffer *vb)
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vb->vb2_queue);
	struct ntv2_vb2buf *ntv2_buf = to_ntv2_vb2buf(vb);
	struct sg_table *sgtable;
	unsigned long size = ntv2_vid->v4l2_format.sizeimage;
	int ret;

	NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer prepare %d\n",
						  ntv2_vid->name, ntv2_buf->index);

	/* check the buffer size */
	if (vb2_plane_size(vb, 0) < size) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* vb2 prepare buffer too small (%d < %d)\n",
							 ntv2_vid->name, (int)vb2_plane_size(vb, 0), (int)size);
		return -EINVAL;
	}

	vb2_set_plane_payload(vb, 0, size);

#ifdef NTV2_USE_VB2_DMA_SG
	sgtable = vb2_dma_sg_plane_desc(&ntv2_buf->vb2_buffer, 0);
#else
#ifdef NTV2_USE_VB2_V4L2_BUFFER
	ret = ntv2_alloc_scatterlist(&ntv2_buf->vmalloc_table, vb2_plane_vaddr(&ntv2_buf->vb2_v4l2_buffer.vb2_buf, 0), size);
#else
	ret = ntv2_alloc_scatterlist(&ntv2_buf->vmalloc_table, vb2_plane_vaddr(&ntv2_buf->vb2_buffer, 0), size);
#endif
	if (ret < 0)
		return -EINVAL;
	sgtable = &ntv2_buf->vmalloc_table;
#endif

	/* check scatter data */
	if ((sgtable->sgl == NULL) ||
		(sgtable->nents == 0)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* vb2 prepare no scatter list\n", ntv2_vid->name);
		return -EINVAL;
	}

	/* map pages */
	ntv2_buf->num_pages = dma_map_sg(&ntv2_vid->ntv2_dev->pci_dev->dev,
									 sgtable->sgl,
									 sgtable->nents,
									 DMA_FROM_DEVICE);
	if (ntv2_buf->num_pages == 0) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* map sg failed\n", ntv2_vid->name);
#ifndef NTV2_USE_VB2_DMA_SG
		ntv2_free_scatterlist(&ntv2_buf->vmalloc_table);
#endif
		return -EINVAL;
	}
	ntv2_buf->sgtable = sgtable;

	return 0;
}

/*
 * Queue this buffer to be filled
 */
static void ntv2_vb2buf_queue(struct vb2_buffer *vb)
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vb->vb2_queue);
	struct ntv2_vb2buf *ntv2_buf = to_ntv2_vb2buf(vb);
	unsigned long flags;

	NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer queue %d\n",
						  ntv2_vid->name, ntv2_buf->index);

	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	list_add_tail(&ntv2_buf->list, &ntv2_vid->vb2buf_list);
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);

 	/* schedule the dma task */
	tasklet_schedule(&ntv2_vid->transfer_task);
}

/*
 * Free buffer resources
 */
#ifdef NTV2_USE_VB2_VOID_FINISH
static void ntv2_vb2buf_finish(struct vb2_buffer *vb)
#else
static int ntv2_vb2buf_finish(struct vb2_buffer *vb)
#endif
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vb->vb2_queue);
	struct ntv2_vb2buf *ntv2_buf = to_ntv2_vb2buf(vb);
	struct sg_table *sgtable = ntv2_buf->sgtable;

	NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer finish %d\n",
						  ntv2_vid->name, ntv2_buf->index);

	/* check scatter data */
	if ((sgtable->sgl == NULL) ||
		(sgtable->nents == 0) ||
		(ntv2_buf->num_pages == 0)) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* vb2 finish no scatter list\n", ntv2_vid->name);
		goto done;
	}

	/* unmap the pages */
	dma_unmap_sg(&ntv2_vid->ntv2_dev->pci_dev->dev,
				 sgtable->sgl,
				 sgtable->nents,
				 DMA_FROM_DEVICE);

	ntv2_buf->num_pages = 0;
#ifndef NTV2_USE_VB2_DMA_SG
	ntv2_free_scatterlist(&ntv2_buf->vmalloc_table);
#endif
	ntv2_buf->sgtable = NULL;

done:
#ifdef NTV2_USE_VB2_VOID_FINISH
	return;
#else
	return 0;
#endif
}

/*
 * Release hardware resources
 */
static void ntv2_vb2buf_cleanup(struct vb2_buffer *vb)
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vb->vb2_queue);
	struct ntv2_vb2buf *ntv2_buf = to_ntv2_vb2buf(vb);

	NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer cleanup %d\n",
						  ntv2_vid->name, ntv2_buf->index);

	/* disable video */
	ntv2_video_disable(ntv2_vid);

	ntv2_buf->init = false;
	ntv2_buf->index = 0;
}

/*
 * Return all queued buffers
 */
static void ntv2_return_all_buffers(struct ntv2_video *ntv2_vid,
									enum vb2_buffer_state state)
{
	struct ntv2_vb2buf *buf;
	struct ntv2_vb2buf *node;
	unsigned long flags;

	NTV2_MSG_VIDEO_STREAM("%s: vb2 return all buffers\n", ntv2_vid->name);

	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	list_for_each_entry_safe(buf, node, &ntv2_vid->vb2buf_list, list) {
		list_del_init(&buf->list);
#ifdef NTV2_USE_VB2_V4L2_BUFFER
		vb2_buffer_done(&buf->vb2_v4l2_buffer.vb2_buf, state);
#else
		vb2_buffer_done(&buf->vb2_buffer, state);
#endif
	}
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);
}

/*
 * Start streaming
 */
static int ntv2_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vq);
	unsigned long flags;
	int result = 0;

	NTV2_MSG_VIDEO_STATE("%s: vb2 queue start\n", ntv2_vid->name);

	ntv2_vid->vb2buf_sequence = 0;

	/* check for enough buffers queued? */
	
	/* enable video */
	result = ntv2_video_enable(ntv2_vid);
	if (result != 0) goto error;

	/* start video capture */
	result = ntv2_video_start(ntv2_vid);
	if (result != 0) goto error;

	/* synchronize vb2 start */
	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	ntv2_vid->vb2_start = true;
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);

	return result;

error:
	/* return all buffer on error */
	ntv2_return_all_buffers(ntv2_vid, VB2_BUF_STATE_QUEUED);
	return result;
}

/*
 * Stop streaming
 */
#ifdef NTV2_USE_VB2_VOID_STREAMING
static void ntv2_stop_streaming(struct vb2_queue *vq)
#else
static int ntv2_stop_streaming(struct vb2_queue *vq)
#endif
{
	struct ntv2_video *ntv2_vid = vb2_get_drv_priv(vq);
	unsigned long flags;

	NTV2_MSG_VIDEO_STATE("%s: vb2 queue stop\n", ntv2_vid->name);

	/* synchronize vb2 stop */
	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	ntv2_vid->vb2_start = false;
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);

	/* stop video */
	ntv2_video_stop(ntv2_vid);
	ntv2_video_flush(ntv2_vid);
	ntv2_video_disable(ntv2_vid);

	/* return all active buffers */
	ntv2_return_all_buffers(ntv2_vid, VB2_BUF_STATE_ERROR);

#ifdef NTV2_USE_VB2_VOID_STREAMING
	return;
#else
	return 0;
#endif
}


static struct vb2_ops ntv2_vb2ops = {
	.queue_setup		= ntv2_queue_setup,
	.buf_init			= ntv2_vb2buf_init,
	.buf_prepare		= ntv2_vb2buf_prepare,
	.buf_queue			= ntv2_vb2buf_queue,
	.buf_finish			= ntv2_vb2buf_finish,
	.buf_cleanup		= ntv2_vb2buf_cleanup,
	.start_streaming	= ntv2_start_streaming,
	.stop_streaming		= ntv2_stop_streaming,
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
};

int ntv2_vb2ops_configure(struct ntv2_video *ntv2_vid)
{
	struct vb2_queue *que;
	int result;

	/* configure the vb2 queue */
	que = &ntv2_vid->vb2_queue;
	que->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	que->io_modes = VB2_MMAP | VB2_USERPTR | VB2_READ;
	que->drv_priv = ntv2_vid;
	que->buf_struct_size = sizeof(struct ntv2_vb2buf);
	que->ops = &ntv2_vb2ops;
#ifdef NTV2_USE_VB2_DMA_SG
	que->mem_ops = &vb2_dma_sg_memops;
#else
	que->mem_ops = &vb2_vmalloc_memops;
#endif
#ifdef NTV2_USE_VB2_TIMESTAMP_FLAGS
	que->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	que->min_buffers_needed = 2;
#else
	que->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
#endif
	que->lock = &ntv2_vid->vb2_mutex;
	que->gfp_flags = 0;

	result = vb2_queue_init(que);
	if (result != 0) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* vb2_queue_init failed code %d\n",
							 ntv2_vid->name, result);
		return result;
	}
	ntv2_vid->vb2_init = true;

	return 0;
}

struct ntv2_vb2buf *ntv2_vb2ops_vb2buf_ready(struct ntv2_video *ntv2_vid)
{
	struct ntv2_vb2buf *buf = NULL;
	unsigned long flags;

	if (ntv2_vid == NULL)
		return NULL;

	/* get the next buffer */
	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	if (ntv2_vid->vb2_start && !list_empty(&ntv2_vid->vb2buf_list)) {
		buf = list_first_entry(&ntv2_vid->vb2buf_list, struct ntv2_vb2buf, list);
	}
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);

	if (buf != NULL)
		NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer ready %d\n",
							  ntv2_vid->name, buf->index);

	return buf;
}

void ntv2_vb2ops_vb2buf_done(struct ntv2_vb2buf *ntv2_buf)
{
	struct ntv2_video *ntv2_vid;
	struct v4l2_timecode *timecode;
	unsigned long flags;

	if (ntv2_buf == NULL)
		return;

	ntv2_vid = ntv2_buf->ntv2_vid;
	if (ntv2_vid == NULL)
		return;

	NTV2_MSG_VIDEO_STREAM("%s: vb2 buffer done %d\n",
						  ntv2_vid->name, ntv2_buf->index);

#ifdef NTV2_USE_VB2_V4L2_BUFFER
	if ((ntv2_buf->vb2_v4l2_buffer.flags & V4L2_BUF_FLAG_TIMECODE) != 0) {
		timecode = &ntv2_buf->vb2_v4l2_buffer.timecode;
#else
	if ((ntv2_buf->vb2_buffer.v4l2_buf.flags & V4L2_BUF_FLAG_TIMECODE) != 0) {
		timecode = &ntv2_buf->vb2_buffer.v4l2_buf.timecode;
#endif
		NTV2_MSG_VIDEO_STREAM("%s: vb2 timecode type %d  drop %s  time %02d:%02d:%02d:%02d  user %02x %02x %02x %02x\n",
							  ntv2_vid->name,
							  timecode->type,
							  (((timecode->flags & V4L2_TC_FLAG_DROPFRAME) != 0)? "y" : "n"),
							  timecode->hours, 
							  timecode->minutes,
							  timecode->seconds,
							  timecode->frames,
							  timecode->userbits[0],
							  timecode->userbits[1],
							  timecode->userbits[2],
							  timecode->userbits[3]);
	}

	/* remove from list */
	spin_lock_irqsave(&ntv2_vid->vb2_lock, flags);
	if (ntv2_vid->vb2_start) {
		list_del_init(&ntv2_buf->list);

		/* mark as done */
#ifdef NTV2_USE_VB2_V4L2_BUFFER
		vb2_set_plane_payload(&ntv2_buf->vb2_v4l2_buffer.vb2_buf, 0, ntv2_vid->v4l2_format.sizeimage);
		vb2_buffer_done(&ntv2_buf->vb2_v4l2_buffer.vb2_buf, VB2_BUF_STATE_DONE);
#else
		vb2_set_plane_payload(&ntv2_buf->vb2_buffer, 0, ntv2_vid->v4l2_format.sizeimage);
		vb2_buffer_done(&ntv2_buf->vb2_buffer, VB2_BUF_STATE_DONE);
#endif
	}
	spin_unlock_irqrestore(&ntv2_vid->vb2_lock, flags);
}
