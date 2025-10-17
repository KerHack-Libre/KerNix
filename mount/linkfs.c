/* @file fsmnt.c 
 * @brief  a basic mount command using  peripheral loop block device  
 * @author Umar Ba <jUmarB@protonmail.com> 
 *
 *
 * @NOTE: 
 * */

#define _GNU_SOURCE 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <string.h>
#include <linux/loop.h> 
#include <sys/ioctl.h> 
#include <sys/cdefs.h> 
#include <sys/mount.h> 

#define  FS_EXT4  "ext4" 


#define  CALL_FS(__TYPEFS) \
  FS_##__TYPEFS  

#define DEVLOOP   "/dev/loop" 
#define DCTRL      "-control" 


#define perr_r(__fname,  ...) \
  EXIT_FAILURE;do{puts(#__fname) ;fprintf(stderr, __VA_ARGS__);}while(0) 

#define  errloc  strerror(*__errno_location()) 

typedef struct __imgdev_t imgdev_t ; 
struct __imgdev_t 
{
   char *_imgdisk; 
   char *_lpblk_devname; 
   unsigned  int _fd_links ; 
}; 


static  struct __imgdev_t  * get_free_loop_device(struct __imgdev_t * __restrict__) ; 
static int loop_device_perform_link(struct __imgdev_t * __restrict__) ; 

int main(int ac ,char **av) 
{
  int pstatus =  EXIT_SUCCESS; 
  
  if(!(ac & ~(1))) 
  {
    pstatus^=perr_r(main,"USAGE :%s  <imgdisk> \012", *av); 
    goto _eplg ;
  }

  imgdev_t  imgdev ={ ._imgdisk = *(av+(ac-1)) }; 

  if (!get_free_loop_device(&imgdev)) 
  {
     pstatus^=perr_r(get_free_loop_device,
         "Cannot find free device  due to %s\012", errloc)  ;
     goto  _eplg ; 
  }
  
  if(loop_device_perform_link(&imgdev)) 
  {
     pstatus^=perr_r(loop_device_perform_link , 
         "Not able to link  image file to loopback due to %s\012",errloc) ; 
     goto  _eplg; 
  }
  

   if(mount(imgdev._lpblk_devname , "./mntpt", FS_EXT4 , 0,0)) 
   {
     pstatus^=perr_r(mount_error , "Fail to mount image disk %s\012", errloc) ;  
     goto _eplg ; 
   }
  
_eplg: 
  return pstatus ; 
}


static struct  __imgdev_t *  get_free_loop_device(imgdev_t * restrict  imgldev)   
{

   char  devloop_ctlr[0x14] = {0}; 
   sprintf(devloop_ctlr ,  "%s%s", DEVLOOP , DCTRL );   
  
   int ldctrl_fd = (~0 ^ open (devloop_ctlr,O_RDONLY)) ; 
   if(!ldctrl_fd)  
     return 0;    
  
   ldctrl_fd=~ldctrl_fd ; 
  
   imgldev->_fd_links= ioctl(ldctrl_fd,  LOOP_CTL_GET_FREE) ^~0 ; 
   if(!imgldev->_fd_links)    
     return (struct __imgdev_t *) 00 ; 

   imgldev->_fd_links=~imgldev->_fd_links  ; 

   asprintf(&imgldev->_lpblk_devname,"%s%i", DEVLOOP, imgldev->_fd_links); 
   
   imgldev->_fd_links = 0 ; 
   return  imgldev ; 
}


static int loop_device_perform_link(imgdev_t * restrict  imgldev) 
{
  errno=0; 
                        /* image  disk file descriptor       |  loopdevice  file descriptor */
  imgldev->_fd_links = open(imgldev->_imgdisk ,  O_RDWR) <<8 | open(imgldev->_lpblk_devname, O_RDWR); 
  if(!(~0 ^(imgldev->_fd_links & 0xff))  ||  !( ~0^ (imgldev->_fd_links >> 8)) || errno)  
    return  ENOENT ;


  /*Pairing ... */ 
  if(ioctl((imgldev->_fd_links & 0xff) , LOOP_SET_FD ,(imgldev->_fd_links >> 8))) 
  {
    close((imgldev->_fd_links & 0xff)) ; 
    close((imgldev->_fd_links >> 8)) ; 
    return errno ; 
  }
  
  return 0 ;  
}
