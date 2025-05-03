// # Written by Rohith Saradhy
// # Email -> rohithsaradhy@gmail.com
#include "spi_common.h"
#include "NexysDDMTD.h"



const int64_t MAX_CNT   = 4094967296;
static const uint FIFO_DEPTH = 512;
int skipFirst3 = 0;


int memDump_DDMTD(int addr_mem,int num_words,char filename[100], int mem_no );
int calcValues(void* virtual_address, int byte_count) ;


int main(int argc, char** argv)
{
  // Startup the SPI interface on the Pi.
  init_spi();
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); 
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

  //Switch Off Heating...
  // heating_status(0); //0--> both off; 1--> heat ddmtd1; 2--> head ddmtd2; 

  int N;
  if(argc == 2)
    N= atoi(argv[1]); 
  else
    N=1;


  // char filename1[100] = "./data/ddmtd1.txt";
  // char filename2[100] = "./data/ddmtd2.txt";

  // remove(filename1);
  // remove(filename2);

  // FILE *fp1 = fopen(filename1,"w");
  // FILE *fp2 = fopen(filename2,"w");

  int mem_full=0; 
  int num_of_words = 2*FIFO_DEPTH; 
  int addr_mem     = ADDR_MEM1;

  int num_Bytes = 4*num_of_words+2; // Cause each word is 32 bits so 4 bytes per word and +2 because the first two bytes are for sending command
  char* data_buf = malloc(num_Bytes);
  char*  cmd_buf = malloc(num_Bytes);
  char* total_data_buf =  malloc(N*num_Bytes);

  // printf("Data_Allocated: 2*%f MB \n",((float)(num_Bytes)/1000000));

  memset(data_buf, 0xff,num_Bytes );
  memset(cmd_buf, 0xff, num_Bytes );
// print_nWords(cmd_buf,num_Bytes,2) ; //Test to see if the memory buffer has been initialized.

  cmd_buf[0] = ADDR_MEM1 & 0xFF;
  cmd_buf[1] = 0;

  int N_count =0;
  int numBytesRead =0; //Number of bytes read
  int i=0;



 
  // 
  // // //Debug Tools...
  // readCounter();
  // readFIFO_readCount();
  // readFIFO_writeCount();
  // // END Debug Tools...

  
  //Clear out the FIFO
  bcm2835_spi_transfernb(cmd_buf, data_buf, num_Bytes); 
  bcm2835_spi_transfernb(cmd_buf, data_buf, num_Bytes); 
  memset(data_buf, 0xff,num_Bytes );


  
  // Offset parameter
  uint num_of_words_toRemove = 2; // 1word from both ddmtd1 & ddmtd2
  uint offset_bytes = num_of_words_toRemove*2*4 + 2; //2 since ddmtd1 & ddmtd2; 4 is the number of bytes per word; +2 for the ignoring the returned command.
  startAcq();
  while(i < 100000000)
  {
      if (N_count>=N) break;
      mem_full = send3Byte_NexysDDMTD(MEM_FULL,0x0); // 3::both, 2::Mem1, 1::Mem2 , 0 None
      if(mem_full==3)
      {
          N_count = N_count + 1;
          bcm2835_spi_transfernb(cmd_buf, data_buf, num_Bytes);          
          memcpy(total_data_buf+numBytesRead,data_buf+offset_bytes,num_Bytes-offset_bytes);
          numBytesRead = numBytesRead + num_Bytes - offset_bytes;
          // memset(data_buf, 0xff,num_Bytes ); //cleaning data
      }

      i=i+1;
  }
  stopAcq();   
  calcValues(total_data_buf, numBytesRead);

  // fclose(fp1);
  // fclose(fp2);

  free(data_buf);
  free(cmd_buf);
  free(total_data_buf);




  end_spi();
  return 0;
}





int calcValues(void* virtual_address, int byte_count) 
{
  int offset;
  uint val1,val2;
  uint val1n,val2n;

  float phase_rise=0;
  float phase_fall=0;

  
  char *p = virtual_address;
  int mod_num = 2; //number of channels
  int word_byte = 4; //bytes per word
  int skip_words =2; //skip just the first word
  int i=0;
  int rise_count=0;
  int fall_count=0;


  for (offset = 0; offset < byte_count-mod_num*word_byte; offset=offset+mod_num*word_byte){
    val1  = (uint)(0xffffffff&(p[0+offset]|p[1+offset]<<8|p[2+offset]<<16|p[3+offset]<<24));
    val1n = (uint)(0xffffffff&(p[0+offset+mod_num*word_byte]|p[1+offset+mod_num*word_byte]<<8|p[2+offset+mod_num*word_byte]<<16|p[3+offset+mod_num*word_byte]<<24));

    val2  = (uint)(0xffffffff&(p[0+offset+word_byte]|p[1+offset+word_byte]<<8|p[2+offset+word_byte]<<16|p[3+offset+word_byte]<<24));
    val2n = (uint)(0xffffffff&(p[0+offset+word_byte+mod_num*word_byte]|p[1+offset+word_byte+mod_num*word_byte]<<8|p[2+offset+word_byte+mod_num*word_byte]<<16|p[3+offset+word_byte+mod_num*word_byte]<<24));

    if (i%2 == 0) {
      phase_rise = phase_rise + ((float)((val1n-val1)+(val2n-val2)))/2;
      rise_count=rise_count+1;
    }
    else {
      phase_fall = phase_fall + ((float)((val1n-val1)+(val2n-val2)))/2;
      fall_count=fall_count+1;
    }
    i=i+1;
    // printf("%u,%u, \n ",val1n-val1,val2n-val2);
  }
  printf("%5.3f,%5.3f \n ",phase_rise/rise_count,50000-phase_fall/fall_count);
  return 0;
}