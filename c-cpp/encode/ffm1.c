#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE *pFile;
    char szFilename[32];
    int  y;
  
    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        printf("Could not find file\n");
        return;
  
    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
    // Close file
    fclose(pFile);
}

int main(int agc, char * agrv[])
{
    AVFormatContext         *pFormatCtx = NULL;
    int                     i, videoStream;
    AVCodecContext          *pCodecCtxOrig = NULL;
    AVCodecContext          *pCodecCtx = NULL;
    AVCodec                 *pCodec = NULL;
    AVFrame                 *pFrame = NULL;
    AVFrame                 *pFrameRGB = NULL;
    AVPacket                packet;
    int                     frameFinished, numbytes;
    uint8_t                 *buffer = NULL;
    struct SwsContext       *sws_ctx = NULL;    

    av_register_all();

    char * fileName = "/users/tomj0/downloads/1280.avi";
    
    //open video file C:\Users\tomj0\Downloads\1280.avi
    if (avformat_open_input(&pFormatCtx, fileName, NULL, NULL) !=0){
        printf("Cannot open file\n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0){
        printf("Couldn't find stream information\n");
        return -1;
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, fileName, 0);    

    //Find the first video stream
    videoStream = -1;
    for (i=0; i <= pFormatCtx->nb_streams; i++){
        if (pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1){
        printf("Cannot find video stream\n");
        return -1;
    }

    //Get a pointer to the codec context for the video steram
    pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
    //Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec==NULL){
        printf("Unsupported coded\n");
        return -1;
    }

    //Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) !=0 ){
        printf("Cannot copy context\n");
        return -1;
    }

    //Open codec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) <0){
        printf("Could not open codec\n");
        return -1;
    }

    // Allocate video frame
    pFrame = av_frame_alloc();

    //Allocate an AVFrame structure
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB==NULL){
        return -1;
    }
    
    // Determine required buffer size and allocate buffer
    int numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    
    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
			   pCodecCtx->height,
			   pCodecCtx->pix_fmt,
			   pCodecCtx->width,
			   pCodecCtx->height,
			   AV_PIX_FMT_RGB24,
			   SWS_BILINEAR,
			   NULL,
			   NULL,
			   NULL
	);
    
    // Read frames and save first five frames to disk
    i=0;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            // Did we get a video frame?
            if(frameFinished) {
	            // Convert the image from its native format to RGB
	            sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
		        pFrame->linesize, 0, pCodecCtx->height,
		        pFrameRGB->data, pFrameRGB->linesize);

                //Save the frame
	            if(++i<=5)
	                SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
            }
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }
    
    // Free the RGB image
    av_free(buffer);
    av_frame_free(&pFrameRGB);
  
    // Free the YUV frame
    av_frame_free(&pFrame);
  
    // Close the codecs
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    return 0;
}