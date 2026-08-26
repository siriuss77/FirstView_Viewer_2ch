/**************************************************************************************************
 *
 *      File Name       : StrdParser.cpp
 *      Description     :  I3 UserData
 *
 *      Creator         :   tony ( datech Co., Ltd )
 *      Create Date     :   2019/07/02
 *      Update History  :   
 *
 *************************************************************************************************/
#include "stdafx.h"
#include <string.h>
#include <CLUDE/version.h>
#include <HELPER/CLUtil.H>
#include "BBoxUtil.h"
#include "StrdParser.h"

#define ZONE_STRD_INIT	1
#define ZONE_STRD_FUNC	1
#define ZONE_STRD_ERROR	1

/////////////////////////////////////////////////////////////////////////////

//===========================================================================


typedef unsigned long long       uint64_t;

typedef unsigned int       uint32_t;

typedef unsigned short int uint16_t;

typedef signed   short int int16_t;

typedef signed         int int32_t;

typedef unsigned char      byte_t;

 

inline uint32_t convert_endian_4byte (uint32_t v)

{

        return (((v>>24)&0xff)       )|

               (((v>>16)&0xff) << 8  )|

               (((v>> 8)&0xff) << 16 )|

               (((v    )&0xff) << 24 );

}

 

/////////////////////////////////////////////////////////////////////////////

//===========================================================================

const uint32_t FOURCC_RIFF = 0x46464952;  

const uint32_t FOURCC_AVI  = 0x20495641;  

const uint32_t FOURCC_LIST = 0x5453494C;  

const uint32_t FOURCC_hdrl = 0x6C726468;  

const uint32_t FOURCC_avih = 0x68697661;  

const uint32_t FOURCC_strl = 0x6C727473;  

const uint32_t FOURCC_strh = 0x68727473;  

const uint32_t FOURCC_strf = 0x66727473;  

const uint32_t FOURCC_STRD = 0x64727473;  

const uint32_t FOURCC_vids = 0x73646976;  

const uint32_t FOURCC_auds = 0x73647561;  

const uint32_t FOURCC_INFO = 0x4F464E49;  

const uint32_t FOURCC_ISFT = 0x54465349;  

const uint32_t FOURCC_idx1 = 0x31786469;  

const uint32_t FOURCC_movi = 0x69766F6D;  

const uint32_t FOURCC_JUNK = 0x4B4E554A;  

const uint32_t FOURCC_vprp = 0x70727076;  

const uint32_t FOURCC_PAD  = 0x20444150;  

const uint32_t FOURCC_DIV3 = 861292868;  

const uint32_t FOURCC_DIVX = 1482049860;  

const uint32_t FOURCC_XVID = 1145656920;  

const uint32_t FOURCC_DX50 = 808802372;  

 

#define FOURCC(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))

 

/////////////////////////////////////////////////////////////////////////////

//===========================================================================

typedef struct _riff_header_t

{

        uint32_t riff ;

        uint32_t size ;

        uint32_t type ;

} riff_header_t;

 

typedef struct _list_header_t

{

        uint32_t list   ;

        uint32_t size   ;

        uint32_t fourcc ;

} list_header_t;

 

typedef struct _chunk_data_t

{

        byte_t  	type[4] ; // "head" or "data"

        uint32_t  address_offset;
		 uint32_t  undefined;
		 uint32_t  length;

} chunk_data_t;

typedef struct _move_chunk_data_t

{

        byte_t  	type[4] ; // "xxdc" or "xxwb"

        
		 uint32_t  flags;
		 uint32_t  address_offset;
		 uint32_t  length;

} move_chunk_data_t;

typedef struct _chunk_header_t

{

        uint32_t fourcc ;

       chunk_data_t header;
		
} chunk_header_t;

 

/////////////////////////////////////////////////////////////////////////////

//===========================================================================

typedef struct _MainAVIHeader_t

{

        uint32_t MicroSecPerFrame;   // frame display rate (or 0)

        uint32_t MaxBytesPerSec;     // max. transfer rate

        uint32_t PaddingGranularity; // pad to multiples of this size;

        uint32_t Flags;              // the ever-present flags

        uint32_t TotalFrames;        // # frames in file

        uint32_t InitialFrames;

        uint32_t Streams;

        uint32_t SuggestedBufferSize;

        uint32_t Width;

        uint32_t Height;

        uint32_t Reserved[4];

} MainAVIHeader_t;

 

typedef struct _RECT_t

{  

    int16_t left;  

    int16_t top;   

    int16_t right; 

    int16_t bottom;

} RECT_t;  

 

typedef struct _AVIStreamHeader_t

