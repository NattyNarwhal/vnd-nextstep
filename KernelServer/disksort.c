/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/* Copyright (c) 1995 NeXT Computer, Inc. All Rights Reserved */
/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ufs_disksubr.c	8.5 (Berkeley) 1/21/94
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/syslog.h>

/*
 * Seek sort for disks.  We depend on the driver which calls us using b_resid
 * as the current cylinder number.
 *
 * The argument ap structure holds a b_actf activity chain pointer on which we
 * keep two queues, sorted in ascending cylinder order.  The first queue holds
 * those requests which are positioned after the current cylinder (in the first
 * request); the second holds requests which came in after their cylinder number
 * was passed.  Thus we implement a one way scan, retracting after reaching the
 * end of the drive to the first request on the second queue, at which time it
 * becomes the first queue.
 *
 * A one-way scan is natural because of the way UNIX read-ahead blocks are
 * allocated.
 */

/*
 * For portability with historic industry practice, the
 * cylinder number has to be maintained in the `b_resid'
 * field.
 */
#define	b_cylinder	b_resid

void
disksort(ap, bp)
	register struct buf *ap, *bp;
{
	register struct buf *bq;

	/* If the queue is empty, then it's easy. */
	if (ap->b_actf == NULL) {
		bp->b_actf = NULL;
		ap->b_actf = bp;
		return;
	}

	/*
	 * If we lie after the first (currently active) request, then we
	 * must locate the second request list and add ourselves to it.
	 */
	bq = ap->b_actf;
	if (bp->b_cylinder < bq->b_cylinder) {
		while (bq->b_actf) {
			/*
			 * Check for an ``inversion'' in the normally ascending
			 * cylinder numbers, indicating the start of the second
			 * request list.
			 */
			if (bq->b_actf->b_cylinder < bq->b_cylinder) {
				/*
				 * Search the second request list for the first
				 * request at a larger cylinder number.  We go
				 * before that; if there is no such request, we
				 * go at end.
				 */
				do {
					if (bp->b_cylinder <
					    bq->b_actf->b_cylinder)
						goto insert;
					if (bp->b_cylinder ==
					    bq->b_actf->b_cylinder &&
					    bp->b_blkno < bq->b_actf->b_blkno)
						goto insert;
					bq = bq->b_actf;
				} while (bq->b_actf);
				goto insert;		/* after last */
			}
			bq = bq->b_actf;
		}
		/*
		 * No inversions... we will go after the last, and
		 * be the first request in the second request list.
		 */
		goto insert;
	}
	/*
	 * Request is at/after the current request...
	 * sort in the first request list.
	 */
	while (bq->b_actf) {
		/*
		 * We want to go after the current request if there is an
		 * inversion after it (i.e. it is the end of the first
		 * request list), or if the next request is a larger cylinder
		 * than our request.
		 */
		if (bq->b_actf->b_cylinder < bq->b_cylinder ||
		    bp->b_cylinder < bq->b_actf->b_cylinder ||
		    (bp->b_cylinder == bq->b_actf->b_cylinder &&
		    bp->b_blkno < bq->b_actf->b_blkno))
			goto insert;
		bq = bq->b_actf;
	}
	/*
	 * Neither a second list nor a larger request... we go at the end of
	 * the first list, which is the same as the end of the whole schebang.
	 */
insert:	bp->b_actf = bq->b_actf;
	bq->b_actf = bp;
}
