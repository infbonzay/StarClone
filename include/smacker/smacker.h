/*
	libsmacker - A C library for decoding .smk Smacker Video files
	Copyright (C) 2012-2013 Greg Kennedy

	libsmacker is a cross-platform C library which can be used for
	decoding Smacker Video files produced by RAD Game Tools.

	This software is released under the following license:
	Creative Commons Attribution-NonCommercial 2.0 (CC BY-NC 2.0)

	You are free:
	* to Share - to copy, distribute and transmit the work
	* to Remix - to adapt the work

	Under the following conditions:

	* Attribution - You must attribute the work in the manner specified by
		the author or licensor (but not in any way that suggests that
		they endorse you or your use of the work).
	* Noncommercial - You may not use this work for commercial purposes.

	This is a human-readable summary of the Legal Code (the full license).

	You should have received a copy of the full license
		along with libsmacker.  If not, see
		<http://creativecommons.org/licenses/by-nc/2.0>.
*/

#ifndef SMACKER_H
#define SMACKER_H

#include "mpq.h"

/* forward-declaration for an struct */
/* a few defines as return codes from smk_next() */
#define SMK_DONE	0x00
#define SMK_MORE	0x01
#define SMK_LAST	0x02
#define SMK_ERROR	-1

/* file-processing mode, pass to smk_open_file */
#define SMK_MODE_DISK	0x00
#define SMK_MODE_MEMORY	0x01

/* Y-scale meanings */
#define	SMK_FLAG_Y_NONE	0x00
#define	SMK_FLAG_Y_INTERLACE	0x01
#define	SMK_FLAG_Y_DOUBLE	0x02

#define SMK_FULL_DECODE		0x00
#define SMK_SKIP_DECODE		0x01
/* track mask and enable bits */
#define	SMK_AUDIO_TRACK_0	0x01
#define	SMK_AUDIO_TRACK_1	0x02
#define	SMK_AUDIO_TRACK_2	0x04
#define	SMK_AUDIO_TRACK_3	0x08
#define	SMK_AUDIO_TRACK_4	0x10
#define	SMK_AUDIO_TRACK_5	0x20
#define	SMK_AUDIO_TRACK_6	0x40
#define	SMK_VIDEO_TRACK	0x80

/* SMACKER DATA STRUCTURES */
struct smk_video_t
{
		/* enable/disable decode switch */
		unsigned char enable;

		/* video info */
		unsigned int	w;
		unsigned int	h;
		/* Y scale mode (constants defined in smacker.h)
			0: unscaled
			1: doubled
			2: interlaced */
		unsigned char	y_scale_mode;

		/* version ('2' or '4') */
		unsigned char	v;

		/* Huffman trees */
/*		unsigned int tree_size[4]; */
		struct smk_huff_big_t	*tree[4];

		/* Palette data type: pointer to last-decoded-palette */
		unsigned char *palette;
		/* Last-unpacked frame */
		unsigned char *frame;
};
struct smk_audio_t
{
		/* enable/disable switch (per track) */
		unsigned char enable;

		/* Info */
		unsigned char	channels;
		unsigned char	bitdepth;
		unsigned int	rate;

/* 		int	max_buffer; */

		/* compression type
			0: raw PCM
			1: SMK DPCM
			2: Bink (Perceptual), unsupported */
		unsigned char	compress;

		/* pointer to last-decoded-audio-buffer */
		void	*buffer;
		unsigned int	buffer_size;
};
struct smk_t
{
	/* meta-info */
	/* file mode: see flags, smacker.h */
	unsigned char	mode;

	/* microsec per frame, total frames
		stored as a double to handle scaling (large positive millisec / frame values may
		overflow a ul */
	double	usf;

	unsigned int  f;
	/* does file contain a ring frame? */
	unsigned char ring_frame;

	/* Index of current frame */
	unsigned int	cur_frame;

	/* SOURCE union.
		Where the data is going to be read from (or be stored),
		depending on the file mode. */
	union
	{
		struct
		{
			/* on-disk mode */
//			FILE	*fp;
			HANDLE	fp;
			unsigned int	*chunk_offset;
		} file;

		/* in-memory mode: unprocessed chunks */
		unsigned char	**chunk_data;
	} source;

	/* shared
		array of "chunk sizes"*/
	unsigned int	*chunk_size;

	/* Holds per-frame flags (i.e. 'keyframe') */
	unsigned char	*keyframe;
	/* Holds per-frame type mask (e.g. 'audio track 3, 2, and palette swap') */
	unsigned char	*frame_type;

	/* pointers to video and audio structures */
	/* Video data type: enable/disable decode switch,
		video info and flags,
		pointer to last-decoded-palette */
	struct smk_video_t *video;

	/* audio structure */
	struct smk_audio_t *audio[7];
};

/* PUBLIC FUNCTIONS */

/* OPEN OPERATIONS */
/* open an smk (from a file) */
smk_t *smk_open_file(HANDLE hmpq,const char *filename, unsigned char mode);
/* read an smk (from a memory buffer) */
smk_t *smk_open_memory(const unsigned char *buffer, unsigned int size);

/* CLOSE OPERATIONS */
/* close out an smk file and clean up memory */
void smk_close(smk_t *object);

/* GET FILE INFO OPERATIONS */
char smk_info_all(const smk_t *object, unsigned int *frame, unsigned int *frame_count, double *usf);
char smk_info_video(const smk_t *object, unsigned int *w, unsigned int *h, unsigned char *y_scale_mode);
char smk_info_audio(const smk_t *object, unsigned char *track_mask, unsigned char channels[7], unsigned char bitdepth[7], unsigned int audio_rate[7]);

/* ENABLE/DISABLE Switches */
char smk_enable_all(smk_t *object, unsigned char mask);
char smk_enable_video(smk_t *object, unsigned char enable);
char smk_enable_audio(smk_t *object, unsigned char track, unsigned char enable);

/* Retrieve palette */
unsigned char * smk_get_palette(const smk_t *object);
/* Retrieve video frame, as a buffer of size w*h */
unsigned char * smk_get_video(const smk_t *object);
/* Retrieve decoded audio chunk, track N */
unsigned char * smk_get_audio(const smk_t *object, unsigned char track);
/* Get size of currently pointed decoded audio chunk, track N */
unsigned int smk_get_audio_size(const smk_t *object, unsigned char track);

/* rewind to first frame and unpack */
char smk_first(smk_t *object,int flagdecode);
/* advance to next frame and unpack */
char smk_next(smk_t *object,int flagdecode);
/* seek to first keyframe before/at N in an smk */
char smk_seek_keyframe(smk_t *object, unsigned int );

#endif