{              

    uint32_t fccType;

    uint32_t fccHandler;         

    uint32_t Flags;              

    uint16_t Priority;           

    uint16_t Language;           

    uint32_t InitialFrames;      

    uint32_t Scale;              

    uint32_t Rate;               

    uint32_t Start;              

    uint32_t Length;             

    uint32_t SuggestedBufferSize;

    uint32_t Quality;            

    uint32_t SampleSize;         

    //RECT_t   FrameRect;          

} AVIStreamHeader_t;  

 

// strl - strh==vids  strf

typedef struct _BitmapInfoHeader_t

{  

    uint32_t  biSize;  

    int32_t   biWidth;  

    int32_t   biHeight;  

    uint16_t  biPlanes;  

    uint16_t  biBitCount;  

    uint32_t  biCompression;  

    uint32_t  biSizeImage;  

    int32_t   biXPelsPerMeter;  

    int32_t   biYPelsPerMeter;  

    //uint32_t  biClrUsed;  

    //uint32_t  biClrImportant;  

} BitmapInfoHeader_t;  

 

// strl - strh==auds  strf

typedef struct _WaveFormatEx_t

{  

    uint16_t FormatTag;  

    uint16_t Channels;         

    uint32_t SamplesPerSec;   

    uint32_t AvgBytesPerSec;  

    uint16_t BlockAlign;       

    uint16_t BitsPerSample;    

    uint16_t Size;            

} WaveFormatEx_t;



// AVI

typedef struct _avi_info_t

{

        uint32_t HasVideo;

        uint32_t HasAudio;

		uint32_t HasStrd;


 

        uint32_t avi_size;

        uint32_t movi_size;

        

        uint32_t audio_data_size;

        uint32_t audio_data_count;

        uint32_t video_data_size;

        uint32_t video_data_count;

 

        MainAVIHeader_t    Header;

        AVIStreamHeader_t  Video;             // strh

        BitmapInfoHeader_t VideoBitmapInfo;   // strf

        AVIStreamHeader_t  Audio;             // strh

        WaveFormatEx_t     AudioWaveFormatEx; // strf

		chunk_header_t			UserData;				//strd
		

} avi_info_t;

