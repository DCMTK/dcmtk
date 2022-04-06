/*-
 * Copyright (C) 2010 Gabor Kovesdan <gabor@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef CITRUS_LOCK_H
#define CITRUS_LOCK_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_THREADS

#ifdef HAVE_WINDOWS_H

#include <windows.h>
#define WLOCK(lock)  AcquireSRWLockExclusive(lock);
#define UNLOCK(lock) ReleaseSRWLockExclusive(lock);

#else /* HAVE_WINDOWS_H */

#ifdef HAVE_PTHREAD_H

#include <pthread.h>
#define WLOCK(lock)  pthread_rwlock_wrlock(lock);
#define UNLOCK(lock) pthread_rwlock_unlock(lock);

#else /* HAVE_PTHREAD_H */

#error Threads are enabled but no read/write lock function was found for module oficonv

#endif /* HAVE_PTHREAD_H */

#endif /* HAVE_WINDOWS_H */

#else /* WITH_THREADS */

#define WLOCK(lock)  /* nothing */ ;
#define UNLOCK(lock) /* nothing */ ;

#endif /* WITH_THREADS */

#endif /* CITRUS_LOCK_H */
