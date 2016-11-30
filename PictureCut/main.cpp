
#include "SavePicture.h"
#include "getopt.h"
#include <vector>
#include <string>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h> 
#endif

struct Config{
	Config():_bDelJpg(false),_num(0){}
	~Config(){}
	void Init(int alltime, int framerate)
	{
		int interval = alltime / (_num+1);
		if(interval<=0)
			interval=1;

		for(int i=1;i<=_num;i++){
			_vecTime.push_back(i*interval);
		}
	}

	void MakeFileName()
	{
		 std::string::size_type found = _inputFile.find_last_of("/\\");
		 _inputFileName = _inputFile.substr(found+1);
	}

	int _num;
	std::vector<int> _vecTime;

	std::string _inputFile;
	std::string _inputFileName;
	std::string _outputDir;
	bool _bDelJpg;
};

int main(int argc, char* argv[])
{
	Config cfg;

	int ch;  
	opterr = 0;  
	while ((ch = getopt(argc,argv,"i:n:d:hcv"))!=-1)
	{  
		switch(ch)
		{
		case 'i':  //输入文件
			cfg._inputFile = optarg;
			cfg.MakeFileName();
			break;
		case 'n':  //图片张数
			cfg._num = atoi(optarg);
			break;
		case 'd':
			cfg._outputDir = optarg;
			break;
		case 'c':
			cfg._bDelJpg = true;
			break;
		case 'h':
			printf("Example : PictrueCut -i video.mp4  -n 5 -d /home/name/testroom\n");
		//	printf("Note : -c can delete *.jpg\n");
			return 0;
			break;
		case 'v':
			printf("v0.1\n");
			return 0;
			break;

		default:  
			printf("other option :%c\n",ch);
		}
	}

	if(cfg._outputDir.size()==0 || cfg._inputFile.size()==0 || cfg._num==0)
	{
		printf("input param error\n");
		return -2;
	}

	if(cfg._bDelJpg == true){
#ifdef WIN32
#else
#endif
	}

#ifdef WIN32
#else
	{
		char str[256]={0};
		sprintf(str, "%s/snapshot", cfg._outputDir.c_str());
		mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
#endif

	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if(avformat_open_input(&pFormatCtx,cfg._inputFile.c_str(),NULL,NULL)!=0){
		printf("Couldn't open input stream.（无法打开输入流）\n");
		return -1;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.（无法获取流信息）\n");
		return -1;
	}
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			videoindex=i;
			break;
		}
	}

	if(videoindex==-1)
	{
		printf("Didn't find a video stream.（没有找到视频流）\n");
		return -1;
	}
	pCodecCtx=pFormatCtx->streams[videoindex]->codec;
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL)
	{
		printf("Codec not found.（没有找到解码器）\n");
		return -1;
	}
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
	{
		printf("Could not open codec.（无法打开解码器）\n");
		return -1;
	}
	AVFrame	*pFrame;
	pFrame=av_frame_alloc();

	int ret, got_picture;
	int y_size = pCodecCtx->width * pCodecCtx->height;

	AVPacket *packet=(AVPacket *)av_malloc(sizeof(AVPacket));
	av_new_packet(packet, y_size);

	//输出一下信息-----------------------------
	//av_dump_format(pFormatCtx,0,cfg._inputFile.c_str(),0);

	float framerate_temp = (pFormatCtx->streams[videoindex]->r_frame_rate.num)/(pFormatCtx->streams[videoindex]->r_frame_rate.den);
	int tns  = (pFormatCtx->duration)/1000000;
	cfg.Init(tns, framerate_temp);

	for(int i=0;i<cfg._num;i++){
		int64_t timestamp = cfg._vecTime[i];
		printf("jpg %d time: %d\n", i, cfg._vecTime[i]);
		timestamp *= AV_TIME_BASE;
		int ret = av_seek_frame(pFormatCtx,  -1, timestamp, AVSEEK_FLAG_BACKWARD);
		if(ret>=0){
			while(av_read_frame(pFormatCtx, packet)>=0)
			{
				if(packet->stream_index==videoindex)
				{
					//if(packet->flags==1 && (packet->data[4]&0x1f) == 5)
					//if(packet->flags==1)
					{
						ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
						if(ret < 0)
						{
							printf("Decode Error.（解码错误）\n");
							return -1;
						}

						if(got_picture)
						{
							char str[256]={0};
							sprintf(str, "%s/snapshot/%s-%d.jpg", cfg._outputDir.c_str(), cfg._inputFileName.c_str(), i);
							MSavePicture(pFrame, str, pCodecCtx->width, pCodecCtx->height);

							break;
						}
					}
				}
				av_free_packet(packet);
			}
		}
	}

	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}
