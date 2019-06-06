#ifndef AWS_COMMON_RING_BUFFER_H
#define AWS_COMMON_RING_BUFFER_H
/*
 * Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws/common/atomics.h>
#include <aws/common/byte_buf.h>

/**
 * Lockless ring buffer implementation that is thread safe assuming a single thread acquires and a single thread releases.
 * For any other use case (other than the single-threaded use-case), must manage thread-safety manually.
 * Also note: release must happen in the same order as acquire.
 */
struct aws_ring_buffer {
    struct aws_allocator *allocator;
    uint8_t *allocation;
    struct aws_atomic_var head;
    struct aws_atomic_var tail;
    uint8_t *allocation_end;
};

AWS_EXTERN_C_BEGIN

/**
 * Initializes a ring buffer with an allocation of size `size`. Returns AWS_OP_SUCCESS on a successful initialization,
 * AWS_OP_ERR otherwise. 
 */
AWS_COMMON_API int aws_ring_buffer_init(struct aws_ring_buffer *ring_buf, struct aws_allocator *allocator, size_t size);

/**
 * Cleans up the ring buffer's resources.
 */
AWS_COMMON_API void aws_ring_buffer_clean_up(struct aws_ring_buffer *ring_buf);

/**
 * Attempts to acquire `requested_size` buffer and stores the result in `dest` if successful. Returns AWS_OP_SUCCESS if
 * the requested size was available for use, AWS_OP_ERR otherwise. 
 */
AWS_COMMON_API int aws_ring_buffer_acquire_hard(struct aws_ring_buffer *ring_buf, size_t requested_size, struct aws_byte_buf *dest);

/**
 * Attempts to acquire `requested_size` buffer and stores the result in `dest` if successful. If not available, it will attempt to
 * acquire anywhere from 1 byte to `requested_size`. Returns AWS_OP_SUCCESS if some buffer space is available for use, 
 * AWS_OP_ERR otherwise.
 */
AWS_COMMON_API int aws_ring_buffer_acquire_soft(struct aws_ring_buffer *ring_buf, size_t requested_size, struct aws_byte_buf *dest);

/**
 * Releases `buf` back to the ring buffer for further use. This function must be called on buffers in the order in which they were
 * acquired or bad things will happen.
 */
AWS_COMMON_API void aws_ring_buffer_release(struct aws_ring_buffer *ring_buffer, const struct aws_byte_buf *buf);

/**
 * Returns true if the memory in `buf` was vended by this ring buffer, false otherwise.
 */
AWS_COMMON_API bool aws_ring_buffer_buf_belongs_to_pool(const struct aws_ring_buffer *ring_buffer, const struct aws_byte_buf *buf);

AWS_EXTERN_C_END

#endif /* AWS_COMMON_RING_BUFFER_H */