bool security_file_change (const char* filepath, bool avi_to_security)
{
	bool 	security_file = false;
	
		FILE* fp;
		int read_pos = 0;
        fp = fopen (filepath, "rb+");
        if (0==fp)
        {
               return false;
        }
 
        //--------------------------------------------------------------------------

        // RIFF header

        //--------------------------------------------------------------------------
        riff_header_t riff_header;

         // [RIFF] size [AVI ]
        fread(&riff_header, sizeof(riff_header), 1, fp);
		read_pos = sizeof(riff_header);
	   printf ("%c%c%c%c %d %c%c%c%c \r\n",
               (riff_header.riff>> 0)&0xff,
               (riff_header.riff>> 8)&0xff,
               (riff_header.riff>>16)&0xff,
               (riff_header.riff>>24)&0xff,
                riff_header.size ,
               (riff_header.type>> 0)&0xff,
               (riff_header.type>> 8)&0xff,
               (riff_header.type>>16)&0xff,
               (riff_header.type>>24)&0xff
                );

         if (riff_header.riff != 0x46464952) // "RIFF"
        {
               fclose (fp);
               return false;
        }

         if (riff_header.type != 0x20495641) // "AVI "
        {
               fclose (fp);
               return false;
        }

        //--------------------------------------------------------------------------

        // RIFF body

        //--------------------------------------------------------------------------
        int32_t  seek_size;
        uint32_t id;
        int32_t  size;
        uint32_t fourcc;

		 uint32_t movi_offset_address = 0;

        uint32_t data_size;

        MainAVIHeader_t    MainAVIHeader;
        AVIStreamHeader_t  AVIStreamHeader;
        BitmapInfoHeader_t BitmapInfoHeader;
        WaveFormatEx_t     WaveFormatEx;

        uint32_t           strh = 0;
		 byte_t  read_code[32];
		 byte_t  security_code[32];
		 byte_t  scramble_code[32] = { \
		 		0x64, 0x61, 0x74, 0x65, 0x63, 0x68, 0x06, 0x35, \
		 		0xb5, 0x69, 0x5b, 0x47, 0xd8, 0x88, 0xd1, 0x59, \
		 		0x09, 0x34, 0x13, 0x88, 0x34, 0x53, 0x5c, 0x9f, \
		 		0x63, 0xb6, 0x90, 0x73, 0x57, 0xe7, 0x24, 0xeb \
		 };

        while ( !feof(fp) )
        {
               printf ("%8d: ", ftell (fp));

                if (0==fread(&id, 4, 1, fp))
               {
                       break;
               }

               if (0==fread(&size,4, 1, fp))
               {
                       break;
               }

				read_pos += 8;
               printf ("<%c%c%c%c> %d \r\n",
                       (id>> 0)&0xff,
                       (id>> 8)&0xff,
                       (id>>16)&0xff,
                       (id>>24)&0xff,
                       size        );

               if (0==id)
               {
                       break;
               }

               if (0>=size)
               {
                       break;
               }

#if 0
				//idx1
 				if( id == 0x31786469 )
				{
					int idx1ChunkSize = size;
					byte_t* idx1Chunk = (byte_t*)malloc(idx1ChunkSize);
					
					if(idx1Chunk != NULL){
						if (0==fread (idx1Chunk, idx1ChunkSize, 1, fp))
						{
							free(idx1Chunk);
							break;
						}
						read_pos += idx1ChunkSize;

						if(idx1ChunkSize >= sizeof(chunk_data_t))
						{
							move_chunk_data_t *p_chunk_data = (move_chunk_data_t *)idx1Chunk;
							int idx1ChunkDataCount = idx1ChunkSize / sizeof(chunk_data_t);
							
							for(int i = 0; i < idx1ChunkDataCount; i++){
								if((p_chunk_data->type[2] == 'd' && p_chunk_data->type[3] == 'c') || (p_chunk_data->type[2] == 'w' && p_chunk_data->type[3] == 'b')){
										if(-1 == fseek(fp, movi_offset_address + p_chunk_data->address_offset + 8, SEEK_SET)){
									//if(-1 == fseek(m_fp, p_chunk_data->address_offset + 8, SEEK_SET))		// skip 30 33 64 74 31 00 00 00 	| 03dt(size)...
											break;
										}
										
									   data_size = sizeof(read_code);
									   if (0==fread (&read_code, data_size, 1, fp))
	                               {
	                                      break;
	                               }

									  if((avi_to_security && security_file == false) || (avi_to_security == false && security_file)){
											
											for(int32_t j=0; j<sizeof(security_code); j++) {
										        security_code[j] = read_code[j]^scramble_code[j];
										    }
											
											//for(int32_t i=0; i<sizeof(security_code); i++) {
										    //    printf("0x%02x, ", security_code[i]);
										    //    if(((i+1)%8) == 0) printf("\n");
										    //}
											
											if(-1 != fseek(fp, -data_size, SEEK_CUR))
												fwrite((void *)security_code, data_size, 1, fp);
									  }
								}

								p_chunk_data++;
							}
						}

						free(idx1Chunk);
						break;
					}
					else {
						 break;
					}
 				}
              else 			
#endif								
				if (id==0x5453494c)  // "LIST"
               {
                       if (0==fread(&fourcc,  4, 1, fp))
                       {
                              break;
                       }
 						  read_pos += 4;
                       printf ("          %c%c%c%c\r\n",
                              (fourcc>> 0)&0xff,
                              (fourcc>> 8)&0xff,
                              (fourcc>>16)&0xff,
                              (fourcc>>24)&0xff);

 						  //movi
                       if (fourcc == 0x69766F6D)
                       {
                       		  movi_offset_address = read_pos - 4;
                              //movi_size = size;
                       }
#if 0											 
					   else if( fourcc == 0x4F464E49) //INFO
					   {
					   		char data[4] = {'J', 'U', 'N', 'K' };
								
					   		fseek(fp, -12 , SEEK_CUR);
							fwrite((void *)data, 4, 1, fp);
							fseek(fp, 8 , SEEK_CUR);
							
					   		printf ("LIST INFO ==> JUNK\r\n");							
					   }
#endif
               }
               else
               {
                       seek_size = (int32_t) size;
                       data_size = (int32_t) size;

                        // "avih"
                       if (id==0x68697661) 
                       {
                              data_size = sizeof(MainAVIHeader);
                              if (0==fread (&MainAVIHeader, data_size, 1, fp))
                              {
                                      break;
                              }
								  read_pos += data_size;									                         

                              seek_size = ( size!=data_size ) ?  size-data_size : 0;
                       } 

                       // "strh"
                       if (id==0x68727473) 
                       {

                              data_size = sizeof(AVIStreamHeader);
                              if (0==fread (&AVIStreamHeader, data_size, 1, fp))
                              {
                                      break;
                              }
								  
                              printf ("          %c%c%c%c:%c%c%c%c\r\n",
                                      (AVIStreamHeader.fccType   >> 0)&0xff,
                                      (AVIStreamHeader.fccType   >> 8)&0xff,
                                      (AVIStreamHeader.fccType   >>16)&0xff,
                                      (AVIStreamHeader.fccType   >>24)&0xff,
                                      (AVIStreamHeader.fccHandler>> 0)&0xff,
                                      (AVIStreamHeader.fccHandler>> 8)&0xff,
                                      (AVIStreamHeader.fccHandler>>16)&0xff,
                                      (AVIStreamHeader.fccHandler>>24)&0xff); 

                              strh = 0;
								  if(avi_to_security){
									   // "vids"
	                              if (0x73646976==AVIStreamHeader.fccType)
	                              {
	                                      strh = 1;
											   AVIStreamHeader.fccType = 0x78787876; // vxxx
											   AVIStreamHeader.fccHandler = 0x58585848; // HXXX
	                              }									 

	                              // "auds"
	                              if (0x73647561==AVIStreamHeader.fccType)
	                              {
	                                      strh = 2;
												AVIStreamHeader.fccType = 0x78787861; // axxx
												AVIStreamHeader.fccHandler = 0x58585841; //AXXX
	                              }
								  }
								  else {
										//"vxxx"
									  if (0x78787876==AVIStreamHeader.fccType){
											security_file = true;
											strh = 1;
											AVIStreamHeader.fccType = 0x73646976; // vids
											AVIStreamHeader.fccHandler = 0x34363248; // H264
									  }
									   // "axxx"
	                              if (0x78787861==AVIStreamHeader.fccType){
											security_file = true;
											strh = 2;
											AVIStreamHeader.fccType = 0x73647561; // auds
											AVIStreamHeader.fccHandler = 0x0; //
	                              }
								  }

								  if(strh){
									  	fseek(fp, -data_size , SEEK_CUR);
										fwrite((void *)&AVIStreamHeader, data_size, 1, fp);
								  }

								  read_pos += data_size; 
                              seek_size = ( size!=data_size ) ?  size-data_size : 0;
                       } 

                       // "strf"
                       if (id==0x66727473) 
                       {
                              data_size = 0;
 
                              if (1==strh)
                              {
                                      data_size = sizeof(BitmapInfoHeader);
                                      if (0==fread (&BitmapInfoHeader, data_size, 1, fp))
                                      {
                                             break;
                                      }
											if(avi_to_security)
												BitmapInfoHeader.biCompression	= 0x58585848; // HXXX
											else
												BitmapInfoHeader.biCompression	= 0x34363248; // H264

											fseek(fp, -data_size , SEEK_CUR);
											fwrite((void *)&BitmapInfoHeader, data_size, 1, fp);
										
											read_pos += data_size;
                              }

                              if (2==strh)
                              {
                                      data_size = sizeof(WaveFormatEx);
                                      if (0==fread (&WaveFormatEx, data_size, 1, fp))
                                      {
                                             break;
                                      }
											read_pos += data_size;
                              }
 
                              seek_size = ( size!=data_size ) ?  size-data_size : 0;
                       }


 						  // "strd"
                      if ( id == 0x64727473 && size > sizeof(uint32_t))
                      {
                      	uint32_t fourcc ;
							data_size = sizeof(fourcc);
							printf ("          # strd  Chunk Size (%d)\r\n", size);
									
							if (0==fread ((void *)&fourcc, sizeof(fourcc), 1, fp))
							{
								break;
							}
							read_pos += data_size;

							printf ("          # strd  fourcc(%x)\r\n", fourcc);

							 seek_size = ( size!=data_size ) ?  size-data_size : 0;
                      }

                       // "00dc" : Compressed video frame			"00wb" : Audio data
                       if (((id&0xffff0000)==0x63640000) || ((id&0xffff0000)==0x62770000)) 
                       {
#if 0                       		
								  data_size = sizeof(security_code);
								  if (0==fread (&security_code, data_size, 1, fp))
                              {
                                      break;
                              }

								  if((avi_to_security && security_file == false) || (avi_to_security == false && security_file)){
										byte_t z[32];
										
										for(int32_t i=0; i<sizeof(z); i++) {
									        z[i] = security_code[i]^scramble_code[i];
									    }
										
										for(int32_t i=0; i<sizeof(security_code); i++) {
									        printf("0x%02x, ", z[i]);
									        if(((i+1)%8) == 0) printf("\n");
									    }
										
										fseek(fp, -data_size , SEEK_CUR);
										fwrite((void *)&z, data_size, 1, fp);
								  }
										
								  seek_size = ( size!=data_size ) ?  size-data_size : 0;
#endif									
                       }

                       // "00db" : Uncompressed video frame
                       if ((id&0xffff0000)==0x62640000) 
                       {

                       }

                       // "00pc" : Palette change
                       if ((id&0xffff0000)==0x63700000) 
                       {

                       }

 
                       // 
                       if ( 0!=seek_size )
                       {
                              if ( data_size != size )
                              {
                                      printf ("          # data_size(%d) != size(%d)\r\n", data_size, size);
                              }
															
                              if (0!=(seek_size%2))
                              {
                                      seek_size+=(seek_size%2);
                              }

                              if (-1==fseek (fp, seek_size, SEEK_CUR))
                              {
                                      break;
                              }
								  read_pos += seek_size;
                       }
               }
        }

         fclose (fp);
        return true;
}

