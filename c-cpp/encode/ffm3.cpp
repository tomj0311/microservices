#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>
#include <stdlib.h>


AVCodec* codec;
AVCodecContext* c = NULL;
uint8_t* outbuf;
int i, out_size, outbuf_size;

int main(){

    avcodec_register_all();                                 // mandatory to register ffmpeg functions

    printf("Video encoding\n");

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);            // finding the H264 encoder
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    else printf("H264 codec found\n");

    c = avcodec_alloc_context3(codec);
    c->bit_rate = 400000;
    c->width = 1280;                                        // resolution must be a multiple of two (1280x720),(1900x1080),(720x480)
    c->height = 720;
    c->time_base.num = 1;                                   // framerate numerator
    c->time_base.den = 25;                                  // framerate denominator
    c->gop_size = 10;                                       // emit one intra frame every ten frames
    c->max_b_frames = 1;                                    // maximum number of b-frames between non b-frames
    c->keyint_min = 1;                                      // minimum GOP size
    c->i_quant_factor = (float)0.71;                        // qscale factor between P and I frames
    c->b_frame_strategy = 20;                               ///// find out exactly what this does
    c->qcompress = (float)0.6;                              ///// find out exactly what this does
    c->qmin = 20;                                           // minimum quantizer
    c->qmax = 51;                                           // maximum quantizer
    c->max_qdiff = 4;                                       // maximum quantizer difference between frames
    c->refs = 4;                                            // number of reference frames
    c->trellis = 1;                                         // trellis RD Quantization
    c->pix_fmt = AV_PIX_FMT_YUV420P;                        // universal pixel format for video encoding
    c->codec_id = AV_CODEC_ID_H264;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    if (avcodec_open2(c, codec,NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");          // opening the codec
        exit(1);
    }
    else printf("H264 codec opened\n");

    outbuf_size = 100000 + c->width*c->height*(32>>3);      // allocate output buffer
    outbuf = static_cast<uint8_t *>(malloc(outbuf_size));
    printf("Setting buffer size to: %d\n",outbuf_size);

    FILE* f = fopen("example.mpg","wb");                    // opening video file for writing
    if(!f) printf("x  -  Cannot open video file for writing\n");
    else printf("Opened video file for writing\n");

    // encode video
    for(i=0; i<10*100; i++) {
        fflush(stdout);

        screenCap();                                                                                                // taking screenshot

        int nbytes = avpicture_get_size(PIX_FMT_YUV420P, c->width, c->height);                                      // allocating outbuffer
        uint8_t* outbuffer = (uint8_t*)av_malloc(nbytes*sizeof(uint8_t));

        AVFrame* inpic = avcodec_alloc_frame();                                                                     // mandatory frame allocation
        AVFrame* outpic = avcodec_alloc_frame();

        outpic->pts = (int64_t)((float)i * (1000.0/((float)(c->time_base.den))) * 90);                              // setting frame pts
        avpicture_fill((AVPicture*)inpic, (uint8_t*)pPixels, PIX_FMT_RGB32, c->width, c->height);                   // fill image with input screenshot
        avpicture_fill((AVPicture*)outpic, outbuffer, PIX_FMT_YUV420P, c->width, c->height);                        // clear output picture for buffer copy
        av_image_alloc(outpic->data, outpic->linesize, c->width, c->height, c->pix_fmt, 1); 

        inpic->data[0] += inpic->linesize[0]*(screenHeight-1);                                                      // flipping frame
        inpic->linesize[0] = -inpic->linesize[0];                                                                   // flipping frame

        struct SwsContext* fooContext = sws_getContext(screenWidth, screenHeight, PIX_FMT_RGB32, c->width, c->height, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
        sws_scale(fooContext, inpic->data, inpic->linesize, 0, c->height, outpic->data, outpic->linesize);          // converting frame size and format

        out_size = avcodec_encode_video(c, outbuf, outbuf_size, outpic);                                            // encoding video
        printf("Encoding frame %3d (size=%5d)\n", i, out_size);
        fwrite(outbuf, 1, out_size, f);

        delete [] pPixels;                                                                                          // freeing memory
        av_free(outbuffer);     
        av_free(inpic);
        av_free(outpic);
    }

    for(; out_size; i++) {
        fflush(stdout);

        out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);                                              // encode the delayed frames
        printf("Writing frame %3d (size=%5d)\n", i, out_size);
        fwrite(outbuf, 1, out_size, f);
    }

    outbuf[0] = 0x00;
    outbuf[1] = 0x00;                                                                                               // add sequence end code to have a real mpeg file
    outbuf[2] = 0x01;
    outbuf[3] = 0xb7;
    fwrite(outbuf, 1, 4, f);
    fclose(f);

    avcodec_close(c);                                                                                               // freeing memory
    free(outbuf);
    av_free(c);
    printf("Closed codec and Freed\n");

}