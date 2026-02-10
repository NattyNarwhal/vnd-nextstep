/*
 * This program exchanges messages with out-of-line data with the kernel server 
 * named "mydriver".
 */
 
#import <mach/mach.h>
#import <mach/mach_error.h>
#import <servers/netname.h>
#import <ansi/stdlib.h>
#import <ansi/stdio.h>
#import "../Library/mydriver.h"
#import <bsd/strings.h>

#define DATA "blah "
int main()
{
    kern_return_t   r;
    port_name_t mydriver_port;
    user_data_t data;
    unsigned int data_len;

    /* Look up the advertised port of mydriver so we can send it a message. */
    r = netname_look_up(name_server_port, "", "mydriver0", &mydriver_port);
    if (r != KERN_SUCCESS)
    {
	mach_error("Can't find mydriver's advertised port", r);
	exit(1);
    }

    //
    // Allocate space for data.  Need to use vm_allocate to set up
    // data or else the LKS won't get paged aligned data.
    //
    r = vm_allocate(task_self(), (vm_address_t *) &data, vm_page_size, TRUE);
    if (r != KERN_SUCCESS)
    {
	mach_error("Call to vm_allocate failed", r);
	exit(1);
    }

    strcpy(data, DATA);
    data_len = strlen(data) + 1;
    printf("Data is: %s\n", data);

    /* Send out-of-line data to the LKS. */
    printf("Calling mydriver_import().\n");
    r = mydriver_import(mydriver_port, data, data_len);
    if (r != KERN_SUCCESS)
    {
	mach_error("Error calling mydriver_import", r);
	exit(1);
    }

    /* remove the out_of_band data from our task's vm */
    r = vm_deallocate(task_self(), (vm_address_t) data, vm_page_size);
    if (r != KERN_SUCCESS)
    {
	mach_error("Error calling mydriver_import vm_deallocate", r);
	exit(1);
    }

    /* Receive out-of-line data from the LKS. */
    printf("Calling mydriver_export().\n");
    r = mydriver_export(mydriver_port, &data, &data_len);
    if (r != KERN_SUCCESS)
    {
	mach_error("Error calling mydriver_export", r);
	exit(1);
    }
    
    printf("Trying to access data.\n");
    printf("Contents of data are: %s\n", data);

    /* remove the out_of_band data from our task's vm */
    r = vm_deallocate(task_self(), (vm_address_t) data, vm_page_size);
    if (r != KERN_SUCCESS)
    {
	mach_error("Error calling mydriver_export vm_deallocate", r);
	exit(1);
    }
    
    return 0;
}