bool read_avi_header (const char* filepath, avi_info_t* ai, byte_t **ppUserData, int * pUserDataSize)
{
		FILE* fp;
		int read_pos = 0;
        //fp = fopen (filepath, "rb+");
        fp = fopen (filepath, "rb");
        if (0==fp)
        {
               return false;
        }
 
        //--------------------------------------------------------------------------

        // RIFF header

        //--------------------------------------------------------------------------
        riff_header_t riff_header;

         // [RIFF] size [AVI ]
        fread(&riff_header, sizeof(riff_header), 1, fp);
		read_pos = sizeof(riff_header);
	   printf ("%c%c%c%c %d %c%c%c%c \r\n",
               (riff_header.riff>> 0)&0xff,
               (riff_header.riff>> 8)&0xff,
               (riff_header.riff>>16)&0xff,
               (riff_header.riff>>24)&0xff,
                riff_header.size ,
               (riff_header.type>> 0)&0xff,
               (riff_header.type>> 8)&0xff,
               (riff_header.type>>16)&0xff,
               (riff_header.type>>24)&0xff
                );

         if (riff_header.riff != 0x46464952) // "RIFF"
        {
               fclose (fp);
               return false;
        }

         if (riff_header.type != 0x20495641) // "AVI "
        {
               fclose (fp);
               return false;
        }
        ai->avi_size = riff_header.size;

        //--------------------------------------------------------------------------

        // RIFF body

        //--------------------------------------------------------------------------
        int32_t  seek_size;
        uint32_t id;
        int32_t  size;
        uint32_t fourcc;

        uint32_t data_size;

        MainAVIHeader_t    MainAVIHeader;
        AVIStreamHeader_t  AVIStreamHeader;
        BitmapInfoHeader_t BitmapInfoHeader;
        WaveFormatEx_t     WaveFormatEx;

        uint32_t           strh = 0;

        while ( !feof(fp) )
        {
               printf ("%8d: ", ftell (fp));

                if (0==fread(&id, 4, 1, fp))
               {
                       break;
               }

               if (0==fread(&size,4, 1, fp))
               {
                       break;
               }

				read_pos += 8;
               printf ("<%c%c%c%c> %d \r\n",
                       (id>> 0)&0xff,
                       (id>> 8)&0xff,
                       (id>>16)&0xff,
                       (id>>24)&0xff,
                       size        );

               if (0==id)
               {
                       break;
               }

               if (0>=size)
               {
                       break;
               }

#if 0
				//idx1
 				if( id == 0x31786469 )
				{
					char data[4] = {'J', 'U', 'N', 'K' };
								
			   		fseek(fp, -8 , SEEK_CUR);
					fwrite((void *)data, 4, 1, fp);
					fseek(fp, 4 , SEEK_CUR);
					
			   		printf ("idx1 ==> JUNK\r\n");	
 				}
               else 
#endif							 	
				if (id==0x5453494c)  // "LIST"
               {
                       if (0==fread(&fourcc,  4, 1, fp))
                       {
                              break;
                       }
 						  read_pos += 4;
                       printf ("          %c%c%c%c\r\n",
                              (fourcc>> 0)&0xff,
                              (fourcc>> 8)&0xff,
                              (fourcc>>16)&0xff,
                              (fourcc>>24)&0xff);
 
                       if (fourcc == 0x69766F6D)
                       {
                              ai->movi_size = size;
                       }
#if 0											 
					   else if( fourcc == 0x4F464E49) //INFO
					   {
					   		char data[4] = {'J', 'U', 'N', 'K' };
								
					   		fseek(fp, -12 , SEEK_CUR);
							fwrite((void *)data, 4, 1, fp);
							fseek(fp, 8 , SEEK_CUR);
							
					   		printf ("LIST INFO ==> JUNK\r\n");							
					   }
#endif
               }
               else
               {
                       seek_size = (int32_t) size;
                       data_size = (int32_t) size;

                        // "avih"
                       if (id==0x68697661) 
                       {
                              data_size = sizeof(MainAVIHeader);
                              if (0==fread (&MainAVIHeader, data_size, 1, fp))
                              {
                                      break;
                              }
								  read_pos += data_size;									
                              memcpy (&ai->Header, &MainAVIHeader, data_size);                              

                              seek_size = ( size!=data_size ) ?  size-data_size : 0;
                       } 

                       // "strh"
                       if (id==0x68727473) 
                       {

                              data_size = sizeof(AVIStreamHeader);
                              if (0==fread (&AVIStreamHeader, data_size, 1, fp))
                              {
                                      break;
                              }
								  read_pos += data_size; 

                              printf ("          %c%c%c%c:%c%c%c%c\r\n",
                                      (AVIStreamHeader.fccType   >> 0)&0xff,
                                      (AVIStreamHeader.fccType   >> 8)&0xff,
                                      (AVIStreamHeader.fccType   >>16)&0xff,
                                      (AVIStreamHeader.fccType   >>24)&0xff,
                                      (AVIStreamHeader.fccHandler>> 0)&0xff,
                                      (AVIStreamHeader.fccHandler>> 8)&0xff,
                                      (AVIStreamHeader.fccHandler>>16)&0xff,
                                      (AVIStreamHeader.fccHandler>>24)&0xff); 

                              strh = 0;
                              // "vids"
                              if (0x73646976==AVIStreamHeader.fccType)
                              {
                                      strh = 1;
                                      memcpy(&ai->Video, &AVIStreamHeader, data_size);
                                      ai->HasVideo = 1;
                              }

                              // "auds"
                              if (0x73647561==AVIStreamHeader.fccType)
                              {
                                      strh = 2;
                                      memcpy(&ai->Audio, &AVIStreamHeader, data_size);
                                      ai->HasAudio = 1;
                              }
                              seek_size = ( size!=data_size ) ?  size-data_size : 0;
                       } 

                       // "strf"
                       if (id==0x66727473) 
                       {
                              data_size = 0;
 
                              if (1==strh)
                              {
                                      data_size = sizeof(BitmapInfoHeader);
                                      if (0==fread (&BitmapInfoHeader, data_size, 1, fp))
                                      {
                                             break;
                                      }
											read_pos += data_size;
                                      memcpy(&ai->VideoBitmapInfo, &BitmapInfoHeader, data_size);
                              }

                              if (2==strh)
                              {
                                      data_size = sizeof(WaveFormatEx);
                                      if (0==fread (&WaveFormatEx, data_size, 1, fp))
                                      {
                                             break;
                                      }
											read_pos += data_size;
                                      memcpy(&ai->AudioWaveFormatEx, &WaveFormatEx, data_size);
                              }
 
                              seek_size = ( size!=data_size ) ?  size-data_size : 0;
                       }


 						  // "strd"
                      if ( id == 0x64727473 && size > sizeof(uint32_t))
                      {
                      	uint32_t fourcc ;
							data_size = size;
							printf ("          # strd  Chunk Size (%d)\r\n", size);
									
							if (0==fread ((void *)&fourcc, sizeof(fourcc), 1, fp))
							{
								break;
							}
							read_pos += sizeof(fourcc);

							printf ("          # strd  fourcc(%x)\r\n", fourcc);
						
							*pUserDataSize = (size - sizeof(fourcc));
							
                      	*ppUserData = (byte_t*)malloc(*pUserDataSize);
							if(*ppUserData != NULL){
								if (0==fread (*ppUserData, *pUserDataSize, 1, fp))
								{
									break;
								}
								read_pos += *pUserDataSize;
							  	ai->HasStrd = 1;

							 	//free(*ppUserData);
							}

                      }

                       // "00dc" : Compressed video frame
                       if ((id&0xffff0000)==0x63640000) 
                       {
                              ai->video_data_size += size;
                              ai->video_data_count++;
                       }

                       // "00db" : Uncompressed video frame
                       if ((id&0xffff0000)==0x62640000) 
                       {
                              ai->video_data_size += size;
                              ai->video_data_count++;
                       }

                       // "00pc" : Palette change
                       if ((id&0xffff0000)==0x63700000) 
                       {
                              ai->video_data_size += size;
                              ai->video_data_count++;
                       }

                       // "00wb" : Audio data
                       if ((id&0xffff0000)==0x62770000) 
                       {
                              ai->audio_data_size += size;
                              ai->audio_data_count++;
                       }

 
                       // 
                       if ( 0!=seek_size )
                       {
                              if ( data_size != size )
                              {
                                      printf ("          # data_size(%d) != size(%d)\r\n", data_size, size);
                              }
															
                              if (0!=(seek_size%2))
                              {
                                      seek_size+=(seek_size%2);
                              }

                              if (-1==fseek (fp, seek_size, SEEK_CUR))
                              {
                                      break;
                              }
								  read_pos += seek_size;
                       }
               }
        }

         fclose (fp);
        return true;
}

