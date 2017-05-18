#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (((fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

//Method used for creating a new file
int fs_create(char *filename, int mode) {

  //Check if the mode is valid
  if(mode != O_CREAT){
    printf("Invalid file create mode!\n");
    return SYSERR;
  }

  //Check if the file already exists.
  if(next_open_fd != 0){
    for(int i = 0; i<next_open_fd; i++){
      if(strcmp(oft[i].de->name,filename)==0){
        printf("Filename already exists!\n");
        return SYSERR;
      }
    }
  }

  //Check if filetable is full
  if(next_open_fd >= NUM_FD){
    printf("File table is full and creating can't be completed!\n");
    return SYSERR;
  }

  //Check if the inodes exists
  if(fsd.ninodes == fsd.inodes_used){
    printf("No inodes exists!\n");
    return SYSERR;

  }

  oft[next_open_fd].state = FSTATE_CLOSED;
  oft[next_open_fd].fileptr = 0;
  oft[next_open_fd].de = getmem(sizeof(struct dirent));
  strcpy(oft[next_open_fd].de->name, filename);
  oft[next_open_fd].de->inode_num = fsd.inodes_used;
  oft[next_open_fd].in.id = fsd.inodes_used;
  oft[next_open_fd].in.device = 0;
  oft[next_open_fd].in.size = 0;
  oft[next_open_fd].in.type = INODE_TYPE_FILE;
  oft[next_open_fd].in.nlink = 1;
  fs_put_inode_by_num(0, oft[next_open_fd].in.id, &oft[next_open_fd].in);
  int fd = next_open_fd;
  next_open_fd++;
  fsd.inodes_used++;
  return fd;
}

//Method for opening the file
int fs_open(char *filename, int flags){

 //Check if the file exists.
  if(next_open_fd != 0){
    for(int i = 0; i<next_open_fd; i++){
      if(strcmp(oft[i].de->name,filename) == 0){

        //Check if the file is already open
        if(oft[i].state == FSTATE_OPEN){
  		  printf("File is already open!\n");
          return SYSERR;
        }

        oft[i].state = FSTATE_OPEN;
        oft[i].mode = flags;
        return i;
      }
    }
    return SYSERR;
  } else{
  	printf("File does not exist!\n");
    return SYSERR;
  }
}

int fs_close(int fd){

  if(oft[fd].state == FSTATE_CLOSED){
    printf("File is already closed!\n");
    return SYSERR;
  }
  
  else{
    oft[fd].state = FSTATE_CLOSED;
    printf("\nFile is closed\n");
    return OK;
  }
}


int fs_seek(int fd, int offset){
  int fp = oft[fd].fileptr;

  if(fp + offset > oft[fd].in.size || (fp + offset) < 0 )
  {
    kprintf("Not a valid offset!\n");
    return SYSERR;
  }

  oft[fd].fileptr = fp + offset;
  return OK;
}


int fs_read(int fd, void *buf, int nbytes){

//Check if the file is closed
  if(oft[fd].state == FSTATE_CLOSED){
      printf("\nFile is closed!\n");
      return SYSERR;
  }

//Check if the file is in read mode
  if(oft[fd].mode == O_WRONLY){
      printf("\nThis is a write only file\n");
      return SYSERR;
  }

 int num_blocks = (nbytes/MDEV_BLOCK_SIZE) + 1;
 int start_blk = oft[fd].fileptr/MDEV_BLOCK_SIZE;
 int start_blk_addr = oft[fd].fileptr;
 int i = start_blk;
 
 int i_num = oft[fd].de->inode_num;

 struct inode *in = getmem(sizeof(struct inode));
 fs_get_inode_by_num(0,i_num,in);

 int total_bytes = nbytes;

 int offset = oft[fd].fileptr-start_blk_addr;
 int block=0, space, j = 0;

 while(nbytes > 0){

  block=in->blocks[i];

  if(j==0){

    if(MDEV_BLOCK_SIZE -(oft[fd].fileptr % MDEV_BLOCK_SIZE) < nbytes){
      space = MDEV_BLOCK_SIZE - (oft[fd].fileptr % MDEV_BLOCK_SIZE);
      bs_bread(dev0, block, offset,block_cache, space);
      strcpy(buf,block_cache);
      nbytes -= space;
      offset += space;
    }
    else{
      bs_bread(dev0, block, offset,block_cache, nbytes);
      strcpy(buf,block_cache);
      offset += nbytes;
      nbytes=0;
    }
    j++;
  }
  else{
    if(nbytes > MDEV_BLOCK_SIZE){
      bs_bread(dev0, block, 0,block_cache, MDEV_BLOCK_SIZE);
      strncat(buf,block_cache,MDEV_BLOCK_SIZE);
      nbytes -= MDEV_BLOCK_SIZE;
      offset += MDEV_BLOCK_SIZE;
    }
    else{
      bs_bread(dev0, block, 0,block_cache, nbytes);
      strncat(buf,block_cache,nbytes);
      nbytes = 0;
    }
  }
  i++;
  j++;
}

 oft[fd].fileptr += total_bytes;
 return oft[fd].fileptr;
}


int fs_write(int fd, void *buf, int nbytes){
  
  //Check if the file is already open
  if(oft[fd].state != FSTATE_OPEN || (oft[fd].mode == O_RDONLY)){
    printf("The file is not open or write operation is not allowed!\n");
    return SYSERR;
  }

  int total_blks = (nbytes/MDEV_BLOCK_SIZE) + 1;
  int start_blk = (oft[fd].fileptr)/MDEV_BLOCK_SIZE;
  int i = start_blk;
  int start_blk_addr = start_blk * MDEV_BLOCK_SIZE;

  int offset = oft[fd].fileptr - start_blk_addr;
  struct inode *ind = getmem(sizeof(struct inode));
  int j = 0, space, block;
  int total_bytes = nbytes;

  while(nbytes > 0){
    block = ind->blocks[i];
    if(j == 0){  
            if((MDEV_BLOCK_SIZE-(oft[fd].fileptr%MDEV_BLOCK_SIZE)) < nbytes){
                  space = MDEV_BLOCK_SIZE-(oft[fd].fileptr%MDEV_BLOCK_SIZE);
                  strncpy(block_cache,buf,space);
                  nbytes -= space;
                  bs_bwrite(dev0, block, offset,block_cache, space);
                  offset += space;
            }
            else{
                  strncpy(block_cache,buf,nbytes);
                  bs_bwrite(dev0, block, offset,block_cache, nbytes);
                  offset += nbytes;
                  nbytes = 0;
            }
        }
    
    else{
        if(nbytes>MDEV_BLOCK_SIZE){
            strncpy(block_cache,buf+offset,MDEV_BLOCK_SIZE);
            nbytes-=MDEV_BLOCK_SIZE;
            offset+=MDEV_BLOCK_SIZE;
            bs_bwrite(dev0, block, 0, block_cache, MDEV_BLOCK_SIZE);
          }
          else{
            strncpy(block_cache, buf + offset, nbytes);
            bs_bwrite(dev0, block, 0, block_cache, nbytes);
            offset += nbytes;
            nbytes = 0;
          }
        }
  i++;
  j++;
}
 oft[fd].fileptr += total_bytes;
 freemem(ind,sizeof(ind));
 return oft[fd].fileptr;
}

#endif /* FS */