/*
   (c) Copyright 2012-2013  DirectFB integrated media GmbH
   (c) Copyright 2001-2013  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Shimokawa <andi@directfb.org>,
              Marek Pikarski <mass@directfb.org>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/



//#define DIRECT_DEBUG_ENABLE

#include <config.h>

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <pthread.h>

#include <direct/hash.h>
#include <direct/list.h>

#include <fusion/Debug.h>
#include <fusion/fusion.h>
#include <fusion/shmalloc.h>
#include <fusion/shm/shm_internal.h>

#include <directfb.h>

#include <core/coredefs.h>
#include <core/coretypes.h>

#include <core/core.h>
#include <core/core_parts.h>
#include <core/fonts.h>
#include <core/graphics_state.h>
#include <core/layer_context.h>
#include <core/layer_region.h>
#include <core/palette.h>
#include <core/surface.h>
#include <core/system.h>
#include <core/windows.h>
#include <core/windows_internal.h>

#include <core/CoreDFB.h>
#include <core/CoreSlave.h>
#include <core/Renderer.h>

#include <core/Task.h>
#include <core/TaskManager.h>

#include <gfx/generic/GenefxEngine.h>
#include <gfx/util.h>

#include <direct/build.h>
#include <direct/debug.h>
#include <direct/direct.h>
#include <direct/interface.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>
#include <direct/perf.h>
#include <direct/signals.h>
#include <direct/thread.h>
#include <direct/util.h>

#include <fusion/conf.h>
#include <fusion/hash.h>

#include <misc/conf.h>
#include <misc/util.h>

#if defined(DFB_DYNAMIC_LINKING) && defined(SOPATH)
#include <dlfcn.h>
#endif

D_DEBUG_DOMAIN( DirectFB_Core, "DirectFB/Core", "DirectFB Core" );

/******************************************************************************/

extern CorePart dfb_clipboard_core;
extern CorePart dfb_colorhash_core;
extern CorePart dfb_graphics_core;
extern CorePart dfb_input_core;
extern CorePart dfb_layer_core;
extern CorePart dfb_screen_core;
extern CorePart dfb_surface_core;
extern CorePart dfb_system_core;
extern CorePart dfb_wm_core;

static CorePart *core_parts[] = {
     &dfb_clipboard_core,
     &dfb_colorhash_core,
     &dfb_surface_core,
     &dfb_system_core,
     &dfb_input_core,
     &dfb_graphics_core,
     &dfb_screen_core,
     &dfb_layer_core,
     &dfb_wm_core
};

void *
dfb_core_get_part( CoreDFB        *core,
                   DFBCorePartID   part_id )
{
     switch (part_id) {
          case DFCP_CLIPBOARD:
               return dfb_clipboard_core.data_local;

          case DFCP_COLORHASH:
               return dfb_colorhash_core.data_local;

          case DFCP_GRAPHICS:
               return dfb_graphics_core.data_local;

          case DFCP_INPUT:
               return dfb_input_core.data_local;

          case DFCP_LAYER:
               return dfb_layer_core.data_local;

          case DFCP_SCREEN:
               return dfb_screen_core.data_local;

          case DFCP_SURFACE:
               return dfb_surface_core.data_local;

          case DFCP_SYSTEM:
               return dfb_system_core.data_local;

          case DFCP_WM:
               return dfb_wm_core.data_local;

          default:
               D_BUG( "unknown core part" );
     }

     return NULL;
}

/******************************************************************************/

/*
 * one entry in the cleanup stack
 */
struct _CoreCleanup {
     DirectLink       link;

     CoreCleanupFunc  func;        /* the cleanup function to be called */
     void            *data;        /* context of the cleanup function */
     bool             emergency;   /* if true, cleanup is also done during
                                      emergency shutdown (from signal hadler) */
};

/******************************************************************************/

/*
 * ckecks if stack is clean, otherwise prints warning, then calls core_deinit()
 */
static void dfb_core_deinit_check( void *ctx );

static void dfb_core_thread_init_handler( DirectThread *thread, void *arg );

static void dfb_core_process_cleanups( CoreDFB *core, bool emergency );

static DirectSignalHandlerResult dfb_core_signal_handler( int   num,
                                                          void *addr,
                                                          void *ctx );

/******************************************************************************/

static int dfb_core_arena_initialize( void        *ctx );
static int dfb_core_arena_shutdown  ( void        *ctx,
                                      bool         emergency );
static int dfb_core_arena_join      ( void        *ctx );
static int dfb_core_arena_leave     ( void        *ctx,
                                      bool         emergency );

/******************************************************************************/

#if defined(DFB_DYNAMIC_LINKING) && defined(SOPATH)
/*
 * the library handle for dlopen'ing ourselves
 */
static void* dfb_lib_handle = NULL;
#endif

/******************************************************************************/

CoreDFB         *core_dfb      = NULL;
static pthread_mutex_t  core_dfb_lock = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************/

static FusionCallHandlerResult
Core_AsyncCall_Handler( int           caller,   /* fusion id of the caller */
                        int           call_arg, /* optional call parameter */
                        void         *call_ptr, /* optional call parameter */
                        void         *ctx,      /* optional handler context */
                        unsigned int  serial,
                        int          *ret_val )
{
     AsyncCall *call = call_ptr;

     call->func( call->ctx, call->ctx2 );

     return FCHR_RETURN;
}

/******************************************************************************/