UINT64 get_time_from_gsensor_data(byte_t *pData, int data_size)
{
	UINT64 time = 0;
	/*
	buf[len++] = 'M';
		buf[len++] = (signed char)(pAccel->x / 4);
		buf[len++] = (signed char)(pAccel->y / 4);
		buf[len++] = (signed char)(pAccel->z / 4);
		buf[len++] = (tm_t.tm_year + 1900 )% 100;
		buf[len++] = tm_t.tm_mon + 1;
		buf[len++] = tm_t.tm_mday;
		buf[len++] = tm_t.tm_hour;
		buf[len++] = tm_t.tm_min;
		buf[len++] = tm_t.tm_sec;
		buf[len++] = ((tick / 100) % 10);
		buf[len++] = (tick % 100);
 */
 	if(data_size > 11){

		if(data_size > 16){
			UINT *p_tick = (UINT *)&pData[12];
			time = (UINT64)*p_tick;
 		}
		else {	
			struct tm tmThis;
			
			tmThis.tm_year = 70+((pData[4]+30)%100);
			tmThis.tm_mon = pData[5]-1;
			tmThis.tm_mday = pData[6];
			tmThis.tm_hour = pData[7];
			tmThis.tm_min = pData[8];
			tmThis.tm_sec = pData[9];

			time = mktime(&tmThis) * 1000;
/*
			printf ("%d-%02d-%02d %02d:%02d:%02d\r\n", \
				tmThis.tm_year, \
				tmThis.tm_mon, \
				tmThis.tm_mday, \
				tmThis.tm_hour, \
				tmThis.tm_min, \
				tmThis.tm_sec);
*/				
		}
 	}
	return time;
}

