set(LIB_NAME de_bespoke_boost_asio_standalone) # cmake target name
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_INCLUDE_DIR}/libxml/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_INCLUDE_DIR}/libxml/*.cpp")

    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    add_library(${LIB_NAME} INTERFACE)

    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBASIO=1)
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBBOOSTASIO=1)

endif()

#[[

set(LIB_NAME de_bespoke_boost_asio_standalone)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/asio.hpp
        # asio/
        ${LIB_HEADER_DIR}/asio/any_io_executor.hpp
        ${LIB_HEADER_DIR}/asio/associated_allocator.hpp
        ${LIB_HEADER_DIR}/asio/associated_executor.hpp
        ${LIB_HEADER_DIR}/asio/async_result.hpp
        ${LIB_HEADER_DIR}/asio/awaitable.hpp
        ${LIB_HEADER_DIR}/asio/basic_datagram_socket.hpp
        ${LIB_HEADER_DIR}/asio/basic_deadline_timer.hpp
        ${LIB_HEADER_DIR}/asio/basic_io_object.hpp
        ${LIB_HEADER_DIR}/asio/basic_raw_socket.hpp
        ${LIB_HEADER_DIR}/asio/basic_seq_packet_socket.hpp
        ${LIB_HEADER_DIR}/asio/basic_serial_port.hpp
        ${LIB_HEADER_DIR}/asio/basic_signal_set.hpp
        ${LIB_HEADER_DIR}/asio/basic_socket.hpp
        ${LIB_HEADER_DIR}/asio/basic_socket_acceptor.hpp
        ${LIB_HEADER_DIR}/asio/basic_socket_iostream.hpp
        ${LIB_HEADER_DIR}/asio/basic_socket_streambuf.hpp
        ${LIB_HEADER_DIR}/asio/basic_streambuf.hpp
        ${LIB_HEADER_DIR}/asio/basic_streambuf_fwd.hpp
        ${LIB_HEADER_DIR}/asio/basic_stream_socket.hpp
        ${LIB_HEADER_DIR}/asio/basic_waitable_timer.hpp
        ${LIB_HEADER_DIR}/asio/bind_executor.hpp
        ${LIB_HEADER_DIR}/asio/buffer.hpp
        ${LIB_HEADER_DIR}/asio/buffered_read_stream.hpp
        ${LIB_HEADER_DIR}/asio/buffered_read_stream_fwd.hpp
        ${LIB_HEADER_DIR}/asio/buffered_stream.hpp
        ${LIB_HEADER_DIR}/asio/buffered_stream_fwd.hpp
        ${LIB_HEADER_DIR}/asio/buffered_write_stream.hpp
        ${LIB_HEADER_DIR}/asio/buffered_write_stream_fwd.hpp
        ${LIB_HEADER_DIR}/asio/buffers_iterator.hpp
        ${LIB_HEADER_DIR}/asio/completion_condition.hpp
        ${LIB_HEADER_DIR}/asio/compose.hpp
        ${LIB_HEADER_DIR}/asio/connect.hpp
        ${LIB_HEADER_DIR}/asio/coroutine.hpp
        ${LIB_HEADER_DIR}/asio/co_spawn.hpp
        ${LIB_HEADER_DIR}/asio/deadline_timer.hpp
        ${LIB_HEADER_DIR}/asio/defer.hpp
        ${LIB_HEADER_DIR}/asio/detached.hpp
        ${LIB_HEADER_DIR}/asio/dispatch.hpp
        ${LIB_HEADER_DIR}/asio/error.hpp
        ${LIB_HEADER_DIR}/asio/error_code.hpp
        ${LIB_HEADER_DIR}/asio/execution.hpp
        ${LIB_HEADER_DIR}/asio/execution_context.hpp
        ${LIB_HEADER_DIR}/asio/executor.hpp
        ${LIB_HEADER_DIR}/asio/executor_work_guard.hpp
        ${LIB_HEADER_DIR}/asio/handler_alloc_hook.hpp
        ${LIB_HEADER_DIR}/asio/handler_continuation_hook.hpp
        ${LIB_HEADER_DIR}/asio/handler_invoke_hook.hpp
        ${LIB_HEADER_DIR}/asio/high_resolution_timer.hpp
        ${LIB_HEADER_DIR}/asio/io_context.hpp
        ${LIB_HEADER_DIR}/asio/io_context_strand.hpp
        ${LIB_HEADER_DIR}/asio/io_service.hpp
        ${LIB_HEADER_DIR}/asio/io_service_strand.hpp
        ${LIB_HEADER_DIR}/asio/is_applicable_property.hpp
        ${LIB_HEADER_DIR}/asio/is_executor.hpp
        ${LIB_HEADER_DIR}/asio/is_read_buffered.hpp
        ${LIB_HEADER_DIR}/asio/is_write_buffered.hpp
        ${LIB_HEADER_DIR}/asio/multiple_exceptions.hpp
        ${LIB_HEADER_DIR}/asio/packaged_task.hpp
        ${LIB_HEADER_DIR}/asio/placeholders.hpp
        ${LIB_HEADER_DIR}/asio/post.hpp
        ${LIB_HEADER_DIR}/asio/prefer.hpp
        ${LIB_HEADER_DIR}/asio/query.hpp
        ${LIB_HEADER_DIR}/asio/read.hpp
        ${LIB_HEADER_DIR}/asio/read_at.hpp
        ${LIB_HEADER_DIR}/asio/read_until.hpp
        ${LIB_HEADER_DIR}/asio/redirect_error.hpp
        ${LIB_HEADER_DIR}/asio/require.hpp
        ${LIB_HEADER_DIR}/asio/require_concept.hpp
        ${LIB_HEADER_DIR}/asio/serial_port.hpp
        ${LIB_HEADER_DIR}/asio/serial_port_base.hpp
        ${LIB_HEADER_DIR}/asio/signal_set.hpp
        ${LIB_HEADER_DIR}/asio/socket_base.hpp
        ${LIB_HEADER_DIR}/asio/spawn.hpp
        ${LIB_HEADER_DIR}/asio/ssl.hpp
        ${LIB_HEADER_DIR}/asio/static_thread_pool.hpp
        ${LIB_HEADER_DIR}/asio/steady_timer.hpp
        ${LIB_HEADER_DIR}/asio/strand.hpp
        ${LIB_HEADER_DIR}/asio/streambuf.hpp
        ${LIB_HEADER_DIR}/asio/system_context.hpp
        ${LIB_HEADER_DIR}/asio/system_error.hpp
        ${LIB_HEADER_DIR}/asio/system_executor.hpp
        ${LIB_HEADER_DIR}/asio/system_timer.hpp
        ${LIB_HEADER_DIR}/asio/this_coro.hpp
        ${LIB_HEADER_DIR}/asio/thread.hpp
        ${LIB_HEADER_DIR}/asio/thread_pool.hpp
        ${LIB_HEADER_DIR}/asio/time_traits.hpp
        ${LIB_HEADER_DIR}/asio/unyield.hpp
        ${LIB_HEADER_DIR}/asio/uses_executor.hpp
        ${LIB_HEADER_DIR}/asio/use_awaitable.hpp
        ${LIB_HEADER_DIR}/asio/use_future.hpp
        ${LIB_HEADER_DIR}/asio/version.hpp
        ${LIB_HEADER_DIR}/asio/wait_traits.hpp
        ${LIB_HEADER_DIR}/asio/write.hpp
        ${LIB_HEADER_DIR}/asio/write_at.hpp
        ${LIB_HEADER_DIR}/asio/yield.hpp
        # asio/detail/
        ${LIB_HEADER_DIR}/asio/detail/array.hpp
        ${LIB_HEADER_DIR}/asio/detail/array_fwd.hpp
        ${LIB_HEADER_DIR}/asio/detail/assert.hpp
        ${LIB_HEADER_DIR}/asio/detail/atomic_count.hpp
        ${LIB_HEADER_DIR}/asio/detail/base_from_completion_cond.hpp
        ${LIB_HEADER_DIR}/asio/detail/bind_handler.hpp
        ${LIB_HEADER_DIR}/asio/detail/blocking_executor_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/buffered_stream_storage.hpp
        ${LIB_HEADER_DIR}/asio/detail/buffer_resize_guard.hpp
        ${LIB_HEADER_DIR}/asio/detail/buffer_sequence_adapter.hpp
        ${LIB_HEADER_DIR}/asio/detail/bulk_executor_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/call_stack.hpp
        ${LIB_HEADER_DIR}/asio/detail/chrono.hpp
        ${LIB_HEADER_DIR}/asio/detail/chrono_time_traits.hpp
        ${LIB_HEADER_DIR}/asio/detail/completion_handler.hpp
        ${LIB_HEADER_DIR}/asio/detail/concurrency_hint.hpp
        ${LIB_HEADER_DIR}/asio/detail/conditionally_enabled_event.hpp
        ${LIB_HEADER_DIR}/asio/detail/conditionally_enabled_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/config.hpp
        ${LIB_HEADER_DIR}/asio/detail/consuming_buffers.hpp
        ${LIB_HEADER_DIR}/asio/detail/cstddef.hpp
        ${LIB_HEADER_DIR}/asio/detail/cstdint.hpp
        ${LIB_HEADER_DIR}/asio/detail/date_time_fwd.hpp
        ${LIB_HEADER_DIR}/asio/detail/deadline_timer_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/dependent_type.hpp
        ${LIB_HEADER_DIR}/asio/detail/descriptor_ops.hpp
        ${LIB_HEADER_DIR}/asio/detail/descriptor_read_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/descriptor_write_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/dev_poll_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/epoll_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/event.hpp
        ${LIB_HEADER_DIR}/asio/detail/eventfd_select_interrupter.hpp
        ${LIB_HEADER_DIR}/asio/detail/executor_function.hpp
        ${LIB_HEADER_DIR}/asio/detail/executor_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/fd_set_adapter.hpp
        ${LIB_HEADER_DIR}/asio/detail/fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/functional.hpp
        ${LIB_HEADER_DIR}/asio/detail/future.hpp
        ${LIB_HEADER_DIR}/asio/detail/gcc_arm_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/gcc_hppa_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/gcc_sync_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/gcc_x86_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/global.hpp
        ${LIB_HEADER_DIR}/asio/detail/handler_alloc_helpers.hpp
        ${LIB_HEADER_DIR}/asio/detail/handler_cont_helpers.hpp
        ${LIB_HEADER_DIR}/asio/detail/handler_invoke_helpers.hpp
        ${LIB_HEADER_DIR}/asio/detail/handler_tracking.hpp
        ${LIB_HEADER_DIR}/asio/detail/handler_type_requirements.hpp
        ${LIB_HEADER_DIR}/asio/detail/handler_work.hpp
        ${LIB_HEADER_DIR}/asio/detail/hash_map.hpp
        ${LIB_HEADER_DIR}/asio/detail/io_control.hpp
        ${LIB_HEADER_DIR}/asio/detail/io_object_impl.hpp
        ${LIB_HEADER_DIR}/asio/detail/is_buffer_sequence.hpp
        ${LIB_HEADER_DIR}/asio/detail/is_executor.hpp
        ${LIB_HEADER_DIR}/asio/detail/keyword_tss_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/kqueue_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/limits.hpp
        ${LIB_HEADER_DIR}/asio/detail/local_free_on_block_exit.hpp
        ${LIB_HEADER_DIR}/asio/detail/macos_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/memory.hpp
        ${LIB_HEADER_DIR}/asio/detail/mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/noncopyable.hpp
        ${LIB_HEADER_DIR}/asio/detail/non_const_lvalue.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_event.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_global.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_signal_blocker.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_socket_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_static_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/null_tss_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/object_pool.hpp
        ${LIB_HEADER_DIR}/asio/detail/old_win_sdk_compat.hpp
        ${LIB_HEADER_DIR}/asio/detail/operation.hpp
        ${LIB_HEADER_DIR}/asio/detail/op_queue.hpp
        ${LIB_HEADER_DIR}/asio/detail/pipe_select_interrupter.hpp
        ${LIB_HEADER_DIR}/asio/detail/pop_options.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_event.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_fd_set_adapter.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_global.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_signal_blocker.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_static_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/posix_tss_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/push_options.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_descriptor_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_null_buffers_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_serial_port_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_accept_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_connect_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_recvfrom_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_recvmsg_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_recv_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_sendto_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_send_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_socket_service_base.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactive_wait_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactor_fwd.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactor_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/reactor_op_queue.hpp
        ${LIB_HEADER_DIR}/asio/detail/recycling_allocator.hpp
        ${LIB_HEADER_DIR}/asio/detail/regex_fwd.hpp
        ${LIB_HEADER_DIR}/asio/detail/resolver_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/resolver_service_base.hpp
        ${LIB_HEADER_DIR}/asio/detail/resolve_endpoint_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/resolve_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/resolve_query_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/scheduler.hpp
        ${LIB_HEADER_DIR}/asio/detail/scheduler_operation.hpp
        ${LIB_HEADER_DIR}/asio/detail/scheduler_thread_info.hpp
        ${LIB_HEADER_DIR}/asio/detail/scoped_lock.hpp
        ${LIB_HEADER_DIR}/asio/detail/scoped_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/select_interrupter.hpp
        ${LIB_HEADER_DIR}/asio/detail/select_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/service_registry.hpp
        ${LIB_HEADER_DIR}/asio/detail/signal_blocker.hpp
        ${LIB_HEADER_DIR}/asio/detail/signal_handler.hpp
        ${LIB_HEADER_DIR}/asio/detail/signal_init.hpp
        ${LIB_HEADER_DIR}/asio/detail/signal_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/signal_set_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/socket_holder.hpp
        ${LIB_HEADER_DIR}/asio/detail/socket_ops.hpp
        ${LIB_HEADER_DIR}/asio/detail/socket_option.hpp
        ${LIB_HEADER_DIR}/asio/detail/socket_select_interrupter.hpp
        ${LIB_HEADER_DIR}/asio/detail/socket_types.hpp
        ${LIB_HEADER_DIR}/asio/detail/solaris_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/source_location.hpp
        ${LIB_HEADER_DIR}/asio/detail/static_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/std_event.hpp
        ${LIB_HEADER_DIR}/asio/detail/std_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/std_global.hpp
        ${LIB_HEADER_DIR}/asio/detail/std_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/std_static_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/std_thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/strand_executor_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/strand_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/string_view.hpp
        ${LIB_HEADER_DIR}/asio/detail/thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/thread_context.hpp
        ${LIB_HEADER_DIR}/asio/detail/thread_group.hpp
        ${LIB_HEADER_DIR}/asio/detail/thread_info_base.hpp
        ${LIB_HEADER_DIR}/asio/detail/throw_error.hpp
        ${LIB_HEADER_DIR}/asio/detail/throw_exception.hpp
        ${LIB_HEADER_DIR}/asio/detail/timer_queue.hpp
        ${LIB_HEADER_DIR}/asio/detail/timer_queue_base.hpp
        ${LIB_HEADER_DIR}/asio/detail/timer_queue_ptime.hpp
        ${LIB_HEADER_DIR}/asio/detail/timer_queue_set.hpp
        ${LIB_HEADER_DIR}/asio/detail/timer_scheduler.hpp
        ${LIB_HEADER_DIR}/asio/detail/timer_scheduler_fwd.hpp
        ${LIB_HEADER_DIR}/asio/detail/tss_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/type_traits.hpp
        ${LIB_HEADER_DIR}/asio/detail/variadic_templates.hpp
        ${LIB_HEADER_DIR}/asio/detail/wait_handler.hpp
        ${LIB_HEADER_DIR}/asio/detail/wait_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/winapp_thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/wince_thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_async_manager.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_async_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_resolver_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_resolve_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_socket_connect_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_socket_recv_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_socket_send_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_ssocket_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_ssocket_service_base.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_timer_scheduler.hpp
        ${LIB_HEADER_DIR}/asio/detail/winrt_utils.hpp
        ${LIB_HEADER_DIR}/asio/detail/winsock_init.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_event.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_fd_set_adapter.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_fenced_block.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_global.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_handle_read_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_handle_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_handle_write_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_io_context.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_null_buffers_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_operation.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_overlapped_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_overlapped_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_serial_port_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_accept_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_connect_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_recvfrom_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_recvmsg_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_recv_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_send_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_socket_service_base.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_thread_info.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_iocp_wait_op.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_object_handle_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_static_mutex.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_thread.hpp
        ${LIB_HEADER_DIR}/asio/detail/win_tss_ptr.hpp
        ${LIB_HEADER_DIR}/asio/detail/work_dispatcher.hpp
        ${LIB_HEADER_DIR}/asio/detail/wrapped_handler.hpp
        # asio/detail/impl/
        ${LIB_HEADER_DIR}/asio/detail/impl/buffer_sequence_adapter.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/descriptor_ops.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/dev_poll_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/dev_poll_reactor.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/epoll_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/epoll_reactor.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/eventfd_select_interrupter.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/handler_tracking.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/kqueue_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/kqueue_reactor.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/null_event.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/pipe_select_interrupter.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/posix_event.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/posix_mutex.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/posix_thread.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/posix_tss_ptr.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/reactive_descriptor_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/reactive_serial_port_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/reactive_socket_service_base.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/resolver_service_base.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/scheduler.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/select_reactor.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/select_reactor.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/service_registry.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/service_registry.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/signal_set_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/socket_ops.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/socket_select_interrupter.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/strand_executor_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/strand_executor_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/strand_service.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/strand_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/throw_error.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/timer_queue_ptime.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/timer_queue_set.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/winrt_ssocket_service_base.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/winrt_timer_scheduler.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/winrt_timer_scheduler.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/winsock_init.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_event.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_iocp_handle_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_iocp_io_context.hpp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_iocp_io_context.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_iocp_serial_port_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_iocp_socket_service_base.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_mutex.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_object_handle_service.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_static_mutex.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_thread.ipp
        ${LIB_HEADER_DIR}/asio/detail/impl/win_tss_ptr.ipp
        # asio/execution/
        ${LIB_HEADER_DIR}/asio/execution/allocator.hpp
        ${LIB_HEADER_DIR}/asio/execution/any_executor.hpp
        ${LIB_HEADER_DIR}/asio/execution/bad_executor.hpp
        ${LIB_HEADER_DIR}/asio/execution/blocking.hpp
        ${LIB_HEADER_DIR}/asio/execution/blocking_adaptation.hpp
        ${LIB_HEADER_DIR}/asio/execution/bulk_execute.hpp
        ${LIB_HEADER_DIR}/asio/execution/bulk_guarantee.hpp
        ${LIB_HEADER_DIR}/asio/execution/connect.hpp
        ${LIB_HEADER_DIR}/asio/execution/context.hpp
        ${LIB_HEADER_DIR}/asio/execution/context_as.hpp
        ${LIB_HEADER_DIR}/asio/execution/execute.hpp
        ${LIB_HEADER_DIR}/asio/execution/executor.hpp
        ${LIB_HEADER_DIR}/asio/execution/invocable_archetype.hpp
        ${LIB_HEADER_DIR}/asio/execution/mapping.hpp
        ${LIB_HEADER_DIR}/asio/execution/occupancy.hpp
        ${LIB_HEADER_DIR}/asio/execution/operation_state.hpp
        ${LIB_HEADER_DIR}/asio/execution/outstanding_work.hpp
        ${LIB_HEADER_DIR}/asio/execution/prefer_only.hpp
        ${LIB_HEADER_DIR}/asio/execution/receiver.hpp
        ${LIB_HEADER_DIR}/asio/execution/receiver_invocation_error.hpp
        ${LIB_HEADER_DIR}/asio/execution/relationship.hpp
        ${LIB_HEADER_DIR}/asio/execution/schedule.hpp
        ${LIB_HEADER_DIR}/asio/execution/scheduler.hpp
        ${LIB_HEADER_DIR}/asio/execution/sender.hpp
        ${LIB_HEADER_DIR}/asio/execution/set_done.hpp
        ${LIB_HEADER_DIR}/asio/execution/set_error.hpp
        ${LIB_HEADER_DIR}/asio/execution/set_value.hpp
        ${LIB_HEADER_DIR}/asio/execution/start.hpp
        ${LIB_HEADER_DIR}/asio/execution/submit.hpp
        # asio/execution/detail/
        ${LIB_HEADER_DIR}/asio/execution/detail/as_invocable.hpp
        ${LIB_HEADER_DIR}/asio/execution/detail/as_operation.hpp
        ${LIB_HEADER_DIR}/asio/execution/detail/as_receiver.hpp
        ${LIB_HEADER_DIR}/asio/execution/detail/bulk_sender.hpp
        ${LIB_HEADER_DIR}/asio/execution/detail/submit_receiver.hpp
        ${LIB_HEADER_DIR}/asio/execution/detail/void_receiver.hpp
        # asio/execution/impl/
        ${LIB_HEADER_DIR}/asio/execution/impl/bad_executor.ipp
        ${LIB_HEADER_DIR}/asio/execution/impl/receiver_invocation_error.ipp
        # asio/generic/
        ${LIB_HEADER_DIR}/asio/generic/basic_endpoint.hpp
        ${LIB_HEADER_DIR}/asio/generic/datagram_protocol.hpp
        ${LIB_HEADER_DIR}/asio/generic/raw_protocol.hpp
        ${LIB_HEADER_DIR}/asio/generic/seq_packet_protocol.hpp
        ${LIB_HEADER_DIR}/asio/generic/stream_protocol.hpp
        # asio/generic/detail/
        ${LIB_HEADER_DIR}/asio/generic/detail/endpoint.hpp
        ${LIB_HEADER_DIR}/asio/generic/detail/impl/endpoint.ipp
        # asio/impl/
        ${LIB_HEADER_DIR}/asio/impl/awaitable.hpp
        ${LIB_HEADER_DIR}/asio/impl/buffered_read_stream.hpp
        ${LIB_HEADER_DIR}/asio/impl/buffered_write_stream.hpp
        ${LIB_HEADER_DIR}/asio/impl/compose.hpp
        ${LIB_HEADER_DIR}/asio/impl/connect.hpp
        ${LIB_HEADER_DIR}/asio/impl/co_spawn.hpp
        ${LIB_HEADER_DIR}/asio/impl/defer.hpp
        ${LIB_HEADER_DIR}/asio/impl/detached.hpp
        ${LIB_HEADER_DIR}/asio/impl/dispatch.hpp
        ${LIB_HEADER_DIR}/asio/impl/error.ipp
        ${LIB_HEADER_DIR}/asio/impl/error_code.ipp
        ${LIB_HEADER_DIR}/asio/impl/execution_context.hpp
        ${LIB_HEADER_DIR}/asio/impl/execution_context.ipp
        ${LIB_HEADER_DIR}/asio/impl/executor.hpp
        ${LIB_HEADER_DIR}/asio/impl/executor.ipp
        ${LIB_HEADER_DIR}/asio/impl/handler_alloc_hook.ipp
        ${LIB_HEADER_DIR}/asio/impl/io_context.hpp
        ${LIB_HEADER_DIR}/asio/impl/io_context.ipp
        ${LIB_HEADER_DIR}/asio/impl/multiple_exceptions.ipp
        ${LIB_HEADER_DIR}/asio/impl/post.hpp
        ${LIB_HEADER_DIR}/asio/impl/read.hpp
        ${LIB_HEADER_DIR}/asio/impl/read_at.hpp
        ${LIB_HEADER_DIR}/asio/impl/read_until.hpp
        ${LIB_HEADER_DIR}/asio/impl/redirect_error.hpp
        ${LIB_HEADER_DIR}/asio/impl/serial_port_base.hpp
        ${LIB_HEADER_DIR}/asio/impl/serial_port_base.ipp
        ${LIB_HEADER_DIR}/asio/impl/spawn.hpp
        ${LIB_HEADER_DIR}/asio/impl/src.cpp
        ${LIB_HEADER_DIR}/asio/impl/src.hpp
        ${LIB_HEADER_DIR}/asio/impl/system_context.hpp
        ${LIB_HEADER_DIR}/asio/impl/system_context.ipp
        ${LIB_HEADER_DIR}/asio/impl/system_executor.hpp
        ${LIB_HEADER_DIR}/asio/impl/thread_pool.hpp
        ${LIB_HEADER_DIR}/asio/impl/thread_pool.ipp
        ${LIB_HEADER_DIR}/asio/impl/use_awaitable.hpp
        ${LIB_HEADER_DIR}/asio/impl/use_future.hpp
        ${LIB_HEADER_DIR}/asio/impl/write.hpp
        ${LIB_HEADER_DIR}/asio/impl/write_at.hpp
        # asio/ip/
        ${LIB_HEADER_DIR}/asio/ip/address.hpp
        ${LIB_HEADER_DIR}/asio/ip/address_v4.hpp
        ${LIB_HEADER_DIR}/asio/ip/address_v4_iterator.hpp
        ${LIB_HEADER_DIR}/asio/ip/address_v4_range.hpp
        ${LIB_HEADER_DIR}/asio/ip/address_v6.hpp
        ${LIB_HEADER_DIR}/asio/ip/address_v6_iterator.hpp
        ${LIB_HEADER_DIR}/asio/ip/address_v6_range.hpp
        ${LIB_HEADER_DIR}/asio/ip/bad_address_cast.hpp
        ${LIB_HEADER_DIR}/asio/ip/basic_endpoint.hpp
        ${LIB_HEADER_DIR}/asio/ip/basic_resolver.hpp
        ${LIB_HEADER_DIR}/asio/ip/basic_resolver_entry.hpp
        ${LIB_HEADER_DIR}/asio/ip/basic_resolver_iterator.hpp
        ${LIB_HEADER_DIR}/asio/ip/basic_resolver_query.hpp
        ${LIB_HEADER_DIR}/asio/ip/basic_resolver_results.hpp
        ${LIB_HEADER_DIR}/asio/ip/host_name.hpp
        ${LIB_HEADER_DIR}/asio/ip/icmp.hpp
        ${LIB_HEADER_DIR}/asio/ip/multicast.hpp
        ${LIB_HEADER_DIR}/asio/ip/network_v4.hpp
        ${LIB_HEADER_DIR}/asio/ip/network_v6.hpp
        ${LIB_HEADER_DIR}/asio/ip/resolver_base.hpp
        ${LIB_HEADER_DIR}/asio/ip/resolver_query_base.hpp
        ${LIB_HEADER_DIR}/asio/ip/tcp.hpp
        ${LIB_HEADER_DIR}/asio/ip/udp.hpp
        ${LIB_HEADER_DIR}/asio/ip/unicast.hpp
        ${LIB_HEADER_DIR}/asio/ip/v6_only.hpp
        # asio/ip/detail/
        ${LIB_HEADER_DIR}/asio/ip/detail/endpoint.hpp
        ${LIB_HEADER_DIR}/asio/ip/detail/impl/endpoint.ipp
        ${LIB_HEADER_DIR}/asio/ip/detail/socket_option.hpp
        # asio/ip/impl/
        ${LIB_HEADER_DIR}/asio/ip/impl/address.hpp
        ${LIB_HEADER_DIR}/asio/ip/impl/address.ipp
        ${LIB_HEADER_DIR}/asio/ip/impl/address_v4.hpp
        ${LIB_HEADER_DIR}/asio/ip/impl/address_v4.ipp
        ${LIB_HEADER_DIR}/asio/ip/impl/address_v6.hpp
        ${LIB_HEADER_DIR}/asio/ip/impl/address_v6.ipp
        ${LIB_HEADER_DIR}/asio/ip/impl/basic_endpoint.hpp
        ${LIB_HEADER_DIR}/asio/ip/impl/host_name.ipp
        ${LIB_HEADER_DIR}/asio/ip/impl/network_v4.hpp
        ${LIB_HEADER_DIR}/asio/ip/impl/network_v4.ipp
        ${LIB_HEADER_DIR}/asio/ip/impl/network_v6.hpp
        ${LIB_HEADER_DIR}/asio/ip/impl/network_v6.ipp
        # asio/local/
        ${LIB_HEADER_DIR}/asio/local/basic_endpoint.hpp
        ${LIB_HEADER_DIR}/asio/local/connect_pair.hpp
        ${LIB_HEADER_DIR}/asio/local/datagram_protocol.hpp
        ${LIB_HEADER_DIR}/asio/local/stream_protocol.hpp
        # asio/local/detail/
        ${LIB_HEADER_DIR}/asio/local/detail/endpoint.hpp
        ${LIB_HEADER_DIR}/asio/local/detail/impl
        ${LIB_HEADER_DIR}/asio/local/detail/impl/endpoint.ipp
        # asio/posix/
        ${LIB_HEADER_DIR}/asio/posix/basic_descriptor.hpp
        ${LIB_HEADER_DIR}/asio/posix/basic_stream_descriptor.hpp
        ${LIB_HEADER_DIR}/asio/posix/descriptor.hpp
        ${LIB_HEADER_DIR}/asio/posix/descriptor_base.hpp
        ${LIB_HEADER_DIR}/asio/posix/stream_descriptor.hpp
        # asio/ssl/
        ${LIB_HEADER_DIR}/asio/ssl/context.hpp
        ${LIB_HEADER_DIR}/asio/ssl/context_base.hpp
        ${LIB_HEADER_DIR}/asio/ssl/error.hpp
        ${LIB_HEADER_DIR}/asio/ssl/host_name_verification.hpp
        ${LIB_HEADER_DIR}/asio/ssl/rfc2818_verification.hpp
        ${LIB_HEADER_DIR}/asio/ssl/stream.hpp
        ${LIB_HEADER_DIR}/asio/ssl/stream_base.hpp
        ${LIB_HEADER_DIR}/asio/ssl/verify_context.hpp
        ${LIB_HEADER_DIR}/asio/ssl/verify_mode.hpp
        # asio/ssl/detail/
        ${LIB_HEADER_DIR}/asio/ssl/detail/buffered_handshake_op.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/engine.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/handshake_op.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/io.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/openssl_init.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/openssl_types.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/password_callback.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/read_op.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/shutdown_op.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/stream_core.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/verify_callback.hpp
        ${LIB_HEADER_DIR}/asio/ssl/detail/write_op.hpp
        # asio/ssl/detail/impl/
        ${LIB_HEADER_DIR}/asio/ssl/detail/impl/engine.ipp
        ${LIB_HEADER_DIR}/asio/ssl/detail/impl/openssl_init.ipp
        # asio/ssl/impl/
        ${LIB_HEADER_DIR}/asio/ssl/impl/context.hpp
        ${LIB_HEADER_DIR}/asio/ssl/impl/context.ipp
        ${LIB_HEADER_DIR}/asio/ssl/impl/error.ipp
        ${LIB_HEADER_DIR}/asio/ssl/impl/host_name_verification.ipp
        ${LIB_HEADER_DIR}/asio/ssl/impl/rfc2818_verification.ipp
        ${LIB_HEADER_DIR}/asio/ssl/impl/src.hpp
        # asio/traits/
        ${LIB_HEADER_DIR}/asio/traits/bulk_execute_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/bulk_execute_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/connect_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/connect_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/equality_comparable.hpp
        ${LIB_HEADER_DIR}/asio/traits/execute_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/execute_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/prefer_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/prefer_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/query_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/query_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/query_static_constexpr_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/require_concept_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/require_concept_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/require_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/require_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/schedule_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/schedule_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/set_done_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/set_done_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/set_error_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/set_error_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/set_value_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/set_value_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/start_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/start_member.hpp
        ${LIB_HEADER_DIR}/asio/traits/static_query.hpp
        ${LIB_HEADER_DIR}/asio/traits/static_require.hpp
        ${LIB_HEADER_DIR}/asio/traits/static_require_concept.hpp
        ${LIB_HEADER_DIR}/asio/traits/submit_free.hpp
        ${LIB_HEADER_DIR}/asio/traits/submit_member.hpp
        # asio/ts/
        ${LIB_HEADER_DIR}/asio/ts/buffer.hpp
        ${LIB_HEADER_DIR}/asio/ts/executor.hpp
        ${LIB_HEADER_DIR}/asio/ts/internet.hpp
        ${LIB_HEADER_DIR}/asio/ts/io_context.hpp
        ${LIB_HEADER_DIR}/asio/ts/net.hpp
        ${LIB_HEADER_DIR}/asio/ts/netfwd.hpp
        ${LIB_HEADER_DIR}/asio/ts/socket.hpp
        ${LIB_HEADER_DIR}/asio/ts/timer.hpp
        # asio/windows/
        ${LIB_HEADER_DIR}/asio/windows/basic_object_handle.hpp
        ${LIB_HEADER_DIR}/asio/windows/basic_overlapped_handle.hpp
        ${LIB_HEADER_DIR}/asio/windows/basic_random_access_handle.hpp
        ${LIB_HEADER_DIR}/asio/windows/basic_stream_handle.hpp
        ${LIB_HEADER_DIR}/asio/windows/object_handle.hpp

        ${LIB_HEADER_DIR}/asio/windows/overlapped_handle.hpp
        ${LIB_HEADER_DIR}/asio/windows/overlapped_ptr.hpp
        ${LIB_HEADER_DIR}/asio/windows/random_access_handle.hpp
        ${LIB_HEADER_DIR}/asio/windows/stream_handle.hpp
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/asio.cpp
        # ${LIB_SOURCE_DIR}/asio_ssl.cpp
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_definitions(${LIB_NAME} PRIVATE _WIN32_WINNT=0x0601) # Windows 7
    # target_compile_definitions(${LIB_NAME} PUBLIC ASIO_SEPARATE_COMPILATION=1)
    target_compile_definitions(${LIB_NAME} PUBLIC ASIO_DYN_LINK=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _bespoke)
    else()
        if (WIN32)
            target_link_libraries(${LIB_NAME} PRIVATE ws2_32 mswsock)
        endif()

        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()
        
    target_link_libraries(${LIB_NAME} PUBLIC de_openssl_static)

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)

]]
