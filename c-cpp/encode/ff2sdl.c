#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include <stdio.h>


int main(int argc, char * argv[]){

    AVFormatContext     *pFormatContext = NULL;
    int                 i, videoStream;
    AVCodecContext      *pCodecContextOrig = NULL;
    AVCodecContext      *pCodecContext = NULL;
    AVCodec             *pCodec = NULL;
    AVFrame             *pFrame = NULL;
    AVFrame             *pFrameRGB = NULL;
    AVPacket            packet;
    int                 frameFinished;
    float               aspect_ratio;
    struct SwsContext   *sws_ctx = NULL;

    // SDL_Overlay     *bmp;       //SDL_Overlay is deprecated https://stackoverflow.com/questions/17579286/sdl2-0-alternative-for-sdl-overlay
    SDL_Texture         *texture;
    SDL_Surface         *screen;
    SDL_Rect            rect;
    SDL_Event           event;

    av_register_all();

    char * videoFile = "river_creek_stream_nature_flowing_821.mp4";
    // char * videoFile = "desktop_capture.avi";

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    //Open vide file
    if (avformat_open_input(&pFormatContext, videoFile, NULL, NULL)!=0){
        printf("Cannot open video file\n");
        return -1;
    }

    // Retrive steram information
    if (avformat_find_stream_info(pFormatContext, NULL) <0 ){
        printf("Cannnot open stream information\n");
    }

    // Dump information about file onto standard error
    // av_dump_format(pFormatContext, 0, argv[1], 0);
    for (i=0; i<pFormatContext->nb_streams; i++){
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            pCodecContextOrig = pFormatContext->streams[i]->codec;
            videoStream = i;
            break;
        }
    }
    if (videoStream < 0){
        return -1;
    }

    pCodec = avcodec_find_decoder(pCodecContextOrig->codec_id);
    if (pCodec == NULL){
        printf("unsupported codec\n");
        return -1;
    }

    //copy context
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (avcodec_copy_context(pCodecContext, pCodecContextOrig) != 0){
        printf("Could not copy context\n");
        return -1;
    }

    //open codec
    if (avcodec_open2(pCodecContext, pCodec, NULL) <0 ){
        printf("Could not open codec");
        return -1;
    }

    // Allocate video frame
    pFrame = av_frame_alloc();

    // Allocate an AVFrame structure
    pFrameRGB = av_frame_alloc();
    if(pFrameRGB == NULL)
        return -1;

    uint8_t *buffer = NULL;
    int numbytes;

    numbytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecContext->width, 
                pCodecContext->height);

    buffer = (uint8_t *)av_malloc(numbytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24, 
                pCodecContext->width, pCodecContext->height);
    
    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecContext->width,
        pCodecContext->height,
        pCodecContext->pix_fmt,
        pCodecContext->width,
        pCodecContext->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

    i=0;
    while(av_read_frame(pFormatContext, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecContext, pFrame, &frameFinished, &packet);
            
            // Did we get a video frame?
            if(frameFinished) {
                // Convert the image from its native format to RGB
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                            pFrame->linesize, 0, pCodecContext->height,
                            pFrameRGB->data, pFrameRGB->linesize);
            
                // Save the frame to disk
                // if(++i<=5)
                //      SaveFrame(pFrameRGB, pCodecContext->width, 
                //             pCodecContext->height, i);
            }
        }
            
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    // Free the RGB image
    av_free(buffer);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecContext);
    avcodec_close(pCodecContextOrig);

    // Close the video file
    avformat_close_input(&pFormatContext);

    return 0;
}