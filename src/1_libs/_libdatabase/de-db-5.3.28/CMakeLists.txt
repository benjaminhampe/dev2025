set(LIB_NAME de_db)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS 
		# ${LIB_HEADER_DIR}/.IGNORE_ME
		${LIB_HEADER_DIR}/clib_port.h
		# ${LIB_HEADER_DIR}/config.log
		# ${LIB_HEADER_DIR}/config.status
		${LIB_HEADER_DIR}/db.h
		${LIB_HEADER_DIR}/dbstl_common.h
		${LIB_HEADER_DIR}/db_185.h
		${LIB_HEADER_DIR}/db_config.h
		${LIB_HEADER_DIR}/db_cxx.h
		${LIB_HEADER_DIR}/db_int.h
		# ${LIB_HEADER_DIR}/include.tcl
		# ${LIB_HEADER_DIR}/libtool
		# ${LIB_HEADER_DIR}/Makefile
		# ${LIB_HEADER_DIR}/Test.class
		# ${LIB_HEADER_DIR}/Test.java
	)
    set(LIB_SOURCES 
                # ${LIB_SOURCE_DIR}/dist/api_flags.c
		${LIB_SOURCE_DIR}/dist/errno.h

		${LIB_SOURCE_DIR}/lang/cxx/cxx_channel.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_db.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_dbc.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_dbt.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_env.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_except.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_lock.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_logc.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_mpool.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_multi.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_rid.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_seq.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_site.cpp
		${LIB_SOURCE_DIR}/lang/cxx/cxx_txn.cpp

		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_base_iterator.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_common.in
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_container.cpp
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_container.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_dbc.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_dbt.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_element_ref.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_exception.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_inner_utility.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_map.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_resource_manager.cpp
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_resource_manager.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_set.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_utility.h
		${LIB_SOURCE_DIR}/lang/cxx/stl/dbstl_vector.h
		
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/db.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/env.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/historic.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/index.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/library.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/lock.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/log.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/mpool.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/rep.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/sequence.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/test.html
		# ${LIB_SOURCE_DIR}/lang/tcl/docs/txn.html
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_compat.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_db.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_dbcursor.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_db_pkg.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_env.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_internal.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_lock.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_log.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_mp.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_mutex.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_rep.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_seq.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_txn.c
		# ${LIB_SOURCE_DIR}/lang/tcl/tcl_util.c

		${LIB_SOURCE_DIR}/src/btree/btree.src
		${LIB_SOURCE_DIR}/src/btree/btree_auto.c
		${LIB_SOURCE_DIR}/src/btree/btree_autop.c
		${LIB_SOURCE_DIR}/src/btree/bt_compact.c
		${LIB_SOURCE_DIR}/src/btree/bt_compare.c
		${LIB_SOURCE_DIR}/src/btree/bt_compress.c
		${LIB_SOURCE_DIR}/src/btree/bt_conv.c
		${LIB_SOURCE_DIR}/src/btree/bt_curadj.c
		${LIB_SOURCE_DIR}/src/btree/bt_cursor.c
		${LIB_SOURCE_DIR}/src/btree/bt_delete.c
		${LIB_SOURCE_DIR}/src/btree/bt_method.c
		${LIB_SOURCE_DIR}/src/btree/bt_open.c
		${LIB_SOURCE_DIR}/src/btree/bt_put.c
		${LIB_SOURCE_DIR}/src/btree/bt_rec.c
		${LIB_SOURCE_DIR}/src/btree/bt_reclaim.c
		${LIB_SOURCE_DIR}/src/btree/bt_recno.c
		${LIB_SOURCE_DIR}/src/btree/bt_rsearch.c
		${LIB_SOURCE_DIR}/src/btree/bt_search.c
		${LIB_SOURCE_DIR}/src/btree/bt_split.c
		${LIB_SOURCE_DIR}/src/btree/bt_stat.c
		${LIB_SOURCE_DIR}/src/btree/bt_upgrade.c
		${LIB_SOURCE_DIR}/src/btree/bt_verify.c

                # ${LIB_SOURCE_DIR}/src/clib/atoi.c
                # ${LIB_SOURCE_DIR}/src/clib/atol.c
                # ${LIB_SOURCE_DIR}/src/clib/bsearch.c
                # ${LIB_SOURCE_DIR}/src/clib/getcwd.c
                # ${LIB_SOURCE_DIR}/src/clib/getopt.c
                # ${LIB_SOURCE_DIR}/src/clib/isalpha.c
                # ${LIB_SOURCE_DIR}/src/clib/isdigit.c
                # ${LIB_SOURCE_DIR}/src/clib/isprint.c
                # ${LIB_SOURCE_DIR}/src/clib/isspace.c
                # ${LIB_SOURCE_DIR}/src/clib/memcmp.c
                # ${LIB_SOURCE_DIR}/src/clib/memmove.c
                # ${LIB_SOURCE_DIR}/src/clib/printf.c
                # ${LIB_SOURCE_DIR}/src/clib/qsort.c
                # ${LIB_SOURCE_DIR}/src/clib/raise.c
                # ${LIB_SOURCE_DIR}/src/clib/rand.c
                # ${LIB_SOURCE_DIR}/src/clib/snprintf.c
                # ${LIB_SOURCE_DIR}/src/clib/strcasecmp.c
                # ${LIB_SOURCE_DIR}/src/clib/strcat.c
                # ${LIB_SOURCE_DIR}/src/clib/strchr.c
                # ${LIB_SOURCE_DIR}/src/clib/strdup.c
                # ${LIB_SOURCE_DIR}/src/clib/strerror.c
                # ${LIB_SOURCE_DIR}/src/clib/strncat.c
                # ${LIB_SOURCE_DIR}/src/clib/strncmp.c
                # ${LIB_SOURCE_DIR}/src/clib/strrchr.c
                ${LIB_SOURCE_DIR}/src/clib/strsep.c
                # ${LIB_SOURCE_DIR}/src/clib/strtol.c
                # ${LIB_SOURCE_DIR}/src/clib/strtoul.c
                # ${LIB_SOURCE_DIR}/src/clib/time.c

		${LIB_SOURCE_DIR}/src/common/clock.c
                # ${LIB_SOURCE_DIR}/src/common/crypto_stub.c
		${LIB_SOURCE_DIR}/src/common/dbt.c
		${LIB_SOURCE_DIR}/src/common/db_byteorder.c
		${LIB_SOURCE_DIR}/src/common/db_compint.c
		${LIB_SOURCE_DIR}/src/common/db_err.c
		${LIB_SOURCE_DIR}/src/common/db_getlong.c
		${LIB_SOURCE_DIR}/src/common/db_idspace.c
		${LIB_SOURCE_DIR}/src/common/db_log2.c
		${LIB_SOURCE_DIR}/src/common/db_shash.c
		${LIB_SOURCE_DIR}/src/common/mkpath.c
		${LIB_SOURCE_DIR}/src/common/openflags.c
		${LIB_SOURCE_DIR}/src/common/os_method.c
		${LIB_SOURCE_DIR}/src/common/util_arg.c
		${LIB_SOURCE_DIR}/src/common/util_cache.c
		${LIB_SOURCE_DIR}/src/common/util_log.c
		${LIB_SOURCE_DIR}/src/common/util_sig.c
		${LIB_SOURCE_DIR}/src/common/zerofill.c

		${LIB_SOURCE_DIR}/src/crypto/aes_method.c
		${LIB_SOURCE_DIR}/src/crypto/crypto.c
		# ${LIB_SOURCE_DIR}/src/crypto/crypto.html

		${LIB_SOURCE_DIR}/src/crypto/mersenne/mt19937db.c

		${LIB_SOURCE_DIR}/src/crypto/rijndael/rijndael-alg-fst.c
		${LIB_SOURCE_DIR}/src/crypto/rijndael/rijndael-alg-fst.h
		${LIB_SOURCE_DIR}/src/crypto/rijndael/rijndael-api-fst.c
		${LIB_SOURCE_DIR}/src/crypto/rijndael/rijndael-api-fst.h

		${LIB_SOURCE_DIR}/src/db/crdel.src
		${LIB_SOURCE_DIR}/src/db/crdel_auto.c
		${LIB_SOURCE_DIR}/src/db/crdel_autop.c
		${LIB_SOURCE_DIR}/src/db/crdel_rec.c
		${LIB_SOURCE_DIR}/src/db/db.c
		${LIB_SOURCE_DIR}/src/db/db.src
		${LIB_SOURCE_DIR}/src/db/db_am.c
		${LIB_SOURCE_DIR}/src/db/db_auto.c
		${LIB_SOURCE_DIR}/src/db/db_autop.c
		${LIB_SOURCE_DIR}/src/db/db_backup.c
		${LIB_SOURCE_DIR}/src/db/db_cam.c
		${LIB_SOURCE_DIR}/src/db/db_cds.c
		${LIB_SOURCE_DIR}/src/db/db_compact.c
		${LIB_SOURCE_DIR}/src/db/db_conv.c
		${LIB_SOURCE_DIR}/src/db/db_copy.c
		${LIB_SOURCE_DIR}/src/db/db_dispatch.c
		${LIB_SOURCE_DIR}/src/db/db_dup.c
		${LIB_SOURCE_DIR}/src/db/db_iface.c
		${LIB_SOURCE_DIR}/src/db/db_join.c
		${LIB_SOURCE_DIR}/src/db/db_meta.c
		${LIB_SOURCE_DIR}/src/db/db_method.c
		${LIB_SOURCE_DIR}/src/db/db_open.c
		${LIB_SOURCE_DIR}/src/db/db_overflow.c
		${LIB_SOURCE_DIR}/src/db/db_ovfl_vrfy.c
		${LIB_SOURCE_DIR}/src/db/db_pr.c
		${LIB_SOURCE_DIR}/src/db/db_rec.c
		${LIB_SOURCE_DIR}/src/db/db_reclaim.c
		${LIB_SOURCE_DIR}/src/db/db_remove.c
		${LIB_SOURCE_DIR}/src/db/db_rename.c
		${LIB_SOURCE_DIR}/src/db/db_ret.c
		${LIB_SOURCE_DIR}/src/db/db_setid.c
		${LIB_SOURCE_DIR}/src/db/db_setlsn.c
		${LIB_SOURCE_DIR}/src/db/db_sort_multiple.c
		${LIB_SOURCE_DIR}/src/db/db_stati.c
		${LIB_SOURCE_DIR}/src/db/db_truncate.c
		${LIB_SOURCE_DIR}/src/db/db_upg.c
		${LIB_SOURCE_DIR}/src/db/db_upg_opd.c
		${LIB_SOURCE_DIR}/src/db/db_vrfy.c
		${LIB_SOURCE_DIR}/src/db/db_vrfyutil.c
		${LIB_SOURCE_DIR}/src/db/db_vrfy_stub.c
		${LIB_SOURCE_DIR}/src/db/partition.c

		${LIB_SOURCE_DIR}/src/dbinc/atomic.h
		${LIB_SOURCE_DIR}/src/dbinc/btree.h
		${LIB_SOURCE_DIR}/src/dbinc/clock.h
		${LIB_SOURCE_DIR}/src/dbinc/crypto.h
		${LIB_SOURCE_DIR}/src/dbinc/cxx_int.h
		${LIB_SOURCE_DIR}/src/dbinc/db.in
		${LIB_SOURCE_DIR}/src/dbinc/db_185.in
		${LIB_SOURCE_DIR}/src/dbinc/db_am.h
		${LIB_SOURCE_DIR}/src/dbinc/db_cxx.in
		${LIB_SOURCE_DIR}/src/dbinc/db_dispatch.h
		${LIB_SOURCE_DIR}/src/dbinc/db_int.in
		${LIB_SOURCE_DIR}/src/dbinc/db_join.h
		${LIB_SOURCE_DIR}/src/dbinc/db_page.h
		${LIB_SOURCE_DIR}/src/dbinc/db_swap.h
		${LIB_SOURCE_DIR}/src/dbinc/db_upgrade.h
		${LIB_SOURCE_DIR}/src/dbinc/db_verify.h
		${LIB_SOURCE_DIR}/src/dbinc/debug.h
		${LIB_SOURCE_DIR}/src/dbinc/fop.h
		${LIB_SOURCE_DIR}/src/dbinc/globals.h
		${LIB_SOURCE_DIR}/src/dbinc/hash.h
		${LIB_SOURCE_DIR}/src/dbinc/heap.h
		${LIB_SOURCE_DIR}/src/dbinc/hmac.h
		${LIB_SOURCE_DIR}/src/dbinc/lock.h
		${LIB_SOURCE_DIR}/src/dbinc/log.h
		${LIB_SOURCE_DIR}/src/dbinc/log_verify.h
		${LIB_SOURCE_DIR}/src/dbinc/mp.h
		${LIB_SOURCE_DIR}/src/dbinc/mutex.h
		${LIB_SOURCE_DIR}/src/dbinc/mutex_int.h
		${LIB_SOURCE_DIR}/src/dbinc/os.h
		${LIB_SOURCE_DIR}/src/dbinc/partition.h
		${LIB_SOURCE_DIR}/src/dbinc/perfmon.h
		${LIB_SOURCE_DIR}/src/dbinc/qam.h
		${LIB_SOURCE_DIR}/src/dbinc/queue.h
		${LIB_SOURCE_DIR}/src/dbinc/region.h
		${LIB_SOURCE_DIR}/src/dbinc/rep.h
		${LIB_SOURCE_DIR}/src/dbinc/repmgr.h
		${LIB_SOURCE_DIR}/src/dbinc/shqueue.h
		${LIB_SOURCE_DIR}/src/dbinc/tcl_db.h
		${LIB_SOURCE_DIR}/src/dbinc/txn.h
		${LIB_SOURCE_DIR}/src/dbinc/win_db.h
		${LIB_SOURCE_DIR}/src/dbinc/xa.h

		${LIB_SOURCE_DIR}/src/dbinc_auto/api_flags.in
		${LIB_SOURCE_DIR}/src/dbinc_auto/btree_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/btree_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/clib_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/common_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/crdel_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/crypto_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/dbreg_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/dbreg_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/db_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/db_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/env_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/ext_185_def.in
		${LIB_SOURCE_DIR}/src/dbinc_auto/ext_185_prot.in
		${LIB_SOURCE_DIR}/src/dbinc_auto/ext_def.in
		${LIB_SOURCE_DIR}/src/dbinc_auto/ext_prot.in
		${LIB_SOURCE_DIR}/src/dbinc_auto/fileops_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/fileops_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/hash_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/hash_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/heap_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/heap_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/hmac_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/int_def.in
		${LIB_SOURCE_DIR}/src/dbinc_auto/lock_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/log_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/mp_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/mutex_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/os_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/qam_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/qam_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/repmgr_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/repmgr_automsg.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/repmgr_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/rep_automsg.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/rep_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/sequence_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/tcl_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/txn_auto.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/txn_ext.h
		${LIB_SOURCE_DIR}/src/dbinc_auto/xa_ext.h

		${LIB_SOURCE_DIR}/src/dbreg/dbreg.c
		${LIB_SOURCE_DIR}/src/dbreg/dbreg.src
		${LIB_SOURCE_DIR}/src/dbreg/dbreg_auto.c
		${LIB_SOURCE_DIR}/src/dbreg/dbreg_autop.c
		${LIB_SOURCE_DIR}/src/dbreg/dbreg_rec.c
		${LIB_SOURCE_DIR}/src/dbreg/dbreg_stat.c
		${LIB_SOURCE_DIR}/src/dbreg/dbreg_util.c

		${LIB_SOURCE_DIR}/src/env/env_alloc.c
		${LIB_SOURCE_DIR}/src/env/env_backup.c
		${LIB_SOURCE_DIR}/src/env/env_config.c
		${LIB_SOURCE_DIR}/src/env/env_failchk.c
		${LIB_SOURCE_DIR}/src/env/env_file.c
		${LIB_SOURCE_DIR}/src/env/env_globals.c
		${LIB_SOURCE_DIR}/src/env/env_method.c
		${LIB_SOURCE_DIR}/src/env/env_name.c
		${LIB_SOURCE_DIR}/src/env/env_open.c
		${LIB_SOURCE_DIR}/src/env/env_recover.c
		${LIB_SOURCE_DIR}/src/env/env_region.c
		${LIB_SOURCE_DIR}/src/env/env_register.c
		${LIB_SOURCE_DIR}/src/env/env_sig.c
		${LIB_SOURCE_DIR}/src/env/env_stat.c

		${LIB_SOURCE_DIR}/src/fileops/fileops.src
		${LIB_SOURCE_DIR}/src/fileops/fileops_auto.c
		${LIB_SOURCE_DIR}/src/fileops/fileops_autop.c
		${LIB_SOURCE_DIR}/src/fileops/fop_basic.c
		${LIB_SOURCE_DIR}/src/fileops/fop_rec.c
		${LIB_SOURCE_DIR}/src/fileops/fop_util.c

		${LIB_SOURCE_DIR}/src/hash/hash.c
		${LIB_SOURCE_DIR}/src/hash/hash.src
		${LIB_SOURCE_DIR}/src/hash/hash_auto.c
		${LIB_SOURCE_DIR}/src/hash/hash_autop.c
		${LIB_SOURCE_DIR}/src/hash/hash_compact.c
		${LIB_SOURCE_DIR}/src/hash/hash_conv.c
		${LIB_SOURCE_DIR}/src/hash/hash_dup.c
		${LIB_SOURCE_DIR}/src/hash/hash_func.c
		${LIB_SOURCE_DIR}/src/hash/hash_meta.c
		${LIB_SOURCE_DIR}/src/hash/hash_method.c
		${LIB_SOURCE_DIR}/src/hash/hash_open.c
		${LIB_SOURCE_DIR}/src/hash/hash_page.c
		${LIB_SOURCE_DIR}/src/hash/hash_rec.c
		${LIB_SOURCE_DIR}/src/hash/hash_reclaim.c
		${LIB_SOURCE_DIR}/src/hash/hash_stat.c
                # ${LIB_SOURCE_DIR}/src/hash/hash_stub.c
		${LIB_SOURCE_DIR}/src/hash/hash_upgrade.c
		${LIB_SOURCE_DIR}/src/hash/hash_verify.c

		${LIB_SOURCE_DIR}/src/heap/heap.c
		${LIB_SOURCE_DIR}/src/heap/heap.src
		${LIB_SOURCE_DIR}/src/heap/heap_auto.c
		${LIB_SOURCE_DIR}/src/heap/heap_autop.c
		${LIB_SOURCE_DIR}/src/heap/heap_backup.c
		${LIB_SOURCE_DIR}/src/heap/heap_conv.c
		${LIB_SOURCE_DIR}/src/heap/heap_method.c
		${LIB_SOURCE_DIR}/src/heap/heap_open.c
		${LIB_SOURCE_DIR}/src/heap/heap_rec.c
		${LIB_SOURCE_DIR}/src/heap/heap_reclaim.c
		${LIB_SOURCE_DIR}/src/heap/heap_stat.c
                # ${LIB_SOURCE_DIR}/src/heap/heap_stub.c
		${LIB_SOURCE_DIR}/src/heap/heap_verify.c

		${LIB_SOURCE_DIR}/src/hmac/hmac.c
		${LIB_SOURCE_DIR}/src/hmac/sha1.c

		# ${LIB_SOURCE_DIR}/src/lock/Design
		${LIB_SOURCE_DIR}/src/lock/lock.c
		# ${LIB_SOURCE_DIR}/src/lock/lock_alloc.incl
		${LIB_SOURCE_DIR}/src/lock/lock_deadlock.c
		${LIB_SOURCE_DIR}/src/lock/lock_failchk.c
		${LIB_SOURCE_DIR}/src/lock/lock_id.c
		${LIB_SOURCE_DIR}/src/lock/lock_list.c
		${LIB_SOURCE_DIR}/src/lock/lock_method.c
		${LIB_SOURCE_DIR}/src/lock/lock_region.c
		${LIB_SOURCE_DIR}/src/lock/lock_stat.c
                # ${LIB_SOURCE_DIR}/src/lock/lock_stub.c
		${LIB_SOURCE_DIR}/src/lock/lock_timer.c
		${LIB_SOURCE_DIR}/src/lock/lock_util.c

		${LIB_SOURCE_DIR}/src/log/log.c
		${LIB_SOURCE_DIR}/src/log/log_archive.c
		${LIB_SOURCE_DIR}/src/log/log_compare.c
		${LIB_SOURCE_DIR}/src/log/log_debug.c
		${LIB_SOURCE_DIR}/src/log/log_get.c
		${LIB_SOURCE_DIR}/src/log/log_method.c
		${LIB_SOURCE_DIR}/src/log/log_print.c
		${LIB_SOURCE_DIR}/src/log/log_put.c
		${LIB_SOURCE_DIR}/src/log/log_stat.c
		${LIB_SOURCE_DIR}/src/log/log_verify.c
		${LIB_SOURCE_DIR}/src/log/log_verify_auto.c
		${LIB_SOURCE_DIR}/src/log/log_verify_int.c
                # ${LIB_SOURCE_DIR}/src/log/log_verify_stub.c
		${LIB_SOURCE_DIR}/src/log/log_verify_util.c

		${LIB_SOURCE_DIR}/src/mp/mp_alloc.c
		${LIB_SOURCE_DIR}/src/mp/mp_backup.c
		${LIB_SOURCE_DIR}/src/mp/mp_bh.c
		${LIB_SOURCE_DIR}/src/mp/mp_fget.c
		${LIB_SOURCE_DIR}/src/mp/mp_fmethod.c
		${LIB_SOURCE_DIR}/src/mp/mp_fopen.c
		${LIB_SOURCE_DIR}/src/mp/mp_fput.c
		${LIB_SOURCE_DIR}/src/mp/mp_fset.c
		${LIB_SOURCE_DIR}/src/mp/mp_method.c
		${LIB_SOURCE_DIR}/src/mp/mp_mvcc.c
		${LIB_SOURCE_DIR}/src/mp/mp_region.c
		${LIB_SOURCE_DIR}/src/mp/mp_register.c
		${LIB_SOURCE_DIR}/src/mp/mp_resize.c
		${LIB_SOURCE_DIR}/src/mp/mp_stat.c
		${LIB_SOURCE_DIR}/src/mp/mp_sync.c
		${LIB_SOURCE_DIR}/src/mp/mp_trickle.c

		${LIB_SOURCE_DIR}/src/mutex/mut_alloc.c
		${LIB_SOURCE_DIR}/src/mutex/mut_failchk.c
                # ${LIB_SOURCE_DIR}/src/mutex/mut_fcntl.c
		${LIB_SOURCE_DIR}/src/mutex/mut_method.c
                # ${LIB_SOURCE_DIR}/src/mutex/mut_pthread.c
		${LIB_SOURCE_DIR}/src/mutex/mut_region.c
		${LIB_SOURCE_DIR}/src/mutex/mut_stat.c
                # ${LIB_SOURCE_DIR}/src/mutex/mut_stub.c
		${LIB_SOURCE_DIR}/src/mutex/mut_tas.c
		${LIB_SOURCE_DIR}/src/mutex/mut_win32.c
		# ${LIB_SOURCE_DIR}/src/mutex/README
                # ${LIB_SOURCE_DIR}/src/mutex/test_mutex.c
                # ${LIB_SOURCE_DIR}/src/mutex/uts4_cc.s

		${LIB_SOURCE_DIR}/src/os/os_abort.c
		${LIB_SOURCE_DIR}/src/os/os_abs.c
                ${LIB_SOURCE_DIR}/src/os/os_addrinfo.c
		${LIB_SOURCE_DIR}/src/os/os_alloc.c
		${LIB_SOURCE_DIR}/src/os/os_clock.c
		${LIB_SOURCE_DIR}/src/os/os_config.c
		${LIB_SOURCE_DIR}/src/os/os_cpu.c
		${LIB_SOURCE_DIR}/src/os/os_ctime.c
		${LIB_SOURCE_DIR}/src/os/os_dir.c
		${LIB_SOURCE_DIR}/src/os/os_errno.c
		${LIB_SOURCE_DIR}/src/os/os_fid.c
		${LIB_SOURCE_DIR}/src/os/os_flock.c
		${LIB_SOURCE_DIR}/src/os/os_fsync.c
		${LIB_SOURCE_DIR}/src/os/os_getenv.c
		${LIB_SOURCE_DIR}/src/os/os_handle.c
		${LIB_SOURCE_DIR}/src/os/os_map.c
                ${LIB_SOURCE_DIR}/src/os/os_mkdir.c
		${LIB_SOURCE_DIR}/src/os/os_open.c
		${LIB_SOURCE_DIR}/src/os/os_path.c
		${LIB_SOURCE_DIR}/src/os/os_pid.c
		${LIB_SOURCE_DIR}/src/os/os_rename.c
		${LIB_SOURCE_DIR}/src/os/os_root.c
		${LIB_SOURCE_DIR}/src/os/os_rpath.c
		${LIB_SOURCE_DIR}/src/os/os_rw.c
		${LIB_SOURCE_DIR}/src/os/os_seek.c
		${LIB_SOURCE_DIR}/src/os/os_stack.c
		${LIB_SOURCE_DIR}/src/os/os_stat.c
		${LIB_SOURCE_DIR}/src/os/os_tmpdir.c
		${LIB_SOURCE_DIR}/src/os/os_truncate.c
		${LIB_SOURCE_DIR}/src/os/os_uid.c
		${LIB_SOURCE_DIR}/src/os/os_unlink.c
		${LIB_SOURCE_DIR}/src/os/os_yield.c

		${LIB_SOURCE_DIR}/src/qam/qam.c
		${LIB_SOURCE_DIR}/src/qam/qam.src
		${LIB_SOURCE_DIR}/src/qam/qam_auto.c
		${LIB_SOURCE_DIR}/src/qam/qam_autop.c
		${LIB_SOURCE_DIR}/src/qam/qam_conv.c
		${LIB_SOURCE_DIR}/src/qam/qam_files.c
		${LIB_SOURCE_DIR}/src/qam/qam_method.c
		${LIB_SOURCE_DIR}/src/qam/qam_open.c
		${LIB_SOURCE_DIR}/src/qam/qam_rec.c
		${LIB_SOURCE_DIR}/src/qam/qam_stat.c
                # ${LIB_SOURCE_DIR}/src/qam/qam_stub.c
		${LIB_SOURCE_DIR}/src/qam/qam_upgrade.c
		${LIB_SOURCE_DIR}/src/qam/qam_verify.c

		# ${LIB_SOURCE_DIR}/src/rep/mlease.html
		# ${LIB_SOURCE_DIR}/src/rep/rep.msg
		${LIB_SOURCE_DIR}/src/rep/rep_automsg.c
		${LIB_SOURCE_DIR}/src/rep/rep_backup.c
		${LIB_SOURCE_DIR}/src/rep/rep_elect.c
		${LIB_SOURCE_DIR}/src/rep/rep_lease.c
		${LIB_SOURCE_DIR}/src/rep/rep_log.c
		${LIB_SOURCE_DIR}/src/rep/rep_method.c
		${LIB_SOURCE_DIR}/src/rep/rep_record.c
		${LIB_SOURCE_DIR}/src/rep/rep_region.c
		${LIB_SOURCE_DIR}/src/rep/rep_stat.c
                # ${LIB_SOURCE_DIR}/src/rep/rep_stub.c
		${LIB_SOURCE_DIR}/src/rep/rep_util.c
		${LIB_SOURCE_DIR}/src/rep/rep_verify.c

		# ${LIB_SOURCE_DIR}/src/repmgr/repmgr.msg
		# ${LIB_SOURCE_DIR}/src/repmgr/repmgr.src
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_auto.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_automsg.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_autop.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_elect.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_method.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_msg.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_net.c
                # ${LIB_SOURCE_DIR}/src/repmgr/repmgr_posix.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_queue.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_rec.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_sel.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_stat.c
		# ${LIB_SOURCE_DIR}/src/repmgr/repmgr_stub.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_util.c
		${LIB_SOURCE_DIR}/src/repmgr/repmgr_windows.c

		${LIB_SOURCE_DIR}/src/sequence/sequence.c
		${LIB_SOURCE_DIR}/src/sequence/seq_stat.c

		${LIB_SOURCE_DIR}/src/txn/txn.c
		${LIB_SOURCE_DIR}/src/txn/txn.src
		${LIB_SOURCE_DIR}/src/txn/txn_auto.c
		${LIB_SOURCE_DIR}/src/txn/txn_autop.c
		${LIB_SOURCE_DIR}/src/txn/txn_chkpt.c
		${LIB_SOURCE_DIR}/src/txn/txn_failchk.c
		${LIB_SOURCE_DIR}/src/txn/txn_method.c
		${LIB_SOURCE_DIR}/src/txn/txn_rec.c
		${LIB_SOURCE_DIR}/src/txn/txn_recover.c
		${LIB_SOURCE_DIR}/src/txn/txn_region.c
		${LIB_SOURCE_DIR}/src/txn/txn_stat.c
		${LIB_SOURCE_DIR}/src/txn/txn_util.c

		${LIB_SOURCE_DIR}/src/xa/xa.c
		${LIB_SOURCE_DIR}/src/xa/xa_map.c
	)

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/res/resource.rc)
    # endif()
    
    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DB=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_STATISTICS=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_REPLICATION_THREADS=1)
    target_compile_definitions(${LIB_NAME} PUBLIC DB_WIN32=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_MUTEX_SUPPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_GETCWD=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_PRINTF=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_SNPRINTF=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_VSNPRINTF=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_STRSEP=1)

    target_link_libraries(${LIB_NAME} PRIVATE ws2_32)
    if(MSVC)
    else()
		target_compile_options(${LIB_NAME} PRIVATE -Wno-deprecated-non-prototype)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