DFBResult
dfb_core_create( CoreDFB **ret_core )
{
     int      ret;
#if FUSION_BUILD_MULTI
     char     buf[16];
#endif
     CoreDFB       *core   = NULL;
     CoreDFBShared *shared = NULL;

     (void)shared;

     D_ASSERT( ret_core != NULL );
     D_ASSERT( dfb_config != NULL );

     D_DEBUG_AT( DirectFB_Core, "%s...\n", __FUNCTION__ );

     pthread_mutex_lock( &core_dfb_lock );

     D_ASSERT( core_dfb == NULL || core_dfb->refs > 0 );

     if (core_dfb) {
          D_MAGIC_ASSERT( core_dfb, CoreDFB );

          core_dfb->refs++;

          *ret_core = core_dfb;

          pthread_mutex_unlock( &core_dfb_lock );

          return DFB_OK;
     }

     direct_initialize();


     D_INFO( "DirectFB/Core: %s Application Core. ("BUILDTIME") %s%s\n",
             FUSION_BUILD_MULTI ? "Multi" : "Single",
             DIRECT_BUILD_DEBUG ? "[ DEBUG ]" : "",
             DIRECT_BUILD_TRACE ? "[ TRACE ]" : "" );


#if defined(DFB_DYNAMIC_LINKING) && defined(SOPATH)
     if (!dfb_lib_handle)
#ifdef RTLD_GLOBAL
          dfb_lib_handle = dlopen(SOPATH, RTLD_GLOBAL|RTLD_LAZY);
#else
          /* RTLD_GLOBAL is not defined on OpenBSD */
          dfb_lib_handle = dlopen(SOPATH, RTLD_LAZY);
#endif
#endif

     ret = dfb_system_lookup();
     if (ret)
          goto error;

     if (dfb_system_caps() & CSCAPS_SECURE_FUSION) {
          D_INFO( "DirectFB/Core: Forcing 'secure-fusion' option (requested by system module)\n" );
          fusion_config->secure_fusion = true;
     }

     if (dfb_system_caps() & CSCAPS_ALWAYS_INDIRECT) {
          D_INFO( "DirectFB/Core: Forcing 'always-indirect' option (requested by system module)\n" );
          dfb_config->call_nodirect = FCEF_NODIRECT;
     }

     if (dfb_config->task_manager && !fusion_config->secure_fusion) {
          D_ERROR( "DirectFB/Core: Option task-manager without secure-fusion is unsupported!\n" );
          ret = DFB_UNSUPPORTED;
          goto error;
     }

     /* Allocate local core structure. */
     core = D_CALLOC( 1, sizeof(CoreDFB) );
     if (!core) {
          ret = D_OOM();
          goto error;
     }

     core->refs = 1;

     core->init_handler = direct_thread_add_init_handler( dfb_core_thread_init_handler, core );

#if FUSION_BUILD_MULTI
     dfb_system_thread_init();
#endif

     direct_find_best_memcpy();

     D_MAGIC_SET( core, CoreDFB );

     core_dfb = core;

     ret = fusion_enter( dfb_config->session, DIRECTFB_CORE_ABI, FER_ANY, &core->world );
     if (ret)
          goto error;

     core->fusion_id = fusion_id( core->world );

#if FUSION_BUILD_MULTI
     D_DEBUG_AT( DirectFB_Core, "world %d, fusion id %lu\n", fusion_world_index(core->world), core->fusion_id );

     snprintf( buf, sizeof(buf), "%d", fusion_world_index(core->world) );

     setenv( "DIRECTFB_SESSION", buf, true );
#endif

     if (dfb_config->sync) {
          D_INFO( "DirectFB/Core: calling sync()...\n" );
          direct_sync();
     }

     if (dfb_config->core_sighandler)
          direct_signal_handler_add( DIRECT_SIGNAL_ANY, dfb_core_signal_handler, core, &core->signal_handler );

     fusion_call_init( &core_dfb->async_call, Core_AsyncCall_Handler, core, core_dfb->world );
     fusion_call_set_name( &core_dfb->async_call, "Core_AsyncCall" );

     if (dfb_core_is_master( core_dfb ))
          ret = dfb_core_arena_initialize( core_dfb );
     else
          ret = dfb_core_arena_join( core_dfb );
     if (ret)
          goto error;

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     if (dfb_config->block_all_signals)
          direct_signals_block_all();

     if (dfb_config->deinit_check)
          direct_cleanup_handler_add( dfb_core_deinit_check, NULL, &core->cleanup_handler );

     dfb_font_manager_create( core, &core->font_manager );

     *ret_core = core;

     pthread_mutex_unlock( &core_dfb_lock );

     D_DEBUG_AT( DirectFB_Core, "Core successfully created.\n" );

     return DFB_OK;


error:
     if (core) {
          if (core->world) {
               fusion_call_destroy( &core_dfb->async_call );

               fusion_exit( core->world, false );
          }

          if (core->init_handler)
               direct_thread_remove_init_handler( core->init_handler );

          if (core->signal_handler)
               direct_signal_handler_remove( core->signal_handler );

          D_MAGIC_CLEAR( core );

          D_FREE( core );
          core_dfb = NULL;
     }

     pthread_mutex_unlock( &core_dfb_lock );

     direct_shutdown();

     return ret;
}

DFBResult
dfb_core_destroy( CoreDFB *core, bool emergency )
{
     DFBResult ret;

     D_MAGIC_ASSERT( core, CoreDFB );
     D_ASSERT( core->refs > 0 );
     D_ASSERT( core == core_dfb );

     D_DEBUG_AT( DirectFB_Core, "%s...\n", __FUNCTION__ );

     if (!emergency) {
          pthread_mutex_lock( &core_dfb_lock );

          if (--core->refs) {
               pthread_mutex_unlock( &core_dfb_lock );
               return DFB_OK;
          }
     }

     if (!core->shutdown_running)
          core->shutdown_running = 1;
     else {
          if (!emergency)
               pthread_mutex_unlock( &core_dfb_lock );
          D_WARN( "core shutdown already running" );
          return DFB_OK;
     }

     if (core->signal_handler) {
          DirectSignalHandler *signal_handler = core->signal_handler;

          core->signal_handler = NULL;

          direct_signal_handler_remove( signal_handler );
     }

     if (core->cleanup_handler) {
          DirectCleanupHandler *cleanup_handler = core->cleanup_handler;

          core->cleanup_handler = NULL;

          direct_cleanup_handler_remove( cleanup_handler );
     }

     // FIXME: avoid this workaround
#if 0
     direct_thread_sleep( 500000 );
#else //hisi add, fix dfb app exit too slow problem
	 direct_thread_sleep( 70000 );
#endif

     if (core->font_manager)
          dfb_font_manager_destroy( core->font_manager );

     if (dfb_core_is_master( core )) {
          if (emergency) {
               fusion_kill( core->world, 0, SIGKILL, 1000 );
          }
          else {
               fusion_kill( core->world, 0, SIGTERM, 5000 );
               fusion_kill( core->world, 0, SIGKILL, 2000 );
          }
     }

     dfb_core_process_cleanups( core, emergency );

     if (dfb_core_is_master( core_dfb ))
          ret = dfb_core_arena_shutdown( core_dfb, emergency );
     else
          ret = dfb_core_arena_leave( core_dfb, emergency );

     fusion_call_destroy( &core_dfb->async_call );

     fusion_exit( core->world, emergency );

     if (!emergency)
          direct_thread_remove_init_handler( core->init_handler );

     D_MAGIC_CLEAR( core );

     D_FREE( core );
     core_dfb = NULL;

     if (!emergency)
          pthread_mutex_unlock( &core_dfb_lock );

     direct_shutdown();

     return ret;
}

CoreGraphicsState *
dfb_core_create_graphics_state( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->graphics_state_pool != NULL );

     return (CoreGraphicsState*) fusion_object_create( core->shared->graphics_state_pool, core->world, Core_GetIdentity() );
}

CoreLayerContext *
dfb_core_create_layer_context( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( shared->layer_context_pool != NULL );

     return (CoreLayerContext*) fusion_object_create( shared->layer_context_pool, core->world, Core_GetIdentity() );
}

CoreLayerRegion *
dfb_core_create_layer_region( CoreDFB *core )
{
     CoreDFBShared *shared;
     
     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->layer_region_pool != NULL );

     return (CoreLayerRegion*) fusion_object_create( core->shared->layer_region_pool, core->world, Core_GetIdentity() );
}

CorePalette *
dfb_core_create_palette( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->palette_pool != NULL );

     return (CorePalette*) fusion_object_create( core->shared->palette_pool, core->world, Core_GetIdentity() );
}

CoreSurface *
dfb_core_create_surface( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_pool != NULL );

     return (CoreSurface*) fusion_object_create( core->shared->surface_pool, core->world, Core_GetIdentity() );
}

CoreSurfaceAllocation *
dfb_core_create_surface_allocation( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_allocation_pool != NULL );

     return (CoreSurfaceAllocation*) fusion_object_create( core->shared->surface_allocation_pool, core->world, Core_GetIdentity() );
}

