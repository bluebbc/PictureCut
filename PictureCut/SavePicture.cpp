#include "SavePicture.h"

int MSavePicture(AVFrame *pFrameYUV, char *out_file, int width, int height)
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	int size;

	pFormatCtx = avformat_alloc_context();
	//�¸�ʽ����MJPEG����
	fmt = av_guess_format("mjpeg", NULL, NULL);
	pFormatCtx->oformat = fmt;
	//ע�⣺���·��
	if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0)
	{
		printf("����ļ���ʧ��");
		return -1;
	}

	video_st = avformat_new_stream(pFormatCtx, 0);
	if (video_st==NULL)
	{
		return -1;
	}
	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;

	pCodecCtx->width = width;  
	pCodecCtx->height = height;

	pCodecCtx->time_base.num = 1;  
	pCodecCtx->time_base.den = 25;   
	//�����ʽ��Ϣ
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec)
	{
		printf("û���ҵ����ʵı�������");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
	{
		printf("��������ʧ�ܣ�");
		return -1;
	}

	size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//д�ļ�ͷ
	avformat_write_header(pFormatCtx,NULL);

	AVPacket pkt;
	int y_size = pCodecCtx->width * pCodecCtx->height;
	av_new_packet(&pkt,y_size*3);

	int got_picture=0;
	//����
	int ret = avcodec_encode_video2(pCodecCtx, &pkt,pFrameYUV, &got_picture);
	if(ret < 0)
	{
		printf("�������\n");
		return -1;
	}
	if (got_picture==1)
	{
		pkt.stream_index = video_st->index;
		ret = av_write_frame(pFormatCtx, &pkt);
	}

	av_free_packet(&pkt);
	//д�ļ�β
	av_write_trailer(pFormatCtx);

	if (video_st)
	{
		avcodec_close(video_st->codec);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	return 0;
}