///////////////////////////////////////////
CStrdParser::CStrdParser(void)
{
	mp_UserData = NULL;
	m_UserDataSize = 0;

	m_parse_pos = 0;
	m_fp = NULL;
	m_cts = 0;
	m_start_time = 0;
}

CStrdParser::~CStrdParser(void)
{
	CStrdParserClose();
}

HRESULT CStrdParser::GetTextData(char *filepath, byte_t *chunk_data, int *pSize, unsigned long *pCts)
{
	int read_size = 0;

	if(mp_UserData == NULL || m_UserDataSize == 0 || m_parse_pos >= m_UserDataSize)
		return ERROR_HANDLE_EOF;
	
	chunk_data_t *p_chunk_data = (chunk_data_t *)&mp_UserData[m_parse_pos];

	//printf ("          #%s (%d)\r\n", p_chunk_data->type, p_chunk_data->length);
	//
	if(mp_UserData[0] != 'h' && mp_UserData[1] != 'e'){ //oasis save-stream-data-in-header is 1
		while(m_parse_pos < m_UserDataSize){
			if(mp_UserData[m_parse_pos] == '$' && mp_UserData[m_parse_pos + 1] == 'H') { //header
				read_size = 0x20;
				break;
			}
			else if(mp_UserData[m_parse_pos] == 'M'){ // GSENSOR
				read_size = 0x12;
				break;
			}
			else {
				if(mp_UserData[m_parse_pos] != 0){ //PULSE or GPS
					read_size = strlen((char *)&mp_UserData[m_parse_pos]) + 1;
					break;
				}
				else
					m_parse_pos ++;
			}
		};

		if(read_size && *pSize > read_size){
			memcpy((void *)chunk_data, (void *)&mp_UserData[m_parse_pos], read_size);
			*pSize = read_size;
		}
		m_parse_pos += read_size;
	}
	else { //oasis save-stream-data-in-header is 0
		if(m_fp == NULL){
	        m_fp = fopen (filepath, "rb");
		}
		
		if(0==m_fp)
			return ERROR_HANDLE_EOF;
		
		if(-1 == fseek(m_fp, p_chunk_data->address_offset, SEEK_SET))
		//if(-1 == fseek(m_fp, p_chunk_data->address_offset + 8, SEEK_SET))		// skip 30 33 64 74 31 00 00 00 	| 03dt(size)...
			return E_FAIL;

		if(p_chunk_data->length < *pSize)
			read_size = p_chunk_data->length;
		else
			read_size = *pSize;
		
		if(read_size){
			if (0==fread (chunk_data, read_size, 1, m_fp))
				return E_FAIL;
		}
		m_parse_pos += sizeof(chunk_data_t);
	}

	if(read_size){
		if(chunk_data[0] == 'M'){
			int time = 0;
			/*
			buf[len++] = 'M';
			buf[len++] = (signed char)(pAccel->x / 4);
			buf[len++] = (signed char)(pAccel->y / 4);
			buf[len++] = (signed char)(pAccel->z / 4);
			buf[len++] = (tm_t.tm_year + 1900 )% 100;
			buf[len++] = tm_t.tm_mon + 1;
			buf[len++] = tm_t.tm_mday;
			buf[len++] = tm_t.tm_hour;
			buf[len++] = tm_t.tm_min;
			buf[len++] = tm_t.tm_sec;
			buf[len++] = ((tick / 100) % 10);
			buf[len++] = (tick % 100);
			*/

			//=================================================================================
			// [20260825 - MULTI-FILE MERGE SENSOR SYNC FIX : 2CH/3CH COMMON]
			// 다중/비연속 파일 머지 시 센서 데이터(CTS) 연속 동기화 보정 코드
			//=================================================================================
			UINT64 cur_data_time = get_time_from_gsensor_data(chunk_data, read_size);

			if(m_start_time == 0) 
			{
				m_start_time = cur_data_time;
				m_cts = 0;
			}
			else
			{
				INT64 time_diff = (INT64)(cur_data_time - m_start_time);
			
				// 연속 주행(시간 차이가 0 ~ 2초 이내)인 경우: 실제 경과 시간 반영
				// 비연속 파일(중간에 시간이 2초 이상 껑충 뛴 경우): 비디오 시간에 맞게 이전 마지막 시간 바로 뒤로 누적 연결!
				if (time_diff > 0 && (time_diff - (INT64)m_cts) <= 2000)
				{
					m_cts = (unsigned long)time_diff;
				}
				else if (time_diff <= 0 || (time_diff - (INT64)m_cts) > 2000)
				{
					// 새로운 파일(또는 비연속 구간) 시작 시 이전 재생 시점 뒤로 연속 누적
					m_start_time = cur_data_time - (m_cts + 100);
					m_cts += 100;
				}
			}
			//=================================================================================
			
			//printf ("cts = %d\r\n", m_cts);
		}
		
		*pCts = m_cts;
		
		//printf ("          #%s\r\n", chunk_data);

		//p_textData->ParseTextData(chunk_data, m_parse_pos, CWBCSensorData::GSENSOR_OLD_8_BIT); 
	}

	return S_OK;
}
int CStrdParser::CStrdParserOpen(const char* filepath)
{
        avi_info_t avi;
        memset (&avi,0,sizeof(avi));


        if (true==read_avi_header (filepath, &avi, &mp_UserData, &m_UserDataSize))

        {

               printf ("\r\n");

               printf ("OK\r\n");

 
               fps      = 1000000.0f/(double)avi.Header.MicroSecPerFrame;

               duration = avi.Header.TotalFrames / fps;

 

               AudioBlockPerSec  = (double)avi.Audio.Rate / (double)avi.Audio.Scale;

               VideoFramesPerSec = (double)avi.Video.Rate / (double)avi.Video.Scale;

               

               /*

               HeaderSize = avi.Header.TotalFrames * 8 * (avi.audio_data_count + 1);

               AudioSize  = (int)((avi.Audio.Length * avi.AudioWaveFormatEx.AvgBytesPerSec)/AudioBlockPerSec) * avi.audio_data_count;

               VideoSize  = avi.movi_size - HeaderSize - AudioSize;

               */

               HeaderSize = avi.movi_size - avi.video_data_size - avi.audio_data_size;

               AudioSize  = avi.audio_data_size;

               VideoSize  = avi.video_data_size;

 

               FileBitRate  = avi.avi_size * 8.0 / duration / 1000.0;

               AudioBitRate = avi.AudioWaveFormatEx.AvgBytesPerSec*8.0/1000.0;

               VideoBitRate = FileBitRate - AudioBitRate; // Windows AVI File Property

               VideoBitRate = (VideoSize * VideoFramesPerSec * 8.0)/(avi.Header.TotalFrames*1000.0);

 

               printf ("\r\n");

 

                if (avi.HasVideo)

               {

                       printf ("[Video] \r\n");

                       printf ("길이          = %5.3f 초\r\n", duration);

                       printf ("프레임너비   = %d\r\n", avi.Header.Width);

                       printf ("프레임높이   = %d\r\n", avi.Header.Height);

                       printf ("데이터속도   = %5.3f kbps \r\n", VideoBitRate);                     

                       printf ("총비트전송률= %5.3f kbps\r\n", FileBitRate);

                       printf ("프레임속도   = %5.3f 프레임/초\r\n", fps);

 

                       printf ("\r\n");

               }

 

               if (avi.HasAudio)

               {

                       printf ("[Audio] \r\n");

                       printf ("비트전송율      = %5.3f kbps \r\n", AudioBitRate);

                       printf ("채널            = %d \r\n", avi.AudioWaveFormatEx.Channels);

                       printf ("오디오샘플속도= %5.3f KHz\r\n", AudioBlockPerSec / 1000);

 

                       printf ("\r\n");

               }

				if (avi.HasStrd)
				{
					printf ("[User Data] \r\n");
					printf ("Size = %d Byte \r\n", m_UserDataSize);
				}


        }

 

        return avi.HasStrd;

}


void CStrdParser::CStrdParserClose(void)
{
	if(mp_UserData) {
		free(mp_UserData);
		mp_UserData = NULL;
	}

	if(m_fp) {
	 	fclose (m_fp);
		m_fp = NULL;
	}

	m_parse_pos = 0;
	m_cts = 0;
}