CoreSurfaceBuffer *
dfb_core_create_surface_buffer( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_buffer_pool != NULL );

     return (CoreSurfaceBuffer*) fusion_object_create( core->shared->surface_buffer_pool, core->world, Core_GetIdentity() );
}

CoreSurfaceClient *
dfb_core_create_surface_client( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_client_pool != NULL );

     return (CoreSurfaceClient*) fusion_object_create( core->shared->surface_client_pool, core->world, Core_GetIdentity() );
}

CoreWindow *
dfb_core_create_window( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->window_pool != NULL );

     return (CoreWindow*) fusion_object_create( core->shared->window_pool, core->world, Core_GetIdentity() );
}

DFBResult
dfb_core_get_graphics_state( CoreDFB            *core,
                             u32                 object_id,
                             CoreGraphicsState **ret_state )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_state != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->graphics_state_pool != NULL );

     ret = fusion_object_get( core->shared->graphics_state_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_state = (CoreGraphicsState*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_layer_context( CoreDFB           *core,
                            u32                object_id,
                            CoreLayerContext **ret_context )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_context != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->layer_context_pool != NULL );

     ret = fusion_object_get( core->shared->layer_context_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_context = (CoreLayerContext*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_layer_region( CoreDFB          *core,
                           u32               object_id,
                           CoreLayerRegion **ret_region )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_region != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->layer_region_pool != NULL );

     ret = fusion_object_get( core->shared->layer_region_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_region = (CoreLayerRegion*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_palette( CoreDFB      *core,
                      u32           object_id,
                      CorePalette **ret_palette )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_palette != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->palette_pool != NULL );

     ret = fusion_object_get( core->shared->palette_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_palette = (CorePalette*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_surface( CoreDFB      *core,
                      u32           object_id,
                      CoreSurface **ret_surface )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_surface != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_pool != NULL );

     ret = fusion_object_get( core->shared->surface_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_surface = (CoreSurface*) object;

     return DFB_OK;
}

DFBResult
dfb_core_lookup_surface( CoreDFB      *core,
                         u32           object_id,
                         CoreSurface **ret_surface )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_surface != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_pool != NULL );

     ret = fusion_object_lookup( core->shared->surface_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_surface = (CoreSurface*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_surface_allocation( CoreDFB                *core,
                                 u32                     object_id,
                                 CoreSurfaceAllocation **ret_allocation )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_allocation != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_allocation_pool != NULL );

     ret = fusion_object_get( core->shared->surface_allocation_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_allocation = (CoreSurfaceAllocation*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_surface_buffer( CoreDFB            *core,
                             u32                 object_id,
                             CoreSurfaceBuffer **ret_buffer )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_buffer != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_buffer_pool != NULL );

     ret = fusion_object_get( core->shared->surface_buffer_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_buffer = (CoreSurfaceBuffer*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_surface_client( CoreDFB            *core,
                             u32                 object_id,
                             CoreSurfaceClient **ret_client )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_client != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->surface_client_pool != NULL );

     ret = fusion_object_get( core->shared->surface_client_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_client = (CoreSurfaceClient*) object;

     return DFB_OK;
}

DFBResult
dfb_core_get_window( CoreDFB     *core,
                     u32          object_id,
                     CoreWindow **ret_window )
{
     DFBResult     ret;
     FusionObject *object;

     CoreDFBShared *shared;

     (void)shared;

     D_ASSUME( core != NULL );
     D_ASSERT( ret_window != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );
     D_ASSERT( core->shared->window_pool != NULL );

     ret = fusion_object_get( core->shared->window_pool, object_id, &object );
     if (ret)
          return ret;

     *ret_window = (CoreWindow*) object;

     return DFB_OK;
}

DirectResult
dfb_core_enum_surfaces( CoreDFB               *core,
                        FusionObjectCallback   callback,
                        void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->surface_pool, callback, ctx );
}

DirectResult
dfb_core_enum_graphics_states( CoreDFB               *core,
                               FusionObjectCallback   callback,
                               void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->graphics_state_pool, callback, ctx );
}

DirectResult
dfb_core_enum_layer_contexts( CoreDFB               *core,
                              FusionObjectCallback   callback,
                              void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->layer_context_pool, callback, ctx );
}

DirectResult
dfb_core_enum_layer_regions( CoreDFB               *core,
                             FusionObjectCallback   callback,
                             void                  *ctx )
{
     CoreDFBShared *shared;

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return fusion_object_pool_enum( shared->layer_region_pool, callback, ctx );
}

typedef struct {
     DirectLogDomain *domain;
     DirectLogLevel   level;
} DumpContext;

static bool
dump_objects( FusionObjectPool *pool,
              FusionObject     *object,
              void             *ctx )
{
     DumpContext *context = (DumpContext *) ctx;
     const char  *desc    = "";

     if (dfb_core_is_master( core_dfb ) && pool->describe)
          desc = pool->describe( object, pool->ctx );

     direct_log_domain_log( context->domain, context->level, __FUNCTION__, __FILE__, __LINE__,
                            "        %s {%s}\n", ToString_FusionObject( object ), desc );

     direct_trace_print_stack( object->create_stack );

     return true;
}

DirectResult
dfb_core_dump_all( CoreDFB         *core,
                   DirectLogDomain *domain,
                   DirectLogLevel   level )
{
     CoreDFBShared *shared;

     D_DEBUG_AT( DirectFB_Core, "%s()\n", __FUNCTION__ );

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     if (direct_log_domain_check_level( domain, level )) {
          int               i;
          FusionObjectPool *pools[] = {
               shared->graphics_state_pool,
               shared->layer_context_pool,
               shared->layer_region_pool,
               shared->palette_pool,
               shared->surface_pool,
               shared->surface_allocation_pool,
               shared->surface_buffer_pool,
               shared->surface_client_pool,
               shared->window_pool
          };

          for (i=0; i<D_ARRAY_SIZE(pools); i++) {
               if (pools[i]) {
                    DumpContext context;
     
                    context.domain = domain;
                    context.level  = level;
     
                    direct_log_domain_log( domain, level, __FUNCTION__, __FILE__, __LINE__,
                                           "  - Objects in '%s' -\n", pools[i]->name );
     
                    fusion_object_pool_enum( pools[i], dump_objects, &context );
               }
          }
     }

     return DR_OK;
}

DirectResult
dfb_core_wait_all( CoreDFB   *core,
                   long long  timeout )
{
     CoreDFBShared *shared;
     long long      start;

     D_DEBUG_AT( DirectFB_Core, "%s( timeout %lld us )\n", __FUNCTION__, timeout );

     D_ASSERT( core != NULL || core_dfb != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     start = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

     while (true) {
          int               i;
          FusionObjectPool *pools[] = {
               shared->graphics_state_pool,
               shared->layer_context_pool,
               shared->layer_region_pool,
               shared->palette_pool,
               shared->surface_pool,
               shared->surface_allocation_pool,
               shared->surface_buffer_pool,
               shared->surface_client_pool,
               shared->window_pool
          };

          long long now = direct_clock_get_time( DIRECT_CLOCK_MONOTONIC );

          for (i=0; i<D_ARRAY_SIZE(pools); i++) {
               if (pools[i]) {
                    size_t    num = 0;
                    DFBResult ret = fusion_object_pool_size( pools[i], &num );

                    D_ASSERT( ret == DFB_OK );

                    if (num > 0) {
                         if (now - start >= timeout) {
                              D_DEBUG_AT( DirectFB_Core, "  -> still %zu objects in pool, timeout!\n", num );
                              return DR_TIMEOUT;
                         }

                         D_DEBUG_AT( DirectFB_Core, "  -> still %zu objects in '%s', waiting 10ms...\n", num, pools[i]->name );

                         break;
                    }
               }
          }

          if (i < D_ARRAY_SIZE(pools)) {
               D_DEBUG_AT( DirectFB_Core, "  -> waiting another 10ms for objects to be dead...\n" );

               direct_thread_sleep( 10000 );
          }
          else
               break;
     }

     return DR_OK;
}

DirectResult
core_arena_add_shared_field( CoreDFB         *core,
                             const char      *name,
                             void            *data )
{
     DirectResult       ret;
     char              *shname;

     D_DEBUG_AT( DirectFB_Core, "%s( '%s' -> %p )\n", __FUNCTION__, name, data );

     D_ASSERT( core != NULL );
     D_ASSERT( core->shared != NULL );

     /* Give it the requested name. */
     shname = SHSTRDUP( core->shared->shmpool, name );
     if (shname)
          ret = fusion_hash_replace( core->shared->field_hash, shname, data, NULL, NULL );
     else
          ret = D_OOSHM();

     return ret;
}

DirectResult
core_arena_get_shared_field( CoreDFB         *core,
                             const char      *name,
                             void           **data )
{
     void *ptr;

     D_DEBUG_AT( DirectFB_Core, "%s( '%s' )\n", __FUNCTION__, name );

     D_ASSERT( core != NULL );
     D_ASSERT( core->shared != NULL );

     /* Lookup entry. */
     ptr = fusion_hash_lookup( core->shared->field_hash, name );

     D_DEBUG_AT( DirectFB_Core, "  -> %p\n", ptr );

     if (!ptr)
          return DR_ITEMNOTFOUND;

     *data = ptr;

     return DR_OK;
}

bool
dfb_core_is_master( CoreDFB *core )
{
     D_MAGIC_ASSERT( core, CoreDFB );

     return core->fusion_id == FUSION_ID_MASTER;
}

void
dfb_core_activate( CoreDFB *core )
{
     CoreDFBShared *shared;

     (void)shared;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     /* Let others enter the world. */
     fusion_world_activate( core->world );
}

FusionWorld *
dfb_core_world( CoreDFB *core )
{
//     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     return core->world;
}

FusionSHMPoolShared *
dfb_core_shmpool( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return shared->shmpool;
}

FusionSHMPoolShared *
dfb_core_shmpool_data( CoreDFB *core )
{
     CoreDFBShared *shared;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     return shared->shmpool_data;
}

DFBResult
dfb_core_suspend( CoreDFB *core )
{
     DFBResult ret;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     if (!dfb_core_is_master( core ))
          return DFB_ACCESSDENIED;

     if (core->suspended)
          return DFB_BUSY;

     ret = dfb_input_core.Suspend( dfb_input_core.data_local );
     if (ret)
          goto error_input;

     ret = dfb_layer_core.Suspend( dfb_layer_core.data_local );
     if (ret)
          goto error_layers;

     ret = dfb_screen_core.Suspend( dfb_screen_core.data_local );
     if (ret)
          goto error_screens;

     ret = dfb_graphics_core.Suspend( dfb_graphics_core.data_local );
     if (ret)
          goto error_graphics;

     core->suspended = true;

     return DFB_OK;

error_graphics:
     dfb_screen_core.Resume( dfb_screen_core.data_local );
error_screens:
     dfb_layer_core.Resume( dfb_layer_core.data_local );
error_layers:
     dfb_input_core.Resume( dfb_input_core.data_local );
error_input:
     return ret;
}

DFBResult
dfb_core_resume( CoreDFB *core )
{
     DFBResult ret;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     if (!dfb_core_is_master( core ))
          return DFB_ACCESSDENIED;

     if (!core->suspended)
          return DFB_BUSY;

     ret = dfb_graphics_core.Resume( dfb_graphics_core.data_local );
     if (ret)
          goto error_graphics;

     ret = dfb_screen_core.Resume( dfb_screen_core.data_local );
     if (ret)
          goto error_screens;

     ret = dfb_layer_core.Resume( dfb_layer_core.data_local );
     if (ret)
          goto error_layers;

     ret = dfb_input_core.Resume( dfb_input_core.data_local );
     if (ret)
          goto error_input;

     core->suspended = false;

     return DFB_OK;

error_input:
     dfb_layer_core.Suspend( dfb_layer_core.data_local );
error_layers:
     dfb_screen_core.Suspend( dfb_screen_core.data_local );
error_screens:
     dfb_graphics_core.Suspend( dfb_graphics_core.data_local );
error_graphics:
     return ret;
}

CoreCleanup *
dfb_core_cleanup_add( CoreDFB         *core,
                      CoreCleanupFunc  func,
                      void            *data,
                      bool             emergency )
{
     CoreCleanup *cleanup;

     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     cleanup = D_CALLOC( 1, sizeof(CoreCleanup) );

     cleanup->func      = func;
     cleanup->data      = data;
     cleanup->emergency = emergency;

     direct_list_prepend( &core->cleanups, &cleanup->link );

     return cleanup;
}

void
dfb_core_cleanup_remove( CoreDFB     *core,
                         CoreCleanup *cleanup )
{
     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     direct_list_remove( &core->cleanups, &cleanup->link );

     D_FREE( cleanup );
}

DFBFontManager *
dfb_core_font_manager( CoreDFB *core )
{
     D_ASSUME( core != NULL );

     if (!core)
          core = core_dfb;

     D_MAGIC_ASSERT( core, CoreDFB );

     return core->font_manager;
}

/******************************************************************************/

struct __CoreDFB_CoreMemoryPermission {
     DirectLink                    link;

     CoreMemoryPermissionFlags     flags;

     void                         *data;
     void                         *end;
     size_t                        length;
};

DFBResult
dfb_core_memory_permissions_add( CoreDFB                   *core,
                                 CoreMemoryPermissionFlags  flags,
                                 void                      *data,
                                 size_t                     length,
                                 CoreMemoryPermission     **ret_permission )
{
     CoreMemoryPermission *permission;

     D_DEBUG_AT( DirectFB_Core, "%s( flags 0x%02x, data %p, length " _ZU " )\n", __FUNCTION__, flags, data, length );

     D_MAGIC_ASSERT( core, CoreDFB );

     permission = D_CALLOC( 1, sizeof(CoreMemoryPermission) );
     if (!permission)
          return D_OOM();

     permission->flags  = flags;
     permission->data   = data;
     permission->end    = data + length;
     permission->length = length;

     direct_mutex_lock( &core->memory_permissions_lock );

     direct_list_prepend( &core->memory_permissions, &permission->link );

     direct_mutex_unlock( &core->memory_permissions_lock );

     *ret_permission = permission;

     return DFB_OK;
}

DFBResult
dfb_core_memory_permissions_remove( CoreDFB                   *core,
                                    CoreMemoryPermission      *permission )
{
     D_DEBUG_AT( DirectFB_Core, "%s( flags 0x%02x, data %p, length " _ZU " )\n", __FUNCTION__,
                 permission->flags, permission->data, permission->length );

     D_MAGIC_ASSERT( core, CoreDFB );

     direct_mutex_lock( &core->memory_permissions_lock );

     direct_list_remove( &core->memory_permissions, &permission->link );

     direct_mutex_unlock( &core->memory_permissions_lock );

     D_FREE( permission );

     return DFB_OK;
}

DFBResult
dfb_core_memory_permissions_check( CoreDFB                   *core,
                                   CoreMemoryPermissionFlags  flags,
                                   void                      *data,
                                   size_t                     length )
{
     CoreMemoryPermission *permission;

     D_LOG( DirectFB_Core, DEBUG_9, "%s( flags 0x%02x, data %p, length " _ZU " )\n", __FUNCTION__, flags, data, length );

     D_MAGIC_ASSERT( core, CoreDFB );

     direct_mutex_lock( &core->memory_permissions_lock );

     direct_list_foreach (permission, core->memory_permissions) {
          if (permission->data <= data && permission->end >= data + length &&
              D_FLAGS_ARE_SET( permission->flags, flags ))
          {
               D_LOG( DirectFB_Core, DEBUG_9, "  -> found flags 0x%02x, data %p, length " _ZU "\n",
                      permission->flags, permission->data, permission->length );

               direct_mutex_unlock( &core->memory_permissions_lock );

               return DFB_OK;
          }
     }

     direct_mutex_unlock( &core->memory_permissions_lock );

     return DFB_ITEMNOTFOUND;
}

/******************************************************************************/

static void
dfb_core_deinit_check( void *ctx )
{
     if (core_dfb && core_dfb->refs) {
          D_WARN( "Application exited without deinitialization of DirectFB!" );

          direct_print_interface_leaks();

          dfb_core_destroy( core_dfb, false );
     }
}

static void
dfb_core_thread_init_handler( DirectThread *thread, void *arg )
{
     dfb_system_thread_init();
}

static void
dfb_core_process_cleanups( CoreDFB *core, bool emergency )
{
     D_MAGIC_ASSERT( core, CoreDFB );

     while (core->cleanups) {
          CoreCleanup *cleanup = (CoreCleanup*) core->cleanups;

          core->cleanups = core->cleanups->next;

          if (cleanup->emergency || !emergency)
               cleanup->func( cleanup->data, emergency );

          D_FREE( cleanup );
     }
}

static DirectSignalHandlerResult
dfb_core_signal_handler( int   num,
                         void *addr,
                         void *ctx )
{
     bool     locked;
     CoreDFB *core = ctx;

     D_ASSERT( core == core_dfb );

     locked = pthread_mutex_trylock( &core_dfb_lock ) == 0;

     dfb_core_destroy( core, true );

     if (locked)
          pthread_mutex_unlock( &core_dfb_lock );

     return DSHR_OK;
}

/******************************************************************************/

static bool
region_callback( FusionObjectPool *pool,
                 FusionObject     *object,
                 void             *ctx )
{
     CoreLayerRegion *region = (CoreLayerRegion *) object;

     if (region->state & CLRSF_ENABLED)
          dfb_layer_region_disable( region );

     return true;
}

static int
dfb_core_shutdown( CoreDFB *core, bool emergency )
{
     DFBResult      ret;
     CoreDFBShared *shared;
     int            loops = 10;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     dfb_gfx_init_tls();

     /* Suspend input core to stop all input threads before shutting down. */
     if (dfb_input_core.initialized)
          dfb_input_core.Suspend( dfb_input_core.data_local );

     TaskManager_SyncAll();

     core->shutdown_tid = direct_gettid();


     if (dfb_wm_core.initialized)
          dfb_wm_deactivate_all_stacks( dfb_wm_core.data_local );

     dfb_core_enum_layer_regions( core, region_callback, core );


     fusion_stop_dispatcher( core->world, false );

     dfb_gfx_cleanup();

     while (loops--) {
          fusion_dispatch( core->world, 16384 );

          ret = dfb_core_wait_all( core, 10000 );
          if (ret == DFB_OK)
               break;

          dfb_gfx_cleanup();
     }

     if (ret == DFB_TIMEOUT) {
          if (direct_config_get_int_value( "shutdown-info" )) {
               D_ERROR( "DirectFB/Core: Some objects remain alive, application or internal ref counting issue!\n" );

               dfb_core_dump_all( core, &DirectFB_Core, DIRECT_LOG_VERBOSE );

               direct_print_interface_leaks();
          }
     }

     /* Destroy window objects. */
     fusion_object_pool_destroy( shared->window_pool, core->world );
     shared->window_pool = NULL;

     /* Close window stacks. */
     if (dfb_wm_core.initialized)
          dfb_wm_close_all_stacks( dfb_wm_core.data_local );

     CoreDFB_Deinit_Dispatch( &shared->call );

     /* Destroy layer context and region objects. */
     fusion_object_pool_destroy( shared->layer_region_pool, core->world );
     fusion_object_pool_destroy( shared->layer_context_pool, core->world );

     /* Shutdown WM core. */
     dfb_core_part_shutdown( core, &dfb_wm_core, emergency );

     /* Shutdown layer core. */
     dfb_core_part_shutdown( core, &dfb_layer_core, emergency );
     dfb_core_part_shutdown( core, &dfb_screen_core, emergency );

     TaskManager_SyncAll();

     /* Destroy surface and palette objects. */
     fusion_object_pool_destroy( shared->graphics_state_pool, core->world );
     fusion_object_pool_destroy( shared->surface_client_pool, core->world );
     fusion_object_pool_destroy( shared->surface_pool, core->world );
     fusion_object_pool_destroy( shared->surface_buffer_pool, core->world );
     fusion_object_pool_destroy( shared->surface_allocation_pool, core->world );
     fusion_object_pool_destroy( shared->palette_pool, core->world );

     direct_perf_dump_all();

     Renderer_DeleteEngines();

     /* Destroy remaining core parts. */
     dfb_core_part_shutdown( core, &dfb_graphics_core, emergency );
     dfb_core_part_shutdown( core, &dfb_surface_core, emergency );
     dfb_core_part_shutdown( core, &dfb_input_core, emergency );
     dfb_core_part_shutdown( core, &dfb_system_core, emergency );
     dfb_core_part_shutdown( core, &dfb_colorhash_core, emergency );
     dfb_core_part_shutdown( core, &dfb_clipboard_core, emergency );

     /* Destroy shared memory pool for surface data. */
     fusion_shm_pool_destroy( core->world, shared->shmpool_data );

     // FIXME: cleanup items
     direct_hash_destroy( core->resource.identities );

     direct_mutex_deinit( &core->memory_permissions_lock );

     TaskManager_Shutdown();

     if (direct_config_get_int_value( "dfb-error-shutdown-timeout" ))
          return DFB_TIMEOUT;

     return DFB_OK;
}

DFBResult
dfb_core_initialize( CoreDFB *core )
{
     int            i;
     DFBResult      ret;
     CoreDFBShared *shared;

     D_MAGIC_ASSERT( core, CoreDFB );

     direct_hash_create( 23, &core->resource.identities );

     direct_mutex_init( &core->memory_permissions_lock );

     shared = core->shared;
     D_MAGIC_ASSERT( shared, CoreDFBShared );

     ret = fusion_shm_pool_create( core->world, "DirectFB Data Pool", 0x1000000,
                                   fusion_config->debugshm, &shared->shmpool_data );
     if (ret)
          return ret;

     shared->graphics_state_pool = dfb_graphics_state_pool_create( core->world );
     shared->layer_context_pool  = dfb_layer_context_pool_create( core->world );
     shared->layer_region_pool   = dfb_layer_region_pool_create( core->world );
     shared->palette_pool        = dfb_palette_pool_create( core->world );
     shared->surface_pool        = dfb_surface_pool_create( core->world );
     shared->surface_allocation_pool = dfb_surface_allocation_pool_create( core->world );
     shared->surface_buffer_pool = dfb_surface_buffer_pool_create( core->world );
     shared->surface_client_pool = dfb_surface_client_pool_create( core->world );
     shared->window_pool         = dfb_window_pool_create( core->world );

     TaskManager_Initialise();

     for (i=0; i<D_ARRAY_SIZE(core_parts); i++) {
          if ((ret = dfb_core_part_initialize( core, core_parts[i] )))
               return ret;
     }

     register_genefx();

     if (dfb_config->resource_manager) {
          DirectInterfaceFuncs *funcs;

          ret = DirectGetInterface( &funcs, "ICoreResourceManager", dfb_config->resource_manager, NULL, NULL );
          if (ret == DFB_OK) {
               void *ptr;

               ret = funcs->Allocate( &ptr );
               if (ret == DFB_OK) {
                    ret = funcs->Construct( ptr, core );
                    if (ret == DFB_OK) {
                         D_INFO( "Core/Resource: Using resource manager '%s'\n", dfb_config->resource_manager );

                         core->resource.manager = ptr;
                    }
                    else
                         D_DERROR( ret, "Core/Resource: Failed to construct manager '%s'!\n", dfb_config->resource_manager );
               }
               else
                    D_DERROR( ret, "Core/Resource: Failed to allocate manager '%s'!\n", dfb_config->resource_manager );
          }
          else
               D_DERROR( ret, "Core/Resource: Failed to load manager '%s'!\n", dfb_config->resource_manager );
     }

     return DFB_OK;
}

static int
dfb_core_leave( CoreDFB *core, bool emergency )
{
     int i;

     D_MAGIC_ASSERT( core, CoreDFB );

     for (i=D_ARRAY_SIZE(core_parts)-1; i>=0; i--)
          dfb_core_part_leave( core, core_parts[i], emergency );

     CoreSlave_Deinit_Dispatch( &core->slave_call );

     // FIXME: cleanup items
     direct_hash_destroy( core->resource.identities );

     direct_mutex_deinit( &core->memory_permissions_lock );

     return DFB_OK;
}

static int
dfb_core_join( CoreDFB *core )
{
     int i;

     D_MAGIC_ASSERT( core, CoreDFB );

     direct_hash_create( 23, &core->resource.identities );

     direct_mutex_init( &core->memory_permissions_lock );

     CoreSlave_Init_Dispatch( core, core, &core->slave_call );

     if (fusion_config->secure_fusion)
          CoreDFB_Register( core, core->slave_call.call_id );

     for (i=0; i<D_ARRAY_SIZE(core_parts); i++) {
          DFBResult ret;

          if ((ret = dfb_core_part_join( core, core_parts[i] ))) {
               dfb_core_leave( core, true );
               return ret;
          }
     }

     return DFB_OK;
}

/******************************************************************************/

static void
dfb_core_leave_callback( FusionWorld *world,
                         FusionID     fusion_id,
                         void        *ctx )
{
     Core_Resource_DisposeIdentity( fusion_id );
}

static int
dfb_core_arena_initialize( void *ctx )
{
     DFBResult            ret;
     CoreDFB             *core = ctx;
     CoreDFBShared       *shared;
     FusionSHMPoolShared *pool;

     D_MAGIC_ASSERT( core, CoreDFB );

     D_DEBUG_AT( DirectFB_Core, "Initializing...\n" );

     /* Create the shared memory pool first! */
     ret = fusion_shm_pool_create( core->world, "DirectFB Main Pool", 0x400000,
                                   fusion_config->debugshm, &pool );
     if (ret)
          return ret;

     /* Allocate shared structure in the new pool. */
     shared = SHCALLOC( pool, 1, sizeof(CoreDFBShared) );
     if (!shared) {
          fusion_shm_pool_destroy( core->world, pool );
          return D_OOSHM();
     }

     core->shared = shared;

     shared->shmpool = pool;
     shared->secure  = fusion_config->secure_fusion;

     ret = fusion_hash_create( pool, HASH_STRING, HASH_PTR, 7, &shared->field_hash );
     if (ret) {
          SHFREE( pool, shared );
          fusion_shm_pool_destroy( core->world, pool );
          return ret;
     }

     D_MAGIC_SET( shared, CoreDFBShared );

     CoreDFB_Init_Dispatch( core, core, &shared->call );

     fusion_call_add_permissions( &shared->call, 0, FUSION_CALL_PERMIT_EXECUTE );

     fusion_world_set_leave_callback( core->world, dfb_core_leave_callback, NULL );

     /* Register shared data. */
     fusion_world_set_root( core->world, shared );

     D_MAGIC_ASSERT( core, CoreDFB );

     /* Initialize. */
     ret = CoreDFB_Initialize( core );
     if (ret) {
          dfb_core_arena_shutdown( core, true );
          return ret;
     }

     return DFB_OK;
}

static int
dfb_core_arena_shutdown( void *ctx,
                         bool  emergency)
{
     DFBResult            ret;
     CoreDFB             *core = ctx;
     CoreDFBShared       *shared;
     FusionSHMPoolShared *pool;

     D_MAGIC_ASSERT( core, CoreDFB );

     shared = core->shared;

     D_MAGIC_ASSERT( shared, CoreDFBShared );

     pool = shared->shmpool;

     D_DEBUG_AT( DirectFB_Core, "Shutting down...\n" );

     if (!dfb_core_is_master( core )) {
          D_WARN( "refusing shutdown in slave" );
          return dfb_core_leave( core, emergency );
     }

     /* Shutdown. */
     ret = dfb_core_shutdown( core, emergency );

     fusion_hash_destroy( shared->field_hash );

     D_MAGIC_CLEAR( shared );

     SHFREE( pool, shared );

     fusion_shm_pool_destroy( core->world, pool );

     return ret;
}

static int
dfb_core_arena_join( void *ctx )
{
     DFBResult  ret;
     CoreDFB   *core = ctx;

     D_MAGIC_ASSERT( core, CoreDFB );

     D_DEBUG_AT( DirectFB_Core, "Joining...\n" );

     /* Get shared data. */
     core->shared = fusion_world_get_root( core->world );

     if (fusion_config->secure_fusion != core->shared->secure) {
          D_ERROR( "DirectFB/Core: Local secure-fusion config (%d) does not match with running session (%d)!\n",
                   fusion_config->secure_fusion, core->shared->secure );

          return DFB_UNSUPPORTED;
     }

     /* Join. */
     ret = dfb_core_join( core );
     if (ret)
          return ret;

     return DFB_OK;
}

static int
dfb_core_arena_leave( void *ctx,
                      bool  emergency)
{
     DFBResult  ret;
     CoreDFB   *core = ctx;

     D_MAGIC_ASSERT( core, CoreDFB );

     D_DEBUG_AT( DirectFB_Core, "Leaving...\n" );

     /* Leave. */
     ret = dfb_core_leave( core, emergency );
     if (ret)
          return ret;

     return DFB_OK;
}

/*********************************************************************************************************************/

static DirectTLS core_tls_key;

static void
core_tls_destroy( void *arg )
{
     CoreTLS *core_tls = arg;

     D_MAGIC_ASSERT( core_tls, CoreTLS );

     D_MAGIC_CLEAR( core_tls );

     D_FREE( core_tls );
}

void
Core_TLS__init( void )
{
     direct_tls_register( &core_tls_key, core_tls_destroy );
}

void
Core_TLS__deinit( void )
{
     direct_tls_unregister( &core_tls_key );
}


CoreTLS *
Core_GetTLS()
{
     CoreTLS *core_tls;

     core_tls = direct_tls_get( core_tls_key );
     if (!core_tls) {
          core_tls = D_CALLOC( 1, sizeof(CoreTLS) );
          if (!core_tls) {
               D_OOM();
               return NULL;
          }

          D_MAGIC_SET( core_tls, CoreTLS );

          direct_tls_set( core_tls_key, core_tls );
     }

     D_MAGIC_ASSERT( core_tls, CoreTLS );

     return core_tls;
}

void
Core_PushIdentity( FusionID caller )
{
     CoreTLS *core_tls = Core_GetTLS();

     if (core_tls) {
          core_tls->identity_count++;

          if (core_tls->identity_count <= CORE_TLS_IDENTITY_STACK_MAX)
               core_tls->identity[core_tls->identity_count-1] = caller ? caller : core_dfb->fusion_id;
          else
               D_WARN( "identity stack overflow" );
     }
     else
          D_WARN( "TLS error" );
}

void
Core_PopIdentity()
{
     CoreTLS *core_tls = Core_GetTLS();

     if (core_tls) {
          D_ASSERT( core_tls->identity_count > 0 );

          if (core_tls->identity_count > 0)
               core_tls->identity_count--;
          else
               D_BUG( "no identity" );
     }
     else
          D_WARN( "TLS error" );
}

FusionID
Core_GetIdentity()
{
     CoreTLS *core_tls = Core_GetTLS();

     if (core_tls) {
          if (core_tls->identity_count == 0) {
               D_ASSERT( core_dfb != NULL );

               D_ASSUME( core_dfb->fusion_id != 0 );

               return core_dfb->fusion_id;
          }

          if (core_tls->identity_count <= CORE_TLS_IDENTITY_STACK_MAX)
               return core_tls->identity[core_tls->identity_count-1];

          D_WARN( "wrong identity due to overflow" );

          return core_tls->identity[CORE_TLS_IDENTITY_STACK_MAX-1];
     }

     D_WARN( "TLS error" );

     return 0;
}

#if FUSION_BUILD_MULTI
void
Core_PushCalling( void )
{
     CoreTLS *core_tls = Core_GetTLS();

     if (core_tls)
          core_tls->calling++;
     else
          D_WARN( "TLS error" );
}

void
Core_PopCalling( void )
{
     CoreTLS *core_tls = Core_GetTLS();

     if (core_tls) {
          if (core_tls->calling == 0) {
               D_BUG( "Pop counter underrun" );
               return;
          }

          core_tls->calling--;
     }
     else
          D_WARN( "TLS error" );
}

int
Core_GetCalling( void )
{
     CoreTLS *core_tls = Core_GetTLS();

     if (core_tls)
          return core_tls->calling;

     D_WARN( "TLS error" );

     return 0;
}
#endif


D_DEBUG_DOMAIN( Core_Resource, "Core/Resource", "Core Resource" );

DFBResult
Core_Resource_CheckSurface( const CoreSurfaceConfig *config,
                            CoreSurfaceTypeFlags     type,
                            u64                      resource_id,
                            CorePalette             *palette )
{
     ICoreResourceClient *client;

     D_DEBUG_AT( Core_Resource, "%s( %dx%d, %s, type %d, resource id "_ZU" ) <- identity %lu\n", __FUNCTION__, config->size.w, config->size.h,
                 dfb_pixelformat_name( config->format ), type, (size_t)resource_id, Core_GetIdentity() );

     if (Core_GetIdentity() == core_dfb->fusion_id)
          return DFB_OK;

     if (core_dfb->resource.manager) {
          client = Core_Resource_GetClient( Core_GetIdentity() );
          if (!client)
               return DFB_DEAD;

          return client->CheckSurface( client, config, type, resource_id, palette );
     }

     return DFB_OK;
}

DFBResult
Core_Resource_CheckSurfaceUpdate( CoreSurface             *surface,
                                  const CoreSurfaceConfig *config )
{
     ICoreResourceClient *client;

     D_DEBUG_AT( Core_Resource, "%s( %dx%d, %s, type %d, resource id "_ZU" ) <- identity %lu\n", __FUNCTION__, config->size.w, config->size.h,
                 dfb_pixelformat_name( config->format ), surface->type, (size_t)surface->resource_id, surface->object.identity );

     if (!surface->object.identity || surface->object.identity == core_dfb->fusion_id)
          return DFB_OK;

     if (core_dfb->resource.manager) {
          client = Core_Resource_GetClient( surface->object.identity );
          if (!client)
               return DFB_DEAD;

          return client->CheckSurfaceUpdate( client, surface, config );
     }

     return DFB_OK;
}

DFBResult
Core_Resource_AddSurface( CoreSurface *surface )
{
     ICoreResourceClient *client;

     D_DEBUG_AT( Core_Resource, "%s( %dx%d, %s, type %d, resource id "_ZU" ) <- identity %lu\n", __FUNCTION__, surface->config.size.w, surface->config.size.h,
                 dfb_pixelformat_name( surface->config.format ), surface->type, (size_t)surface->resource_id, Core_GetIdentity() );

     if (!surface->object.identity || surface->object.identity == core_dfb->fusion_id)
          return DFB_OK;

     if (core_dfb->resource.manager) {
          client = Core_Resource_GetClient( surface->object.identity );
          if (!client)
               return DFB_DEAD;

          return client->AddSurface( client, surface );
     }

     return DFB_OK;
}

DFBResult
Core_Resource_RemoveSurface( CoreSurface *surface )
{
     ICoreResourceClient *client;

     D_DEBUG_AT( Core_Resource, "%s( %dx%d, %s, type %d, resource id "_ZU" ) <- identity %lu\n", __FUNCTION__, surface->config.size.w, surface->config.size.h,
                 dfb_pixelformat_name( surface->config.format ), surface->type, (size_t)surface->resource_id, surface->object.identity );

     if (!surface->object.identity || surface->object.identity == core_dfb->fusion_id)
          return DFB_OK;

     if (core_dfb->resource.manager) {
          client = Core_Resource_GetClient( surface->object.identity );
          if (!client)
               return DFB_DEAD;

          return client->RemoveSurface( client, surface );
     }

     return DFB_OK;
}

DFBResult
Core_Resource_UpdateSurface( CoreSurface             *surface,
                             const CoreSurfaceConfig *config )
{
     ICoreResourceClient *client;

     D_DEBUG_AT( Core_Resource, "%s( %dx%d, %s, type %d, resource id "_ZU" ) <- identity %lu\n", __FUNCTION__, config->size.w, config->size.h,
                 dfb_pixelformat_name( config->format ), surface->type, (size_t)surface->resource_id, surface->object.identity );

     if (!surface->object.identity || surface->object.identity == core_dfb->fusion_id)
          return DFB_OK;

     if (core_dfb->resource.manager) {
          client = Core_Resource_GetClient( surface->object.identity );
          if (!client)
               return DFB_DEAD;

          return client->UpdateSurface( client, surface, config );
     }

     return DFB_OK;
}


struct __Core__CoreResourceCleanup {
     DirectLink                    link;

     int                           magic;

     FusionID                      fusion_id;

     CoreResourceCleanupCallback   callback;
     void                         *ctx;
     void                         *ctx2;
};

typedef struct {
     ICoreResourceClient *client;

     CoreSlave            slave;

     DirectLink          *cleanups;
} ResourceIdentity;

DFBResult
Core_Resource_AddIdentity( FusionID fusion_id,
                           u32      slave_call )
{
     DFBResult         ret;
     ResourceIdentity *identity;
     char              buf[512] = { 0 };
     size_t            len;
     FusionID          call_owner;

     D_DEBUG_AT( Core_Resource, "%s( %lu )\n", __FUNCTION__, fusion_id );

     fusion_get_fusionee_path( core_dfb->world, fusion_id, buf, 512, &len );

     D_INFO( "Core/Resource: Adding ID %lu - '%s'\n", fusion_id, buf );

     identity = direct_hash_lookup( core_dfb->resource.identities, fusion_id );
     if (identity) {
          D_BUG( "alredy registered" );
          return DFB_BUSY;
     }

     identity = D_CALLOC( 1, sizeof(ResourceIdentity) );
     if (!identity)
          return D_OOM();

     fusion_call_init_from( &identity->slave.call, slave_call, dfb_core_world(core_dfb) );

     ret = fusion_call_get_owner( &identity->slave.call, &call_owner );
     if (ret) {
          D_FREE( identity );
          return ret;
     }

     if (call_owner != fusion_id) {
          D_ERROR( "Core/Resource: Slave call owner (%lu) does not match new identity (%lu)!\n", call_owner, fusion_id );
          D_FREE( identity );
          return ret;
     }

     if (core_dfb->resource.manager) {
          ret = core_dfb->resource.manager->CreateClient( core_dfb->resource.manager, fusion_id, &identity->client );
          if (ret) {
               D_DERROR( ret, "Core/Resource: ICoreResourceManager::CreateClient() failed!\n" );
               D_FREE( identity );
               return ret;
          }
     }

     ret = direct_hash_insert( core_dfb->resource.identities, fusion_id, identity );
     if (ret) {
          D_DERROR( ret, "Core/Resource: Could not insert identity into hash table!\n" );

          if (identity->client)
               identity->client->Release( identity->client );

          D_FREE( identity );
     }

     return ret;
}

void
Core_Resource_DisposeIdentity( FusionID fusion_id )
{
     ResourceIdentity *identity;

     D_DEBUG_AT( Core_Resource, "%s( %lu )\n", __FUNCTION__, fusion_id );

     identity = direct_hash_lookup( core_dfb->resource.identities, fusion_id );
     if (identity) {
          CoreResourceCleanup *cleanup, *next;

          direct_list_foreach_safe (cleanup, next, identity->cleanups) {
               D_MAGIC_ASSERT( cleanup, CoreResourceCleanup );

               D_DEBUG_AT( Core_Resource, "  -> running cleanup callback %p\n", cleanup->callback );
               if (direct_log_domain_check( &Core_Resource )) // avoid call to direct_trace_lookup_symbol_at
                    D_DEBUG_AT( Core_Resource, "  -> '%s'\n", direct_trace_lookup_symbol_at( cleanup->callback ) );

               cleanup->callback( cleanup->ctx, cleanup->ctx2 );

               D_MAGIC_CLEAR( cleanup );

               D_FREE( cleanup );
          }

          if (identity->client)
               identity->client->Release( identity->client );

          direct_hash_remove( core_dfb->resource.identities, fusion_id );

          D_FREE( identity );
     }
}


ICoreResourceClient *
Core_Resource_GetClient( FusionID fusion_id )
{
     ResourceIdentity *identity;

     D_DEBUG_AT( Core_Resource, "%s( %lu )\n", __FUNCTION__, fusion_id );

     identity = direct_hash_lookup( core_dfb->resource.identities, fusion_id );
     if (identity)
          return identity->client;

     return NULL;
}

CoreSlave *
Core_Resource_GetSlave( FusionID fusion_id )
{
     ResourceIdentity *identity;

     D_DEBUG_AT( Core_Resource, "%s( %lu )\n", __FUNCTION__, fusion_id );

     identity = direct_hash_lookup( core_dfb->resource.identities, fusion_id );
     if (identity)
          return &identity->slave;

     return NULL;
}

DFBResult
Core_Resource_AddCleanup( FusionID                      fusion_id,
                          CoreResourceCleanupCallback   callback,
                          void                         *ctx,
                          void                         *ctx2,
                          CoreResourceCleanup         **ret_cleanup )
{
     ResourceIdentity    *identity;
     CoreResourceCleanup *cleanup;

     D_DEBUG_AT( Core_Resource, "%s( %lu, %p )\n", __FUNCTION__, fusion_id, callback );

     D_ASSERT( fusion_id != 0 );
     D_ASSERT( callback != NULL );

     identity = direct_hash_lookup( core_dfb->resource.identities, fusion_id );
     if (!identity)
          return DFB_DEAD;

     cleanup = D_CALLOC( 1, sizeof(CoreResourceCleanup) );
     if (!cleanup)
          return D_OOM();

     cleanup->fusion_id = fusion_id;
     cleanup->callback  = callback;
     cleanup->ctx       = ctx;
     cleanup->ctx2      = ctx2;

     D_MAGIC_SET( cleanup, CoreResourceCleanup );

     direct_list_append( &identity->cleanups, &cleanup->link );

     *ret_cleanup = cleanup;

     return DFB_OK;
}

DFBResult
Core_Resource_DisposeCleanup( CoreResourceCleanup *cleanup )
{
     ResourceIdentity *identity;

     D_DEBUG_AT( Core_Resource, "%s( %p )\n", __FUNCTION__, cleanup );

     D_MAGIC_ASSERT( cleanup, CoreResourceCleanup );

     identity = direct_hash_lookup( core_dfb->resource.identities, cleanup->fusion_id );
     if (!identity)
          return DFB_DEAD;

     direct_list_remove( &identity->cleanups, &cleanup->link );

     D_MAGIC_CLEAR( cleanup );

     D_FREE( cleanup );

     return DFB_OK;
}

