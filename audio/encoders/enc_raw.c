#include "audio/encoders.h"
#include "filters/audio/internal.h"

#include <assert.h>

typedef struct enc_raw_t {
    audio_info_t *info;
    hnd_t filter_chain;
    int64_t last_sample;
} enc_raw_t;

static hnd_t init( hnd_t filter_chain, const char *opts[] )
{
    assert( filter_chain );
    enc_raw_t *h = calloc( 1, sizeof( enc_raw_t ) );
    audio_hnd_t *chain = h->filter_chain = filter_chain;
    h->info = malloc( sizeof( audio_info_t ) );
    memcpy( h->info, af_get_last_filter(chain)->info, sizeof( audio_info_t ) );
    
    return h;
}

static audio_samples_t *get_next_packet( hnd_t handle )
{
    enc_raw_t *h = handle;
    
    audio_samples_t *smp = malloc( sizeof( audio_samples_t ) );
    int res = af_get_samples( smp, h->filter_chain, h->last_sample, h->last_sample + h->info->framelen );
    if( res == AUDIO_ERROR ) {
        free( smp );
        return NULL;
    }
    h->last_sample += h->info->framelen;
    
    return smp;
}

static void free_packet( hnd_t handle, audio_samples_t *packet )
{
    af_free_samples( packet );
    free( packet );
}

static void close( hnd_t handle )
{
    enc_raw_t *h = handle;
    
    free( h->info );
    free( h );
}

const audio_encoder_t aenc_raw = {
    .init = init,
    .get_next_packet = get_next_packet,
    .free_packet = free_packet,
    .close = close
};

